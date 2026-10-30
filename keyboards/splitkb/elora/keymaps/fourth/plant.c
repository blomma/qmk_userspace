#include QMK_KEYBOARD_H
#include "canvas.h"
#include "plant.h"

// The plant is drawn in a geometric style: a sunburst for a head, half-moon
// leaves, a single line for a stem and symmetric roots.

// Key presses from a seed to the plant starting over
#define PLANT_PRESSES 800

// The animation moves on in steps of STEP_MS. After a pause, such as the
// display being off, it catches up at most MAX_STEPS.
#define STEP_MS 50
#define MAX_STEPS 4

// Growth runs from 0 for a seed to GROWTH_FULL when the plant starts over.
// The stages overlap: the roots grow while the stem does, the bud forms
// while the stem is still reaching up.
#define GROWTH_FULL 1024
#define ROOTS_DONE 480
#define SPROUT 16
#define STEM_DONE 760
#define BUD_START 640
#define BLOOM_START 800
// Full bloom, held for the last presses before the plant starts over
#define BLOOM_DONE 960
_Static_assert(SPROUT < BUD_START && BUD_START < BLOOM_START &&
                   BLOOM_START < BLOOM_DONE && BLOOM_DONE <= GROWTH_FULL &&
                   STEM_DONE <= GROWTH_FULL && ROOTS_DONE <= GROWTH_FULL,
               "the growth stages run in order");

// The scene, in pixels. GROUND_Y is the row of the ground line.
#define GROUND_Y 102
#define GROUND_HALF_WIDTH 24
#define STEM_X 32
#define STEM_HEIGHT 60
#define SUN_X 51
#define SUN_Y 11
#define SUN_RADIUS 5
#define SUN_RAYS 8
#define RAY_FROM 8
#define RAY_TO 10

// Each leaf grows from a node on the stem, on alternate sides starting on
// the right
static const struct {
    uint8_t height;
    uint8_t length;
} leaf_nodes[] = {{10, 15}, {23, 13}, {34, 9}};

// The head is a disc ringed with SPIKES spikes. At full bloom the disc has
// radius HEAD_RADIUS and the spikes reach SPIKE_LENGTH beyond it. A bud
// grows to BUD_RADIUS before it opens.
#define SPIKES 16
#define HEAD_RADIUS 10
#define SPIKE_LENGTH 6
#define BUD_RADIUS 4

// The tap root grows ROOT_DEPTH straight down. Side roots branch off it in
// pairs, slanting down either way.
#define ROOT_DEPTH 18
static const struct {
    uint8_t depth;
    uint8_t length;
} side_roots[] = {{4, 9}, {9, 7}, {14, 4}};

// Positions and distances are in 1/256 of a pixel
#define FIX(px) ((int32_t)(px) * 256)

typedef struct {
    int32_t x, y;
} point_t;

// Angles run from 0 to 255 for a full turn, clockwise from pointing right
// as y grows downwards: 64 points down and 192 up. The sine of a quarter
// turn, 127 times sin(i / 256 of a turn).
// clang-format off
static const uint8_t PROGMEM quarter_sine[65] = {
      0,   3,   6,   9,  12,  16,  19,  22,  25,  28,  31,  34,  37,  40,  43,  46,
     49,  51,  54,  57,  60,  63,  65,  68,  71,  73,  76,  78,  81,  83,  85,  88,
     90,  92,  94,  96,  98, 100, 102, 104, 106, 107, 109, 111, 112, 113, 115, 116,
    117, 118, 120, 121, 122, 122, 123, 124, 125, 125, 126, 126, 126, 127, 127, 127,
    127,
};
// clang-format on

// 127 times the sine of angle
static int16_t sine(uint8_t angle) {
    const uint8_t i     = angle & 63;
    const int16_t value = pgm_read_byte(&quarter_sine[angle & 64 ? 64 - i : i]);
    return angle & 128 ? -value : value;
}

static int16_t cosine(uint8_t angle) {
    return sine(angle + 64);
}

static int32_t magnitude(int32_t v) {
    return v < 0 ? -v : v;
}

// The pixel v falls in. Right shifts of negative numbers are arithmetic in
// GCC, so this rounds the same way either side of 0.
static int16_t to_pixel(int32_t v) {
    return (v + 128) >> 8;
}

static point_t move(point_t p, uint8_t angle, int32_t distance) {
    return (point_t){p.x + cosine(angle) * distance / 127,
                     p.y + sine(angle) * distance / 127};
}

static void plot(point_t p, bool on) {
    canvas_set_pixel(to_pixel(p.x), to_pixel(p.y), on);
}

static void line(point_t from, point_t to, bool on) {
    const int32_t dx = to.x - from.x, dy = to.y - from.y;
    // Half pixel steps, so no pixel along the way is skipped
    const int32_t steps = MAX(magnitude(dx), magnitude(dy)) / 128 + 1;
    for (int32_t i = 0; i <= steps; i++) {
        plot((point_t){from.x + dx * i / steps, from.y + dy * i / steps}, on);
    }
}

// Whether a pixel dx, dy from the centre of a circle is inside it. Adding
// the radius rounds the edge off nicely at small sizes.
static bool in_circle(int16_t dx, int16_t dy, int16_t radius) {
    return dx * dx + dy * dy <= radius * radius + radius;
}

// Lights or blanks a disc, or with outline just its edge
static void circle(point_t centre, int16_t radius, bool on, bool outline) {
    const int16_t x = to_pixel(centre.x), y = to_pixel(centre.y);
    for (int16_t dy = -radius; dy <= radius; dy++) {
        for (int16_t dx = -radius; dx <= radius; dx++) {
            if (in_circle(dx, dy, radius) &&
                !(outline && in_circle(dx, dy, radius - 1))) {
                canvas_set_pixel(x + dx, y + dy, on);
            }
        }
    }
}

// The angle from the origin to dx, dy, in 1/65536 of a turn, which is the
// angle in 1/256 px. Accurate to within a tenth of the angles the rest of
// the plant uses.
static uint16_t angle_of(int16_t dx, int16_t dy) {
    const int32_t x = magnitude(dx), y = magnitude(dy);
    if (x == 0 && y == 0) {
        return 0;
    }
    // atan(t) is close to t * pi / 4 + 0.273 * t * (1 - t) for t from 0 to
    // 1, here in 1/256 of the angle for t in 1/256: 32 and 11 are pi / 4 and
    // 0.273 in 1/256 of a turn
    const int32_t t     = MIN(x, y) * 256 / MAX(x, y);
    int32_t       angle = 32 * t + 11 * t * (256 - t) / 256;
    if (y > x) {
        angle = FIX(64) - angle;
    }
    if (dx < 0) {
        angle = FIX(128) - angle;
    }
    if (dy < 0) {
        angle = FIX(256) - angle;
    }
    return angle;
}

// A star of points spikes, the first pointing right, reaching out to outer
// with inner where they meet, both in 1/256 px. The spikes have straight
// sides. Testing every pixel against the outline, rather than drawing a
// triangle per spike, keeps them even.
static void star(point_t centre, int32_t inner, int32_t outer, uint8_t points) {
    const int16_t  x = to_pixel(centre.x), y = to_pixel(centre.y);
    const int16_t  r      = to_pixel(outer);
    const uint16_t period = 65536 / points;
    // Where a spike pointing right meets the next one round
    const uint8_t half = 128 / points;
    const int32_t vx   = inner * cosine(half) / 127,
                  vy   = inner * sine(half) / 127;
    for (int16_t dy = -r; dy <= r; dy++) {
        for (int16_t dx = -r; dx <= r; dx++) {
            // Turn the pixel with the nearest spike until that points right.
            // Folded onto the side towards the next spike, it is inside
            // when on the same side of the spike's edge as the centre.
            const uint8_t spike = (uint16_t)(angle_of(dx, dy) + period / 2) /
                                  period * 256 / points;
            const int32_t u = FIX(dx * cosine(spike) + dy * sine(spike)) / 127;
            const int32_t v =
                magnitude(FIX(dy * cosine(spike) - dx * sine(spike)) / 127);
            if ((vx - outer) * v - vy * (u - outer) >= 0) {
                canvas_set_pixel(x + dx, y + dy, true);
            }
        }
    }
}

// A triangle from base towards angle, length pixels long, half_width (in
// 1/256 px) to either side of its middle at the base
static void spike(point_t base, uint8_t angle, int16_t length,
                  int32_t half_width) {
    if (length < 1) {
        return;
    }
    const int16_t steps = length * 2;
    for (int16_t t = 0; t <= steps; t++) {
        const point_t middle = move(base, angle, (int32_t)t * 128);
        const int32_t half   = half_width * (steps - t) / steps;
        for (int32_t u = -half; u <= half; u += 128) {
            plot(move(middle, angle + 64, u), true);
        }
    }
}

// A half moon with its flat side from base towards angle, length pixels
// long, bulging towards bulge
static void half_moon(point_t base, uint8_t angle, int16_t length,
                      uint8_t bulge) {
    if (length < 1) {
        return;
    }
    const int16_t steps  = length * 2;
    const int32_t height = FIX(length) * 2 / 5;
    for (int16_t t = 0; t <= steps; t++) {
        const point_t middle = move(base, angle, (int32_t)t * 128);
        const int32_t across = height * sine(t * 128 / steps) / 127;
        for (int32_t u = 0; u <= across; u += 128) {
            plot(move(middle, bulge, u), true);
        }
    }
}

// State the animation steps through

// Key presses since the plant started over, counted outside the OLED pass
static uint16_t presses;
// Key presses that haven't nudged the plant yet
static uint8_t nudges;

// The growth the plant shows, easing towards what the presses add up to
static uint16_t growth;
// Set while a plant that has dropped its spikes shrinks back into the ground
static bool withering;

// How far the top of the stem leans to the right and how fast it moves, in
// 1/256 px
static int32_t  sway, sway_speed;
static uint16_t wind_phase, gust_phase;
static uint8_t  flutter_phase, sun_phase;

// Spikes dropped by the last plant, each falling at its own speed (in 1/256
// px a step), swinging from side to side and turning over in its own time
static struct {
    point_t at;
    uint8_t speed;
    uint8_t phase;
    bool    falling;
} dropped[SPIKES];

void plant_count_keypress(void) {
    presses++;
    if (nudges < UINT8_MAX) {
        nudges++;
    }
}

static int16_t stem_height(void) {
    if (growth < SPROUT) {
        return 0;
    }
    return 2 + (int32_t)(STEM_HEIGHT - 2) * MIN(growth, STEM_DONE) / STEM_DONE;
}

// The sway at the top of a stem this tall. A short stem leans less.
static int32_t stem_sway(int16_t stem) {
    return sway * stem / STEM_HEIGHT;
}

// The point on the stem height pixels above the ground. The stem bends to
// sway, more the higher up.
static point_t stem_point(int16_t height, int16_t stem) {
    return (point_t){
        FIX(STEM_X) + stem_sway(stem) * height * height / (stem * stem),
        FIX(GROUND_Y - height)};
}

// 0 at the start of the bloom, 256 at full bloom
static int32_t bloom(void) {
    return (int32_t)256 * (MIN(growth, BLOOM_DONE) - BLOOM_START) /
           (BLOOM_DONE - BLOOM_START);
}

static int16_t head_radius(void) {
    if (growth < BLOOM_START) {
        return 1 + (BUD_RADIUS - 1) * (growth - BUD_START) /
                       (BLOOM_START - BUD_START);
    }
    return BUD_RADIUS + (HEAD_RADIUS - BUD_RADIUS) * bloom() / 256;
}

static uint8_t spike_angle(uint8_t i) {
    return i * 256 / SPIKES;
}

static void drop_spikes(void) {
    const int16_t stem = stem_height();
    const point_t head = stem_point(stem, stem);
    for (uint8_t i = 0; i < SPIKES; i++) {
        const uint8_t angle = spike_angle(i);
        dropped[i].at = move(head, angle, FIX(HEAD_RADIUS + SPIKE_LENGTH / 2));
        // Spread the speeds and phases over the spikes, out of step with
        // their order around the head
        dropped[i].speed   = 96 + i * 37 % 112;
        dropped[i].phase   = angle + i * 73;
        dropped[i].falling = true;
    }
}

static void step(void) {
    // Grow, or start over once the presses add up to a whole plant
    if (presses >= PLANT_PRESSES) {
        presses -= PLANT_PRESSES;
        drop_spikes();
        withering = true;
    }
    const uint16_t target = (uint32_t)presses * GROWTH_FULL / PLANT_PRESSES;
    if (growth < target) {
        growth = target;
    } else if (growth > target) {
        growth -= MAX(1, (growth - target) / 12);
    }
    if (growth == target) {
        withering = false;
    }

    // The wind bends the stem one way and the other, gusts come and go on
    // top of it, and the stem springs back
    wind_phase += 1100;
    gust_phase += 380;
    const int32_t wind =
        (FIX(1) * sine(wind_phase >> 8) + FIX(1) * sine(gust_phase >> 8) / 2) /
        127;
    sway_speed += (wind - sway) / 8 - sway_speed / 8;
    // A key press pushes it further the way it is already going
    for (; nudges > 0; nudges--) {
        sway_speed += sway_speed < 0 ? -FIX(1) / 4 : FIX(1) / 4;
    }
    sway = MAX(-FIX(4), MIN(FIX(4), sway + sway_speed));

    flutter_phase += 7;
    sun_phase++;

    for (uint8_t i = 0; i < SPIKES; i++) {
        if (dropped[i].falling) {
            dropped[i].at.y += dropped[i].speed;
            dropped[i].at.x += FIX(1) * sine(dropped[i].phase) / 127 / 2;
            dropped[i].phase += 11;
            dropped[i].falling = dropped[i].at.y < FIX(GROUND_Y - 1);
        }
    }
}

static void draw_sun(void) {
    const point_t centre = {FIX(SUN_X), FIX(SUN_Y)};
    circle(centre, SUN_RADIUS, true, true);
    // The rays turn slowly
    for (uint8_t i = 0; i < SUN_RAYS; i++) {
        const uint8_t angle = i * 256 / SUN_RAYS + sun_phase / 4;
        line(move(centre, angle, FIX(RAY_FROM)),
             move(centre, angle, FIX(RAY_TO)), true);
    }
}

static void draw_roots(void) {
    // How far the roots have grown down
    const int16_t reach =
        (int32_t)ROOT_DEPTH * MIN(growth, ROOTS_DONE) / ROOTS_DONE;
    const point_t top = {FIX(STEM_X), FIX(GROUND_Y + 1)};
    if (reach > 0) {
        line(top, move(top, 64, FIX(reach - 1)), true);
    }
    // A side root starts once the tap root is past it and grows as fast
    for (uint8_t i = 0; i < ARRAY_SIZE(side_roots); i++) {
        const int16_t length =
            MIN(side_roots[i].length, reach - side_roots[i].depth);
        if (length < 1) {
            continue;
        }
        const point_t from = move(top, 64, FIX(side_roots[i].depth));
        line(from, move(from, 64 - 32, FIX(length)), true);
        line(from, move(from, 64 + 32, FIX(length)), true);
    }
}

static void draw_head(int16_t stem) {
    const point_t head   = stem_point(stem, stem);
    const int16_t radius = head_radius();
    if (growth < BLOOM_START) {
        // Still a bud
        circle(head, radius, true, false);
        return;
    }

    // The spikes grow out of the disc as it opens. A withering head has
    // dropped them.
    const int32_t spikes = withering ? 0 : FIX(SPIKE_LENGTH) * bloom() / 256;
    star(head, FIX(radius), FIX(radius) + spikes, SPIKES);

    // Inside the disc: a gap, a ring and a solid middle
    circle(head, radius - 2, false, false);
    if (radius - 4 >= 2) {
        circle(head, radius - 4, true, true);
    }
    circle(head, MAX(1, radius * 3 / 10), true, false);
}

static void draw_plant(void) {
    draw_roots();

    const int16_t stem = stem_height();
    if (stem < 6) {
        // The seed, until the shoot is up
        circle((point_t){FIX(STEM_X), FIX(GROUND_Y + 2)}, 1, true, false);
    }

    for (int16_t height = 1; height <= stem; height++) {
        plot(stem_point(height, stem), true);
    }

    for (uint8_t i = 0; i < ARRAY_SIZE(leaf_nodes); i++) {
        const int16_t length =
            MIN(leaf_nodes[i].length, (stem - leaf_nodes[i].height) * 2 / 3);
        if (length < 2) {
            continue;
        }
        const bool    right = i % 2 == 0;
        const point_t node  = stem_point(leaf_nodes[i].height, stem);
        // Rising a little from the stem, tipping as it sways, each leaf
        // fluttering in its own time
        const uint8_t angle = (right ? 246 : 138) + stem_sway(stem) / 128 +
                              sine(flutter_phase + i * 45) / 50;
        half_moon(node, angle, length, right ? angle - 64 : angle + 64);
    }

    if (growth >= BUD_START && stem > 0) {
        draw_head(stem);
    }
}

void plant_render(void) {
#if OLED_TIMEOUT > 0
    // Sending a change turns the display on and pushes its timeout back, so
    // a plant that never stops moving would keep the display on for good.
    // Once the keyboard has been idle as long as the timeout, the plant
    // stops and turns the display off itself, every pass, in case a change
    // still on its way turns it back on.
    if (last_input_activity_elapsed() >= OLED_TIMEOUT) {
        oled_off();
        return;
    }
#endif

    static uint32_t last_step;
    const uint32_t  now   = timer_read32();
    uint32_t        steps = (now - last_step) / STEP_MS;
    if (steps == 0) {
        return;
    }
    if (steps > MAX_STEPS) {
        steps     = MAX_STEPS;
        last_step = now;
    } else {
        last_step += steps * STEP_MS;
    }
    while (steps-- > 0) {
        step();
    }

    canvas_clear();
    draw_sun();
    for (int16_t x = -GROUND_HALF_WIDTH; x < GROUND_HALF_WIDTH; x++) {
        canvas_set_pixel(STEM_X + x, GROUND_Y, true);
    }
    draw_plant();
    for (uint8_t i = 0; i < SPIKES; i++) {
        if (dropped[i].falling) {
            // A small spike, turning over as it falls
            spike(dropped[i].at, dropped[i].phase, 3, FIX(1));
        }
    }
    canvas_show();
}

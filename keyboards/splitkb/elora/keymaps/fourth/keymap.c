#include QMK_KEYBOARD_H
#include "frames.h"
#include "keymap_swedish.h"

static int num_keypresses = 0;
static int current_frame  = 0;

enum layers {
    BASE = 0,
    SYM,
    NAV,
    GAME,
    SETTING
};

// Tap Dance declarations
enum {
    TD_LCBR_RCBR,
    TD_LPR_RPR,
    TD_LBRC_RBRC,
    TD_COMMA_LESS,
    TD_DOT_GRTR
};

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
    [TD_LCBR_RCBR]  = ACTION_TAP_DANCE_DOUBLE(SE_LCBR, SE_RCBR),
    [TD_LPR_RPR]    = ACTION_TAP_DANCE_DOUBLE(SE_LPRN, SE_RPRN),
    [TD_LBRC_RBRC]  = ACTION_TAP_DANCE_DOUBLE(SE_LBRC, SE_RBRC),
    [TD_COMMA_LESS] = ACTION_TAP_DANCE_DOUBLE(SE_COMM, SE_LABK),
    [TD_DOT_GRTR]   = ACTION_TAP_DANCE_DOUBLE(SE_DOT, SE_RABK),
};

// clang-format off
// const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = LAYOUT(
//   'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'L' ,       'R' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R',
//   'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'L' ,       'R' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R',
//   'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'L' ,       'R' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R',
//   'L' , 'L' , 'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R' , 'R' , 'R',
//                     'L' , 'L' , 'L' , 'L' , 'L' ,       'R' , 'R' , 'R' , 'R' , 'R'
// );

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
//    ┌─────────┬───────────┬───────────┬──────────────────┬────────────────┬──────────────────┐              ┌─────────────┐       ┌──────────┐     ┌───────────────────┬─────────────────┬───────────┬───────────┬─────────────────┬────────────┐
//    │   esc   │     1     │     2     │        3         │       4        │        5         │              │             │       │          │     │         6         │        7        │     8     │     9     │        0        │    bspc    │
//    ├─────────┼───────────┼───────────┼──────────────────┼────────────────┼──────────────────┤              ├─────────────┤       ├──────────┤     ├───────────────────┼─────────────────┼───────────┼───────────┼─────────────────┼────────────┤
//    │   tab   │     q     │     w     │        e         │       r        │        t         │              │             │       │          │     │         y         │        u        │     i     │     o     │        p        │  SE_ARNG   │
//    ├─────────┼───────────┼───────────┼──────────────────┼────────────────┼──────────────────┤              ├─────────────┤       ├──────────┤     ├───────────────────┼─────────────────┼───────────┼───────────┼─────────────────┼────────────┤
//    │ MO(NAV) │ LGUI_T(a) │ LALT_T(s) │    LCTL_T(d)     │   LSFT_T(f)    │        g         │              │             │       │          │     │         h         │    RGUI_T(j)    │ RALT_T(k) │ RCTL_T(l) │ RSFT_T(SE_ODIA) │ LT(NAV, ä) │
//    ├─────────┼───────────┼───────────┼──────────────────┼────────────────┼──────────────────┼──────────────┼─────────────┤       ├──────────┼─────┼───────────────────┼─────────────────┼───────────┼───────────┼─────────────────┼────────────┤
//    │         │     z     │     x     │        c         │       v        │        b         │              │ TG(SETTING) │       │ TG(GAME) │     │         n         │        m        │     ,     │     .     │                 │            │
//    └─────────┴───────────┴───────────┼──────────────────┼────────────────┼──────────────────┼──────────────┼─────────────┤       ├──────────┼─────┼───────────────────┼─────────────────┼───────────┼───────────┴─────────────────┴────────────┘
//                                      │ TD(TD_LCBR_RCBR) │ TD(TD_LPR_RPR) │ TD(TD_LBRC_RBRC) │ LT(SYM, spc) │             │       │          │ ent │ TD(TD_COMMA_LESS) │ TD(TD_DOT_GRTR) │           │
//                                      └──────────────────┴────────────────┴──────────────────┴──────────────┴─────────────┘       └──────────┴─────┴───────────────────┴─────────────────┴───────────┘
[BASE] = LAYOUT(
  KC_ESC  , KC_1         , KC_2         , KC_3             , KC_4           , KC_5             ,                     KC_TRNS     ,         KC_TRNS  ,            KC_6              , KC_7            , KC_8         , KC_9         , KC_0            , KC_BSPC         ,
  KC_TAB  , KC_Q         , KC_W         , KC_E             , KC_R           , KC_T             ,                     KC_TRNS     ,         KC_TRNS  ,            KC_Y              , KC_U            , KC_I         , KC_O         , KC_P            , SE_ARNG         ,
  MO(NAV) , LGUI_T(KC_A) , LALT_T(KC_S) , LCTL_T(KC_D)     , LSFT_T(KC_F)   , KC_G             ,                     KC_TRNS     ,         KC_TRNS  ,            KC_H              , RGUI_T(KC_J)    , RALT_T(KC_K) , RCTL_T(KC_L) , RSFT_T(SE_ODIA) , LT(NAV, SE_ADIA),
  KC_TRNS , KC_Z         , KC_X         , KC_C             , KC_V           , KC_B             , KC_TRNS           , TG(SETTING) ,         TG(GAME) , KC_TRNS  , KC_N              , KC_M            , KC_COMMA     , KC_DOT       , KC_TRNS         , KC_TRNS         ,
                                          TD(TD_LCBR_RCBR) , TD(TD_LPR_RPR) , TD(TD_LBRC_RBRC) , LT(SYM, KC_SPACE) , KC_TRNS     ,         KC_TRNS  , KC_ENTER , TD(TD_COMMA_LESS) , TD(TD_DOT_GRTR) , KC_TRNS
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┐     ┌─────┐       ┌─────┐     ┌─────┬─────┬─────┬─────────┬─────────┬─────┐
//    │     │     │     │     │     │     │     │     │       │     │     │     │     │     │         │         │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├─────┼─────┼─────┼─────────┼─────────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │  +  │  {  │  }  │    ´    │    \    │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├─────┼─────┼─────┼─────────┼─────────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │  ~  │  (  │  )  │    ^    │ SE_LABK │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────────┼─────────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │  -  │  [  │  ]  │ SE_QUOT │ SE_PIPE │     │
//    └─────┴─────┴─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────────┴─────────┴─────┘
//                      │     │     │     │     │     │       │     │     │     │     │     │
//                      └─────┴─────┴─────┴─────┴─────┘       └─────┴─────┴─────┴─────┴─────┘
[SYM] = LAYOUT(
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           SE_PLUS , SE_LCBR , SE_RCBR , SE_ACUT , SE_BSLS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           SE_TILD , SE_LPRN , SE_RPRN , SE_CIRC , SE_LABK , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , SE_MINS , SE_LBRC , SE_RBRC , SE_QUOT , SE_PIPE , KC_TRNS,
                                KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┐     ┌─────┐       ┌─────┐     ┌──────┬──────┬─────────┬──────┬─────┬─────┐
//    │ f1  │ f2  │ f3  │ f4  │ f5  │ f6  │     │     │       │     │     │  f7  │  f8  │   f9    │ f10  │ f11 │ f12 │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├──────┼──────┼─────────┼──────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │ home │ pgdn │ pAGE_UP │ end  │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├──────┼──────┼─────────┼──────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │ left │ down │   up    │ rght │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼──────┼──────┼─────────┼──────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │      │      │         │      │     │     │
//    └─────┴─────┴─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼──────┼──────┼─────────┼──────┴─────┴─────┘
//                      │     │     │     │     │     │       │     │     │      │      │         │
//                      └─────┴─────┴─────┴─────┴─────┘       └─────┴─────┴──────┴──────┴─────────┘
[NAV] = LAYOUT(
  KC_F1   , KC_F2   , KC_F3   , KC_F4   , KC_F5   , KC_F6   ,           KC_TRNS ,         KC_TRNS ,           KC_F7   , KC_F8   , KC_F9      , KC_F10   , KC_F11  , KC_F12 ,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           KC_HOME , KC_PGDN , KC_PAGE_UP , KC_END   , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           KC_LEFT , KC_DOWN , KC_UP      , KC_RIGHT , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS    , KC_TRNS  , KC_TRNS , KC_TRNS,
                                KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┐     ┌─────┐       ┌─────┐     ┌─────┬─────┬─────┬─────┬─────┬─────┐
//    │     │     │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │  a  │  s  │  d  │  f  │     │     │     │       │     │     │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    └─────┴─────┴─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────┴─────┴─────┘
//                      │     │     │     │ spc │     │       │     │     │     │     │     │
//                      └─────┴─────┴─────┴─────┴─────┘       └─────┴─────┴─────┴─────┴─────┘
[GAME] = LAYOUT(
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,            KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,            KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_A    , KC_S    , KC_D    , KC_F    , KC_TRNS ,            KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS  , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
                                KC_TRNS , KC_TRNS , KC_TRNS , KC_SPACE , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS
),

//    ┌─────────────────────────────┬─────────────────────────┬─────┬─────┬─────┬─────┐     ┌─────┐       ┌─────┐     ┌─────┬─────┬─────┬─────┬─────┬─────┐
//    │           QK_BOOT           │                         │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    ├─────────────────────────────┼─────────────────────────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │ QK_RGB_MATRIX_MODE_PREVIOUS │ QK_RGB_MATRIX_MODE_NEXT │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    ├─────────────────────────────┼─────────────────────────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │  QK_RGB_MATRIX_VALUE_DOWN   │ QK_RGB_MATRIX_VALUE_UP  │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    ├─────────────────────────────┼─────────────────────────┼─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
//    │  QK_RGB_MATRIX_SPEED_DOWN   │ QK_RGB_MATRIX_SPEED_UP  │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    └─────────────────────────────┴─────────────────────────┴─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────┴─────┴─────┘
//                                                                  │     │     │     │     │     │       │     │     │     │     │     │
//                                                                  └─────┴─────┴─────┴─────┴─────┘       └─────┴─────┴─────┴─────┴─────┘
[SETTING] = LAYOUT(
  QK_BOOT                     , KC_TRNS                 , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  QK_RGB_MATRIX_MODE_PREVIOUS , QK_RGB_MATRIX_MODE_NEXT , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  QK_RGB_MATRIX_VALUE_DOWN    , QK_RGB_MATRIX_VALUE_UP  , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  QK_RGB_MATRIX_SPEED_DOWN    , QK_RGB_MATRIX_SPEED_UP  , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
                                                                    KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS
)
};

// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        num_keypresses = num_keypresses + 1;
        if (num_keypresses % GROW_RATE == 0) {
            if (current_frame < (ANIM_FRAMES - 1)) {
                current_frame = current_frame + 1;
            } else {
                current_frame = 0;
            }
        }
    }

    return true;
};

void oled_render_layer_state(void) {
    switch (get_highest_layer(layer_state)) {
        case BASE:
            oled_write_raw_P(base_logo, sizeof(base_logo));
            break;
        case SYM:
            oled_write_raw_P(sym_logo, sizeof(sym_logo));
            break;
        case NAV:
            oled_write_raw_P(sym_logo, sizeof(sym_logo));
            break;
        case GAME:
            oled_write_raw_P(game_logo, sizeof(game_logo));
            break;
        case SETTING:
            oled_clear();
            oled_write_P(PSTR("MM: "), false);
            oled_write(get_u8_str(rgb_matrix_get_mode(), ' '), false);
            oled_write_P(PSTR("\n"), false);

            oled_write_P(PSTR("MV: "), false);
            oled_write(get_u8_str(rgb_matrix_get_val(), ' '), false);
            oled_write_P(PSTR("\n"), false);

            oled_write_P(PSTR("MS: "), false);
            oled_write(get_u8_str(rgb_matrix_get_speed(), ' '), false);
            oled_write_P(PSTR("\n"), false);

            break;
    }
}

void oled_render_animation_phase(void) {
    oled_write_raw_P(plant_anim[current_frame],
                     sizeof(plant_anim[current_frame]));
}

bool oled_task_user(void) {
    bool is_oled_enabled = last_input_activity_elapsed() < 60000;

    if (!is_oled_enabled) {
        oled_off();

        return false;
    }

    if (!is_oled_on()) {
        oled_on();
    }

    if (is_keyboard_master()) {
        oled_render_animation_phase();
    } else {
        oled_render_layer_state();
    }

    return false;
}

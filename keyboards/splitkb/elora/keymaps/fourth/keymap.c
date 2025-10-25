#include QMK_KEYBOARD_H
#include "frames.h"
#include "i18n.h"

static int num_keypresses = 0;
static int current_frame  = 0;

enum layers { BASE = 0, SYM, NAV, GAME, SETTING };

//    ┌─────┬─────┬─────┬─────┬─────┬─────┐     ┌─────┐       ┌─────┐     ┌─────┬─────┬─────┬─────┬─────┬─────┐
//    │     │     │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    └─────┴─────┴─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────┴─────┴─────┘
//                      │     │     │     │     │     │       │     │     │     │     │     │
//                      └─────┴─────┴─────┴─────┴─────┘       └─────┴─────┴─────┴─────┴─────┘
// [TEMPLATE] = LAYOUT(
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
//                                 KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS
// )

// clang-format off
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = LAYOUT(
  'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'L' ,       'R' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R',
  'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'L' ,       'R' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R',
  'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'L' ,       'R' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R',
  'L' , 'L' , 'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R' , 'R' , 'R',
                    'L' , 'L' , 'L' , 'L' , 'L' ,       'R' , 'R' , 'R' , 'R' , 'R'
);

#define _NAV    MO(NAV)
#define _GAME   TG(GAME)
#define _SET    TG(SETTING)

#define LGUI_A  MT(MOD_LGUI, KC_A)
#define LALT_S  MT(MOD_LALT, KC_S)
#define LCTL_D  MT(MOD_LCTL, KC_D)
#define LSFT_F  MT(MOD_LSFT, KC_F)

#define SYM_SPC LT(SYM, KC_SPACE)

#define RGUI_J  MT(MOD_RGUI, KC_J)
#define RALT_K  MT(MOD_RALT, KC_K)
#define RCTL_L  MT(MOD_RCTL, KC_L)
#define RSFT_O  MT(MOD_RSFT, SE_OSLH)
#define NAV_A   LT(NAV, SE_ADIA)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
//    ┌──────┬────────┬────────┬────────┬────────┬─────┐         ┌──────┐       ┌───────┐     ┌─────┬────────┬────────┬────────┬────────┬───────┐
//    │ esc  │   1    │   2    │   3    │   4    │  5  │         │      │       │       │     │  6  │   7    │   8    │   9    │   0    │ bspc  │
//    ├──────┼────────┼────────┼────────┼────────┼─────┤         ├──────┤       ├───────┤     ├─────┼────────┼────────┼────────┼────────┼───────┤
//    │ tab  │   q    │   w    │   e    │   r    │  t  │         │      │       │       │     │  y  │   u    │   i    │   o    │   p    │   å   │
//    ├──────┼────────┼────────┼────────┼────────┼─────┤         ├──────┤       ├───────┤     ├─────┼────────┼────────┼────────┼────────┼───────┤
//    │ _NAV │ LGUI_A │ LALT_S │ LCTL_D │ LSFT_F │  g  │         │      │       │       │     │  h  │ RGUI_J │ RALT_K │ RCTL_L │ RSFT_O │ NAV_A │
//    ├──────┼────────┼────────┼────────┼────────┼─────┼─────────┼──────┤       ├───────┼─────┼─────┼────────┼────────┼────────┼────────┼───────┤
//    │      │   z    │   x    │   c    │   v    │  b  │         │ _SET │       │ _GAME │     │  n  │   m    │   ,    │   .    │        │       │
//    └──────┴────────┴────────┼────────┼────────┼─────┼─────────┼──────┤       ├───────┼─────┼─────┼────────┼────────┼────────┴────────┴───────┘
//                             │        │        │     │ SYM_SPC │      │       │       │ ent │     │        │        │
//                             └────────┴────────┴─────┴─────────┴──────┘       └───────┴─────┴─────┴────────┴────────┘
[BASE] = LAYOUT(
  KC_ESC  , KC_1   , KC_2   , KC_3    , KC_4    , KC_5    ,           KC_TRNS ,         KC_TRNS ,            KC_6    , KC_7    , KC_8     , KC_9   , KC_0    , KC_BSPC,
  KC_TAB  , KC_Q   , KC_W   , KC_E    , KC_R    , KC_T    ,           KC_TRNS ,         KC_TRNS ,            KC_Y    , KC_U    , KC_I     , KC_O   , KC_P    , SE_AA  ,
  _NAV    , LGUI_A , LALT_S , LCTL_D  , LSFT_F  , KC_G    ,           KC_TRNS ,         KC_TRNS ,            KC_H    , RGUI_J  , RALT_K   , RCTL_L , RSFT_O  , NAV_A  ,
  KC_TRNS , KC_Z   , KC_X   , KC_C    , KC_V    , KC_B    , KC_TRNS , _SET    ,         _GAME   , KC_TRNS  , KC_N    , KC_M    , KC_COMMA , KC_DOT , KC_TRNS , KC_TRNS,
                              KC_TRNS , KC_TRNS , KC_TRNS , SYM_SPC , KC_TRNS ,         KC_TRNS , KC_ENTER , KC_TRNS , KC_TRNS , KC_TRNS
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┐     ┌─────┐       ┌─────┐     ┌─────┬─────┬─────┬─────┬─────┬─────┐
//    │     │     │     │     │     │     │     │     │       │     │     │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │  +  │  {  │  }  │  ´  │  \  │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┤       ├─────┤     ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │  ~  │  (  │  )  │  ^  │  <  │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │       │     │     │  -  │  [  │  ]  │  '  │     │     │
//    └─────┴─────┴─────┼─────┼─────┼─────┼─────┼─────┤       ├─────┼─────┼─────┼─────┼─────┼─────┴─────┴─────┘
//                      │     │     │     │     │     │       │     │     │     │     │     │
//                      └─────┴─────┴─────┴─────┴─────┘       └─────┴─────┴─────┴─────┴─────┘
[SYM] = LAYOUT(
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS     , KC_TRNS     , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           SE_PLUS , SE_LCBR_MAC , SE_RCBR_MAC , SE_ACUT , SE_BSLS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           SE_TILD , SE_LPRN     , SE_RPRN     , SE_CIRC , SE_LESS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , SE_MINS , SE_LBRC     , SE_RBRC     , SE_APOS , KC_TRNS , KC_TRNS,
                                KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS     , KC_TRNS
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

  // QK_RGB_MATRIX_VALUE_UP   , QK_RGB_MATRIX_SPEED_UP   , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                                 KC_LEFT , KC_DOWN , KC_UP      , KC_RIGHT , KC_TRNS , KC_TRNS,
  // QK_RGB_MATRIX_VALUE_DOWN , QK_RGB_MATRIX_SPEED_DOWN , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_HOME , KC_PGDN , KC_PAGE_UP , KC_END   , KC_TRNS , KC_TRNS,

// [BASE_OTHER] = LAYOUT(
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS                 , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
//                                 KC_TRNS , KC_TRNS , KC_TRNS , LT(SYM_OTHER, KC_SPACE) , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS
// ),
// [SYM_OTHER] = LAYOUT(
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           SE_PLUS , SE_LCBR , SE_RCBR , SE_ACUT , SE_BSLS , KC_TRNS,
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,           KC_TRNS ,         KC_TRNS ,           SE_TILD , SE_LPRN , SE_RPRN , SE_CIRC , SE_LESS , KC_TRNS,
//   KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , SE_MINS , SE_LBRC , SE_RBRC , SE_APOS , KC_TRNS , KC_TRNS,
//                                 KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,         KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS
// ),
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
    oled_write_raw_P(plant_anim[current_frame], sizeof(plant_anim[current_frame]));
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

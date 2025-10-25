#include QMK_KEYBOARD_H
#include "i18n.h"

enum layers { BASE = 0, SYM, NAV, GAME, SETTING };

// Tap Dance declarations
enum { TD_LCBR_RCBR, TD_LPR_RPR, TD_LBRC_RBRC, TD_COMMA_LESS, TD_DOT_GRTR };

// clang-format off

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
    [TD_LCBR_RCBR]  = ACTION_TAP_DANCE_DOUBLE(SE_LCBR_MAC, SE_RCBR_MAC),
    [TD_LPR_RPR]    = ACTION_TAP_DANCE_DOUBLE(SE_LPRN, SE_RPRN),
    [TD_LBRC_RBRC]  = ACTION_TAP_DANCE_DOUBLE(SE_LBRC, SE_RBRC),
    [TD_COMMA_LESS] = ACTION_TAP_DANCE_DOUBLE(KC_COMMA, SE_LESS),
    [TD_DOT_GRTR]   = ACTION_TAP_DANCE_DOUBLE(KC_DOT, SE_GRTR),
};

// clang-format off
// const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = LAYOUT(
//   'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'L' ,       'R' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R',
//   'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'L' ,       'R' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R',
//   'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'L' ,       'R' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R',
//   'L' , 'L' , 'L' , 'L' , 'L' , 'L' , 'L' , 'L' ,       'R' , 'R' , 'R' , 'R' , 'R' , 'R' , 'R' , 'R',
//                     'L' , 'L' , 'L' , 'L' , 'L' ,       'R' , 'R' , 'R' , 'R' , 'R'
// );

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
//    ┌──────┬────────┬────────┬──────────────────┬────────────────┬──────────────────┐                                  ┌───────────────────┬─────────────────┬────────┬────────┬────────┬───────┐
//    │ esc  │   1    │   2    │        3         │       4        │        5         │                                  │         6         │        7        │   8    │   9    │   0    │ bspc  │
//    ├──────┼────────┼────────┼──────────────────┼────────────────┼──────────────────┤                                  ├───────────────────┼─────────────────┼────────┼────────┼────────┼───────┤
//    │ tab  │   q    │   w    │        e         │       r        │        t         │                                  │         y         │        u        │   i    │   o    │   p    │   å   │
//    ├──────┼────────┼────────┼──────────────────┼────────────────┼──────────────────┤                                  ├───────────────────┼─────────────────┼────────┼────────┼────────┼───────┤
//    │ _NAV │ LGUI_A │ LALT_S │      LCTL_D      │     LSFT_F     │        g         │                                  │         h         │     RGUI_J      │ RALT_K │ RCTL_L │ RSFT_O │ NAV_A │
//    ├──────┼────────┼────────┼──────────────────┼────────────────┼──────────────────┼─────────┬──────┐   ┌───────┬─────┼───────────────────┼─────────────────┼────────┼────────┼────────┼───────┤
//    │      │   z    │   x    │        c         │       v        │        b         │         │ _SET │   │ _GAME │     │         n         │        m        │   ,    │   .    │        │       │
//    └──────┴────────┴────────┼──────────────────┼────────────────┼──────────────────┼─────────┼──────┤   ├───────┼─────┼───────────────────┼─────────────────┼────────┼────────┴────────┴───────┘
//                             │ TD(TD_LCBR_RCBR) │ TD(TD_LPR_RPR) │ TD(TD_LBRC_RBRC) │ SYM_SPC │      │   │       │ ent │ TD(TD_COMMA_LESS) │ TD(TD_DOT_GRTR) │        │
//    ┌──────┬────────┬────────┼──────────────────┼────────────────┼──────────────────┴─────────┴──────┘   └───────┴─────┴───────────────────┼─────────────────┼────────┼────────┬────────┬───────┐
//    │  no  │   no   │   no   │        no        │       no       │                                                                         │       no        │   no   │   no   │   no   │  no   │
//    └──────┴────────┴────────┴──────────────────┴────────────────┘                                                                         └─────────────────┴────────┴────────┴────────┴───────┘
[BASE] = LAYOUT_elora_hlc(
  KC_ESC  , KC_1   , KC_2   , KC_3             , KC_4           , KC_5             ,                                              KC_6              , KC_7            , KC_8     , KC_9   , KC_0    , KC_BSPC,
  KC_TAB  , KC_Q   , KC_W   , KC_E             , KC_R           , KC_T             ,                                              KC_Y              , KC_U            , KC_I     , KC_O   , KC_P    , SE_AA  ,
  _NAV    , LGUI_A , LALT_S , LCTL_D           , LSFT_F         , KC_G             ,                                              KC_H              , RGUI_J          , RALT_K   , RCTL_L , RSFT_O  , NAV_A  ,
  KC_TRNS , KC_Z   , KC_X   , KC_C             , KC_V           , KC_B             , KC_TRNS , _SET    ,     _GAME   , KC_TRNS  , KC_N              , KC_M            , KC_COMMA , KC_DOT , KC_TRNS , KC_TRNS,
                              TD(TD_LCBR_RCBR) , TD(TD_LPR_RPR) , TD(TD_LBRC_RBRC) , SYM_SPC , KC_TRNS ,     KC_TRNS , KC_ENTER , TD(TD_COMMA_LESS) , TD(TD_DOT_GRTR) , KC_TRNS                              ,
  KC_NO   , KC_NO  , KC_NO  , KC_NO            , KC_NO          ,                                                                                     KC_NO           , KC_NO    , KC_NO  , KC_NO   , KC_NO
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┐                           ┌─────┬─────┬─────┬─────┬─────┬─────┐
//    │     │     │     │     │     │     │                           │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤                           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │                           │  +  │  {  │  }  │  ´  │  \  │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤                           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │                           │  ~  │  (  │  )  │  ^  │  <  │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┬─────┐   ┌─────┬─────┼─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │   │     │     │  -  │  [  │  ]  │  '  │     │     │
//    └─────┴─────┴─────┼─────┼─────┼─────┼─────┼─────┤   ├─────┼─────┼─────┼─────┼─────┼─────┴─────┴─────┘
//                      │     │     │     │     │     │   │     │     │     │     │     │
//    ┌─────┬─────┬─────┼─────┼─────┼─────┴─────┴─────┘   └─────┴─────┴─────┼─────┼─────┼─────┬─────┬─────┐
//    │ no  │ no  │ no  │ no  │ no  │                                       │ no  │ no  │ no  │ no  │ no  │
//    └─────┴─────┴─────┴─────┴─────┘                                       └─────┴─────┴─────┴─────┴─────┘
[SYM] = LAYOUT_elora_hlc(
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                             KC_TRNS , KC_TRNS     , KC_TRNS     , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                             SE_PLUS , SE_LCBR_MAC , SE_RCBR_MAC , SE_ACUT , SE_BSLS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                             SE_TILD , SE_LPRN     , SE_RPRN     , SE_CIRC , SE_LESS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,     KC_TRNS , KC_TRNS , SE_MINS , SE_LBRC     , SE_RBRC     , SE_APOS , KC_TRNS , KC_TRNS,
                                KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,     KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS     , KC_TRNS                                  ,
  KC_NO   , KC_NO   , KC_NO   , KC_NO   , KC_NO   ,                                                                 KC_NO       , KC_NO       , KC_NO   , KC_NO   , KC_NO
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┐                           ┌──────┬──────┬─────────┬──────┬─────┬─────┐
//    │ f1  │ f2  │ f3  │ f4  │ f5  │ f6  │                           │  f7  │  f8  │   f9    │ f10  │ f11 │ f12 │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤                           ├──────┼──────┼─────────┼──────┼─────┼─────┤
//    │     │     │     │     │     │     │                           │ home │ pgdn │ pAGE_UP │ end  │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤                           ├──────┼──────┼─────────┼──────┼─────┼─────┤
//    │     │     │     │     │     │     │                           │ left │ down │   up    │ rght │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┬─────┐   ┌─────┬─────┼──────┼──────┼─────────┼──────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │   │     │     │      │      │         │      │     │     │
//    └─────┴─────┴─────┼─────┼─────┼─────┼─────┼─────┤   ├─────┼─────┼──────┼──────┼─────────┼──────┴─────┴─────┘
//                      │     │     │     │     │     │   │     │     │      │      │         │
//    ┌─────┬─────┬─────┼─────┼─────┼─────┴─────┴─────┘   └─────┴─────┴──────┼──────┼─────────┼──────┬─────┬─────┐
//    │ no  │ no  │ no  │ no  │ no  │                                        │  no  │   no    │  no  │ no  │ no  │
//    └─────┴─────┴─────┴─────┴─────┘                                        └──────┴─────────┴──────┴─────┴─────┘
[NAV] = LAYOUT_elora_hlc(
  KC_F1   , KC_F2   , KC_F3   , KC_F4   , KC_F5   , KC_F6   ,                                             KC_F7   , KC_F8   , KC_F9      , KC_F10   , KC_F11  , KC_F12 ,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                             KC_HOME , KC_PGDN , KC_PAGE_UP , KC_END   , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                             KC_LEFT , KC_DOWN , KC_UP      , KC_RIGHT , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,     KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS    , KC_TRNS  , KC_TRNS , KC_TRNS,
                                KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,     KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS                                  ,
  KC_NO   , KC_NO   , KC_NO   , KC_NO   , KC_NO   ,                                                                 KC_NO   , KC_NO      , KC_NO    , KC_NO   , KC_NO
),

//    ┌─────┬─────┬─────┬─────┬─────┬─────┐                           ┌─────┬─────┬─────┬─────┬─────┬─────┐
//    │     │     │     │     │     │     │                           │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤                           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │                           │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┤                           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │  a  │  s  │  d  │  f  │     │                           │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼─────┼─────┼─────┼─────┬─────┐   ┌─────┬─────┼─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │     │     │     │     │     │   │     │     │     │     │     │     │     │     │
//    └─────┴─────┴─────┼─────┼─────┼─────┼─────┼─────┤   ├─────┼─────┼─────┼─────┼─────┼─────┴─────┴─────┘
//                      │     │     │     │ spc │     │   │     │     │     │     │     │
//    ┌─────┬─────┬─────┼─────┼─────┼─────┴─────┴─────┘   └─────┴─────┴─────┼─────┼─────┼─────┬─────┬─────┐
//    │ no  │ no  │ no  │ no  │ no  │                                       │ no  │ no  │ no  │ no  │ no  │
//    └─────┴─────┴─────┴─────┴─────┘                                       └─────┴─────┴─────┴─────┴─────┘
[GAME] = LAYOUT_elora_hlc(
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                              KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                              KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_A    , KC_S    , KC_D    , KC_F    , KC_TRNS ,                                              KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS  , KC_TRNS ,     KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
                                KC_TRNS , KC_TRNS , KC_TRNS , KC_SPACE , KC_TRNS ,     KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS                              ,
  KC_NO   , KC_NO   , KC_NO   , KC_NO   , KC_NO   ,                                                                  KC_NO   , KC_NO   , KC_NO   , KC_NO   , KC_NO
),

//    ┌─────────────────────────────┬─────────────────────────┬─────┬─────┬─────┬─────┐                           ┌─────┬─────┬─────┬─────┬─────┬─────┐
//    │           QK_BOOT           │                         │     │     │     │     │                           │     │     │     │     │     │     │
//    ├─────────────────────────────┼─────────────────────────┼─────┼─────┼─────┼─────┤                           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │ QK_RGB_MATRIX_MODE_PREVIOUS │ QK_RGB_MATRIX_MODE_NEXT │     │     │     │     │                           │     │     │     │     │     │     │
//    ├─────────────────────────────┼─────────────────────────┼─────┼─────┼─────┼─────┤                           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │  QK_RGB_MATRIX_VALUE_DOWN   │ QK_RGB_MATRIX_VALUE_UP  │     │     │     │     │                           │     │     │     │     │     │     │
//    ├─────────────────────────────┼─────────────────────────┼─────┼─────┼─────┼─────┼─────┬─────┐   ┌─────┬─────┼─────┼─────┼─────┼─────┼─────┼─────┤
//    │  QK_RGB_MATRIX_SPEED_DOWN   │ QK_RGB_MATRIX_SPEED_UP  │     │     │     │     │     │     │   │     │     │     │     │     │     │     │     │
//    └─────────────────────────────┴─────────────────────────┴─────┼─────┼─────┼─────┼─────┼─────┤   ├─────┼─────┼─────┼─────┼─────┼─────┴─────┴─────┘
//                                                                  │     │     │     │     │     │   │     │     │     │     │     │
//    ┌─────────────────────────────┬─────────────────────────┬─────┼─────┼─────┼─────┴─────┴─────┘   └─────┴─────┴─────┼─────┼─────┼─────┬─────┬─────┐
//    │             no              │           no            │ no  │ no  │ no  │                                       │ no  │ no  │ no  │ no  │ no  │
//    └─────────────────────────────┴─────────────────────────┴─────┴─────┴─────┘                                       └─────┴─────┴─────┴─────┴─────┘
[SETTING] = LAYOUT_elora_hlc(
  QK_BOOT                     , KC_TRNS                 , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                             KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  QK_RGB_MATRIX_MODE_PREVIOUS , QK_RGB_MATRIX_MODE_NEXT , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                             KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  QK_RGB_MATRIX_VALUE_DOWN    , QK_RGB_MATRIX_VALUE_UP  , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                             KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  QK_RGB_MATRIX_SPEED_DOWN    , QK_RGB_MATRIX_SPEED_UP  , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,     KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
                                                                    KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,     KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS                              ,
  KC_NO                       , KC_NO                   , KC_NO   , KC_NO   , KC_NO   ,                                                                 KC_NO   , KC_NO   , KC_NO   , KC_NO   , KC_NO
)
};

// clang-format on

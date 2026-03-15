#include QMK_KEYBOARD_H
#include "i18n.h"

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

// There is an extra row added for the Halcyon modules. Currently only the Encoder module is
// supported but we reserve 5 keys per half for future expansion. Your personal keymap will also
// need to be updated to include this row, and the `LAYOUT` macro will need to be updated to
// `LAYOUT_elora_hlc` in order to compile.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
//    ┌─────────┬───────────┬───────────┬──────────────────┬────────────────┬──────────────────┐                                                 ┌───────────────────┬─────────────────┬───────────┬───────────┬───────────┬────────────┐
//    │   esc   │     1     │     2     │        3         │       4        │        5         │                                                 │         6         │        7        │     8     │     9     │     0     │    bspc    │
//    ├─────────┼───────────┼───────────┼──────────────────┼────────────────┼──────────────────┤                                                 ├───────────────────┼─────────────────┼───────────┼───────────┼───────────┼────────────┤
//    │   tab   │     q     │     w     │        e         │       r        │        t         │                                                 │         y         │        u        │     i     │     o     │     p     │     å      │
//    ├─────────┼───────────┼───────────┼──────────────────┼────────────────┼──────────────────┤                                                 ├───────────────────┼─────────────────┼───────────┼───────────┼───────────┼────────────┤
//    │ MO(NAV) │ LGUI_T(a) │ LALT_T(s) │    LCTL_T(d)     │   LSFT_T(f)    │        g         │                                                 │         h         │    RGUI_T(j)    │ RALT_T(k) │ RCTL_T(l) │ RSFT_T(ö) │ LT(NAV, ä) │
//    ├─────────┼───────────┼───────────┼──────────────────┼────────────────┼──────────────────┼──────────────┬─────────────┐   ┌──────────┬─────┼───────────────────┼─────────────────┼───────────┼───────────┼───────────┼────────────┤
//    │         │     z     │     x     │        c         │       v        │        b         │              │ TG(SETTING) │   │ TG(GAME) │     │         n         │        m        │     ,     │     .     │           │            │
//    └─────────┴───────────┴───────────┼──────────────────┼────────────────┼──────────────────┼──────────────┼─────────────┤   ├──────────┼─────┼───────────────────┼─────────────────┼───────────┼───────────┴───────────┴────────────┘
//                                      │ TD(TD_LCBR_RCBR) │ TD(TD_LPR_RPR) │ TD(TD_LBRC_RBRC) │ LT(SYM, spc) │             │   │          │ ent │ TD(TD_COMMA_LESS) │ TD(TD_DOT_GRTR) │           │
//    ┌─────────┬───────────┬───────────┼──────────────────┼────────────────┼──────────────────┴──────────────┴─────────────┘   └──────────┴─────┴───────────────────┼─────────────────┼───────────┼───────────┬───────────┬────────────┐
//    │   no    │    no     │    no     │        no        │       no       │                                                                                        │       no        │    no     │    no     │    no     │     no     │
//    └─────────┴───────────┴───────────┴──────────────────┴────────────────┘                                                                                        └─────────────────┴───────────┴───────────┴───────────┴────────────┘
[BASE] = LAYOUT_elora_hlc(
  KC_ESC  , KC_1         , KC_2         , KC_3             , KC_4           , KC_5             ,                                                             KC_6              , KC_7            , KC_8         , KC_9         , KC_0            , KC_BSPC         ,
  KC_TAB  , KC_Q         , KC_W         , KC_E             , KC_R           , KC_T             ,                                                             KC_Y              , KC_U            , KC_I         , KC_O         , KC_P            , SE_AA           ,
  MO(NAV) , LGUI_T(KC_A) , LALT_T(KC_S) , LCTL_T(KC_D)     , LSFT_T(KC_F)   , KC_G             ,                                                             KC_H              , RGUI_T(KC_J)    , RALT_T(KC_K) , RCTL_T(KC_L) , RSFT_T(SE_OSLH) , LT(NAV, SE_ADIA),
  KC_TRNS , KC_Z         , KC_X         , KC_C             , KC_V           , KC_B             , KC_TRNS           , TG(SETTING) ,     TG(GAME) , KC_TRNS  , KC_N              , KC_M            , KC_COMMA     , KC_DOT       , KC_TRNS         , KC_TRNS         ,
                                          TD(TD_LCBR_RCBR) , TD(TD_LPR_RPR) , TD(TD_LBRC_RBRC) , LT(SYM, KC_SPACE) , KC_TRNS     ,     KC_TRNS  , KC_ENTER , TD(TD_COMMA_LESS) , TD(TD_DOT_GRTR) , KC_TRNS                                                         ,
  KC_NO   , KC_NO        , KC_NO        , KC_NO            , KC_NO          ,                                                                                                    KC_NO           , KC_NO        , KC_NO        , KC_NO           , KC_NO
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

//    ┌─────┬─────┬─────┬──────┬──────┬──────┐                           ┌─────┬─────┬─────┬─────┬─────┬─────┐
//    │     │     │     │      │      │      │                           │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼──────┼──────┼──────┤                           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │      │      │      │                           │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼──────┼──────┼──────┤                           ├─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │  a  │  s  │  d   │  f   │      │                           │     │     │     │     │     │     │
//    ├─────┼─────┼─────┼──────┼──────┼──────┼─────┬─────┐   ┌─────┬─────┼─────┼─────┼─────┼─────┼─────┼─────┤
//    │     │     │     │      │      │      │     │     │   │     │     │     │     │     │     │     │     │
//    └─────┴─────┴─────┼──────┼──────┼──────┼─────┼─────┤   ├─────┼─────┼─────┼─────┼─────┼─────┴─────┴─────┘
//                      │ lalt │ lctl │ lsft │ spc │     │   │     │     │     │     │     │
//    ┌─────┬─────┬─────┼──────┼──────┼──────┴─────┴─────┘   └─────┴─────┴─────┼─────┼─────┼─────┬─────┬─────┐
//    │ no  │ no  │ no  │  no  │  no  │                                        │ no  │ no  │ no  │ no  │ no  │
//    └─────┴─────┴─────┴──────┴──────┘                                        └─────┴─────┴─────┴─────┴─────┘
[GAME] = LAYOUT_elora_hlc(
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                              KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS ,                                              KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_A    , KC_S    , KC_D    , KC_F    , KC_TRNS ,                                              KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
  KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS  , KC_TRNS ,     KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS,
                                KC_LALT , KC_LCTL , KC_LSFT , KC_SPACE , KC_TRNS ,     KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS , KC_TRNS                              ,
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

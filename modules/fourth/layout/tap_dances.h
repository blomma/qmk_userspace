#pragma once

// Include this from keymap.c only. It defines the tap dance table, which QMK
// needs in keymap.c to count the tap dances from its size.

#include "keymap_swedish.h"
#include "cross_platform.h"

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
    [TD_LCBR_RCBR]  = ACTION_TAP_DANCE_XP_DOUBLE(XP_LCBR, XP_RCBR),
    [TD_LPR_RPR]    = ACTION_TAP_DANCE_XP_DOUBLE(SE_LPRN, SE_RPRN),
    [TD_LBRC_RBRC]  = ACTION_TAP_DANCE_XP_DOUBLE(SE_LBRC, SE_RBRC),
    [TD_COMMA_LESS] = ACTION_TAP_DANCE_XP_DOUBLE(SE_COMM, SE_LABK),
    [TD_DOT_GRTR]   = ACTION_TAP_DANCE_XP_DOUBLE(SE_DOT, SE_RABK),
};

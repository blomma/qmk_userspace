# `fourth/layout`

The parts of the `fourth` layout that don't depend on a keyboard's `LAYOUT`
macro. Each keymap still defines its own layers in `keymap.c`.

Enable it in the keymap's `keymap.json`, together with
`fourth/cross_platform`, which the tap dances need:

```json
{
    "modules": ["fourth/cross_platform", "fourth/layout"]
}
```

## Contents

- `layers.h`: the layer list (`BASE`, `SYM`, `NAV`, `GAME`, `SETTING`),
  `LAYER_COUNT` and `layer_name()`, which gives a name for a layer to show on
  a display.
- `tap_dances.h`: the `TD_` tap dances. Include it from `keymap.c` only: it
  defines the tap dance table, which QMK needs in `keymap.c` to count the tap
  dances.
- `config.h`: tapping and RGB timeout settings. QMK reads this before the
  keyboard's and the keymap's `config.h`, so a keymap can still override a
  setting with `#undef` and `#define`.

## Adding a layer

Add it to the enum in `layers.h` and give it a name in `layout.c`. Each
keymap's display code has a table of images per layer, checked against
`LAYER_COUNT`, so the build fails until every display has an entry for the
new layer.

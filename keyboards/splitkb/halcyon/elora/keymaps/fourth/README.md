# Halcyon Elora rev2 — `fourth` keymap

Swedish layout for a Halcyon Elora rev2 with TFT display modules, used on both
Windows and macOS.

## Building

```sh
qmk compile -kb splitkb/halcyon/elora/rev2 -km fourth -e HLC_TFT_DISPLAY=1
```

Flash by double-tapping the reset button (or pressing `QK_BOOT` on the
SETTING layer) and copying the `.uf2` file onto the `RPI-RP2` drive.

## Windows and macOS

A few Swedish symbols need different key combinations on Windows and macOS.
`XP_MODE` on the SETTING layer toggles between them, starting on macOS, and
the layer screen shows the current choice (`Win` or `Mac`). The
[`fourth/cross_platform` module](../../../../../../modules/fourth/cross_platform/README.md) has the
details.

## macOS: set the keyboard type to ISO

The keymap assumes macOS treats the keyboard as ISO, otherwise `<` and `>`
come out as `§` and `°`. The
[`fourth/cross_platform` module](../../../../../../modules/fourth/cross_platform/README.md#macos-set-the-keyboard-type-to-iso)
explains why and how.

For the Halcyon Elora rev2 (vendor `0x8D1D`, product `0xA392`) the key is `41874-36125-0`.
Both halves use the same IDs, so one entry covers whichever half is plugged
in:

```sh
defaults read /Library/Preferences/com.apple.keyboardtype   # look for "41874-36125-0" = 41;
sudo defaults write /Library/Preferences/com.apple.keyboardtype keyboardtype -dict-add "41874-36125-0" -int 41
```

Unplug and replug the keyboard afterwards so macOS picks up the change.

## Shared code

This keymap and the other `fourth` keymap share code through the community
modules in `modules/fourth`, enabled in `keymap.json`:

- [`fourth/cross_platform`](../../../../../../modules/fourth/cross_platform/README.md): the `XP_`
  keycodes, the Windows/macOS toggle and `ACTION_TAP_DANCE_XP_DOUBLE`
- [`fourth/layout`](../../../../../../modules/fourth/layout/README.md): the layer list and names, the
  tap dances, and the tapping and RGB timeout settings

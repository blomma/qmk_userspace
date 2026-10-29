# Elora rev1 — `fourth` keymap

Swedish layout for an Elora rev1 with OLED displays, used on both Windows and
macOS. The layout is the same as the Halcyon Elora `fourth` keymap.

## Building

```sh
qmk compile -kb splitkb/elora/rev1 -km fourth
```

Flash by double-tapping the reset button (or pressing `QK_BOOT` on the
SETTING layer) and copying the `.uf2` file onto the `RPI-RP2` drive.

## Displays

The half plugged into USB shows a plant that grows as you type. The other half
shows the current layer and the host selected with `XP_MODE` (`Win` or `Mac`).
On the SETTING layer it also shows the RGB mode (`MM`), brightness (`MV`) and
speed (`MS`).

Pressing `QK_BOOT` replaces the plant with `Boot`, which stays on the display
while the keyboard is in the bootloader.

## Windows and macOS

A few Swedish symbols need different key combinations on Windows and macOS.
`XP_MODE` on the SETTING layer toggles between them, starting on macOS, and
the layer screen shows the current choice (`Win` or `Mac`). The
[`fourth/cross_platform` module](../../../../../modules/fourth/cross_platform/README.md) has the
details.

## macOS: set the keyboard type to ISO

The keymap assumes macOS treats the keyboard as ISO, otherwise `<` and `>`
come out as `§` and `°`. The
[`fourth/cross_platform` module](../../../../../modules/fourth/cross_platform/README.md#macos-set-the-keyboard-type-to-iso)
explains why and how.

For the Elora rev1 (vendor `0x8D1D`, product `0x9D9D`) the key is `40349-36125-0`.
Both halves use the same IDs, so one entry covers whichever half is plugged
in:

```sh
defaults read /Library/Preferences/com.apple.keyboardtype   # look for "40349-36125-0" = 41;
sudo defaults write /Library/Preferences/com.apple.keyboardtype keyboardtype -dict-add "40349-36125-0" -int 41
```

Unplug and replug the keyboard afterwards so macOS picks up the change.

## Shared code

This keymap and the other `fourth` keymap share code through the community
modules in `modules/fourth`, enabled in `keymap.json`:

- [`fourth/cross_platform`](../../../../../modules/fourth/cross_platform/README.md): the `XP_`
  keycodes, the Windows/macOS toggle and `ACTION_TAP_DANCE_XP_DOUBLE`
- [`fourth/layout`](../../../../../modules/fourth/layout/README.md): the layer list and names, the
  tap dances, and the tapping and RGB timeout settings

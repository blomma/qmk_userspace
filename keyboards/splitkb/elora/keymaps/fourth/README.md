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
They use the `XP_` keycodes from `cross_platform.h`:

| Symbol | Keycode   | Windows | macOS        |
|--------|-----------|---------|--------------|
| `{`    | `XP_LCBR` | AltGr+7 | Shift+Opt+8  |
| `}`    | `XP_RCBR` | AltGr+0 | Shift+Opt+9  |
| `\`    | `XP_BSLS` | AltGr++ | Shift+Opt+7  |
| `\|`   | `XP_PIPE` | AltGr+< | Opt+7        |

Every other symbol uses the same combination on both and comes straight from
QMK's `keymap_swedish.h`.

`XP_MODE` on the SETTING layer toggles between Windows and macOS, starting on
macOS. The mode is saved in EEPROM, separately on each half, so if you
sometimes plug in the other half, set the mode there once too.

## macOS: set the keyboard type to ISO

macOS keeps a keyboard type (ANSI, ISO or JIS) for each keyboard. If it
treats this keyboard as ANSI, it swaps the `§` key and the `<` key, so `<` and
`>` come out as `§` and `°`. The keymap assumes ISO.

macOS stores the type in `/Library/Preferences/com.apple.keyboardtype`, keyed
by `<product ID>-<vendor ID>-0` in decimal:

| Value | Type |
|-------|------|
| 40    | ANSI |
| 41    | ISO  |
| 42    | JIS  |

For this keyboard (vendor `0x8D1D`, product `0x9D9D`) the key is
`40349-36125-0`. Both halves use the same IDs, so one entry covers whichever
half is plugged in.

### Check the current type

```sh
defaults read /Library/Preferences/com.apple.keyboardtype
```

Look for `"40349-36125-0" = 41;`.

### Set it to ISO

```sh
sudo defaults write /Library/Preferences/com.apple.keyboardtype keyboardtype -dict-add "40349-36125-0" -int 41
```

Unplug and replug the keyboard afterwards so macOS picks up the change.

You can also use System Settings → Keyboard → Change Keyboard Type…, but
unplug any other keyboards first. Otherwise macOS may save the type for the
wrong keyboard, especially other splitkb boards, which share the same vendor
ID.

### Other keyboards

To find the IDs of another keyboard:

```sh
hidutil list | grep -i elora
```

The first two columns are the vendor and product IDs in hex. Convert them to
decimal for the key:

```sh
printf "%d-%d-0\n" 0x9D9D 0x8D1D   # product first, then vendor
```

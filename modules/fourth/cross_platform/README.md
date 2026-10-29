# `fourth/cross_platform`

Swedish symbols for a keyboard used on both Windows and macOS.

Enable it in the keymap's `keymap.json`:

```json
{
    "modules": ["fourth/cross_platform"]
}
```

## Symbols

A few Swedish symbols need different key combinations on Windows and macOS.
The module adds an `XP_` keycode for each one, which sends the combination for
the selected host:

| Symbol | Keycode   | Windows | macOS        |
|--------|-----------|---------|--------------|
| `{`    | `XP_LCBR` | AltGr+7 | Shift+Opt+8  |
| `}`    | `XP_RCBR` | AltGr+0 | Shift+Opt+9  |
| `\`    | `XP_BSLS` | AltGr++ | Shift+Opt+7  |
| `\|`   | `XP_PIPE` | AltGr+< | Opt+7        |

Every other symbol uses the same combination on both, so keymaps use the
`SE_` keycodes from QMK's `keymap_swedish.h` for those.

`ACTION_TAP_DANCE_XP_DOUBLE(kc1, kc2)` works like QMK's
`ACTION_TAP_DANCE_DOUBLE`, but either keycode may be an `XP_` symbol.

## Selecting the host

`XP_MODE` toggles between Windows and macOS. The default is macOS.

The selection is saved in EEPROM, separately on each half of a split
keyboard, so if you sometimes plug in the other half, select the host there
once too. The half plugged into USB sends its selection to the other half, so
both halves agree while the keyboard is in use.

From C, `xp_host()` returns the selected host and `xp_host_name()` gives a
short name for it (`Mac` or `Win`) to show on a display.

## macOS: set the keyboard type to ISO

macOS keeps a keyboard type (ANSI, ISO or JIS) for each keyboard. If it
treats a keyboard as ANSI, it swaps the `§` key and the `<` key, so `<` and
`>` come out as `§` and `°`. The macOS combinations above assume ISO.

macOS stores the type in `/Library/Preferences/com.apple.keyboardtype`, keyed
by `<product ID>-<vendor ID>-0` in decimal:

| Value | Type |
|-------|------|
| 40    | ANSI |
| 41    | ISO  |
| 42    | JIS  |

Each keymap's README gives the key for its keyboard. To find it for another
keyboard:

```sh
hidutil list | grep -i elora
```

The first two columns are the vendor and product IDs in hex. Convert them to
decimal for the key, product first:

```sh
printf "%d-%d-0\n" <product ID> <vendor ID>
```

Check the current type, then set it to ISO:

```sh
defaults read /Library/Preferences/com.apple.keyboardtype
sudo defaults write /Library/Preferences/com.apple.keyboardtype keyboardtype -dict-add "<key>" -int 41
```

Unplug and replug the keyboard afterwards so macOS picks up the change.

You can also use System Settings → Keyboard → Change Keyboard Type…, but
unplug any other keyboards first. Otherwise macOS may save the type for the
wrong keyboard, especially other splitkb boards, which share the same vendor
ID.

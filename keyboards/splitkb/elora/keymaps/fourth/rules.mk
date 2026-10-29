SRC += display.c

# The keyboard enables a pointing device for a Myriad joystick card, and its
# driver looks for a Myriad card at startup. There is no Myriad card, so drop
# the pointing device along with the card check and the per scan sync of
# pointer data between the halves.
POINTING_DEVICE_ENABLE = no

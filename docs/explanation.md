# Detailed Explanation

## Microcontroller Setup

- The PIC16F887A is clocked by a 20 MHz crystal oscillator.
- ADC reads the joystick's X and Y axis.
- PWM outputs are generated using CCP1/CCP2 modules.

## Motor Control Logic

- If joystick is pushed forward/backward (Y axis), both motors spin forward/backward.
- If joystick is pushed left/right (X axis), motors spin in opposite directions to rotate the system.

## Proteus Simulation

- The schematic contains:
  - Joystick connected to AN0 and AN1.
  - L298 driver controlling two DC motors.
  - 9V and 12V batteries powering logic and motor separately.
  - Visual indicators (LEDs) for debugging.


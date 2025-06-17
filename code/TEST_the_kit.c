
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 20000000 // 20MHz oscillator

void main() {
    TRISB = 0x00;  // Motor direction
    TRISC = 0x00;  // PWM outputs
    PWM_Init();

    // Forward
    IN1 = 1; IN2 = 0;
    IN3 = 1; IN4 = 0;

    SetPWM(200, 200);  // Constant speed

    while (1);
}

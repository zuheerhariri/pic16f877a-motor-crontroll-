/*
 * File:   PWM_DircontrolV2.c
 * Author: BOSSMOUSSA
 *
 * Created on 05 June 2025, 14:46
 */


// main.c
#include <xc.h>
#define _XTAL_FREQ 20000000

// CONFIG BITS
#pragma config FOSC = HS        // High-Speed Oscillator
#pragma config WDTE = OFF       // Watchdog Timer Disabled
#pragma config PWRTE = OFF      // Power-up Timer Disabled
#pragma config BOREN = ON       // Brown-out Reset Enabled
#pragma config LVP = OFF        // Low Voltage Programming Disabled
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection Off
#pragma config WRT = OFF        // Flash Program Memory Write Enable Off
#pragma config CP = OFF         // Flash Program Memory Code Protection Off

// Motor Driver Pins (connected to PORTB)
#define IN1 RB5  // Left Motor Forward
#define IN2 RB4  // Left Motor Backward
#define IN3 RB3  // Right Motor Forward
#define IN4 RB2  // Right Motor Backward

void ADC_Init() {
    ADCON0 = 0x41; // Turn ON ADC and select AN0
    ADCON1 = 0xC0; // Right justified, AN0 and AN1 enabled
}

unsigned int ADC_Read(unsigned char channel) {
    ADCON0 &= 0x11000101; // Clear channel selection bits
    ADCON0 |= channel << 3; // Select ADC channel
    __delay_ms(2); // Acquisition time
    GO_nDONE = 1; // Start conversion
    while(GO_nDONE); // Wait for conversion
    return ((ADRESH << 8) + ADRESL); // Return result
}

void PWM_Init() {
    PR2 = 0xFF; // Max period
    T2CON = 0x04; // Timer2 on, prescaler 1:1
    CCP1CON = 0x0C; // PWM mode for CCP1 (RC2)
    CCP2CON = 0x0C; // PWM mode for CCP2 (RC1)
    TRISC2 = 0; // CCP1 (PWM Left Motor)
    TRISC1 = 0; // CCP2 (PWM Right Motor)
}

void SetPWM_Left(unsigned int duty) {
    CCPR1L = duty >> 2;
    CCP1CON = (CCP1CON & 0xCF) | ((duty & 0x03) << 4);
}

void SetPWM_Right(unsigned int duty) {
    CCPR2L = duty >> 2;
    CCP2CON = (CCP2CON & 0xCF) | ((duty & 0x03) << 4);
}

void main() {
    TRISB = 0x00; // Set PORTB as output
    TRISA = 0xFF; // Set PORTA as input for analog joystick
    TRISC1 = 0;
    TRISC2 = 0;

    ADC_Init();
    PWM_Init();

    while(1) {
        unsigned int y = ADC_Read(0); // Vertical axis
        unsigned int x = ADC_Read(1); // Horizontal axis

        int center = 512;
        int forwardBackward = y - center; // Positive: forward, Negative: backward
        int leftRight = x - center;       // Positive: right, Negative: left

        // Scale to PWM range (0-1023)
        int leftMotor = forwardBackward + leftRight;
        int rightMotor = forwardBackward - leftRight;

        // Limit motor speeds to 0?1023 range
        if (leftMotor > 1023) leftMotor = 1023;
        if (leftMotor < -1023) leftMotor = -1023;
        if (rightMotor > 1023) rightMotor = 1023;
        if (rightMotor < -1023) rightMotor = -1023;

        // Control left motor
        if (leftMotor > 0) {
            IN1 = 1; IN2 = 0;
            SetPWM_Left(leftMotor);
        } else if (leftMotor < 0) {
            IN1 = 0; IN2 = 1;
            SetPWM_Left(-leftMotor);
        } else {
            IN1 = 0; IN2 = 0;
            SetPWM_Left(0);
        }

        // Control right motor
        if (rightMotor > 0) {
            IN3 = 1; IN4 = 0;
            SetPWM_Right(rightMotor);
        } else if (rightMotor < 0) {
            IN3 = 0; IN4 = 1;
            SetPWM_Right(-rightMotor);
        } else {
            IN3 = 0; IN4 = 0;
            SetPWM_Right(0);
        }

        __delay_ms(50);
    }
}

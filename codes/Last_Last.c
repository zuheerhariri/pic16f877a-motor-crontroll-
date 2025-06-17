/*
 * File:   Last_Last.c
 * Author: BOSSMOUSSA
 * Created on 12 June 2025, 15:00
 */

// CONFIG
#pragma config FOSC = HS        // High Speed Oscillator
#pragma config WDTE = OFF       // Watchdog Timer Disabled
#pragma config PWRTE = OFF      // Power-up Timer Disabled
#pragma config BOREN = ON       // Brown-out Reset Enabled
#pragma config LVP = OFF        // Low Voltage Programming Disabled
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection Off
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits Off
#pragma config CP = OFF         // Flash Program Memory Code Protection Off

#include <xc.h>
#include <stdint.h>
#include <stdlib.h>             // For abs()

#define _XTAL_FREQ 20000000     // 20MHz crystal
#define DEADZONE 30             // Joystick sensitivity threshold

// Motor control pins (L298N)
#define IN1 RB5    // Left motor forward
#define IN2 RB4    // Left motor backward
#define IN3 RB3    // Right motor backward
#define IN4 RB2    // Right motor forward 

void setup() {
    // Configure I/O
    TRISB = 0x00;                // PORTB as output (motor control)
    TRISA = 0xFF;                // PORTA as input (joystick)
    TRISCbits.TRISC1 = 0;        // CCP2 (RC1) as output
    TRISCbits.TRISC2 = 0;        // CCP1 (RC2) as output

    // ADC setup
    ADCON1 = 0x80;               // Right justified, AN0 and AN1 as analog
    ADCON0 = 0x01;               // Turn on ADC module (channel 0 by default)

    // PWM Configuration (1.22kHz with 20MHz clock)
    PR2 = 255;                   // PWM period
    T2CON = 0x07;                // Timer2 ON, prescale 1:16
    CCP1CON = 0x0C;              // CCP1 in PWM mode
    CCP2CON = 0x0C;              // CCP2 in PWM mode
}

// Read ADC value from a given channel (0?7)
uint16_t read_joystick(uint8_t channel) {
    ADCON0 &= 0xC5;                  // Clear channel selection bits
    ADCON0 |= (channel << 3);       // Set new channel To not overwrite important conf bits as ON off & conv 
    __delay_us(30);                 // Wait for acquisition
    GO = 1;                         // Start conversion
    while (GO);                     // Wait until done
    return ((uint16_t)ADRESH << 8) | ADRESL;  // Combine high and low result into one full 10bit or 16bit 
}

// Control motors: direction and speed
void set_motors(int8_t dir, uint8_t left_speed, uint8_t right_speed) {
    if (dir == 1) { // Forward
        IN1 = 1; IN2 = 0;
        IN3 = 0; IN4 = 1;
    } else if (dir == -1) { // Backward
        IN1 = 0; IN2 = 1;
        IN3 = 1; IN4 = 0;
    } else { // Stop
        IN1 = 0; IN2 = 0;
        IN3 = 0; IN4 = 0;
    }

    CCPR1L = left_speed;   // PWM for left motor (RC2 / CCP1)
    CCPR2L = right_speed;  // PWM for right motor (RC1 / CCP2)
}

void main() {
    setup();

    while (1) {
        int y = read_joystick(0) - 512;  // Forward/backward
        int x = read_joystick(1) - 512;  // Left/right

        // Apply deadzone
        if (abs(y) < DEADZONE) y = 0;
        if (abs(x) < DEADZONE) x = 0;

        // Base speed calculation
        uint8_t speed = (abs(y) * 255) / 512; 

        // Adjust turning
        uint8_t left = speed;
        uint8_t right = speed;
        if (x > 0) right = right * (512 - x) / 512;   // Turning right
        if (x < 0) left = left * (512 + x) / 512;     // Turning left

        // Determine direction: forward, backward, or stop
        int8_t dir = (y > 0) ? 1 : ((y < 0) ? -1 : 0);

        // Apply to motors
        set_motors(dir, left, right);

        __delay_ms(20); // Small loop delay
    }
}

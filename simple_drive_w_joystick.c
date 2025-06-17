/*
 * File:   simple_drive_w_joystick.c
 * Author: BOSSMOUSSA
 *
 * Created on 05 June 2025, 12:47
 */

// main.c
#include <xc.h>
#include <stdint.h>

// CONFIG BITS
#pragma config FOSC = HS    // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF   // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON   // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON   // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF    // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used)
#pragma config CPD = OFF    // Data EEPROM Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF    // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CP = OFF     // Flash Program Memory Code Protection bit (Code protection off)

#define _XTAL_FREQ 20000000

// Motor pins
#define IN1 RB5  // Motor A
#define IN2 RB4
#define IN3 RB3  // Motor B
#define IN4 RB2

void ADC_Init() {
    ADCON1 = 0x80;    // Right justify result, Vref=Vdd
    ADCON0 = 0x01;    // Turn on ADC
}

unsigned int ADC_Read(unsigned char channel) {
    ADCON0 &= 0xC5;                 // Clear current channel bits
    ADCON0 |= (channel << 3);      // Select ADC channel
    __delay_ms(2);                 // Acquisition delay
    GO_nDONE = 1;                  // Start conversion
    while(GO_nDONE);               // Wait until done
    return ((ADRESH << 8) + ADRESL); // Combine high and low bytes
}

void setup() {
    TRISB = 0x00;    // Set PORTB as output (motor control)
    TRISA = 0xFF;    // Set PORTA as input (joystick)
    PORTB = 0x00;    // Initialize motors off
    ADC_Init();
}

// Motor control functions
void stop() {
    IN1 = 0; IN2 = 0;
    IN3 = 0; IN4 = 0;
}

void forward() {
    IN1 = 1; IN2 = 0;
    IN3 = 1; IN4 = 0;
}

void backward() {
    IN1 = 0; IN2 = 1;
    IN3 = 0; IN4 = 1;
}

void turnLeft() {
    IN1 = 0; IN2 = 1;
    IN3 = 1; IN4 = 0;
}

void turnRight() {
    IN1 = 1; IN2 = 0;
    IN3 = 0; IN4 = 1;
}

void main() {
    setup();

    while(1) {
        unsigned int xVal = ADC_Read(0);  // X-axis
        unsigned int yVal = ADC_Read(1);  // Y-axis

        // Threshold values (center around ~512)
        if (yVal > 600) {
            forward();
        } else if (yVal < 400) {
            backward();
        } else if (xVal > 600) {
            turnRight();
        } else if (xVal < 400) {
            turnLeft();
        } else {
            stop();
        }

        __delay_ms(100);
    }
}

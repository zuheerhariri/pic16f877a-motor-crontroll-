// PIC16F877A Configuration Bit Settings

// CONFIG

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)

#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)

#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)

#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)

#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)

#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)

#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)

#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)
 
#include <xc.h>
 
#define _XTAL_FREQ 4000000 // 4MHz crystal frequency
 
void main(void) {

    // Set PORTD as output for PWM signals

    TRISD = 0x00;

    PORTD = 0x00;

    // Set PORTC as output for motor direction control

    TRISC = 0x00;

    PORTC = 0x00;

    // Configure PWM Module

    // PWM Period = (PR2 + 1) * 4 * Tosc * TMR2 Prescale Value

    // For 20MHz, prescale=16, PR2=255:

    // PWM Frequency = 20MHz / (4 * 16 * (255+1)) ? 1.22kHz

    // Configure Timer2 for PWM

    T2CON = 0b00000111; // Timer2 ON, prescale 16, postscale 1

    PR2 = 255;          // Maximum period for 8-bit resolution

    // Configure CCP1 and CCP2 modules for PWM mode

    CCP1CON = 0b00001100; // PWM mode for CCP1 (RD5)

    CCP2CON = 0b00001100; // PWM mode for CCP2 (RD3)

    // Set initial duty cycles to maximum (100%)

    CCPR1L = 0b11111111; // Full duty cycle for CCP1

    CCPR2L = 0b11111111; // Full duty cycle for CCP2

    // Set the two least significant bits of duty cycle (DCxB bits)

    // For CCP1 (RD5)

    CCP1CONbits.CCP1X = 1;

    CCP1CONbits.CCP1Y = 1;

    // For CCP2 (RD3)

    CCP2CONbits.CCP2X = 1;

    CCP2CONbits.CCP2Y = 1;

    // Set motor directions (adjust based on your wiring)

    // Example: RC0 and RC1 control Motor1 direction, RC2 and RC3 control Motor2 direction

    PORTCbits.RC0 = 1; // Motor1 forward

    PORTCbits.RC1 = 0;

    PORTCbits.RC2 = 1; // Motor2 forward

    PORTCbits.RC3 = 0;

    while(1) {

        // Main loop - PWM is hardware generated, so no need to do anything here

        // Both motors will run at full speed continuously

        __delay_ms(100); // Small delay to reduce CPU usage

    }

    return;

}
 

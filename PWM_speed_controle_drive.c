/*
 * File:   PWM_speed_controle_drive.c
 * Author: BOSSMOUSSA
 *
 * Created on 05 June 2025, 12:48
 */


#include <xc.h>

// CONFIG
#pragma config FOSC = HS        
#pragma config WDTE = OFF       
#pragma config PWRTE = OFF      
#pragma config BOREN = OFF      
#pragma config LVP = OFF        
#pragma config CPD = OFF        
#pragma config WRT = OFF        
#pragma config CP = OFF         

#define _XTAL_FREQ 20000000  // 20 MHz crystal

// Motor control pins
#define IN1 RB5
#define IN2 RB4
#define IN3 RB3
#define IN4 RB2

// Function prototypes
void ADC_Init();
unsigned int ADC_Read(unsigned char channel);
void PWM_Init();
void set_PWM1(unsigned int duty);
void set_PWM2(unsigned int duty);
void move_forward(unsigned int speed);
void move_backward(unsigned int speed);
void rotate_left(unsigned int speed);
void rotate_right(unsigned int speed);
void stop_motors();

void main() {
    TRISB = 0x00;  // Motor control pins as output
    PORTB = 0x00;

    ADC_Init();
    PWM_Init();

    while(1) {
        unsigned int y = ADC_Read(1); // Y-axis for movement
        unsigned int x = ADC_Read(0); // X-axis for rotation

        int y_offset = (int)y - 512;
        int x_offset = (int)x - 512;

        unsigned int speed_y = (y_offset > 0) ? y_offset : -y_offset;
        unsigned int speed_x = (x_offset > 0) ? x_offset : -x_offset;

        if (speed_y < 50 && speed_x < 50) {
            stop_motors(); // Within dead zone
        } else if (speed_y >= speed_x) {
            if (y_offset > 50)
                move_forward(speed_y);
            else if (y_offset < -50)
                move_backward(speed_y);
        } else {
            if (x_offset > 50)
                rotate_right(speed_x);
            else if (x_offset < -50)
                rotate_left(speed_x);
        }

        __delay_ms(50);
    }
}

// ---------- ADC ----------
void ADC_Init() {
    ADCON0 = 0x41;  // ADC on, channel 0
    ADCON1 = 0x80;  // Right justified
}

unsigned int ADC_Read(unsigned char channel) {
    ADCON0 &= 0xC5;                
    ADCON0 |= (channel << 3);     
    __delay_us(30);
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((ADRESH << 8) + ADRESL);
}

// ---------- PWM ----------
void PWM_Init() {
    TRISC2 = 0; // CCP1
    TRISC1 = 0; // CCP2

    PR2 = 255;
    T2CON = 0x04;

    CCP1CON = 0x0C;
    CCP2CON = 0x0C;

    set_PWM1(0);
    set_PWM2(0);
}

void set_PWM1(unsigned int duty) {
    if(duty > 1023) duty = 1023;
    CCPR1L = duty >> 2;
    CCP1CON = (CCP1CON & 0xCF) | ((duty & 0x03) << 4);
}

void set_PWM2(unsigned int duty) {
    if(duty > 1023) duty = 1023;
    CCPR2L = duty >> 2;
    CCP2CON = (CCP2CON & 0xCF) | ((duty & 0x03) << 4);
}

// ---------- Motor Movement ----------
void move_forward(unsigned int speed) {
    IN1 = 1; IN2 = 0;
    IN3 = 1; IN4 = 0;
    set_PWM1(speed);
    set_PWM2(speed);
}

void move_backward(unsigned int speed) {
    IN1 = 0; IN2 = 1;
    IN3 = 0; IN4 = 1;
    set_PWM1(speed);
    set_PWM2(speed);
}

void rotate_left(unsigned int speed) {
    IN1 = 0; IN2 = 1;
    IN3 = 1; IN4 = 0;
    set_PWM1(speed);
    set_PWM2(speed);
}

void rotate_right(unsigned int speed) {
    IN1 = 1; IN2 = 0;
    IN3 = 0; IN4 = 1;
    set_PWM1(speed);
    set_PWM2(speed);
}

void stop_motors() {
    IN1 = IN2 = IN3 = IN4 = 0;
    set_PWM1(0);
    set_PWM2(0);
}

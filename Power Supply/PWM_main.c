/* 
 * File:   PWM_main.c
 * Author: Ken Kok
 *
 * Created on December 19, 2014, 10:29 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <pic16f1787.h>
#include <pic16f1787.inc>

#pragma config FCMEN    = 0;     // Fail-safe clock monitor disabled
#pragma config IESO     = 0;     // Internal/External Switchover Disabled
#pragma config FOSC     = 0b100; // Selects internal oscillator
#pragma config CLKOUTEN = 1;     // CLKOUT disabled
#pragma config BOREN    = 0b00;  // Brown-Out Reset Disabled
#pragma config CPD      = 1;     // Data is not protected during reprogramming
#pragma config CP       = 1;     // Code is not protected during reprogramming
#pragma config LVP      = 0;     // Enable MCLR
#pragma config MCLRE    = 1;     // Enable MCLR
#pragma config WDTE     = 0b00;  // Disable Watchdog timer
#pragma config DEBUG    = 0;     // In-circuit debugging enabled
#pragma config LPBOR    = 1;     // Low Power brown out reset disabled
#pragma config STVREN   = 0;     // Stack overflow will not cause a reset
#pragma config PLLEN    = 0;     // Disable the 4xPLL
#pragma config VCAPEN   = 1;     // VCAP disabled (would be on RA6)
#pragma config WRT      = 0b11;  // Program memory flash protection off

/*
 * 
 */
int initDev(void) {
/*
 *
 * Initializes the device
 *
 */
    OSCCONbits.SPLLEN = 0;      // Turn Off 4xPLL
    OSCCONbits.SCS    = 0b11;   // Clock is internal
    OSCCONbits.IRCF   = 0b1111; // 32MHz frequency
    
}
int initATD(void) {
/*
 *
 * Initializes the ATD Converter for the PIC16F1787
 *
 */
}
int initPWM(void) {
/*
 *
 * Initializes the PWM for the PIC16F1787
 * Initializes to a frequency of 100kHz (10uS period)
 * and duty cycle of 50%
 *
 */

    PSMC1CONbits.PSMC1EN  =  1;    // Enable PSMC1
    PSMC1CLKbits.P1CPRE   =  0b00; // PSMC1 CLK is /1 prescaler
    PSMC1CLKbits.P1CSRC   =  0b01; // PSMC1 CLK is 64MHz (uses PLL)
    PSMC1ASDCbits.P1ARSEN =  0;    // Disable Auto-Shutdown
    PSMC2CONbits.PSMC2EN  =  1;    // Enable PSMC2
    PSMC2CLKbits.P2CPRE   =  0b00; // PSMC2 CLK is /1 prescaler
    PSMC2CLKbits.P2CSRC   =  0b01; // PSMC2 CLK is 64MHz (uses PLL)
    PSMC2ASDCbits.P2ARSEN =  0;    // Disable Auto-Shutdown
    PSMC3CONbits.PSMC3EN  =  1;    // Enable PSMC3
    PSMC3CLKbits.P3CPRE   =  0b00; // PSMC3 CLK is /1 prescaler
    PSMC3CLKbits.P3CSRC   =  0b01; // PSMC3 CLK is 64MHz (uses PLL)
    PSMC3ASDCbits.P3ARSEN =  0;    // Disable Auto-Shutdown

    CCP1CONbits.CCP1M = 0b1111; // Change CCP1 to PWM
    CCP2CONbits.CCP2M = 0b1111; // Change CCP2 to PWM
    CCP3CONbits.CCP3M = 0b1111; // Change CCP3 to PWM

}
int main(void) {
    initDev();
    initATD();
    initPWM();
    while (1){

    }

}


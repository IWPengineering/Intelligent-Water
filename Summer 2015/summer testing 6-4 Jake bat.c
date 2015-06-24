// Test comment for GitHub
/*
* File: InternalClock.c
* Author: jy1189
*
* Created on April 23, 2015, 11:05 AM
*/
// ****************************************************************************
// *** Include Statemets ******************************************************
// ****************************************************************************
#include "../IWPUtilities/IWPUtilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xc.h>
#include <string.h>
// ****************************************************************************
// *** PIC24F32KA302 Configuration Bit Settings *******************************
// ****************************************************************************
// FBS
#pragma config BWRP = OFF // Boot Segment Write Protect (Disabled)
#pragma config BSS = OFF // Boot segment Protect (No boot program flash segment)
// FGS
#pragma config GWRP = OFF // General Segment Write Protect (General segment may be written)
#pragma config GSS0 = OFF // General Segment Code Protect (No Protection)
// FOSCSEL
#pragma config FNOSC = FRC // Oscillator Select (Fast RC Oscillator (FRC))
#pragma config SOSCSRC = ANA // SOSC Source Type (Analog Mode for use with crystal)
#pragma config LPRCSEL = HP // LPRC Oscillator Power and Accuracy (High Power, High Accuracy Mode)
#pragma config IESO = OFF // Internal External Switch Over bit (Internal External Switchover mode enabled (Two-speed Start-up enabled))
// FOSC
#pragma config POSCMOD = NONE // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = OFF // CLKO Enable Configuration bit (CLKO output disabled)
#pragma config POSCFREQ = HS // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8MHz)
#pragma config SOSCSEL = SOSCHP // SOSC Power Selection Configuration bits (Secondary Oscillator configured for high-power operation)
#pragma config FCKSM = CSDCMD // Clock Switching and Monitor Selection (Both Clock Switching and Fail-safe Clock Monitor are disabled)
// FWDT
#pragma config WDTPS = PS32768 // Watchdog Timer Postscale Select bits (1:32768)
#pragma config FWPSA = PR128 // WDT Prescaler bit (WDT prescaler ratio of 1:128)
#pragma config FWDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware; SWDTEN bit disabled)
#pragma config WINDIS = ON // Windowed Watchdog Timer Disable bit (Windowed WDT enabled)
// FPOR
#pragma config BOREN = BOR3 // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware, SBOREN bit disabled)
#pragma config LVRCFG = OFF // (Low Voltage regulator is not available)
#pragma config PWRTEN = ON // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI // Alternate I2C1 Pin Mapping bit (Use Default SCL1/SDA1 Pins For I2C1)
//#pragma config BORV = V20 // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (2.0V))
#pragma config MCLRE = ON // MCLR Pin Enable bit (RA5 input pin disabled,MCLR pin enabled)
// FICD
#pragma config ICS = PGx1 // ICD Pin Placement Select bits (EMUC/EMUD share PGC1/PGD1)
// FDS
#pragma config DSWDTPS = DSWDTPSF // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC // DSWDT Reference Clock Select bit (DSWDT uses Low Power RC Oscillator (LPRC))
#pragma config DSBOREN = ON // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = ON // Deep Sleep Watchdog Timer Enable bit (DSWDT enabled)



/*********************************************************************
* Function: DefaultInterrupt()
* Input: None
* Output: None
* Overview: This function will automatically call itself and run when it is around midnight
* (when IFS3bits.RTCIF is true)
* sends Text Message of all the data collected that day.
* Note: if any other interrupts need to be added, they go in the "else" section
* ALL INTERRUPTS LIVE HERE
* Pic Dependent
* TestDate: 06-05-2014
********************************************************************/
void __attribute__((__interrupt__,__auto_psv__)) _DefaultInterrupt()
{ }




// ****************************************************************************
// *** Main Function **********************************************************
// ****************************************************************************
void main (void)
{
ANSA = 0; // Make PORTA digital I/O
TRISA = 0xFFFF; // Make PORTA all inputs
ANSB = 0; // All port B pins are digital. Individual ADC are set in the readADC function
TRISB = 0xFFFF; // Sets all of port B to input
TRISBbits.TRISB7 = 0;
TRISBbits.TRISB8 = 0; // RB8 is an output
// From fona code
TRISBbits.TRISB6 = 0; //sets power key as an output (Pin 15)
TRISAbits.TRISA1=0; //sets Vio as an output (pin 3)

U1BRG = 51; // Set baud to 9600, FCY = 8MHz (#pragma config FNOSC = FRC)
U1STA = 0;
U1MODE = 0x8000; //enable UART for 8 bit data
//no parity, 1 stop bit
U1STAbits.UTXEN=1; //enable transmit

initAdc();

char batLevelMessage[160];
batLevelMessage[0] = 0;
char adcString[20];
adcString[0] = 0;
int adcVal, i, j;
float adcFloat;
TRISAbits.TRISA4 = 0;
while(1)
{
    for(i=0;i<5;i++)
    {
        adcVal = readAdc(15);
        adcFloat = (float) adcVal;
        floatToString(adcFloat, adcString);
        concat(batLevelMessage, adcString);
        for(j=0;j<20;j++)
        {
            adcString[j] = 0;
        }
    }
    
    sendTextMessage(batLevelMessage);
    for(i=0;i<160;i++)
    {
        batLevelMessage[i] = 0;
    }

}
//sendTextMessage("init");
}
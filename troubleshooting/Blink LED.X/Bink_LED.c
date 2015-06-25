/*
 * File:   main.c
 * Author: js1663
 *
 * Created on September 17, 2013, 6:20 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

// FBS
#pragma config BWRP = OFF               // Boot Segment Write Protect (Disabled)
#pragma config BSS = OFF                // Boot segment Protect (No boot program flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Write Protect (General segment may be written)
#pragma config GSS0 = OFF               // General Segment Code Protect (No Protection)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC Oscillator (FRC))
#pragma config SOSCSRC = ANA            // SOSC Source Type (Analog Mode for use with crystal)
#pragma config LPRCSEL = HP             // LPRC Oscillator Power and Accuracy (High Power, High Accuracy Mode)
#pragma config IESO = OFF                // Internal External Switch Over bit (Internal External Switchover mode enabled (Two-speed Start-up enabled))

// FOSC
#pragma config POSCMOD = NONE           // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = OFF           // CLKO Enable Configuration bit (CLKO output disabled)
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary Oscillator configured for high-power operation)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Both Clock Switching and Fail-safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32768)
#pragma config FWPSA = PR128            // WDT Prescaler bit (WDT prescaler ratio of 1:128)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bits (WDT disabled in hardware; SWDTEN bit disabled)
#pragma config WINDIS = ON              // Windowed Watchdog Timer Disable bit (Windowed WDT enabled)

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware, SBOREN bit disabled)
#pragma config LVRCFG = OFF             //  (Low Voltage regulator is not available)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Use Default SCL1/SDA1 Pins For I2C1)
#pragma config BORV = V20               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (2.0V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (RA5 input pin disabled,MCLR pin enabled)

// FICD
#pragma config ICS = PGx2               // ICD Pin Placement Select bits (EMUC/EMUD share PGC1/PGD1)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses Low Power RC Oscillator (LPRC))
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = ON             // Deep Sleep Watchdog Timer Enable bit (DSWDT enabled)

//int ReadADC(int);

void delayMs(int ms) //tested 05-20-14
{
    int myIndex;

    while (ms > 0){
        myIndex = 0;
        while (myIndex < 667){
            myIndex++;
        }
        ms--;
    }
}
void delaySCL(void){
    int timeKiller = 0; //don't delete
    int myIndex = 0;
        while (myIndex < 5)
		{
			myIndex++;
		}
}
void main (void) {
//    int i;
//    //ANSA = 0; //Make PORTA digital I/O    // only digital possible
//    TRISBbits.TRISB8 = 0; //Make RB8 an outputs
//    PORTBbits.RB8 = 0; //Clear rb8
//    TRISBbits.TRISB9 = 0; //Make RB8 an outputs
//    PORTBbits.RB9 = 0; //Clear rb8
//
//
//
//    while (1)
//    {
//            /*      int value = ReadADC(12);
//        if (value > 512){
//        PORTA=0xFFFF;}
//        else
//        {PORTA= 0;} */
//        delayMs(1000);
//        PORTBbits.RB8 = 0;
//        PORTBbits.RB9 = 1;
//        delaySCL();
//        PORTBbits.RB8 = 1;
//        PORTBbits.RB9 = 0;
//
//    }
//    delayMs(20000);
//    int pulsesCreated = 0;
//    int endBit = 0;
// while((pulsesCreated < 9) && (endBit != 1))
//    {
//    TRISBbits.TRISB9 = 0; //Make RB9 an outputs
//    PORTBbits.RB9 = 0; //Clear rb9
//
//    //toggle
//        delaySCL();
//        PORTBbits.RB9 = 1; // SDA
//        delaySCL();
//        PORTBbits.RB9 = 0; // SDA
//        delaySCL();
//
//        pulsesCreated++;
//
//        //check to see if it's still hung up
//        int timeOut = 0;
//        while((PORTBbits.RB9 == 0) && (timeOut < 4)){ // SDA is high for
//            delaySCL();
//            timeOut++;
//        }
//        if (timeOut < 4){ // must have pulsed on it's own before timeOut
//            endBit = 1;
//        }
//    }
        int pulsesCreated = 0;
    int endBit = 0;
 while((pulsesCreated < 9) && (endBit != 1))
    {
    TRISBbits.TRISB8 = 0; //Make SCL an outputs
    PORTBbits.RB8 = 0; //Clear SCL

    //toggle
        delaySCL();
        PORTBbits.RB8 = 1; // SCL
        delaySCL();
        PORTBbits.RB8 = 0; // SCL
        delaySCL();

        pulsesCreated++;

        //check to see if it's still hung up
        int timeOut = 0;
        while((PORTBbits.RB9 == 0) && (timeOut < 4)){ // SDA is high for
            delaySCL();
            timeOut++;
        }
        if (timeOut < 4){ // must have pulsed on it's own before timeOut
            endBit = 1;
        }
    }
    while(1){
        PORTBbits.RB9 = 1; // SDA
        delayMs(90);

    }
}


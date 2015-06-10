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
#include "IWPUtilities.h"
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
* Function: batteryLevel()
* Input: None
* Output: char
* Overview: returns an output of a string containing the voltageAvgFloat of three different voltages
* Note:
* TestDate: TBD
********************************************************************/

// Vin = Actual battery level. Vout = what we are recieving.
//Vin = (21/10)*Vout;
//Vin = Vout/(0.4761908)
float batteryLevel(void){ //this has not been tested
    char voltageAvgFloatString[20];
    voltageAvgFloatString[0] = 0;
    float voltageOut1;
    float voltageIn1;
    float voltageOut2;
    float voltageIn2;
    float voltageOut3;
    float voltageIn3;
    float voltageAvg;


    voltageOut1 = readAdc(batteryVoltage); // - adjustmentFactor;
    voltageIn1 = voltageOut1 / (batteryLevelConstant);

    delayMs(50);

    voltageOut2 = readAdc(batteryVoltage); // - adjustmentFactor;
    voltageIn2 = voltageOut2 / (batteryLevelConstant);

    delayMs(50);

    voltageOut3 = readAdc(batteryVoltage); // - adjustmentFactor;
    voltageIn3 = voltageOut3 / (batteryLevelConstant);

    
    voltageAvg = (voltageIn1 + voltageIn2 + voltageIn3)/3 * 0.002;


    floatToString(voltageAvg, voltageAvgFloatString);

    //return voltageAvgFloatString;

    return voltageAvg;
}

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
{
if (IFS3bits.RTCIF)
{ //If Alarm "is ringing", do this ....
/////////////////////////////////////////////
// Should we wake the SIM up here?
/////////////////////////////////////////////
/* message type,
* version # (no version number, we should just be able to check for new values in JSON),
* date? (no date, the sms should have that within it),
* sequence #; incase we didn't get a message (sent and recieved last 30, 60 total),
* message data (all the variables):
* leakageCoefficient (leakRateLongString)
* longestPrime
* volume02String
* volume24String
* volume46String
* volume68String
* volume810String
* volume1012String
* volume1214String
* volume1416String
* volume1618String
* volume1820String
* volume2022String
* volume2224String
* version #?
* (won't include max/ min level in this version)
* error1? (forseeable errors in the future)
* error2? (forseeable errors in the future)
* (won't use battery percentage till january)
* check sum (sum of all the values)
*
*
* {"t":"d","d":[{"l":123.123,"p":123.123,"v":[123.123,123.123,123.123,123.123,123.123,123.123,123.123,123.123,123.123,123.123,123.123,123.123]}]}
*
*/
//Message assembly and sending; Use *floatToString() to send:
char longestPrimeString[20];
longestPrimeString[0] = 0;
char leakRateLongString[20];
leakRateLongString[0] = 0;
char volume02String[20];
volume02String[0] = 0;
char volume24String[20];
volume24String[0] = 0;
char volume46String[20];
volume46String[0] = 0;
char volume68String[20];
volume68String[0] = 0;
char volume810String[20];
volume810String[0] = 0;
char volume1012String[20];
volume1012String[0] = 0;
char volume1214String[20];
volume1214String[0] = 0;
char volume1416String[20];
volume1416String[0] = 0;
char volume1618String[20];
volume1618String[0] = 0;
char volume1820String[20];
volume1820String[0] = 0;
char volume2022String[20];
volume2022String[0] = 0;
char volume2224String[20];
volume2224String[0] = 0;
floatToString(leakRateLong, leakRateLongString);
floatToString(longestPrime, longestPrimeString);
floatToString(volume02, volume02String);
floatToString(volume24, volume24String);
floatToString(volume46, volume46String);
floatToString(volume68, volume68String);
floatToString(volume810, volume810String);
floatToString(volume1012, volume1012String);
floatToString(volume1214, volume1214String);
floatToString(volume1416, volume1416String);
floatToString(volume1618, volume1618String);
floatToString(volume1820, volume1820String);
floatToString(volume2022, volume2022String);
floatToString(volume2224, volume2224String);
long checkSum = longestPrime + leakRateLong + volume02 + volume24 + volume46 + volume68 + volume810 + volume1012 + volume1214 + volume1416 + volume1618 + volume1820 + volume2022 + volume2224;
char stringCheckSum[20];
floatToString(checkSum, stringCheckSum);
//will need more formating for JSON 5-30-2014
char dataMessage[160];
dataMessage[0] = 0;
concat(dataMessage, "(\"t\":\"d\",\"d\":(\"l\":");
concat(dataMessage, leakRateLongString);
concat(dataMessage, ",\"p\":");
concat(dataMessage, longestPrimeString);
concat(dataMessage, ",\"v\":<");
concat(dataMessage, volume02String);
concat(dataMessage, ",");
concat(dataMessage, volume24String);
concat(dataMessage, ",");
concat(dataMessage, volume46String);
concat(dataMessage, ",");
concat(dataMessage, volume68String);
concat(dataMessage, ",");
concat(dataMessage, volume810String);
concat(dataMessage, ",");
concat(dataMessage, volume1012String);
concat(dataMessage, ",");
concat(dataMessage, volume1214String);
concat(dataMessage, ",");
concat(dataMessage, volume1416String);
concat(dataMessage, ",");
concat(dataMessage, volume1618String);
concat(dataMessage, ",");
concat(dataMessage, volume1820String);
concat(dataMessage, ",");
concat(dataMessage, volume2022String);
concat(dataMessage, ",");
concat(dataMessage, volume2224String);
concat(dataMessage, ">))");
/* Not working:
//concat(dataMessage, "bat: ");
//concat(dataMessage, batteryLevel());
*/

// Try to establish network connection
tryToConnectToNetwork();
delayMs(2000);
// Send off the data
sendTextMessage(dataMessage);


pressReset();
IFS3bits.RTCIF = 0; //turns off the alarm interrupt flag
////////////////////////////////////////////////
// Should we put the SIM back to sleep here?
////////////////////////////////////////////////
RTCCSet(); // updates the internal time from the external RTCC if the internal RTCC got off any through out the day
}
else
{
// Other interrupts sent here
}
// Do we need: The ?Return from Interrupt? instruction, RETFIE, exits an interrupt or trap routine.
}




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


char batLevelMessage[160];
batLevelMessage[0] = 0;
char adcString[20];
adcString[0] = 0;
int adcVal, i, j;
float adcFloat;
TRISAbits.TRISA4 = 0;
while(1)
{
    sendMessage("TEST");
    for(i=0;i<5;i++)
    {
        adcVal = readAdc(2);
        adcFloat = (float) adcVal;
        floatToString(adcFloat, adcString);
        concat(batLevelMessage, adcString);
        for(j=0;j<20;j++)
        {
            adcString[j] = 0;
        }
    }
    
    sendMessage(batLevelMessage);
    for(i=0;i<160;i++)
    {
        batLevelMessage[i] = 0;
    }

}
//sendTextMessage("init");
}
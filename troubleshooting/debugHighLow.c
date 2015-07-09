/* 
 * File:   debugHighLow.c
 * Author: Jacqui
 *
 * Created on July 9, 2015, 4:17 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xc.h>
#include <string.h>

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
int mclrPin = 1;
int depthSensorPin = 2;
int simVioPin = 3;
int Pin4 = 4;
int Pin5 = 5;
int rxPin = 6;
int Pin7 = 7;
int GND2Pin = 8;
int Pin9 = 9;
int Pin10 = 10;
int batteryLevelPin = 11;
int Pin12 = 12;
int vccPin = 13;
int waterPresenceSensorPin = 14;
int pwrKeyPin = 15;
int txPin = 16;
int sclI2CPin = 17;
int sdaI2CPin = 18;
int statusPin = 19;
int vCapPin = 20;
int picKit4Pin = 21;
int picKit5Pin = 22;
int yAxisAccelerometerPin = 23;
int xAxisAccelerometerPin = 24;
int netLightPin = 25;
int waterPresenceSensorOnOffPin = 26;
int GNDPin = 27;
int vcc2Pin = 28;

/////////////////////////////////////////////////////////////////////
////                                                             ////
////                PIN MANAGEMENT FUNCTIONS                     ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
void pinDirectionIO(int pin, int io){ // 1 is an input, 0 is an output
	// Pin 1 can't change direction
	if (pin == 2)
	{
		TRISAbits.TRISA0 = io;
	}
	else if (pin == 3)
	{
		TRISAbits.TRISA1 = io;
	}
	else if (pin == 4)
	{
		TRISBbits.TRISB0 = io;
	}
	else if (pin == 5)
	{
		TRISBbits.TRISB1 = io;
	}
	else if (pin == 6)
	{
		TRISBbits.TRISB2 = io;
	}
	else if (pin == 7)
	{
		TRISBbits.TRISB3 = io;
	}
	// Pin8 - Always VSS for PIC24FV32KA302 - Do nothing
	else if (pin == 9)
	{
		TRISAbits.TRISA2 = io;
	}
	else if (pin == 10)
	{
		TRISAbits.TRISA3 = io;
	}
	else if (pin == 11)
	{
		TRISBbits.TRISB4 = io;
	}
	else if (pin == 12)
	{
		TRISAbits.TRISA4 = io;
	}
	//Pin 13 - Always VDD for PIC24FV32KA302 - Do nothing
	else if (pin == 14)
	{
		TRISBbits.TRISB5 = io;
	}
	else if (pin == 15)
	{
		TRISBbits.TRISB6 = io;
	}
	else if (pin == 16)
	{
		TRISBbits.TRISB7 = io;
	} //Usually reserved for TX
	else if (pin == 17)
	{
		TRISBbits.TRISB8 = io;
	}//Usually reserved for I2C
	else if (pin == 18)
	{
		TRISBbits.TRISB9 = io;
	}//Usually Reserved for I2C
	else if (pin == 19)
	{
		TRISAbits.TRISA7 = io;
	}
	// Pin 20 - Always vCap for PIC24FV32KA302 - Do nothing
	else if (pin == 21)
	{
		TRISBbits.TRISB10 = io;
	}
	else if (pin == 22)
	{
		TRISBbits.TRISB11 = io;
	}
	else if (pin == 23)
	{
		TRISBbits.TRISB12 = io;
	}
	else if (pin == 24)
	{
		TRISBbits.TRISB13 = io;
	}
	else if (pin == 25)
	{
		TRISBbits.TRISB14 = io;
	}
	else if (pin == 26)
	{
		TRISBbits.TRISB15 = io;
	}
	// Pin 27 - Always VSS for PIC24FV32KA302 - Do nothing
	// Pin 28 - Always VDD for PIC24FV32KA302 - Do nothing
}


void digitalPinSet(int pin, int set) // 1 for high, 0 for low
{
	if (pin == 1)
	{
		PORTAbits.RA5 = set;
	}
	else if (pin == 2)
	{
		PORTAbits.RA0 = set;
	}
	else if (pin == 3)
	{
		PORTAbits.RA1 = set;
	}
	else if (pin == 4)
	{
		PORTBbits.RB0 = set;
	}
	else if (pin == 5)
	{
		PORTBbits.RB1 = set;
	}
	else if (pin == 6)
	{
		PORTBbits.RB2 = set;
	}
	else if (pin == 7)
	{
		PORTBbits.RB3 = set;
	}
	// Pin8 - Always VSS for PIC24FV32KA302 - Do nothing
	else if (pin == 9)
	{
		PORTAbits.RA2 = set;
	}
	else if (pin == 10)
	{
		PORTAbits.RA3 = set;
	}
	else if (pin == 11)
	{
		PORTBbits.RB4 = set;
	}
	else if (pin == 12)
	{
		PORTAbits.RA4 = set;
	}
	//Pin 13 - Always VDD for PIC24FV32KA302 - Do nothing
	else if (pin == 14)
	{
		PORTBbits.RB5 = set;
	}
	else if (pin == 15)
	{
		PORTBbits.RB6 = set;
	}
	else if (pin == 16)
	{
		PORTBbits.RB7 = set;
	} //Usually reserved for TX
	else if (pin == 17)
	{
		PORTBbits.RB8 = set;
	}//Usually reserved for I2C
	else if (pin == 18)
	{
		PORTBbits.RB9 = set;
	}//Usually Reserved for I2C
	else if (pin == 19)
	{
		PORTAbits.RA7 = set;
	}
	// Pin 20 - Always vCap for PIC24FV32KA302 - Do nothing
	else if (pin == 21)
	{
		PORTBbits.RB10 = set;
	}
	else if (pin == 22)
	{
		PORTBbits.RB11 = set;
	}
	else if (pin == 23)
	{
		PORTBbits.RB12 = set;
	}
	else if (pin == 24)
	{
		PORTBbits.RB13 = set;
	}
	else if (pin == 25)
	{
		PORTBbits.RB14 = set;
	}
	else if (pin == 26)
	{
		PORTBbits.RB15 = set;
	}
	// Pin 27 - Always VSS for PIC24FV32KA302 - Do nothing
	// Pin 28 - Always VDD for PIC24FV32KA302 - Do nothing
}

//TODO: Should be based off of the RB values, not the AN
void specifyAnalogPin(int pin, int analogOrDigital) // analogOrDigital = 1 if analog, 0 is digital
{
	if (pin == 4)
	{
		ANSBbits.ANSB0 = analogOrDigital;
	}
	else if (pin == 5)
	{
		ANSBbits.ANSB1 = analogOrDigital;
	}
	else if (pin == 6)
	{
		ANSBbits.ANSB2 = analogOrDigital;
	}
	else if (pin == 7)
	{
		ANSBbits.ANSB3 = analogOrDigital;
		//TODO: Jacqui needs to find out why pin 7 isn't in the library
	}
	else if (pin == 11)
	{
		ANSBbits.ANSB4 = analogOrDigital;
	}
	else if (pin == 23)
	{
		ANSBbits.ANSB12 = analogOrDigital;
	}
	else if (pin == 24)
	{
		ANSBbits.ANSB13 = analogOrDigital;
	}
	else if (pin == 25)
	{
		ANSBbits.ANSB14 = analogOrDigital;
	}
	else if (pin == 26)
	{
		ANSBbits.ANSB15 = analogOrDigital;
	}
}

void pinSampleSelectRegister(int pin){ //  A/D Sample Select Regiser (this is only used in the readADC() function)
    if (pin == 4)
	{
		AD1CHSbits.CH0SA = 2; //AN2
	}
	else if (pin == 5)
	{
		AD1CHSbits.CH0SA = 3; //AN3
	}
	else if (pin == 6)
	{
		AD1CHSbits.CH0SA = 4;
	}
	else if (pin == 7)
	{
		AD1CHSbits.CH0SA = 5;
	}
	else if (pin == 11)
	{
		AD1CHSbits.CH0SA = 15;
	}
	else if (pin == 23)
	{
		AD1CHSbits.CH0SA = 12;
	}
	else if (pin == 24)
	{
		AD1CHSbits.CH0SA = 11;
	}
	else if (pin == 25)
	{
		AD1CHSbits.CH0SA = 10;
	}
	else if (pin == 26)
	{
		AD1CHSbits.CH0SA = 9;
	}
}

int digitalPinStatus(int pin)
{
	int pinValue;
	if (pin == 1)
	{
		pinValue = PORTAbits.RA5;
	}
	else if (pin == 2)
	{
		pinValue = PORTAbits.RA0;
	}
	else if (pin == 3)
	{
		pinValue = PORTAbits.RA1;
	}
	else if (pin == 4)
	{
		pinValue = PORTBbits.RB0;
	}
	else if (pin == 5)
	{
		pinValue = PORTBbits.RB1;
	}
	else if (pin == 6)
	{
		pinValue = PORTBbits.RB2;
	}
	else if (pin == 7)
	{
		pinValue = PORTBbits.RB3;
	}
	// Pin8 - Always VSS for PIC24FV32KA302 - Do nothing
	else if (pin == 9)
	{
		pinValue = PORTAbits.RA2;
	}
	else if (pin == 10)
	{
		pinValue = PORTAbits.RA3;
	}
	else if (pin == 11)
	{
		pinValue = PORTBbits.RB4;
	}
	else if (pin == 12)
	{
		pinValue = PORTAbits.RA4;
	}
	//Pin 13 - Always VDD for PIC24FV32KA302 - Do nothing
	else if (pin == 14)
	{
		pinValue = PORTBbits.RB5;
	}
	else if (pin == 15)
	{
		pinValue = PORTBbits.RB6;
	}
	else if (pin == 16)
	{
		pinValue = PORTBbits.RB7;
	} //Usually reserved for TX
	else if (pin == 17)
	{
		pinValue = PORTBbits.RB8;
	}//Usually reserved for I2C
	else if (pin == 18)
	{
		pinValue = PORTBbits.RB9;
	}//Usually Reserved for I2C
	else if (pin == 19)
	{
		pinValue = PORTAbits.RA7;
	}
	// Pin 20 - Always vCap for PIC24FV32KA302 - Do nothing
	else if (pin == 21)
	{
		pinValue = PORTBbits.RB10;
	}
	else if (pin == 22)
	{
		pinValue = PORTBbits.RB11;
	}
	else if (pin == 23)
	{
		pinValue = PORTBbits.RB12;
	}
	else if (pin == 24)
	{
		pinValue = PORTBbits.RB13;
	}
	else if (pin == 25)
	{
		pinValue = PORTBbits.RB14;
	}
	else if (pin == 26)
	{
		pinValue = PORTBbits.RB15;
	}
	return pinValue;
	// Pin 27 - Always VSS for PIC24FV32KA302 - Do nothing
	// Pin 28 - Always VDD for PIC24FV32KA302 - Do nothing
}

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

// This toggles the pin you pass it
void debugHighLow(int pin){
    specifyAnalogPin(pin, 0); // makes digital
    pinDirectionIO(pin, 0); // makes output
    if(digitalPinStatus(pin) == 0) {
        digitalPinSet(pin, 1); // makes high
    }
    else{
        digitalPinSet(pin, 0); //makes low
    }
}
void main (void) {

    debugHighLow(Pin4); // This would make pin 4 toggle
}



//12-01-2014 Checked to make sure all the AT+ commands worked for sim800

//This version was used to program the World Vision pumps in Ghana during the summer of 2014

/* For India Mark II pumps
 * Time had to be hard-set due to a GSM problem in Ghana
 * Volume was never calibrated
 */

/*
 * File:   the real thing.c
 * Authors: Tony Beers, John Snyder, Jacqui Young, Avery deGruchy
 *
 * Created on June 26, 2014
 */

// ****************************************************************************
// *** Include Statemets ******************************************************
// ****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xc.h>
#include <string.h>

//------------------------------------------------------------------------------
// ****************************************************************************
// *** PIC24F32KA302 Configuration Bit Settings *******************************
// ****************************************************************************
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
#pragma config IESO = OFF               // Internal External Switch Over bit (Internal External Switchover mode enabled (Two-speed Start-up enabled))

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
#pragma config LVRCFG = OFF             // (Low Voltage regulator is not available)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Use Default SCL1/SDA1 Pins For I2C1)
//#pragma config BORV = V20               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (2.0V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (RA5 input pin disabled,MCLR pin enabled)

// FICD
#pragma config ICS = PGx1               // ICD Pin Placement Select bits (EMUC/EMUD share PGC1/PGD1)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses Low Power RC Oscillator (LPRC))
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = ON             // Deep Sleep Watchdog Timer Enable bit (DSWDT enabled)
//-------------------------------------------------------------------------------

const int pulseWidthThreshold = 0x0800;         // The value to check the pulse width against (2048)
const int networkPulseWidthThreshold = 0x4E20;  // The value to check the pulse width against (about 20000)
const int decimalAccuracy = 3;                  // Number of decimal places to use when converting floats to strings
static char phoneNumber[] = "\"+13018737202\"";        // phoneNumber messages are sent to
//Delays for the given number of milliseconds
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



void initialization(void){
  //------------Sets up the Internal Clock------------
  T1CONbits.TCS = 0;      // Source is Internal Clock (8MHz) x
  T1CONbits.TON = 1;      // Enable the timer (timer 1 is used for the water sensor)
  //------------Sets up all ports as digial inputs-----------------------
  ANSA = 0; // Make PORTA digital I/O
  TRISA = 0xFFFF; // Make PORTA all inputs
  ANSB = 0; // All port B pins are digital.  Individual ADC are set in the readADC function
  TRISB = 0xFFFF; // Sets all of port B to input
}

/*********************************************************************
* Function:        IdleI2C()
* Input:		None.
* Output:		None.
* Overview:		Waits for bus to become Idle
* Note:			None
********************************************************************/
unsigned int IdleI2C(void)
{
	while (I2C1STATbits.TRSTAT);		//Wait for bus Idle
}

/*********************************************************************
* Function:        StartI2C()
* Input:		None.
* Output:		None.
* Overview:		Generates an I2C Start Condition
* Note:			None
********************************************************************/
unsigned int StartI2C(void)
{
	//This function generates an I2C start condition and returns status
	//of the Start.

	I2C1CONbits.SEN = 1;		//Generate Start COndition
	while (I2C1CONbits.SEN);	//Wait for Start COndition
	//return(I2C1STATbits.S);	//Optionally return status
}


/*********************************************************************
* Function:        StopI2C()
* Input:		None.
* Output:		None.
* Overview:		Generates a bus stop condition
* Note:			None
********************************************************************/

unsigned int StopI2C(void)
{
	//This function generates an I2C stop condition and returns status
	//of the Stop.

	I2C1CONbits.PEN = 1;		//Generate Stop Condition
	while (I2C1CONbits.PEN);	//Wait for Stop
	//return(I2C1STATbits.P);	//Optional - return status
}

/*********************************************************************
* Function:        RestartI2C()
* Input:		None.
* Output:		None.
* Overview:		Generates a restart condition and optionally returns status
* Note:			None
********************************************************************/
void RestartI2C(void)
{
	//This function generates an I2C Restart condition and returns status
	//of the Restart.

	I2C1CONbits.RSEN = 1;		//Generate Restart
	while (I2C1CONbits.RSEN);	//Wait for restart
	//return(I2C1STATbits.S);	//Optional - return status
}

void NackI2C(void)
{
    I2C1CONbits.ACKDT = 1;
    I2C1CONbits.ACKEN = 1;
    while(I2C1CONbits.ACKEN){}
}

void AckI2C(void)
{
    I2C1CONbits.ACKDT = 0;
    I2C1CONbits.ACKEN = 1;
    while(I2C1CONbits.ACKEN){}
}

void configI2c(void){
    //From Jake's
    I2C1CONbits.A10M = 0; //Use 7-bit slave addresses
    I2C1CONbits.DISSLW = 1; // Disable Slew rate
    I2C1CONbits.IPMIEN = 0; //should be set to 0 when master
    //IFS1bits.MI2C1IF = 0; // Disable Interupt
    //^From Jake's

    I2C1BRG = 0x4E;  // If Fcy = 8 Mhz this will set the baud to 100 khz
    I2C1CONbits.I2CEN = 1; // Configures I2C pins as I2C (on pins 17 an 18)
}

/*********************************************************************
* Function:        WriteI2C()
* Input:		Byte to write.
* Output:		None.
* Overview:		Writes a byte out to the bus
* Note:			None
********************************************************************/
void WriteI2C(unsigned char byte)
{
	//This function transmits the byte passed to the function
	while (I2C1STATbits.TRSTAT);	//Wait for bus to be idle
	I2C1TRN = byte;                 //Load byte to I2C1 Transmit buffer
	while (I2C1STATbits.TBF);	//wait for data transmission

}

/*********************************************************************
* Function:        ReadI2C()
* Input:		None
* Output:		Returns one Byte from Slave device
* Overview:		Receives Byte & Writes a Nack out to the bus
* Note:			None
********************************************************************/

unsigned int ReadI2C (void)
{
    I2C1CONbits.ACKDT=1;         // Prepares to send NACK
    I2C1CONbits.RCEN=1;          // Gives control of clock to Slave device
    while(!I2C1STATbits.RBF){}   // Waits for register to fill up
    I2C1CONbits.ACKEN = 1;       // Sends NACK or ACK set above
    while(I2C1CONbits.ACKEN){}   // Waits till ACK is sent (hardware reset)
    return I2C1RCV;              // Returns data
   }

/*********************************************************************
* Function:        turnOffClockOscilator()
* Input:		None
* Output:		None
* Overview:		Turns off RTCC Oscillator MCP7940N so it can be
*                       set
* Note:			None
********************************************************************/
void turnOffClockOscilator (void)
{
// turns off oscilator to prepare to set time
    StartI2C();
    WriteI2C(0xde);        //Device Address (RTCC) + Write Command
    IdleI2C();
    WriteI2C(0x00);        //address reg. for sec
    IdleI2C();
    WriteI2C(0x00);        //Turn off oscillator and sets seconds to 0
    IdleI2C();
    StopI2C();
}

//This function converts a BCD to DEC
//Input: BCD Value
//Returns: Hex Value
char BcdToDec(char val)
{
return ( (val/16*10) + (val%16) );
}
//This function converts HEX to BCD
//Input: Hex Value
//Returns: BCD Value
char DecToBcd(char val)
{
    char bcd;
    bcd = (val/10);
    bcd = bcd << 4;
    bcd += val %10;
return (bcd);
}
char getSec (void){
    char sec;         // temp var to hold seconds information

    configI2c();      // sets up I2C
    StartI2C();
    WriteI2C(0xde);   // MCP7490N device address + write command
    IdleI2C();
    WriteI2C(0x00);   // device address for the Seconds register on MCP7490N
    IdleI2C();
    RestartI2C();
    IdleI2C();
    WriteI2C(0xdf);   // MCP7490N device address + read command
    IdleI2C();
    sec = ReadI2C();
    StopI2C();

    sec = sec & 0x7f;         // removes Oscillator bit
    sec = BcdToDec(sec);      // converts sec to a decimal number

    return sec;               // returns the time in sec as a demimal number
}

char getMin (void){
    char min;         // temp var to hold seconds information

    configI2c();      // sets up I2C
    StartI2C();
    WriteI2C(0xde);   // MCP7490N device address + write command
    IdleI2C();
    WriteI2C(0x01);   // device address for the minutes register on MCP7490N
    IdleI2C();
    RestartI2C();
    IdleI2C();
    WriteI2C(0xdf);   // MCP7490N device address + read command
    IdleI2C();
    min = ReadI2C();
    StopI2C();

    min = min & 0x7f;         // removes unused bit
    min = BcdToDec(min);      // converts min to a decimal number

    return min;               // returns the time in min as a demimal number
}
char getHr (void){
    char hr;         // temp var to hold seconds information

    configI2c();      // sets up I2C
    StartI2C();
    WriteI2C(0xde);   // MCP7490N device address + write command
    IdleI2C();
    WriteI2C(0x02);   // device address for the hours register on MCP7490N
    IdleI2C();
    RestartI2C();
    IdleI2C();
    WriteI2C(0xdf);   // MCP7490N device address + read command
    IdleI2C();
    hr = ReadI2C();
    StopI2C();

    hr = hr & 0x1f;         // removes unused bits
    hr = BcdToDec(hr);      // converts hr to a decimal number

    return hr;               // returns the time in hr as a demimal number
}

/*********************************************************************
* Function:        setTime()
* Input:		SS MM HH WW DD MM YY
* Output:		None
* Overview:		Sets time for MCP7940N
* Note:			uses DecToBcd and I2C functions
********************************************************************/
void setTime(char sec, char min, char hr, char wkday, char date, char month, char year)
{
    int leapYear;
    if (year % 4 == 0){
        leapYear = 1; //Is a leap Year
    }
    else{
        leapYear = 0; //Is not a leap Year
    }
    char BCDsec =  DecToBcd(sec); // To BCD
    char BCDmin =  DecToBcd(min); // To BCD
    char BCDhr =  DecToBcd(hr);
    char BCDwkday =  DecToBcd(wkday); // To BCD
    char BCDdate =  DecToBcd(date);
    char BCDmonth =  DecToBcd(month); // To BCD
    char BCDyear =  DecToBcd(year);

    BCDsec = BCDsec | 0x80;         // add turn on oscilator bit
    BCDhr = BCDhr & 0b10111111;     // makes 24 hr time
    BCDwkday = BCDwkday & 0b11110111; // the 0 says the external battery backup supply is disabled.
                                        // To enable: Flip bits and OR it to turn on (NOT CURRENTLY ENABLED).
    if (leapYear == 0){
        BCDmonth = BCDmonth  & 0b11011111;  //Not a leap year
    } else {
        BCDmonth = BCDmonth  | 0b00100000;  //Is a leap year
    }
 configI2c();
 turnOffClockOscilator();
//------------------------------------------------------------------------------
// sets clock
//------------------------------------------------------------------------------
    StartI2C();
    WriteI2C(0xDE);       //Device Address (RTCC) + Write Command
    IdleI2C();
    WriteI2C(0x01);       //Adress for minutes
    IdleI2C();
    WriteI2C(BCDmin);     //Load min
    IdleI2C();
    WriteI2C(BCDhr);      //Load hour
    IdleI2C();
    WriteI2C(BCDwkday);   //Load day of week
    IdleI2C();
    WriteI2C(BCDdate);    //Load Date
    IdleI2C();
    WriteI2C(BCDmonth);   //Load Month
    IdleI2C();
    WriteI2C(BCDyear);    // Load Year
    IdleI2C();
    StopI2C();
//------------------------------------------------------------------------------
// Sets seconds and turns on oscilator
//------------------------------------------------------------------------------
    StartI2C();
    WriteI2C(0xDE);         //Device Address (RTCC) + Write Command
    IdleI2C();
    WriteI2C(0x00);         //address reg. for sec
    IdleI2C();
    WriteI2C(BCDsec);       //Turn on oscillator and sets seconds
    IdleI2C();
    StopI2C();
}

void main (void){

    initialization();
    configI2c();
    char seconds = 30;
    char minutes = 53;
    char hours = 9;
    char weekday = 6;
    char days = 3;
    char months = 6;
    char years = 15;
setTime(seconds, minutes, hours, weekday, days, months, years); // SS MM HH WW DD MM YY



//------------------------------------------------------------------------------
// read seconds  works
//------------------------------------------------------------------------------
  while(1) {
      getHr();
      getMin();
      getSec();

    delayMs(1000);
  }
}


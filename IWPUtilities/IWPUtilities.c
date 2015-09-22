#include "IWPUtilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xc.h>
#include <string.h>

/*********************************
Table of Contents
1) Constants & Global Variables

2) Pin Management Functions

int digitalPinSet(int pin, int io);
void specifyAnalogPin(int pin, int analogOrDigital);
void analogIOandSHinput(int pin, int IO);
int digitalPinStatus(int pin);

3) Initialization

void initialization(void);

4) String Functions

int longLength(long num);
void longToString(long num, char *numString);
int stringLength(char *string);
void concat(char *dest, const char *src);
void floatToString(float myValue, char *myString);

5) SIM Functions

void turnOffSIM();
void turnOnSIM();
void tryToConnectToNetwork();
int connectedToNetwork(void);
void sendMessage (char message[160]);
void sendTextMessage(char message[160]);

6) Sensor Functions

int readWaterSensor(void);
void initAdc(void);
int readAdc(int channel);
float getHandleAngle();
void initializeQueue(float value);
void pushToQueue(float value);
float queueAverage();
float queueDifference();

7) I2C Functions

unsigned int IdleI2C(void);
unsigned int StartI2C(void);
unsigned int StopI2C(void);
void RestartI2C(void);
void NackI2C(void);
void AckI2C(void);
void configI2c(void);
void WriteI2C(unsigned char byte);
unsigned int ReadI2C (void);

8) RTCC Functions

void turnOffClockOscilator (void);
int getSecondI2C (void);
int getMinuteI2C (void);
int getHourI2C (void);
int getYearI2C (void);
int getMonthI2C (void);
int getWkdayI2C (void);
int getDateI2C (void);
void setTime(char sec, char min, char hr, char wkday, char date, char month, char year);

9) Misc Functions

float degToRad(float degrees);
void delayMs(int ms);
int getLowerBCDAsDecimal(int bcd);
int getUpperBCDAsDecimal(int bcd);
int getTimeHour(void);
long timeStamp(void);
void pressReset();
int translate(char digit);
void RTCCSet(void);
int getMinuteOffset();
char BcdToDec(char val);
char DecToBcd(char val);

 **********************************/

const int xAxis = 11; // analog pin connected to x axis of accelerometer
const int yAxis = 12; // analog pin connected to y axis of accelerometer
const int batteryVoltage = 15;                  // analog channel connected to the battery
const float MKII = .169; //0.467/2.0; // 0.4074 L/Radian; transfer variable for mkII delta handle angle to outflow
const float leakSensorVolume = 0.01781283; // This is in Liters; pipe dia. = 33mm; rod diam 12 mm; gage length 24mm
                                           // THE FASTEST WE COULD POSSIBLE BE IF IT DOES PRIME IS 42ms FOR THE AVERAGE PERSON but we'll do half that incase you're pumping hard to get it to prime
const int alarmHour = 0x0000; // The weekday and hour (24 hour format) (in BCD) that the alarm will go off
const int alarmStartingMinute = 1; // The minimum minute that the alarm will go off
const int alarmMinuteMax = 5; // The max number of minutes to offset the alarm (the alarmStartingMinute + a random number between 0 and this number)
const int adjustmentFactor = 511; // Used to ajust the values read from the accelerometer
const int pulseWidthThreshold = 20; // The value to cFheck the pulse width against (2048)
const int networkPulseWidthThreshold = 0x4E20; // The value to check the pulse width against (about 20000)
const int upstrokeInterval = 10; // The number of milliseconds to delay before reading the upstroke
int waterPrimeTimeOut = 7000; // Equivalent to 7 seconds (in 50 millisecond intervals); 50 = upstrokeInterval
long leakRateTimeOut = 3000; // Equivalent to 3 seconds (in 50 millisecond intervals); 50 = upstrokeInterval
const int volumeDelay = 10; // Equivalent to 10ms
//long timeBetweenUpstrokes = 18000; // 18000 seconds (based on upstrokeInterval)
const int decimalAccuracy = 3; // Number of decimal places to use when converting floats to strings
const int angleDeltaThreshold = 1; // The angle delta to check against
const float upstrokeToMeters = 0.01287;
const int minimumAngleDelta = 10;
const float batteryLevelConstant = 0.476;       //This number is found by Vout = (R32 * Vin) / (R32 + R31), Yields Vin = Vout / 0.476
const int secondI2Cvar = 0x00;
const int minuteI2Cvar = 0x01;
const int hourI2Cvar = 0x02;
const int dayI2Cvar = 0x03;
const int dateI2Cvar = 0x04;
const int monthI2Cvar = 0x05;
const int yearI2Cvar = 0x06;

const float angleRadius = .008; // this is 80 millimeters so should it equal 80 or .008?
int depthSensorInUse;
int queueCount = 0;
int queueLength = 7; //don't forget to change angleQueue to this number also
float angleQueue[7];
float theta1 = 0; // angle captured in getHandleAngle
float theta2 = 0; // angle captured in getHandleAngle
float theta3 = 0; // angle captured in getHandleAngle
float omega2 = 0; // angle captured in getHandleAngle
float omega3 = 0;
float alpha = 0;
double timeStep; //Time between getHandleAngle function calls

int prevTimer2 = 0; // Should intially begin at zero

int prevDay;
int prevDayDepthSensor;
float midDayDepth;
//int prevMinute;
//int prevHour; // just for testing, not a real variable
int invalid;
// ****************************************************************************
// *** Global Variables *******************************************************
// ****************************************************************************
//static char phoneNumber[] = "+233247398396"; // Number for the Black Phone
//char phoneNumber[] = "+233545822291"; // Number for the White Phone Ghana trip 3
//char phoneNumber[] = "+233545823475"; // Number for the Black? Phone Ghana trip 3
//char phoneNumber[] = "+19783840645"; // Number for Jake Sargent
//char phoneNumber[] = "+19783840645"; // Number for Jake Sargent
//char phoneNumber[] = "+17177784498"; // Number for Upside Wireless
//char phoneNumber2[] = "+17173039306"; // Tony's number
char phoneNumber[] = "+17173447340"; // Number for Sandy
float longestPrime = 0; // total upstroke fo the longest priming event of the day
float leakRateLong = 0; // largest leak rate recorded for the day
float batteryFloat;
float volume02 = 0; // Total Volume extracted from 0:00-2:00
float volume24 = 0;
float volume46 = 0;
float volume68 = 0;
float volume810 = 0;
float volume1012 = 0;
float volume1214 = 0;
float volume1416 = 0;
float volume1618 = 0;
float volume1820 = 0;
float volume2022 = 0;
float volume2224 = 0;
//Pin assignments
int mclrPin = 1;
int depthSensorPin = 2;
int simVioPin = 3;
int Pin4 = 4;
int Pin5 = 5;
int rxPin = 6;
int depthSensorOnOffPin = 7;
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


//DEBUG DEBUG DEBUG DEBUG
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
//DEBUG DEBUG DEBUG DEBUG

/////////////////////////////////////////////////////////////////////
////                                                             ////
////                    INITIALIZATION                           ////
////                                                             ////
/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function: initialization()
 * Input: None
 * Output: None
 * Overview: configures chip to work in our system (when power is turned on, these are set once)
 * Note: Pic Dependent
 * TestDate: 06-03-14
 ********************************************************************/
void initialization(void)
{
	////------------Sets up all ports as digial inputs-----------------------
	//IO port control
	ANSA = 0; // Make PORTA digital I/O
	TRISA = 0xFFFF; // Make PORTA all inputs
	ANSB = 0; // All port B pins are digital. Individual ADC are set in the readADC function
	TRISB = 0xFFFF; // Sets all of port B to input

	// pinDirectionIO(sclI2CPin, 0); //TRISBbits.TRISB8 = 0; // RB8 is an output


//	// Timer control (for WPS)
	T1CONbits.TCS = 0; // Source is Internal Clock (8MHz)
	T1CONbits.TCKPS = 0b11; // Prescalar to 1:256
	T1CONbits.TON = 1; // Enable the timer (timer 1 is used for the water sensor)

//        // Timer control (for getHandleAngle())
        T2CONbits.T32 = 0; // Using 16-bit timer2
        T2CONbits.TCKPS = 0b11; // Prescalar to 1:256 (Need prescalar of at least 1:8 for this)
        T2CONbits.TON = 1; // Starts 16-bit Timer2


	// UART control
	U1BRG = 51; // Set baud to 9600, FCY = 8MHz (#pragma config FNOSC = FRC)
	U1STA = 0;
	U1MODE = 0x8000; //enable UART for 8 bit data
	//no parity, 1 stop bit
	U1STAbits.UTXEN = 1; //enable transmit
        pinDirectionIO(waterPresenceSensorOnOffPin, 0); //makes water presence sensor pin an output.
	digitalPinSet(waterPresenceSensorOnOffPin, 1); //turns on the water presnece sensor.

	// From fona code (for enabling Texting)
	pinDirectionIO(pwrKeyPin, 0); //TRISBbits.TRISB6 = 0; //sets power key as an output (Pin 15)
	pinDirectionIO(simVioPin, 0); //TRISAbits.TRISA1 = 0; //sets Vio as an output (pin 3)

	//         Fona stuff
	digitalPinSet(simVioPin, 1); //PORTAbits.RA1 = 1; //Tells Fona what logic level to use for UART
	if (digitalPinStatus(statusPin) == 0){ //Checks see if the Fona is off pin
		digitalPinSet(pwrKeyPin, 0); //PORTBbits.RB6 = 0; //set low pin 15 for 100ms to turn on Fona
	}
	while (digitalPinStatus(statusPin) == 0) {} // Wait for Fona to power up
	digitalPinSet(pwrKeyPin, 1);//PORTBbits.RB6 = 1; // Reset the Power Key so it can be turned off later (pin 15)

	// Turn on SIM800
	 turnOnSIM();

                // PUTTY TEST (Serial Communication)
//         specifyAnalogPin(txPin, 0);    // make digital
//         specifyAnalogPin(rxPin, 0);    // make digital
//         pinDirectionIO(txPin, 0);       // make output
//         pinDirectionIO(rxPin, 1);       // make input

	// Moved the RTCCSet function up since we do not rely on network anymore
	configI2c();
	char seconds = 50;
	char minutes = 57;
	char hours = 14;
	char weekday = 4;
	char days = 12;
	char months = 8;
	char years = 15;
        depthSensorInUse = 0; // If Depth Sensor is in use, make a 1. Else make it zero.
	setTime(seconds, minutes, hours, weekday, days, months, years); // SS MM HH WW DD MM YY

	/*
	 *
         * Interrupt and internal clock commands for internal RTCC
        RTCCSet(); // Sets time; Pic asks Sim which asks cell tower to get current time
	_RTCWREN = 1; // allowing us to write to registers; Set Alarm for sending message
	ALCFGRPTbits.CHIME = 1; //don't need to reset alarm?
	ALCFGRPTbits.AMASK = 0b0110; //once a day
	ALCFGRPTbits.ALRMPTR = 0b0010; //sets pointer
	//The following two lines may not work
	ALRMVAL = 0x0000; //set day and month to 0 and decrements pointer
	ALRMVAL = alarmHour; //sets hour to 0 (12am), sets weekday to 0, and decrements pointer
	ALRMVAL = 0x0000;//getMinuteOffset(); //set 5 min after midnight and set 1 second after midnight-
	//*********************************************
	// Make random number between 12:01-12:06
	// Assigned 05-30-2014; completed 06-09-2014
	//*********************************************
	ALCFGRPTbits.ALRMEN = 1; //enables the alarm
	_RTCWREN = 0; //no longer able to write to registers
	IEC3bits.RTCIE = 1; //RTCC Interupt is enabled
	 *
	 */
//        tryToConnectToNetwork();
//        sendTextMessage("(\"t\":\"initialize\")");
	initAdc();


        batteryFloat = batteryLevel();
        char initBatteryString[20];
        initBatteryString[0] = 0;
        floatToString(batteryFloat, initBatteryString);
        char initialMessage[160];
        initialMessage[0] = 0;
        concat(initialMessage, "(\"t\":\"initialize\"");
        concat(initialMessage, ",\"b\":");
        concat(initialMessage, initBatteryString);
        concat(initialMessage, ")");


//        tryToConnectToNetwork();
//        sendTextMessage(initialMessage);
        sendMessage(initialMessage);
        //prevHour = getHourI2C();
        prevDay = getDateI2C();
        if (depthSensorInUse == 1){
            prevDayDepthSensor = BcdToDec(getDateI2C()) - 1; // The minus 1 is so that if the system is installed before noon we will receive data;
            delayMs(1000);
            if (BcdToDec(getHourI2C()) > 11){ // If it's already past noon on the first day, read the depthSensor now
                midDayDepthRead();
            }
        }

        sendTimeMessage();
}

void sendTimeMessage(void){
    char timeHourMessage[20];
        timeHourMessage[0] = 0;
        char timeMinuteMessage[20];
        timeMinuteMessage[0] = 0;
        char timeSecondMessage[20];
        timeSecondMessage[0];
        char timeMessage[160];
        timeMessage[0] = 0;
        char timeWeekMessage[20];
        timeWeekMessage[0] = 0;
        char timeDayMessage[20];
        timeDayMessage[0] = 0;
        char timeMonthMessage[20];
        timeMonthMessage[0];
        char timeYearMessage[20];
        timeYearMessage[0] = 0;


        longToString(BcdToDec(getHourI2C()), timeHourMessage);
        longToString(BcdToDec(getMinuteI2C()), timeMinuteMessage);
        longToString(BcdToDec(getSecondI2C()), timeSecondMessage);
        longToString(BcdToDec(getYearI2C()), timeYearMessage);
        longToString(BcdToDec(getWkdayI2C()), timeWeekMessage);
        longToString(BcdToDec(getDateI2C()), timeDayMessage);
        longToString(BcdToDec(getMonthI2C()), timeMonthMessage);

        concat(timeMessage, timeHourMessage);
        concat(timeMessage, ":");
        concat(timeMessage, timeMinuteMessage);
        concat(timeMessage, ":");
        concat(timeMessage, timeSecondMessage);
        concat(timeMessage, "   ");
        concat(timeMessage, "WeekDay: ");
        concat(timeMessage, timeWeekMessage);
        concat(timeMessage, "  ");
        concat(timeMessage, timeMonthMessage);
        concat(timeMessage, "/");
        concat(timeMessage, timeDayMessage);
        concat(timeMessage, "/");
        concat(timeMessage, timeYearMessage);
        concat(timeMessage, " \r\n");
        sendMessage(timeMessage);
}
/////////////////////////////////////////////////////////////////////
////                                                             ////
////                    STRING FUNCTIONS                         ////
////                                                             ////
/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function: longLength
 * Input: number
 * Output: length
 * Overview: Returns the number of digits in the given integer
 * Note: Library
 * TestDate: 06-03-2014
 ********************************************************************/
int longLength(long num)
{
	int length = 0;
	do
	{
		length++; //Increment length
		num /= 10; //Get rid of the one's place
	} while (num != 0);
	return length;
}

/*********************************************************************
 * Function: longToString
 * Input: integer and string
 * Output: None
 * Overview: Sets the given char array to an array filled with the digits of the given long
 * Note: Library
 * TestDate: 06-04-2014
 ********************************************************************/
void longToString(long num, char *numString)
{
	//Declares an array of digits to refer to
	char const digits[] = "0123456789";
	//Gets the number of digits in the given number
	int length = longLength(num);
	//Creates a char array with the appropriate size (plus 1 for the \0 terminating character)
	char *result = numString;
	// Add 1 to the length to make room for the '-' and inserts the '-' if the number is negative
	if (num < 0)
	{
		length++;
		result[0] = '-';
		num *= -1; // Make the number positive
	}
	//Sets i to be the end of the string
	int i = length - 1;
	//Loops through the char array and inserts digits
	do
	{
		//Set the current index of the array to the corresponding digit
		result[i] = digits[num % 10];
		//Divide num by 10 to lose the one's place
		num /= 10;
		i--;
	} while (num != 0);
	//Insert a terminating \0 character
	result[length] = '\0';
}

/*********************************************************************
 * Function: stringLength
 * Input: string
 * Output: Interger
 * Overview: Returns the number of characters (not including \0) in the given string
 * Note: Library
 * TestDate: 06-09-2014
 ********************************************************************/
int stringLength(char *string)
{
	int i = 0;
	//Checks for the terminating character
	while (string[i] != '\0')
	{
		i++;
	}
	return i;
}

/*********************************************************************
 * Function: concat
 * Input: Two strings
 * Output: None
 * Overview: Concatenates two strings
 * Note: Library
 * TestDate: 06-09-2014
 ********************************************************************/
void concat(char *dest, const char *src)
{
	//Increments the pointer to the end of the string
	while (*dest)
	{
		dest++;
	}
	//Assigns the rest of string two to the incrementing pointer
	while ((*dest++ = *src++) != '\0');
}

/*********************************************************************
 * Function: floatToString
 * Input: float myvalue and character myString
 * Output: None
 * Overview: Fills the character array with the digits in the given float
 * Note: Make the mantissa and exponent positive if they were negative
 * TestDate: 06-20-2014
 ********************************************************************/
//Fills the char array with the digits in the given float
void floatToString(float myValue, char *myString) //tested 06-20-2014
{
	int padLength = 0; // Stores the number of 0's needed for padding (2 if the fractional part was .003)
	long digit; // Stores a digit from the mantissa
	char digitString[5]; // Stores the string version of digit
	char mString[20]; // Stores the mantissa as a string
	char eString[20]; // Stores the exponent as a string
	int decimalShift; // Keeps track of how many decimal places we counted
	long exponent = (long)myValue; // Stores the exponent value of myValue
	float mantissa = myValue - (float)exponent; //Stores the fractional part
	int sLength = 0; // The length of the final string
	// Make the mantissa and exponent positive if they were negative
	if (myValue < 0)
	{
		mantissa *= -1;
		exponent *= -1;
	}
	// Counts the padding needed
	while (mantissa < 1 && mantissa != 0)
	{
		mantissa = mantissa * 10.0; // Stores the mantissa with the given decimal accuracy
		padLength++; // Increment the number of 0's needed
	}
	padLength--; // Subtract one because we don't want to count the last place shift
	mString[0] = '\0';
	eString[0] = '\0';
	digitString[0] = '\0';
	myString[0] = '\0';
	//Gets the string for the exponent
	longToString(exponent, eString);
	// Get the mantissa digits only if needed
	// (if padLength is -1, the number was a whole number. If it is above the decimal accuracy,
	// we had all 0's and do not need a mantissa
	if (padLength > -1 && padLength < decimalAccuracy)
	{
		// Updates the decimal place
		decimalShift = padLength;
		// Extracts the next one's place from the mantissa until we reached our decimal accuracy
		while (decimalShift < decimalAccuracy)
		{
			digit = (long)mantissa; // Get the next digit
			longToString(digit, digitString); // Convert the digit to a string
			concat(mString, digitString); // Add the digit string to the mantissa string
			mantissa = mantissa - (float)digit;
			mantissa = mantissa * 10; // Shift the decimal places to prepare for the next digit
			decimalShift++; // Update the decimal shift count
		}
		if (myValue < 0)
		{
			concat(myString, "-"); // Adds the '-' character
			sLength++; // Add one to the length for the '-' character
		}
		// Concatenates the exponent, decimal point, and mantissa together
		concat(myString, eString);
		concat(myString, ".");
		// Adds 0's to the mantissa string for each 0 needed for padding
		int i;
		for (i = 0; i < padLength; i++)
		{
			concat(myString, "0");
		}
		concat(myString, mString);
		//The length of the final string (lengths of the parts plus 1 for decimal point, 1 for \0 character, and the number of 0's)
		sLength += stringLength(eString) + stringLength(mString) + 2 + padLength;
		// Removes any trailing 0's
		while (myString[sLength - 2] == '0')
		{
			myString[sLength - 2] = '\0';
			sLength--;
		}
	}
	else
	{
		if (myValue < 0)
		{
			concat(myString, "-"); // Adds the '-' character
			sLength++; // Add one to the length for the '-' character
		}
		// Concatenates the exponent, decimal point, and mantissa together
		concat(myString, eString);
		//The length of the final string (lengths of the parts plus 1 for \0 character)
		sLength += stringLength(eString) + 1;
	}
	myString[sLength - 1] = '\0'; // Add terminating character
}

/////////////////////////////////////////////////////////////////////
////                                                             ////
////                    SIM FUNCTIONS                            ////
////                                                             ////
/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function: turnOffSIM
 * Input: None
 * Output: None
 * Overview: Turns of the SIM900
 * Note: Pic Dependent
 * TestDate: Not tested as of 03-05-2015
 ********************************************************************/
void turnOffSIM()
{
	while (digitalPinStatus(statusPin) == 1){ //Checks see if the Fona is on pin
		digitalPinSet(pwrKeyPin, 0); //PORTBbits.RB6 = 0; //set low pin 15 for 100ms to turn off Fona
	}
	while (digitalPinStatus(statusPin) == 0) {} // Wait for Fona to power up
	digitalPinSet(pwrKeyPin, 1);//PORTBbits.RB6 = 1; // Reset the Power Key so it can be turned off later (pin 15)


	// Turn off SIM800
//	while (digitalPinStatus(statusPin) == 1) // While STATUS light is on (SIM900 is on)
//	{
//		digitalPinSet(pwrKeyPin, 0); // Hold in PWRKEY button
//	}
//	digitalPinSet(pwrKeyPin, 0); // Let go of PWRKEY
}

/*********************************************************************
 * Function: turnOnSIM
 * Input: None
 * Output: None
 * Overview: Turns on SIM900
 * Note: Pic Dependent
 * TestDate: Not tested as of 03-05-2015
 ********************************************************************/
void turnOnSIM()
{
    digitalPinSet(simVioPin, 1); //PORTAbits.RA1 = 1; //Tells Fona what logic level to use for UART
	if (digitalPinStatus(statusPin) == 0){ //Checks see if the Fona is off pin
		digitalPinSet(pwrKeyPin, 0); //PORTBbits.RB6 = 0; //set low pin 15 for 100ms to turn on Fona
	}
	while (digitalPinStatus(statusPin) == 0) {} // Wait for Fona to power up
	digitalPinSet(pwrKeyPin, 1);//PORTBbits.RB6 = 1; // Reset the Power Key so it can be turned off later (pin 15)


//	while (digitalPinStatus(statusPin) == 0) // While STATUS light is not on (SIM900 is off)
//	{
//		digitalPinSet(pwrKeyPin, 1); // Hold in PWRKEY button
//	}
//
//	digitalPinSet(pwrKeyPin, 0); // Let go of PWRKEY
}

/*********************************************************************
 * Function: tryToConnectToNetwork
 * Input: None
 * Output: None
 * Overview: This function test for network status and attemps to connect to the
 * network. If no netork is found in a minute, the SIM is reset in order
 * to connect agian.
 * Note: Check connection to network
 * TestDate: Not tested as of 03-05-2015
 ********************************************************************/
void tryToConnectToNetwork()
{
	int networkTimeoutCount = 0; // Stores the number of times we reset the SIM
	int networkTimeout = 0; // Stores the number of times we did not have connection
	int networkConnectionCount = 0; // Stores the number of times we have detected a connection
	int keepTrying = 1; // A flag used to keep trying to connect to the network
	while (keepTrying) // Wait until connected to the network, or we tried for 20 seconds
	{
		delayMs(1000); // Delay for 1 second
		// Check for network take the appropriate action
		if (connectedToNetwork())
		{
			networkConnectionCount++;
			// 4 consecutive connections means we can exit the loop
			if (networkConnectionCount == 4)
			{
				keepTrying = 0;
			}
		}
		else
		{
			// If we have no network, reset the counter
			networkConnectionCount = 0;
			// Increase the network timeout
			networkTimeout++;
			// We tried to connect for 1 minute, so restart the SIM900
			if (networkTimeout > 60)
			{
				turnOffSIM();
				delayMs(3000);
				turnOnSIM();
				delayMs(3000);
				// Reset the network timeout
				networkTimeout = 0;
				networkTimeoutCount++;
				// If we have tried to reset 5 times, we give up and exit
				if (networkTimeoutCount == 5)
				{
					keepTrying = 0;
				}
			}
		}
	}
}

/*********************************************************************
 * Function: connectedToNetwork
 * Input: None
 * Output: pulseDistance
 * Overview: True when there is a network connection
 * Note: Pic Dependent
 * TestDate: Not tested as of 03-05-2015
 ********************************************************************/
int connectedToNetwork(void) //True when there is a network connection
{
	// Make sure you start at the beginning of the positive pulse
	if (digitalPinStatus(netLightPin) == 1) //(PORTBbits.RB14 == 1)
	{
		while (digitalPinStatus(netLightPin)){}; //(PORTBbits.RB14) {}; // TODO: Isn't this a great way to get stuck in a while loop?
	}
	// Wait for rising edge
	while ((digitalPinStatus(netLightPin) == 0)){};//PORTBbits.RB14 == 0) {}; // TODO: Isn't this a great way to get stuck in a while loop?
	// Reset the timer
	TMR1 = 0;
	// Get time at start of positive pulse
	int prevICTime = TMR1;
	// Wait for the pulse to go low
	while (digitalPinStatus(netLightPin)) {}; // TODO: Same as above
	// Wait for the pulse to go high again
	while (digitalPinStatus(netLightPin) == 0) {}; // TODO: Same as above
	// Get time at end of second positive pulse
	int currentICTime = TMR1;
	long pulseDistance = 0;
	if (currentICTime >= prevICTime)
	{
		pulseDistance = (currentICTime - prevICTime);
	}
	else
	{
		pulseDistance = (currentICTime - prevICTime + 0x10000);
	}
	//Check if this value is right
	return (pulseDistance >= networkPulseWidthThreshold); // True, when there is a network connection.
}

/*********************************************************************
 * Function: sendMessage()
 * Input: String
 * Output: None
 * Overview: Transmits the given characters along serial lines
 * Note: Library, Pic Dependent, sendTextMessage() uses this
 * TestDate: 06-02-2014
 ********************************************************************/
void sendMessage(char message[160])
{
	int stringIndex = 0;
	int delayIndex;
	U1BRG = 25; //set baud to 9600, assumes FCY=4Mhz/19200
	U1STA = 0;
	U1MODE = 0x8000; //enable UART for 8 bit data
	//no parity, 1 stop bit
	U1STAbits.UTXEN = 1; //enable transmit
	while (stringIndex < stringLength(message))
	{
		if (U1STAbits.UTXBF == 0)
		{
			U1TXREG = message[stringIndex];
			stringIndex++;
			for (delayIndex = 0; delayIndex < 1000; delayIndex++){}
		}
		else
		{
			for (delayIndex = 0; delayIndex < 30000; delayIndex++){}
		}
	}
}

char intToAscii(unsigned int integer)
{
	return (char)(integer + 48);
}

/*********************************************************************
 * Function: sendTextMessage()
 * Input: String
 * Output: None
 * Overview: sends a Text Message to which ever phone number is in the variable 'phoneNumber'
 * Note: Library
 * TestDate: 06-02-2014
 ********************************************************************/
void sendTextMessage(char message[160]) // Tested 06-02-2014
{
        turnOnSIM();
        delayMs(10000);
	sendMessage("AT+CMGF=1\r\n");//sets to text mode
	delayMs(250);
	sendMessage("AT+CMGS=\""); //beginning of allowing us to send SMS message
	sendMessage(phoneNumber);
	sendMessage("\"\r\n"); //middle of allowing us to send SMS message
	delayMs(250);
	sendMessage(message);
	delayMs(250);
	sendMessage("\x1A"); // method 2: sending hexidecimal representation
	// of 26 to sendMessage function (line 62)
	// & the end of allowing us to send SMS message
	delayMs(5000); // Give it some time to send the message
        turnOffSIM();
}

/////////////////////////////////////////////////////////////////////
////                                                             ////
////                   SENSOR FUNCTIONS                          ////
////                                                             ////
/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function: readWaterSensor
 * Input: None
 * Output: pulseWidth
 * Overview: RB5 is one water sensor, start at beginning of positive pulse
 * Note: Pic Dependent
 * TestDate: Not tested as of 03-05-2015
 ********************************************************************/
int readWaterSensor(void) // RB5 is one water sensor
{
	if (digitalPinStatus(waterPresenceSensorPin) == 1)
	{
		while (digitalPinStatus(waterPresenceSensorPin)) {}; //make sure you start at the beginning of the positive pulse
	}
	while (digitalPinStatus(waterPresenceSensorPin) == 0) {}; //wait for rising edge
	int prevICTime = TMR1; //get time at start of positive pulse
	while (digitalPinStatus(waterPresenceSensorPin)) {};
	int currentICTime = TMR1; //get time at end of positive pulse
	long pulseWidth = 0;
	if (currentICTime >= prevICTime)
	{
		pulseWidth = (currentICTime - prevICTime);
	}
	else
	{
		pulseWidth = (currentICTime - prevICTime + 0x100000000);
	}
	//Check if this value is right
	return (pulseWidth <= pulseWidthThreshold);
}

/*********************************************************************
 * Function: initAdc()
 * Input: None
 * Output: None
 * Overview: Initializes Analog to Digital Converter
 * Note: Pic Dependent
 * TestDate: 06-02-2014
 ********************************************************************/
void initAdc(void)
{
	// 10bit conversion
	AD1CON1 = 0; // Default to all 0s
	AD1CON1bits.ADON = 0; // Ensure the ADC is turned off before configuration
	AD1CON1bits.FORM = 0; // absolute decimal result, unsigned, right-justified
	AD1CON1bits.SSRC = 0; // The SAMP bit must be cleared by software
	AD1CON1bits.SSRC = 0x7; // The SAMP bit is cleared after SAMC number (see
	// AD3CON) of TAD clocks after SAMP bit being set
	AD1CON1bits.ASAM = 0; // Sampling begins when the SAMP bit is manually set
	AD1CON1bits.SAMP = 0; // Don't Sample yet
	// Leave AD1CON2 at defaults
	// Vref High = Vcc Vref Low = Vss
	// Use AD1CHS (see below) to select which channel to convert, don't
	// scan based upon AD1CSSL
	AD1CON2 = 0;
	// AD3CON
	// This device needs a minimum of Tad = 600ns.
	// If Tcy is actually 1/8Mhz = 125ns, so we are using 3Tcy
	//AD1CON3 = 0x1F02; // Sample time = 31 Tad, Tad = 3Tcy
	AD1CON3bits.SAMC = 0x1F; // Sample time = 31 Tad (11.6us charge time)
	AD1CON3bits.ADCS = 0x2; // Tad = 3Tcy
	// Conversions are routed through MuxA by default in AD1CON2
	AD1CHSbits.CH0NA = 0; // Use Vss as the conversion reference
	AD1CSSL = 0; // No inputs specified since we are not in SCAN mode
	// AD1CON2
}

//problem: is in radADC
/*********************************************************************
 * Function: readAdc()
 * Input: channel
 * Output: adcValue
 * Overview: check with accelerometer
 * Note: Pic Dependent
 * TestDate:
 ********************************************************************/
int readAdc(int channel) //check with accelerometer
{
	switch (channel)
	{
	case 0:
		specifyAnalogPin(depthSensorPin, 1);    //make depthSensor Analog
                pinDirectionIO(depthSensorPin, 1);
                pinSampleSelectRegister(depthSensorPin);
		break;
	case 2: //Currently unused, may be used in the future.
		specifyAnalogPin(Pin4, 1); // makes Pin4 analog
                pinDirectionIO(Pin4, 1);    // Pin4 in an input
                pinSampleSelectRegister(Pin4);  // Connect Pin4 as the S/H input

                //ANSBbits.ANSB0 = 1; // AN2 is analog
		//TRISBbits.TRISB0 = 1; // AN2 is an input
		//AD1CHSbits.CH0SA = 2; // Connect AN2 as the S/H input
		break;
	case 4:
		specifyAnalogPin(rxPin, 1);     // make rx analog
                pinDirectionIO(rxPin, 1); // makes rxPin an input
                pinSampleSelectRegister(rxPin); // Connect rxPin as the S/H input
		//ANSBbits.ANSB2 = 1; // AN4 is analog
		//TRISBbits.TRISB2 = 1; // AN4 is an input
		//AD1CHSbits.CH0SA = 4; // Connect AN4 as the S/H input
		break;
	case 11:
		specifyAnalogPin(xAxisAccelerometerPin, 1); // makes xAxis analog
                pinDirectionIO(xAxisAccelerometerPin, 1); // makes xAxis an input
                pinSampleSelectRegister(xAxisAccelerometerPin); // Connect xAxis as the S/H input
		//ANSBbits.ANSB13 = 1; // AN11 is analog
		//TRISBbits.TRISB13 = 1; // AN11 is an input
		//AD1CHSbits.CH0SA = 11; //Connect AN11 as the S/H input (sample and hold)
		break;
	case 12:
		specifyAnalogPin(yAxisAccelerometerPin, 1); // makes yAxis analog
                pinDirectionIO(yAxisAccelerometerPin, 1);    // makes yAxis an input
                pinSampleSelectRegister(yAxisAccelerometerPin); // Connect yAxis as the S/H input
		//PORTBbits.RB12 = 1; // AN12 is analog ***I changed this to ANSBbits.ANSBxx 03-31-2015
		//TRISBbits.TRISB12 = 1; // AN12 is an input
		//AD1CHSbits.CH0SA = 12; // Connect AN12 as the S/H input
		break;
	case 15:
		specifyAnalogPin(batteryLevelPin, 1);   // makes batteryLevelPin analog
                pinDirectionIO(batteryLevelPin, 1); // makes batteryLevelPin an input
                pinSampleSelectRegister(batteryLevelPin); // Connect batteryLevelPin
		break;
	}
	AD1CON1bits.ADON = 1; // Turn on ADC
	AD1CON1bits.SAMP = 1;
	while (!AD1CON1bits.DONE)
	{}
	unsigned int adcValue = ADC1BUF0;
	return adcValue;
}

/*********************************************************************
 * Function: getHandleAngle()
 * Input: None
 * Output: Float
 * Overview: Returns the current angle of the pump. The accelerometer
should be oriented on the pump handle so that when the
pump handle (the side the user is using) is down (water
present), the angle is positive. When the pump handle
(the side the user is using) is up (no water present),
the angle is negative.Gets a snapshot of the current sensor values.
 * Note: Library
 * NOTE2: It turns out that averaging the hangle angles would be the most accurate way to report pumping
 * TestDate: TBD
 ********************************************************************/
float getHandleAngle()
{
        //OLD getHandleAngle Code:
        signed int xValue = readAdc(xAxis) - adjustmentFactor; //added abs() 06-20-2014
	signed int yValue = readAdc(yAxis) - adjustmentFactor; //added abs() 06-20-2014
	float angle = atan2(yValue, xValue) * (180 / 3.141592); //returns angle in degrees 06-20-2014
	// Calculate and return the angle of the pump handle // TODO: 3.141592=PI, make that a constant
        if (angle > 20){
            angle = 20.0;
        }
        else if (angle < -30){
            angle = -30.0;
        }
	return angle;


}

/*********************************************************************
 * Function: initializeQueue()
 * Input: float
 * Output: None
 * Overview: Set all values in the queue to the intial value
 * Note: Library
 * TestDate: 06-20-2014
 ********************************************************************/
void initializeQueue(float value)
{
	int i = 0;
	for (i = 0; i < queueLength; i++)
	{
		angleQueue[i] = value;
	}
}

/*********************************************************************
 * Function: pushToQueue()
 * Input: float
 * Output: None
 * Overview: Shift values down one
 * Note: Library
 * TestDate: 06-20-2014
 ********************************************************************/
void pushToQueue(float value)
{
	int i = 0;
	for (i = 0; i < queueLength - 1; i++)
	{
		angleQueue[i] = angleQueue[i + i];
	}
	// Insert the value at the end of the queue
	angleQueue[queueLength - 1] = value;
}

/*********************************************************************
 * Function: queueAverage()
 * Input: None
 * Output: float
 * Overview: Takes the average of the queue
 * Note: Library
 * TestDate: NOT TESTED
 ********************************************************************/
float queueAverage()
{
	float sum = 0;
	int i = 0;
	// Sum up all the values in the queue
	for (i = 0; i < queueLength; i++)
	{
		sum += angleQueue[i];
	}
	// Returns the average after converting queueLength to a float
	return sum / (queueLength * 1.0);
}

/*********************************************************************
 * Function: queueDifference()
 * Input: None
 * Output: float
 * Overview: Returns the difference between the last and first numbers in the queue
 * Note: Library
 * TestDate: NOT TESTED
 ********************************************************************/
float queueDifference()
{
	return angleQueue[queueLength - 1] - angleQueue[0];
}

/*********************************************************************
 * Function: batteryLevel()
 * Input: None
 * Output: float
 * Overview: returns an output of a float with a value of the battery voltage compared to an
 * expected VCC of 3.6V
 * Note:
 * TestDate: 6/24/2015
 ********************************************************************/
float batteryLevel(void)//this has not been tested
{
	char voltageAvgFloatString[20];
	voltageAvgFloatString[0] = 0;
	float adcVal1;
	float adcVal2;
	float adcVal3;
	float adcAvg;
	float realVoltage;

	adcVal1 = readAdc(batteryVoltage); // - adjustmentFactor;

	delayMs(50);

	adcVal2 = readAdc(batteryVoltage); // - adjustmentFactor;

	delayMs(50);

	adcVal3 = readAdc(batteryVoltage); // - adjustmentFactor;


	adcAvg = (adcVal1 + adcVal2 + adcVal3) / 3;

	// V = adcVal / maxAdcVal * 1 / (voltage divider values) * VCC
        realVoltage = adcAvg / 1024;
        realVoltage *= 2.43;
        realVoltage *= 3.6;
	//realVoltage = adcAvg / 1024 * 1 / (100 / 243) * 3.6;

	//floatToString(battVoltage, voltageAvgFloatString);

	//return voltageAvgFloatString;

	return realVoltage;
}

/*********************************************************************
 * Function: readDepthSensor()
 * Input: None
 * Output: float
 * Overview: returns the depth of the probe in meters
 * Note: Library
 * TestDate: TBD
 ********************************************************************/
float readDepthSensor(void)
{
	float adcVal1;
	float adcVal2;
	float adcVal3;
	float adcAvg;
	float depthInMeters;
	float realVoltage;
	adcVal1 = readAdc(depthSensorPin);

	delayMs(50);

	adcVal2 = readAdc(depthSensorPin);

	delayMs(50);

	adcVal3 = readAdc(depthSensorPin);


	adcAvg = (adcVal1 + adcVal2 + adcVal3) / 3;

	// V = adcVal / maxAdcVal * VCC
	realVoltage = adcAvg / 1024 * 3.6;

	//depthInMeters = 2.2629 * realVoltage * realVoltage - 5.7605 * realVoltage + 3.4137;
        depthInMeters = 2.2629 * realVoltage;
        depthInMeters *= realVoltage;
        depthInMeters -= 5.7605 * realVoltage;
        depthInMeters +=3.4137;

	return depthInMeters;

}

/////////////////////////////////////////////////////////////////////
////                                                             ////
////                    I2C FUNCTIONS                            ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
/*********************************************************************
 * Function: SoftwareReset()
 * Input: None.
 * Output: None.
 * Overview: Resets software for I2C
 * Note: Contains a timeout loop that will disable then enable I2C
 ********************************************************************/
void SoftwareReset(void)
{
    	I2C1CONbits.I2CEN = 0; // doesn't Configure I2C pins as I2C (on pins 17 an 18)
        configI2c();    // Configure 12C pins as 12C (on pins 17 and 18

	IdleI2C(); // Ensure module is idle
	StartI2C(); // Initiate START condition
	while ( I2C1CONbits.SEN ); // Wait until START condition is complete

	int pulsesCreated = 0;
	pinDirectionIO(sclI2CPin, 1); // input
	pinDirectionIO(sdaI2CPin, 1); // input
	if((digitalPinStatus(sclI2CPin) == 1) && (digitalPinStatus(sdaI2CPin) == 0)) {
		pinDirectionIO(sdaI2CPin, 0); // make SDA an input
		digitalPinSet(sdaI2CPin, 0); // set SDA
		while ((pulsesCreated < 9) && digitalPinStatus(sdaI2CPin) == 0){ //PORTBbits.RB9 == 0){
			delaySCL();
			digitalPinSet(sclI2CPin, 1); //PORTBbits.RB9 = 1; // SCL
			delaySCL();
			digitalPinSet(sdaI2CPin, 0); //PORTBbits.RB9 = 0; // SCL
			delaySCL();
			pulsesCreated++;
		}
	}
	// put pulse code here
	RestartI2C(); // Initiate START condition
	while ( I2C1CONbits.RSEN ); // Wait until START condition is complete
	StopI2C(); // Initiate STOP condition
	while ( I2C1CONbits.PEN ); // Wait until STOP condition is complete
}

/*********************************************************************
 * Function: IdleI2C()
 * Input: None.
 * Output: None.
 * Overview: Waits for bus to become Idle
 * Note: Contains a timeout loop that will disable then enable I2C
 ********************************************************************/
unsigned int IdleI2C(void)
{
	int timeOut = 0;
	while (I2C1STATbits.TRSTAT){//Wait for bus Idle
		if (timeOut == 1300){ // time out loop incase I2C gets stuck
			SoftwareReset();
			invalid=0xff;
			return;
		}
	}
	timeOut++;
}


/*********************************************************************
 * Function: StartI2C()
 * Input: None.
 * Output: None.
 * Overview: Generates an I2C Start Condition
 * Note: Contains a timeout loop that will disable then enable I2C
 ********************************************************************/
unsigned int StartI2C(void)
{
	//This function generates an I2C start condition and returns status
	//of the Start.
	int timeOut = 0;
	I2C1CONbits.SEN = 1; //Generate Start COndition
	while (I2C1CONbits.SEN) //Wait for Start COndition
	{
		if (timeOut == 1300){ // time out loop incase I2C gets stuck
			SoftwareReset();
			invalid=0xff;
			break;
		}
		timeOut++;
	}
	//return(I2C1STATbits.S); //Optionally return status
}

/*********************************************************************
 * Function: StopI2C()
 * Input: None.
 * Output: None.
 * Overview: Generates a bus stop condition
 * Note: None
 ********************************************************************/
unsigned int StopI2C(void)
{
	//This function generates an I2C stop condition and returns status
	//of the Stop.
	int timeOut = 0;

	I2C1CONbits.PEN = 1; //Generate Stop Condition
	while (I2C1CONbits.PEN) //Wait for Stop
	{
		if (timeOut == 1300){ // time out loop incase I2C gets stuck
			SoftwareReset();
			invalid=0xff;
			break;
		}
		timeOut++;
	}
	//return(I2C1STATbits.P); //Optional - return status
}

/*********************************************************************
 * Function: RestartI2C()
 * Input: None.
 * Output: None.
 * Overview: Generates a restart condition and optionally returns status
 * Note: None
 ********************************************************************/
void RestartI2C(void)
{
	//This function generates an I2C Restart condition and returns status
	//of the Restart.
	int timeOut = 0;
	I2C1CONbits.RSEN = 1; //Generate Restart
	while (I2C1CONbits.RSEN) //Wait for restart
	{
		if (timeOut == 1300){ // time out loop incase I2C gets stuck
			SoftwareReset();
			invalid=0xff;
			break;
		}
		timeOut++;
	}
	//return(I2C1STATbits.S); //Optional - return status
}

void NackI2C(void)
{
	int timeOut = 0;
	I2C1CONbits.ACKDT = 1;
	I2C1CONbits.ACKEN = 1;
	while (I2C1CONbits.ACKEN)
	{
		if (timeOut == 1300)
		{ // time out loop incase I2C gets stuck
			SoftwareReset();
			invalid=0xff;
			break;
		}
		timeOut++;
	}
}

void AckI2C(void)
{
	int timeOut = 0;
	I2C1CONbits.ACKDT = 0;
	I2C1CONbits.ACKEN = 1;
	while (I2C1CONbits.ACKEN)
	{
		if (timeOut == 1300)
		{ // time out loop incase I2C gets stuck
			SoftwareReset();
			invalid=0xff;
			break;
		}
		timeOut++;
	}
}

void configI2c(void)
{
	//From Jake's
	I2C1CONbits.A10M = 0; //Use 7-bit slave addresses
	I2C1CONbits.DISSLW = 1; // Disable Slew rate
	I2C1CONbits.IPMIEN = 0; //should be set to 0 when master
	//IFS1bits.MI2C1IF = 0; // Disable Interupt
	//^From Jake's
	I2C1BRG = 0x4E; // If Fcy = 8 Mhz this will set the baud to 100 khz
	I2C1CONbits.I2CEN = 1; // Configures I2C pins as I2C (on pins 17 an 18)
}
/*********************************************************************
 * Function: WriteI2C()
 * Input: Byte to write.
 * Output: None.
 * Overview: Writes a byte out to the bus
 * Note: None
 ********************************************************************/
void WriteI2C(unsigned char byte)
{
	//This function transmits the byte passed to the function
	int timeOut1 = 0;
	while (I2C1STATbits.TRSTAT)//Wait for bus to be idle
	{
		if (timeOut1 == 1300)
		{ // time out loop incase I2C gets stuck
			SoftwareReset();
			invalid=0xff;
			return;
		}
		timeOut1++;

	}
	I2C1TRN = byte; //Load byte to I2C1 Transmit buffer
	int timeOut2 = 0;
	while (I2C1STATbits.TBF) //wait for data transmission
	{
		if (timeOut2 == 1300)
		{ // time out loop incase I2C gets stuck
			SoftwareReset();
			invalid=0xff;
			return;
		}
		timeOut2++;

	}


}

/*********************************************************************
 * Function: ReadI2C()
 * Input: None
 * Output: Returns one Byte from Slave device
 * Overview: Receives Byte & Writes a Nack out to the bus
 * Note: None
 ********************************************************************/
unsigned int ReadI2C(void)
{
	int timeOut1 = 0;
	int timeOut2 = 0;
	I2C1CONbits.ACKDT = 1; // Prepares to send NACK
	I2C1CONbits.RCEN = 1; // Gives control of clock to Slave device
	while (!I2C1STATbits.RBF) // Waits for register to fill up
	{
		if (timeOut1 == 1300){ // time out loop incase I2C gets stuck
			SoftwareReset();
                        invalid = 0xff;
			return 0xff; // invalid
		}
		timeOut1++;

	}
	I2C1CONbits.ACKEN = 1; // Sends NACK or ACK set above
	while (I2C1CONbits.ACKEN) // Waits till ACK is sent (hardware reset)
	{
		if (timeOut2 == 1300){ // time out loop incase I2C gets stuck
			SoftwareReset();
                        invalid = 0xff;
			return 0xff; //invalid

		}
		timeOut2++;

	}
	return I2C1RCV; // Returns data
}

/*********************************************************************
 * Function: delaySCL()
 * Input: None
 * Output: None
 * Overview: Pulse length for I2C pulse
 * Note: None
 ********************************************************************/
void delaySCL(void)
{
	int timeKiller = 0; //don't delete
	int myIndex = 0;
	while (myIndex < 5)
	{
		myIndex++;
	}
}


/////////////////////////////////////////////////////////////////////
////                                                             ////
////                    RTCC FUNCTIONS                           ////
////                                                             ////
/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function: readRTCC
 * Input: enum RTCCaddress
 * Output: None
 * Overview: reads from the register specified by the input
 * Note: None
 ********************************************************************/

int readRTCC(enum RTCCregister RTCCregister)
{
    unsigned char address;
    unsigned char mask;
    int data;

    switch(RTCCregister)
    {
        case SEC_REGISTER:
            address = 0x00;
            mask = 0x7F;
            break;

        case MIN_REGISTER:
            address = 0x01;
            mask = 0x7F;
            break;

        case HOUR_REGISTER:
            address = 0x02;
            mask = 0x3F;
            break;

        case WKDAY_REGISTER:
            address = 0x03;
            mask = 0x07;
            break;

        case DATE_REGISTER:
            address = 0x04;
            mask = 0x3F;
            break;

        case MONTH_REGISTER:
            address = 0x05;
            mask = 0x1F;
            break;

        case YEAR_REGISTER:
            address = 0x06;
            mask = 0xFF;
            break;
    }

	configI2c(); // sets up I2C
	StartI2C();
	WriteI2C(0xde); // MCP7490N device address + write command
	IdleI2C();
	WriteI2C(address); // device address for the given register on MCP7490N
	IdleI2C();
	RestartI2C();
	IdleI2C();
	WriteI2C(0xdf); // MCP7490N device address + read command
	IdleI2C();
	data = (int)ReadI2C();
	StopI2C();
        if(invalid == 0xFF)
        {
            invalid = 0;
            data = readRTCC(address);
        }
	data = data & mask; // removes unnecessary bits (mostly control bits and other non-time data)
	return data; // returns the time in address as a BCD number

}

/*********************************************************************
 * Function: turnOffClockOscilator()
 * Input: None
 * Output: None
 * Overview: Turns off RTCC Oscillator MCP7940N so it can be
 * set
 * Note: None
 ********************************************************************/
void turnOffClockOscilator(void)
{
	// turns off oscilator to prepare to set time
	StartI2C();
	WriteI2C(0xde); //Device Address (RTCC) + Write Command
	IdleI2C();
	WriteI2C(0x00); //address reg. for sec
	IdleI2C();
	WriteI2C(0x00); //Turn off oscillator and sets seconds to 0
	IdleI2C();
        if(invalid == 0xFF)
        {
            invalid = 0;
            turnOffClockOscilator();
        }
	StopI2C();
}

int getSecondI2C(void) //may want to pass char address to it in the future
{
	int sec; // temp var to hold seconds information

	configI2c(); // sets up I2C
	StartI2C();
	WriteI2C(0xde); // MCP7490N device address + write command
	IdleI2C();
	WriteI2C(0x00); // device address for the Seconds register on MCP7490N
	IdleI2C();
	RestartI2C();
	IdleI2C();
	WriteI2C(0xdf); // MCP7490N device address + read command
	IdleI2C();
	sec = (int)ReadI2C();
	StopI2C();
        if(invalid == 0xFF)
        {
            invalid = 0;
            sec = getSecondI2C();
        }
	sec = sec & 0x7f; // removes Oscillator bit
	//sec = BcdToDec(sec); // converts sec to a decimal number
	return sec; // returns the time in sec as a demimal number
}

int getMinuteI2C(void)
{
	int min; // temp var to hold seconds information
	configI2c(); // sets up I2C
	StartI2C();
	WriteI2C(0xde); // MCP7490N device address + write command
	IdleI2C();
	WriteI2C(0x01); // device address for the minutes register on MCP7490N
	IdleI2C();
	RestartI2C();
	IdleI2C();
	WriteI2C(0xdf); // MCP7490N device address + read command
	IdleI2C();
	min = (int)ReadI2C();
	StopI2C();
        if(invalid == 0xFF)
        {
            invalid = 0;
            min = getMinuteI2C();
        }
	min = min & 0x7f; // removes unused bit
	//min = BcdToDec(min); // converts min to a decimal number
	return min; // returns the time in min as a demimal number
}

int getHourI2C(void)
{
	int hr; // temp var to hold seconds information
	configI2c(); // sets up I2C
	StartI2C();
	WriteI2C(0xde); // MCP7490N device address + write command
	IdleI2C();
	WriteI2C(0x02); // device address for the hours register on MCP7490N
	IdleI2C();
	RestartI2C();
	IdleI2C();
	WriteI2C(0xdf); // MCP7490N device address + read command
	IdleI2C();
	hr = (int)ReadI2C();
	StopI2C();

        if(invalid == 0xFF)
        {
            invalid = 0;
            hr = getHourI2C();
        }
	hr = hr & 0x3f; // removes unused bits
	//hr = BcdToDec(hr); // converts hr to a decimal number
	return hr; // returns the time in hr as a demimal number
}

int getYearI2C(void)
{
	int yr; // temp var to hold seconds information
	configI2c(); // sets up I2C
	StartI2C();
	WriteI2C(0xde); // MCP7490N device address + write command
	IdleI2C();
	WriteI2C(0x06); // device address for the years register on MCP7490N
	IdleI2C();
	RestartI2C();
	IdleI2C();
	WriteI2C(0xdf); // MCP7490N device address + read command
	IdleI2C();
	yr = (int)ReadI2C();
	StopI2C();
        if(invalid == 0xFF)
        {
            invalid = 0;
            yr = getYearI2C();
        }
	return yr; // returns the time in hr as a demimal number
}

int getMonthI2C(void)
{
	int mnth; // temp var to hold seconds information
	configI2c(); // sets up I2C
	StartI2C();
	WriteI2C(0xde); // MCP7490N device address + write command
	IdleI2C();
	WriteI2C(0x05); // device address for the years register on MCP7490N
	IdleI2C();
	RestartI2C();
	IdleI2C();
	WriteI2C(0xdf); // MCP7490N device address + read command
	IdleI2C();
	mnth = (int)ReadI2C();
	StopI2C();
        if(invalid == 0xFF)
        {
            invalid = 0;
            mnth = getMonthI2C();
        }
        mnth = mnth & 0x1F;
	return mnth; // returns the time in hr as a demimal number

}
int getWkdayI2C(void)
{
	unsigned char wkday; // temp var to hold seconds information
	configI2c(); // sets up I2C
	StartI2C();
	WriteI2C(0xde); // MCP7490N device address + write command
	IdleI2C();
	WriteI2C(0x03); // device address for the years register on MCP7490N
	IdleI2C();
	RestartI2C();
	IdleI2C();
	WriteI2C(0xdf); // MCP7490N device address + read command
	IdleI2C();
	wkday = (int)ReadI2C();
	StopI2C();
        if(invalid == 0xFF)
        {
            invalid = 0;
            wkday = getWkdayI2C();
        }
	wkday = wkday & 0x07; // converts yr to a decimal number
	return wkday; // returns the time in hr as a demimal number
}

int getDateI2C(void)
{
	int date; // temp var to hold dat information
	configI2c(); // sets up I2C
	StartI2C();
	WriteI2C(0xde); // MCP7490N device address + write command
        IdleI2C();
	WriteI2C(0x04); // device address for the date register on MCP7490N
        IdleI2C();
        RestartI2C();
        IdleI2C();
        WriteI2C(0xdf); // MCP7490N device address + read command
        IdleI2C();
        date = (int)ReadI2C();
	StopI2C();
        if(invalid == 0xFF)
        {
            invalid = 0;
            date = getDateI2C();
        }
	date = date & 0x3f; // removes unused bits
	//date = BcdToDec(date); // converts yr to a decimal number
	return date; // returns the time in hr as a demimal number
}

/*********************************************************************
 * Function: setTime()
 * Input: SS MM HH WW DD MM YY
 * Output: None
 * Overview: Sets time for MCP7940N
 * Note: uses DecToBcd and I2C functions
 ********************************************************************/
void setTime(char sec, char min, char hr, char wkday, char date, char month, char year)
{
	int leapYear;
	if (year % 4 == 0)
	{
		leapYear = 1; //Is a leap Year
	}
	else
	{
		leapYear = 0; //Is not a leap Year
	}
	char BCDsec = DecToBcd(sec); // To BCD
	char BCDmin = DecToBcd(min); // To BCD
	char BCDhr = DecToBcd(hr);
	char BCDwkday = DecToBcd(wkday); // To BCD
	char BCDdate = DecToBcd(date);
	char BCDmonth = DecToBcd(month); // To BCD
	char BCDyear = DecToBcd(year);
	BCDsec = BCDsec | 0x80; // add turn on oscilator bit
	BCDhr = BCDhr & 0b10111111; // makes 24 hr time
	BCDwkday = BCDwkday | 0b00001000; // the 0 says the external battery backup supply is enabled.
	// To enable: Flip bits and OR it to turn on (NOT CURRENTLY ENABLED).
	if (leapYear == 0)
	{
		BCDmonth = BCDmonth & 0b11011111; //Not a leap year
	}
	else
	{
		BCDmonth = BCDmonth | 0b00100000; //Is a leap year
	}
	configI2c();
	turnOffClockOscilator();
	//------------------------------------------------------------------------------
	// sets clock
	//------------------------------------------------------------------------------
	StartI2C();
	WriteI2C(0xDE); //Device Address (RTCC) + Write Command
	IdleI2C();
	WriteI2C(0x01); //Adress for minutes
	IdleI2C();
	WriteI2C(BCDmin); //Load min
	IdleI2C();
	WriteI2C(BCDhr); //Load hour
	IdleI2C();
	WriteI2C(BCDwkday); //Load day of week
	IdleI2C();
	WriteI2C(BCDdate); //Load Date
	IdleI2C();
	WriteI2C(BCDmonth); //Load Month
	IdleI2C();
	WriteI2C(BCDyear); // Load Year
	IdleI2C();
	StopI2C();
	//------------------------------------------------------------------------------
	// Sets seconds and turns on oscilator
	//------------------------------------------------------------------------------
	StartI2C();
	WriteI2C(0xDE); //Device Address (RTCC) + Write Command
	IdleI2C();
	WriteI2C(0x00); //address reg. for sec
	IdleI2C();
	WriteI2C(BCDsec); //Turn on oscillator and sets seconds
	IdleI2C();
	StopI2C();
}

/////////////////////////////////////////////////////////////////////
////                                                             ////
////                    MISC FUNCTIONS                           ////
////                                                             ////
/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function: degToRad()
 * Input: float
 * Output: float
 * Overview: Converts angles in degrees to angle in radians.
 * Note: Library
 * TestDate: 06-20-2014
 ********************************************************************/
float degToRad(float degrees)
{
	return degrees * (3.141592 / 180);
}

/*********************************************************************
 * Function: delayMs()
 * Input: milliseconds
 * Output: None
 * Overview: Delays the specified number of milliseconds
 * Note: Depends on Clock speed. Pic Dependent
 * TestDate: 05-20-14
 ********************************************************************/
void delayMs(int ms)
{
	int myIndex;
	while (ms > 0)
	{
		myIndex = 0;
		while (myIndex < 667)
		{
			myIndex++;
		}
		ms--;
	}
}

//Returns the decimal value for the lower 8 bits in a 16 bit BCD (Binary Coded Decimal)
/*********************************************************************
 * Function: getLowerBCDAsDecimal
 * Input: int bcd
 * Output: Decimal verision of BCD added in (lower byte)
 * Overview: Returns the decimal value for the lower 8 bits in a 16 bit BCD (Binary Coded Decimal)
 * Note: Library
 * TestDate: 06-04-2014
 ********************************************************************/
int getLowerBCDAsDecimal(int bcd) //Tested 06-04-2014
{
	//Get the tens digit (located in the second nibble from the right)
	//by shifting off the ones digit and anding
	int tens = (bcd >> 4) & 0b0000000000001111;
	//Get the ones digit (located in the first nibble from the right)
	//by anding (no bit shifting)
	int ones = bcd & 0b0000000000001111;
	//Returns the decimal value by multiplying the tens digit by ten
	//and adding the ones digit
	return (tens * 10) + ones;
}

//Returns the decimal value for the upper 8 bits in a 16 bit BCD (Binary Coded Decimal)
/*********************************************************************
 * Function: getUpperBCDAsDecimal
 * Input: int bcd
 * Output: Decimal verision of BCD added in (upper byte)
 * Overview: Returns the decimal value for the Upper 8 bits in a 16 bit BCD (Binary Coded Decimal)
 * Note: Library
 * TestDate: 06-04-2014
 ********************************************************************/
int getUpperBCDAsDecimal(int bcd) //Tested 06-04-2014
{
	//Get the tens digit (located in the first nibble from the left)
	//by shifting off the rest and anding
	int tens = (bcd >> 12) & 0b0000000000001111;
	//Get the ones digit (located in the second nibble from the left)
	//by shifting off the rest and anding
	int ones = (bcd >> 8) & 0b0000000000001111;
	//Returns the decimal value by multiplying the tens digit by ten
	//and adding the ones digit
	return (tens * 10) + ones;
}

//Returns the hour of day from the internal clock
/*********************************************************************
 * Function: getTimeHour
 * Input: None
 * Output: hourDecimal
 * Overview: Returns the hour of day from the internal clock
 * Note: Pic Dependent
 * TestDate: 06-04-2014
 ********************************************************************/
//Tested 06-04-2014
int getTimeHour(void) //to determine what volume variable to use;
{
	//don't want to write, just want to read
	_RTCWREN = 0;
	//sets the pointer to 0b01 so that reading starts at Weekday/Hour
	_RTCPTR = 0b01;
	// Ask for the hour from the internal clock
	int myHour = RTCVAL;
	int hourDecimal = getLowerBCDAsDecimal(myHour);
	return hourDecimal;
}

/* First, retrieve time string from the SIM 900
Then, parse the string into separate strings for each time partition
Next, translate each time partition, by digit, into a binary string
Finally, piece together strings (16bytes) and write them to the RTCC */
// Tested 06-02-2014
/*********************************************************************
 * Function: timeStamp()
 * Input: void
 * Output: long timeStampValue
 * Overview: Returns the current time in seconds (the seconds passed so far in the day)
 * Note:
 * TestDate: 06-04-2014
 ********************************************************************/
long timeStamp(void)
{
	long timeStampValue = 0;
	//Set the pointer to 0b01 so that reading starts at Weekday/Hour
	_RTCPTR = 0b01; // decrements with read or write
	_RTCWREN = 0; //don't want to write, just want to read
	long binaryWeekdayHour = RTCVAL; // write wkdy & hour to variable, dec. RTCPTR
	long binaryMinuteSecond = RTCVAL; // write min & sec to variable, dec. RTCPTR
	//For some reason, putting the multiplication for hours on one line like this:
	//
	// timeStampValue = getLowerBCDAsDecimal(binaryWeekdayHour) * 60 * 60;
	//
	//caused an error. We would get some unknown value for the timestamp, so
	//we had to break the code up across multiple lines. So don't try to
	//simplify this!
	timeStampValue = getLowerBCDAsDecimal(binaryWeekdayHour);
	timeStampValue = timeStampValue * 60 * 60;
	timeStampValue = timeStampValue + (getUpperBCDAsDecimal(binaryMinuteSecond) * 60);
	timeStampValue = timeStampValue + getLowerBCDAsDecimal(binaryMinuteSecond);
	return timeStampValue; //timeStampValue;
}

/*********************************************************************
 * Function: pressReset()
 * Input: None
 * Output: None
 * Overview: Resets the values
 * Note: Previously used to reset the RTCC, but currently does not.
 * TestDate: 06-17-2014
 ********************************************************************/
void pressReset() //Tested 06-17-2014
{
	//Variable reset (all the variable of the message)
	longestPrime = 0;
	leakRateLong = 0;
	volume02 = 0;
	volume24 = 0;
	volume46 = 0;
	volume68 = 0;
	volume810 = 0;
	volume1012 = 0;
	volume1214 = 0;
	volume1416 = 0;
	volume1618 = 0;
	volume1820 = 0;
	volume2022 = 0;
	volume2224 = 0;
	//getTime(); //Reset RTCC
}

/*********************************************************************
 * Function: translate()
 * Input: String
 * Output: int binaryNumber
 * Overview: The following integers are used for turning the corresponding time-value strings
into binary numbers that are used to program the RTCC registers
 * Note: Library
 * TestDate: 06-02-2014
 ********************************************************************/
int translate(char digit)
{
	int binaryNumber;
	if (digit == '0')
	{
		binaryNumber = 0b0000;
	}
	else if (digit == '1')
	{
		binaryNumber = 0b0001;
	}
	else if (digit == '2')
	{
		binaryNumber = 0b0010;
	}
	else if (digit == '3')
	{
		binaryNumber = 0b0011;
	}
	else if (digit == '4')
	{
		binaryNumber = 0b0100;
	}
	else if (digit == '5')
	{
		binaryNumber = 0b0101;
	}
	else if (digit == '6')
	{
		binaryNumber = 0b0110;
	}
	else if (digit == '7')
	{
		binaryNumber = 0b0111;
	}
	else if (digit == '8')
	{
		binaryNumber = 0b1000;
	}
	else if (digit == '9')
	{
		binaryNumber = 0b1001;
	}
	return binaryNumber;
}

/*********************************************************************
 * Function: RTCCSet()
 * Input: None
 * Output: None
 * Overview: Get time string from SIM900
 * Note: Pic Dependent
 * TestDate: 06-02-2014
 ********************************************************************/
void RTCCSet(void){
	// Write the time to the RTCC
	// The enclosed code was graciously donated by the KWHr project
	__builtin_write_RTCWEN(); //does unlock sequence to enable write to RTCC, sets RTCWEN
	RCFGCAL = 0b0010001100000000;
	RTCPWC = 0b0000010100000000;
	_RTCPTR = 0b11;// decrements with read or write
	// Thanks KWHr!!!
	RTCVAL = getYearI2C();
	RTCVAL = getDateI2C() + (getMonthI2C() << 8);
	RTCVAL = getHourI2C() + (getWkdayI2C() << 8);
	RTCVAL = getSecondI2C() + (getMinuteI2C() << 8); // = binaryMinuteSecond;
	_RTCEN = 1; // = 1; //RTCC module is enabled
	_RTCWREN = 0; // = 0; // disable writing
}

//Returns the minutes and seconds (in BCD) to set the alarm to.
//Generates a random number of seconds between 1 and the alarmMinuteMax
//global variable to use for the minutes and seconds.
/*********************************************************************
 * Function: getMinuteOffset()
 * Input: None
 * Output: int time in BCD
 * Overview: Randomizes sending of text messages a couple minutes after midnight.
 * Note: Library
 * TestDate: 06-13-2014
 ********************************************************************/
int getMinuteOffset()
{
	//Get the number of seconds possible in alarmMinuteMax minuites plus 10 seconds
	//Plus 10 seconds is so that we aren't calling the alarm right at midnight
	int minutesInSeconds = (alarmMinuteMax * 60) + 10;
	//Sets the seed randomly based on the time of day
	long time = timeStamp(); // Gets the time of day in seconds (long)
	int timeConverted = (int)(time / 3); // Convert the time into an int to be supported by srand()
	srand(timeConverted);
	//Get a random time (in seconds)
	int randomTime = rand() % minutesInSeconds;
	//Get the minute part (plus the starting offset minute)
	int minutes = (randomTime / 60) + alarmStartingMinute;
	//Get the remaining seconds after minutes are taken out
	int seconds = randomTime % 60;
	//Get the tens and ones place for the minute
	int minuteTens = minutes / 10;
	int minuteOnes = minutes % 10;
	//Get the tens and ones place for the second
	int secondsTens = seconds / 10;
	int secondsOnes = seconds % 10;
	// Five minutes and one second (for an example reference)
	// 0x0501
	// 0b0000 0101 0000 0001
	//Get the time in BCD by shifting the minutes tens place
	int timeInBCD = minuteTens << 12;
	//Add the shifted minutes ones place
	timeInBCD = timeInBCD + (minuteOnes << 8);
	//Add the shifted seconds tens place
	timeInBCD = timeInBCD + (secondsTens << 4);
	//Add the seconds ones place
	timeInBCD = timeInBCD + secondsOnes;
	return timeInBCD;
}

//This function converts a BCD to DEC
//Input: BCD Value
//Returns: Hex Value
char BcdToDec(char val)
{
	return ((val / 16 * 10) + (val % 16));
}

//This function converts HEX to BCD
//Input: Hex Value
//Returns: BCD Value
char DecToBcd(char val)
{
	return ((val / 10 * 16) + (val % 10));
}
void midDayDepthRead (void) {
    if (depthSensorInUse == 1){
            pinDirectionIO(depthSensorOnOffPin, 0); //makes depth sensor pin an output.
            digitalPinSet(depthSensorOnOffPin, 1); //turns on the depth sensor.

            delayMs(30000); // Wait 30 seconds for the depth sensor to power up

            midDayDepth = readDepthSensor();

            digitalPinSet(depthSensorOnOffPin, 0); //turns off the depth sensor.
            delayMs(1000);
            prevDayDepthSensor = BcdToDec(getDateI2C());

        }
}
void midnightMessage(void)
{
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
        char batteryFloatString[20];
        batteryFloatString[0] = 0;
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
        floatToString(batteryFloat, batteryFloatString);
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
        concat(dataMessage, ",\"b\":");
        concat(dataMessage, batteryFloatString);
        if (depthSensorInUse == 1){ // if you have a depth sensor
            pinDirectionIO(depthSensorOnOffPin, 0); //makes depth sensor pin an output.
            digitalPinSet(depthSensorOnOffPin, 1); //turns on the depth sensor.

            delayMs(30000); // Wait 30 seconds for the depth sensor to power up

            char maxDepthLevelString[20];
            maxDepthLevelString[0] = 0;
            char minDepthLevelString[20];
            minDepthLevelString[0] = 0;
            float currentDepth = readDepthSensor();
            if (midDayDepth > currentDepth){
                floatToString(midDayDepth, maxDepthLevelString);
                floatToString(currentDepth, minDepthLevelString);
            }
            else{
                floatToString(currentDepth, maxDepthLevelString);
                floatToString(midDayDepth, minDepthLevelString);
                
            }
            concat(dataMessage, ",\"d\":<");
            concat(dataMessage, maxDepthLevelString);
            concat(dataMessage, ",");
            concat(dataMessage, minDepthLevelString);
            concat(dataMessage, ">");

            digitalPinSet(depthSensorOnOffPin, 0); //turns off the depth sensor.
        }
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

        // Try to establish network connection
//	tryToConnectToNetwork();
	delayMs(2000);
	// Send off the data
//	sendTextMessage(dataMessage);
        sendMessage(dataMessage);
        sendMessage(" \r \n");

//        prevHour = getHourI2C();
        prevDay = getDateI2C();

	pressReset();
	////////////////////////////////////////////////
	// Should we put the SIM back to sleep here?
	////////////////////////////////////////////////
	RTCCSet(); // updates the internal time from the external RTCC if the internal RTCC got off any through out the day

}



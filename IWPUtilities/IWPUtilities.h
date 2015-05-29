/* 
 * File:   IWPUtilities.h
 * Author: js1715
 *
 * Created on May 29, 2015, 4:41 PM
 */

#ifndef IWPUTILITIES_H
#define	IWPUTILITIES_H

// ****************************************************************************
// *** Include Statemets ******************************************************
// ****************************************************************************

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

//These variables were changed to be constants so that their values would
//not be changed accidentally - 6/6/14 Avery deGruchy
// ****************************************************************************
// *** Constants **************************************************************
// ****************************************************************************
const int xAxis = 11; // analog pin connected to x axis of accelerometer
const int yAxis = 12; // analog pin connected to y axis of accelerometer
const int batteryVoltage = 15;                  // analog pin connected to the battery
const float MKII = 0.467; // 0.4074 L/Radian; transfer variable for mkII delta handle angle to outflow
const float leakSensorVolume = 0.01781283; // This is in Liters; pipe dia. = 33mm; rod diam 12 mm; gage length 24mm
const int alarmHour = 0x0000; // The weekday and hour (24 hour format) (in BCD) that the alarm will go off
const int alarmStartingMinute = 1; // The minimum minute that the alarm will go off
const int alarmMinuteMax = 5; // The max number of minutes to offset the alarm (the alarmStartingMinute + a random number between 0 and this number)
const int adjustmentFactor = 511; // Used to ajust the values read from the accelerometer
const int pulseWidthThreshold = 0x0800; // The value to check the pulse width against (2048)
const int networkPulseWidthThreshold = 0x4E20; // The value to check the pulse width against (about 20000)
const int upstrokeInterval = 10; // The number of milliseconds to delay before reading the upstroke
int waterPrimeTimeOut = 7000; // Equivalent to 7 seconds (in 50 millisecond intervals); 50 = upstrokeInterval
long leakRateTimeOut = 18000; // Equivalent to 18 seconds (in 50 millisecond intervals); 50 = upstrokeInterval
long timeBetweenUpstrokes = 3000; // 3 seconds (based on upstrokeInterval)
const int decimalAccuracy = 3; // Number of decimal places to use when converting floats to strings
const int angleDeltaThreshold = 2; // The angle delta to check against
const float upstrokeToMeters = 0.01287;
const int minimumAngleDelta = 10;
const float batteryLevelConstant = 0.476;       //This number is found by Vout = (R32 * Vin) / (R32 + R31), Yields Vin = Vout / 0.476
int queueCount = 0;
int queueLength = 7; //don't forget to change angleQueue to this number also
float angleQueue[7];
// ****************************************************************************
// *** Global Variables *******************************************************
// ****************************************************************************
//static char phoneNumber[] = "+233247398396"; // Number for the Black Phone
static char phoneNumber[] = "+13018737202"; // Number for Upside Wireless
float longestPrime = 0; // total upstroke fo the longest priming event of the day
float leakRateLong = 0; // largest leak rate recorded for the day
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
static char Pin2 = 'unused';
int simVioPin = 3;
static char Pin4 = 'unused';
static char Pin5 = 'unused';
int rxPin = 6;
static char Pin7 = 'unused';
int GND2Pin = 8;
static char Pin9 = 'unused';
static char Pin10 = 'unused';
int batteryLevelPin = 11;
static char Pin12 = 'unused';
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




int digitalPinSet(int pin, int io);
void specifyAnalogPin(int pin, int analogOrDigital);
void analogIOandSHinput(int pin, int IO);
int digitalPinStatus(int pin);
void initialization(void);
int longLength(long num);
void longToString(long num, char *numString);
int stringLength(char *string);
void concat(char *dest, const char *src);
void floatToString(float myValue, char *myString);
void turnOffSIM();
void turnOnSIM();
void tryToConnectToNetwork();
int connectedToNetwork(void);
void sendMessage (char message[160]);
void sendTextMessage(char message[160]);
int readWaterSensor(void);
void initAdc(void);
int readAdc(int channel);
float getHandleAngle();
void initializeQueue(float value);
void pushToQueue(float value);
float queueAverage();
float queueDifference();
unsigned int IdleI2C(void);
unsigned int StartI2C(void);
unsigned int StopI2C(void);
void RestartI2C(void);
void NackI2C(void);
void AckI2C(void);
void configI2c(void);
void WriteI2C(unsigned char byte);
unsigned int ReadI2C (void);
void turnOffClockOscilator (void);
int getSecondI2C (void);
int getMinuteI2C (void);
int getHourI2C (void);
int getYearI2C (void);
int getMonthI2C (void);
int getWkdayI2C (void);
int getDateI2C (void);
void setTime(char sec, char min, char hr, char wkday, char date, char month, char year);
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


#endif	/* IWPUTILITIES_H */


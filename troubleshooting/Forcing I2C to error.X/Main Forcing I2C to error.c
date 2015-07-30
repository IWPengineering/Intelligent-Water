
/*
* File: InternalClock.c
* Author: jy1189
*
* Created on April 23, 2015, 11:05 AM
*/

//*****************************************************************************

/*
#include "../../IWPUtilities/IWPUtilities.h"
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

void initializationForI2CDebug(void){

	////------------Sets up all ports as digial inputs-----------------------
	//IO port control
	ANSA = 0; // Make PORTA digital I/O
	TRISA = 0xFFFF; // Make PORTA all inputs
	ANSB = 0; // All port B pins are digital. Individual ADC are set in the readADC function
	TRISB = 0xFFFF; // Sets all of port B to input

	// pinDirectionIO(sclI2CPin, 0); //TRISBbits.TRISB8 = 0; // RB8 is an output


//	// Timer control
	T1CONbits.TCS = 0; // Source is Internal Clock (8MHz)
	T1CONbits.TCKPS = 0b11; // Prescalar to 1:256
	T1CONbits.TON = 1; // Enable the timer (timer 1 is used for the water sensor)

	// UART control
	U1BRG = 51; // Set baud to 9600, FCY = 8MHz (#pragma config FNOSC = FRC)
	U1STA = 0;
	U1MODE = 0x8000; //enable UART for 8 bit data
	//no parity, 1 stop bit
	U1STAbits.UTXEN = 1; //enable transmit
        pinDirectionIO(waterPresenceSensorOnOffPin, 0); //makes water presence sensor pin an output.
	digitalPinSet(waterPresenceSensorOnOffPin, 1); //turns on the water presnece sensor.

//	// From fona code
//	pinDirectionIO(pwrKeyPin, 0); //TRISBbits.TRISB6 = 0; //sets power key as an output (Pin 15)
//	pinDirectionIO(simVioPin, 0); //TRISAbits.TRISA1 = 0; //sets Vio as an output (pin 3)
//
//	//         Fona stuff
//	digitalPinSet(simVioPin, 1); //PORTAbits.RA1 = 1; //Tells Fona what logic level to use for UART
//	if (digitalPinStatus(statusPin) == 0){ //Checks see if the Fona is off pin
//		digitalPinSet(pwrKeyPin, 0); //PORTBbits.RB6 = 0; //set low pin 15 for 100ms to turn on Fona
//	}
//	while (digitalPinStatus(statusPin) == 0) {} // Wait for Fona to power up
//	digitalPinSet(pwrKeyPin, 1);//PORTBbits.RB6 = 1; // Reset the Power Key so it can be turned off later (pin 15)
//
//	// Turn on SIM800
//	 turnOnSIM();

//                // PUTTY TEST
         specifyAnalogPin(txPin, 0);    // make digital
         specifyAnalogPin(rxPin, 0);    // make digital
         pinDirectionIO(txPin, 0);       // make output
         pinDirectionIO(rxPin, 1);       // make input

	// Moved the RTCCSet function up since we do not rely on network anymore
	configI2c();
	char seconds = 10;
	char minutes = 35;
	char hours = 15;
	char weekday = 6;
	char days = 24;
	char months = 7;
	char years = 15;
	setTime(seconds, minutes, hours, weekday, days, months, years); // SS MM HH WW DD MM YY


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


        sendMessage(initialMessage);
        prevHour = getHourI2C();
        }

void errorPulse(void){
    I2C1CONbits.I2CEN = 0; // doesn't Configure I2C pins as I2C (on pins 17 an 18)
        configI2c();    // Configure 12C pins as 12C (on pins 17 and 18

	IdleI2C(); // Ensure module is idle
	StartI2C(); // Initiate START condition
	while ( I2C1CONbits.SEN ); // Wait until START condition is complete

	int pulsesCreated = 0;

		pinDirectionIO(sdaI2CPin, 0); // make SDA an output
		digitalPinSet(sdaI2CPin, 0); // set SDA low
		while (pulsesCreated < 6) // There should normally be 9 pulses
                {
                    delaySCL();
                    digitalPinSet(sclI2CPin, 1); //PORTBbits.RB9 = 1; // SCL
                    delaySCL();
                    digitalPinSet(sdaI2CPin, 0); //PORTBbits.RB9 = 0; // SCL
                    delaySCL();
                    pulsesCreated++;
		}

                //Do I still need to do this if I'm trying to cause an error?
                RestartI2C(); // Initiate START condition
                while ( I2C1CONbits.RSEN ); // Wait until START condition is complete
                StopI2C(); // Initiate STOP condition
                while ( I2C1CONbits.PEN ); // Wait until STOP condition is complete
}

// ****************************************************************************
// *** Main Function **********************************************************
// ****************************************************************************
void main(void)
{
    	initializationForI2CDebug();
        
        char hourString[20];
        hourString[0] = 0;
        char minuteString[20];
        minuteString[0] = 0;
        char secondString[20];
        secondString[0] = 0;
        char dayString[20];
        dayString[0] = 0;
        char prevHourString[20];
        prevHourString[0] = 0;
        longToString(BcdToDec(prevHour), prevHourString);



            int errorCountDown = 29;

	while (1)
	{ //MAIN LOOP; repeats indefinitely

            longToString(BcdToDec(getHourI2C()), hourString);
            longToString(BcdToDec(getMinuteI2C()), minuteString);
            longToString(BcdToDec(getSecondI2C()), secondString);
            longToString(BcdToDec(getDateI2C()), dayString);
            sendMessage("PrevHour:");
            sendMessage(prevHourString);
            sendMessage("\r \n");
            sendMessage("Hour: ");
            sendMessage(hourString);
            sendMessage("\r \n");

            sendMessage("Minute: ");
            sendMessage(minuteString);
            sendMessage("\r \n");

            sendMessage("Second: ");
            sendMessage(secondString);
            sendMessage("\r \n");

            sendMessage("Day: ");
            sendMessage(dayString);
            sendMessage("\r \n");

            char errorCountString[20];
            errorCountString[0] = 0;
            longToString(errorCountDown, errorCountString);
            sendMessage("errorCountDown: ");
            sendMessage(errorCountString);
            sendMessage("\r \n");

            delayMs(1000);
            errorCountDown--;
            if (errorCountDown == 0){
                sendMessage("ERROR PULSE initalte \r \n");
                errorPulse();
                sendMessage("ERROR PULSE COMPLETE \r \n");
                delayMs(1000);
                errorCountDown = 28;

            }
            
            sendMessage("\r \n");
        }
}

*/



// The Code below is the code that keeps resending textMessages

/*
* File: InternalClock.c
* Author: jy1189
*
* Created on April 23, 2015, 11:05 AM
*/

//*****************************************************************************


#include "../../IWPUtilities/IWPUtilities.h"
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




// ****************************************************************************
// *** Main Function **********************************************************
// ****************************************************************************
void main(void)
{
    	initialization();

	waterPrimeTimeOut /= upstrokeInterval;
	leakRateTimeOut /= upstrokeInterval;
	//timeBetweenUpstrokes /= upstrokeInterval;

	// Do all of these values need to be reset each time around the loop? Or at the end of the day? 06-16-2014
	int handleMovement = 0; // Either 1 or no 0 if the handle moving upward
	int timeOutStatus = 0; // Used to keep track of the water prime timeout
	int hour = 0; // Hour of day
	float angleCurrent = 0; // Stores the current angle of the pump handle
	float anglePrevious = 0; // Stores the last recoreded angle of the pump handle
	float angleDelta = 0; // Stores the difference between the current and previous angles
	float upStroke = 0; // 0 if there is no upstroke, otherwise stores the delta angle
	float upStrokePrime = 0; // Stores the sum of the upstrokes for calculating the prime
	float upStrokeExtract = 0; // Stores the sum of the upstrokes for calculating volume
	float volumeEvent = 0; // Stores the volume extracted
        float leakRatePrevious = 0; // Stores the previous Leak Rate incase if someone stats to pump before leakage can be measured
        //float extractionStartTime = 0; // The time of day (in seconds) when the extraction started
	//float extractionEndTime = 0; // The time of day (in seconds) when the extraction ended
	//float extractionDuration = 0; // The difference between the extraction start and end times
	long leakTimeCounter = 0; // Used to keep track of the leak time timeout
	float upStrokePrimeMeters = 0; // Stores the upstroke in meters
	float leakRate = 0; // Rate at which water is leaking from the rising main
//	float leakTime = 0; // The number of milliseconds from when the user stops pumping until there is no water (min: 0, max: 10 minutes)
//	long upStrokeDelayCounter = 0;
        int currentHour;

	while (1)
	{ //MAIN LOOP; repeats indefinitely
		////////////////////////////////////////////////////////////
		// Idle Handle Monitor Loop
		// 2 axis of the accelerometer are constantly polled for
		// motion in the upward direction by comparing angles
		////////////////////////////////////////////////////////////
		// Get the angle of the pump handle to measure against
		anglePrevious = getHandleAngle();
		float previousAverage = 0;
		initializeQueue(anglePrevious);
		previousAverage = queueAverage();
		// Set the handle movement to 0 (handle is not moving)
		handleMovement = 0;
		// Loop until the handle starts moving
                float angleAccumulated=0;
		while (handleMovement == 0)
		{
                        currentHour = getHourI2C();
			if ( prevHour != currentHour){ //(prevDay != getDateI2C()){// it's a new day so send midNightMessage();
                            if (prevHour != getHourI2C()){    // Hopes of reducing polling error
                                batteryFloat = batteryLevel();
				midnightMessage();
                            }
			}
			delayMs(upstrokeInterval); // Delay for a short time
                        float newAngle = getHandleAngle();
                        float deltaAngle = abs(newAngle - anglePrevious);
                        anglePrevious = newAngle;
                        if (deltaAngle > 2){ // prevents floating accelerometer values when it's not actually moving
                        angleAccumulated += deltaAngle;
                        }
			// If the angle has changed, set the handleMovement flag
			if (angleAccumulated > 5) //05-30-14 Test for small delta's used to be angleDeltaThreshold
			{
				handleMovement = 1;
			}
		}
                /////////////////////////////////////////////////////////
		// Priming Loop
		// The total amount of upstroke is recorded while the
		// upper water sensor is checked to determine if the
		// pump has been primed
		/////////////////////////////////////////////////////////
		timeOutStatus = 0; // prepares timeoutstatus for new event
		// Get the angle of the pump handle to measure against
		anglePrevious = getHandleAngle();
		upStrokePrime = 0; // gets the variable ready for a new event
                upStroke = 0; // gets variable ready for new event

		while ((timeOutStatus < waterPrimeTimeOut) && !readWaterSensor())
		{
			delayMs(upstrokeInterval);  // delay a short time (10ms)
			angleCurrent = getHandleAngle(); // Get the current angle of the pump handle
			angleDelta = angleCurrent - anglePrevious; // Calculate the change in angle of the pump handle
                        if(angleDelta > 5){
                            upStroke += angleDelta;
                            upStrokePrime += degToRad(upStroke); // Update the upStrokePrime
                            timeOutStatus=0;
                        // upstroke and current angle
			}
                        else{
                        timeOutStatus++;}
			anglePrevious = angleCurrent; // Update the previous angle for the next calculation
			}
		upStrokePrimeMeters = upStrokePrime * upstrokeToMeters;	// Convert to meters

		if (upStrokePrimeMeters > longestPrime) // Updates the longestPrime
		{
			longestPrime = upStrokePrimeMeters;
		}
		///////////////////////////////////////////////////////
		// Volume Calculation loop
		// Tracks the upStroke for the water being extracted
		//(in next loop -->) as well as the time in milliseconds taken for water to leak
		///////////////////////////////////////////////////////
                float absoluteAngleThreshold = 3.5;
                int volumeLoopCounter = 5; // 50ms

                unsigned long extractionDuration = 0;
                float absoluteAngle = 0;
                float totalAbsoluteAngle = absoluteAngleThreshold + 1;
                while(readWaterSensor() && (totalAbsoluteAngle > absoluteAngleThreshold)){
                    totalAbsoluteAngle = 0;

                    int i = 0;
                    while((i < volumeLoopCounter) && readWaterSensor()){
                        angleCurrent = getHandleAngle();
                        angleDelta = angleCurrent - anglePrevious;
                        if (angleDelta < 0){ // absolute value of angleDelta
                            angleDelta = angleDelta * -1;
                        }
                        absoluteAngle = angleDelta; // this should be the absolute value of angleDelta now
                        anglePrevious = angleCurrent;

                        if(angleDelta > 0){
                            angleDelta = angleDelta; //degToRad(angleDelta);
                            upStrokeExtract = upStrokeExtract + angleDelta;
                        }
                            totalAbsoluteAngle =+ absoluteAngle;
                            i++;
                           delayMs(volumeDelay); // Delay for a short time
                           extractionDuration++;
                    }
                }
                    totalAbsoluteAngle = 0;

		///////////////////////////////////////////////////////
		// Leakage Rate loop
		///////////////////////////////////////////////////////
		// Get the angle of the pump handle to measure against
                    int leakCondition;
		anglePrevious = getHandleAngle();
		// Used to keep track of how many milliseconds have passed
		long leakDurationCounter = volumeLoopCounter * volumeDelay; // The volume loop has 50 milliseconds of delay before entry
		while (readWaterSensor() && (leakTimeCounter < leakRateTimeOut))
		{
				angleCurrent = getHandleAngle(); //Get the current angle of the pump handle
				//Calculate the change in angle of the pump handle
				angleDelta = angleCurrent - anglePrevious;
				// Update the previous angle for the next calculation
				anglePrevious = angleCurrent;
				// If the handle moved more than 2 degrees, we will consider that an
				// intentional pump and break out of the loop (2 is in radians)
				if (angleDelta > angleDeltaThreshold)
				{
                                    leakCondition=1;
					break;
                                }
                                if (leakDurationCounter > 100) // change to 20,000 for real code
                                {
                                    leakCondition=2;
                                        break;
                                }
                                   leakCondition=3;
			delayMs(1);
                        leakDurationCounter++;
		}
                switch (leakCondition){
                    case 1:
                        leakRate = leakRatePrevious;
                        break;
                    case 2:
                        leakRate = 0;
                        leakRatePrevious = leakRate;
                        break;
                    case 3:
                        leakRate = leakSensorVolume / ((leakDurationCounter) / 1000.0); // liters/sec
                        leakRatePrevious = leakRate;
                }

		if ((leakRate * 3600) > leakRateLong)
		{
			leakRateLong = leakRate * 3600; //reports in L/hr
		}

                upStrokeExtract = degToRad(upStrokeExtract);
		volumeEvent = (MKII * upStrokeExtract);
                volumeEvent -= (leakRate * extractionDuration / 10.0);

		hour = BcdToDec(getHourI2C());
		switch (hour / 2)
		{ //organize extaction into 2 hours bins
		case 0:
			volume02 = volume02 + volumeEvent;
			break;
		case 1:
			volume24 = volume24 + volumeEvent;
			break;
		case 2:
			volume46 = volume46 + volumeEvent;
			break;
		case 3:
			volume68 = volume68 + volumeEvent;
			break;
		case 4:
			volume810 = volume810 + volumeEvent;
			break;
		case 5:
			volume1012 = volume1012 + volumeEvent;
			break;
		case 6:
			volume1214 = volume1214 + volumeEvent;
			break;
		case 7:
			volume1416 = volume1416 + volumeEvent;
			break;
		case 8:
			volume1618 = volume1618 + volumeEvent;
			break;
		case 9:
			volume1820 = volume1820 + volumeEvent;
			break;
		case 10:
			volume2022 = volume2022 + volumeEvent;
			break;
		case 11:
			volume2224 = volume2224 + volumeEvent;
			break;
		}
	} // End of main loop
}
/*
* File: InternalClock.c
* Author: jy1189
*
* Created on April 23, 2015, 11:05 AM
*/

//*****************************************************************************


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




// ****************************************************************************
// *** Main Function **********************************************************
// ****************************************************************************
void main(void)
{
    	initialization();
    
       
	waterPrimeTimeOut /= upstrokeInterval;
	leakRateTimeOut /= upstrokeInterval;
	timeBetweenUpstrokes /= upstrokeInterval;

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
	float extractionStartTime = 0; // The time of day (in seconds) when the extraction started
	float extractionEndTime = 0; // The time of day (in seconds) when the extraction ended
	float extractionDuration = 0; // The difference between the extraction start and end times
	long leakTimeCounter = 0; // Used to keep track of the leak time timeout
	float upStrokePrimeMeters = 0; // Stores the upstroke in meters
	float leakRate = 0; // Rate at which water is leaking from the rising main
	float leakTime = 0; // The number of milliseconds from when the user stops pumping until there is no water (min: 0, max: 10 minutes)
	long upStrokeDelayCounter = 0;
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
		float deltaAverage = 0, previousAverage = 0;
		initializeQueue(anglePrevious);
		previousAverage = queueAverage();
		// Set the handle movement to 0 (handle is not moving)
		handleMovement = 0;
		// Loop until the handle starts moving
                float angleAccumulated=0;
		while (handleMovement == 0)
		{
                        debugHighLow(Pin4);
                        currentHour = getHourI2C();
			if (prevHour != currentHour){ //(prevDay != getDateI2C()){// it's a new day so send midNightMessage();
				midnightMessage();
			}

			delayMs(upstrokeInterval); // Delay for a short time
                        float newAngle = getHandleAngle();
                        float deltaAngle = abs(newAngle-anglePrevious);
                        anglePrevious=newAngle;
                        angleAccumulated += deltaAngle;
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
               
		while ((timeOutStatus < waterPrimeTimeOut) && !readWaterSensor())
		{
                        debugHighLow(Pin5);
			delayMs(upstrokeInterval);  // delay a short time (10ms)
			angleCurrent = getHandleAngle(); // Get the current angle of the pump handle
			angleDelta = angleCurrent - anglePrevious; // Calculate the change in angle of the pump handle
                        
                        if(angleDelta > 5){
                        upStroke += angleDelta;
                        upStrokePrime += degToRad(upStroke); // Update the upStrokePrime
                        timeOutStatus=0;
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
		// Reset the upstroke extract
		upStrokeExtract = 0;
		// Get the start time (in seconds) of the extraction
		extractionStartTime = timeStamp();
		// Get the angle of the pump handle to measure against
		anglePrevious = getHandleAngle();
		float averageAngle = 0;
		float currentAverage = 0;
		previousAverage = 0;
		deltaAverage = 0;
		// Initialize the queue
		initializeQueue(anglePrevious);
		previousAverage = queueAverage();
		// Used to count the number of milliseonds between upstrokes
		upStrokeDelayCounter = 0;
		// Used to keep track of the total time passed in milliseconds
		leakTime = 0;
		// Used to keep track of how many milliseconds have passed
		int millisecondCounter = 0;
		// Reset upstroke
		upStroke = 0;
		float highest = previousAverage, lowest = previousAverage;
		int checkHighest = 1, checkLowest = 1;
		while (readWaterSensor() && (upStrokeDelayCounter < timeBetweenUpstrokes))
		{
                        debugHighLow(Pin7);
			if (millisecondCounter > upstrokeInterval)
			{
				millisecondCounter = 0; // Reset the counter
				// Get the current angle of the pump handle
				pushToQueue(getHandleAngle());
				// Get the average of the angles in the queue
				averageAngle = queueAverage();
				currentAverage = averageAngle;
				deltaAverage = currentAverage - previousAverage;
				previousAverage = currentAverage;
				if (averageAngle > highest)
				{
					highest = averageAngle;
				}
				else if (averageAngle < lowest)
				{
					lowest = averageAngle;
				}
				if (((highest - averageAngle) > minimumAngleDelta) && checkHighest)
				{
					upStrokeExtract += degToRad(highest - lowest);
					lowest = 100;
					checkHighest = 0;
					checkLowest = 1;
				}
				else if (((averageAngle - lowest) > minimumAngleDelta) && checkLowest)
				{
					highest = -100;
					checkLowest = 0;
					checkHighest = 1;
				}
				//If the handle moved, set the upStroke
				if (deltaAverage > 1.2)
				{
					upStrokeDelayCounter = 0; // Reset the upstroke counter
					upStroke = deltaAverage;
				}
				else
				{
					upStroke = 0;
					upStrokeDelayCounter++;
				}
			}
			if (upStroke == 0)
			{
				leakTime++;
			}
			else
			{
				leakTime = 0;
			}
			millisecondCounter++;
			delayMs(1);
		}
		extractionEndTime = timeStamp();
		// Check for extraction end time being less (if someone starts
		// before 12 AM and ends after 12 AM)
		if (extractionEndTime < extractionStartTime)
		{
			extractionDuration = (86400 - extractionEndTime) + extractionStartTime;
		}
		else
		{
			extractionDuration = extractionEndTime - extractionStartTime;
		}
		///////////////////////////////////////////////////////
		// Leakage Rate loop
		///////////////////////////////////////////////////////
		// Get the angle of the pump handle to measure against
		anglePrevious = getHandleAngle();
		// Used to keep track of how many milliseconds have passed
		millisecondCounter = 0;
		// Reset the counter for the leak timeout
		leakTimeCounter = 0;
		while (readWaterSensor() && (leakTimeCounter < leakRateTimeOut))
		{
                        debugHighLow(Pin9);
			if (millisecondCounter > upstrokeInterval)
			{
				millisecondCounter = 0; // Reset the counter
				// Get the current angle of the pump handle
				angleCurrent = getHandleAngle();
				//Calculate the change in angle of the pump handle
				angleDelta = angleCurrent - anglePrevious;
				// Update the previous angle for the next calculation
				anglePrevious = angleCurrent;
				leakTimeCounter++;
				// If the handle moved more than 2 degrees, we will consider that an
				// intentional pump and break out of the loop (2 is in radians)
				if (angleDelta > angleDeltaThreshold)
				{
					break;
				}
			}
			millisecondCounter++;
			leakTime++;
			delayMs(1);
		}
		//Check if water is present - make sure this function works!
		if (readWaterSensor())
		{
			leakRate = 0;
		}
		else
		{
			// Add one to leakTime to prevent divide by 0 error (and divide by 1000 to get milliseconds into seconds
			leakRate = leakSensorVolume / ((leakTime + 1) / 1000.0); // Leak rate is L/s
		}
		if (leakRate > leakRateLong)
		{
			leakRateLong = leakRate;
		}
		volumeEvent = (MKII * upStrokeExtract) - (leakRate * extractionDuration);
		hour = getHourI2C();
		switch (hour / 2)
		{ //organize extaction into 2 hours bins
			//does this work? what happens if odd hour?
			//wouldn't it be a float and not enter--is that the purpose then, is that what we want?? 06-17-2014
		case 0:
			volume02 = volume02 + volumeEvent;
			break;
		case 1:
			volume24 = volume24 + volumeEvent;
			break;
		case 3:
			volume46 = volume46 + volumeEvent;
			break;
		case 4:
			volume68 = volume68 + volumeEvent;
			break;
		case 5:
			volume810 = volume810 + volumeEvent;
			break;
		case 6:
			volume1012 = volume1012 + volumeEvent;
			break;
		case 7:
			volume1214 = volume1214 + volumeEvent;
			break;
		case 8:
			volume1416 = volume1416 + volumeEvent;
			break;
		case 9:
			volume1618 = volume1618 + volumeEvent;
			break;
		case 10:
			volume1820 = volume1820 + volumeEvent;
			break;
		case 11:
			volume2022 = volume2022 + volumeEvent;
			break;
		case 12:
			volume2224 = volume2224 + volumeEvent;
			break;
		}
	} // End of main loop
}
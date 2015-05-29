/*
* File: InternalClock.c
* Author: jy1189
*
* Created on April 23, 2015, 11:05 AM
*/

//*****************************************************************************


#include "IWPUtilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xc.h>
#include <string.h>


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



// Vin = Actual battery level. Vout = what we are recieving.
//Vin = (21/10)*Vout;
//Vin = Vout/(0.4761908)
char batteryLevel(void){ //this has not been tested
    float voltageOut1;
    float voltageIn1;
    float voltageOut2;
    float voltageIn2;
    float voltageOut3;
    float voltageIn3;
    float voltageAvg;

    char voltageAvgFloat[10];

    voltageOut1 = readAdc(batteryVoltage) - adjustmentFactor;
    voltageIn1 = voltageOut1 / (batteryLevelConstant);

    delayMs(50);

    voltageOut2 = readAdc(batteryVoltage)- adjustmentFactor;
    voltageIn2 = voltageOut2 / (batteryLevelConstant);

    delayMs(50);

    voltageOut3 = readAdc(batteryVoltage)- adjustmentFactor;
    voltageIn3 = voltageOut3 / (batteryLevelConstant);

    voltageAvg = (voltageIn1 + voltageIn2 + voltageIn3)/3;

    floatToString(voltageAvg, voltageAvgFloat);

    return voltageAvgFloat;

}



// ****************************************************************************
// *** Main Function **********************************************************
// ****************************************************************************
void main (void)
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
while(1)
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
while (handleMovement == 0)
{
// Delay for a short time
delayMs(upstrokeInterval);
// Get the current angle of the pump handle
pushToQueue(getHandleAngle());
//Calculate the change in angle of the pump handle
deltaAverage = queueAverage() - previousAverage;
previousAverage = queueAverage();
// If the angle has changed, set the handleMovement flag
if (deltaAverage > angleDeltaThreshold) //05-30-14 Test for small delta's
{
handleMovement = 1;
}
} //Exit loop when handle is moving
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
delayMs(upstrokeInterval);
// Get the current angle of the pump handle
angleCurrent = getHandleAngle();
// Calculate the change in angle of the pump handle
angleDelta = angleCurrent - anglePrevious;
// If the pump angle has changed, update the up stroke, otherwise set it to 0
if (angleDelta > angleDeltaThreshold)
{
upStroke = angleDelta;
}
else
{
upStroke = 0;
}
// Update the previous angle for the next calculation
anglePrevious = angleCurrent;
// If no upstroke is happening, increase the timeout
if (upStroke == 0)
{
timeOutStatus++;
}
// Otherwise, reset the timeout because movement occurred
else if (upStroke > 0)
{
timeOutStatus = 0;
upStrokePrime += degToRad(upStroke); // Update the upStrokePrime
}
}
// Convert to meters
upStrokePrimeMeters = upStrokePrime * upstrokeToMeters;
// Updates the longestPrime
if (upStrokePrimeMeters > longestPrime)
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
while(readWaterSensor() && (upStrokeDelayCounter < timeBetweenUpstrokes))
{
if(millisecondCounter > upstrokeInterval)
{
millisecondCounter = 0; // Reset the counter
// Get the current angle of the pump handle
pushToQueue(getHandleAngle());
// Get the average of the angles in the queue
averageAngle = queueAverage();
currentAverage = averageAngle;
deltaAverage = currentAverage - previousAverage;
previousAverage = currentAverage;
if(averageAngle > highest)
{
highest = averageAngle;
}
else if(averageAngle < lowest)
{
lowest = averageAngle;
}
if(((highest - averageAngle) > minimumAngleDelta) && checkHighest)
{
upStrokeExtract += degToRad(highest - lowest);
lowest = 100;
checkHighest = 0;
checkLowest = 1;
}
else if(((averageAngle - lowest) > minimumAngleDelta) && checkLowest)
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
if(upStroke == 0)
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
if(extractionEndTime < extractionStartTime)
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
while(readWaterSensor() && (leakTimeCounter < leakRateTimeOut))
{
if(millisecondCounter > upstrokeInterval)
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
hour = getTimeHour();
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
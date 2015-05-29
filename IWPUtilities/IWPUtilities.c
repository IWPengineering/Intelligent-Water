const int decimalAccuracy = 3; // Number of decimal places to use when converting floats to strings



int digitalPinStatus(int pin)
{
int pinValue;
if (pin == 1)
{
pinValue = PORTAbits.RA5;
}
else if (pin==2)
{
pinValue = PORTAbits.RA0;
}
else if (pin==3)
{
pinValue = PORTAbits.RA1;
}
else if (pin==4)
{
pinValue = PORTBbits.RB0;
}
else if (pin==5)
{
pinValue = PORTBbits.RB1;
}
else if (pin==6)
{
pinValue = PORTBbits.RB2;
}
else if (pin==7)
{
pinValue = PORTBbits.RB3;
}
// Pin8 - Always VSS for PIC24FV32KA302 - Do nothing
else if (pin==9)
{
pinValue = PORTAbits.RA2;
}
else if (pin==10)
{
pinValue = PORTAbits.RA3;
}
else if (pin==11)
{
pinValue = PORTBbits.RB4;
}
else if (pin==12)
{
pinValue = PORTAbits.RA4;
}
//Pin 13 - Always VDD for PIC24FV32KA302 - Do nothing
else if (pin==14)
{
pinValue = PORTBbits.RB5;
}
else if (pin==15)
{
pinValue = PORTBbits.RB6;
}
else if (pin==16)
{
pinValue = PORTBbits.RB7;
} //Usually reserved for TX
else if (pin==17)
{
pinValue = PORTBbits.RB8;
}//Usually reserved for I2C
else if (pin==18)
{
pinValue = PORTBbits.RB9;
}//Usually Reserved for I2C
else if (pin==19)
{
pinValue = PORTAbits.RA7;
}
// Pin 20 - Always vCap for PIC24FV32KA302 - Do nothing
else if (pin==21)
{
pinValue = PORTBbits.RB10;
}
else if (pin==22)
{
pinValue = PORTBbits.RB11;
}
else if (pin==23)
{
pinValue = PORTBbits.RB12;
}
else if (pin==24)
{
pinValue = PORTBbits.RB13;
}
else if (pin==25)
{
pinValue = PORTBbits.RB14;
}
else if (pin==26)
{
pinValue = PORTBbits.RB15;
}
return pinValue;
// Pin 27 - Always VSS for PIC24FV32KA302 - Do nothing
// Pin 28 - Always VDD for PIC24FV32KA302 - Do nothing
}



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
}while(num != 0);
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
if(num < 0)
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
}while(num != 0);
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
while(string[i] != '\0')
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
if(myValue < 0)
{
mantissa *= -1;
exponent *= -1;
}
// Counts the padding needed
while(mantissa < 1 && mantissa != 0)
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
if(padLength > -1 && padLength < decimalAccuracy)
{
// Updates the decimal place
decimalShift = padLength;
// Extracts the next one's place from the mantissa until we reached our decimal accuracy
while(decimalShift < decimalAccuracy)
{
digit = (long)mantissa; // Get the next digit
longToString(digit, digitString); // Convert the digit to a string
concat(mString, digitString); // Add the digit string to the mantissa string
mantissa = mantissa - (float)digit;
mantissa = mantissa * 10; // Shift the decimal places to prepare for the next digit
decimalShift++; // Update the decimal shift count
}
if(myValue < 0)
{
concat(myString, "-"); // Adds the '-' character
sLength++; // Add one to the length for the '-' character
}
// Concatenates the exponent, decimal point, and mantissa together
concat(myString, eString);
concat(myString, ".");
// Adds 0's to the mantissa string for each 0 needed for padding
int i;
for(i = 0; i < padLength; i++)
{
concat(myString, "0");
}
concat(myString, mString);
//The length of the final string (lengths of the parts plus 1 for decimal point, 1 for \0 character, and the number of 0's)
sLength += stringLength(eString) + stringLength(mString) + 2 + padLength;
// Removes any trailing 0's
while(myString[sLength - 2] == '0')
{
myString[sLength - 2] = '\0';
sLength--;
}
}
else
{
if(myValue < 0)
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
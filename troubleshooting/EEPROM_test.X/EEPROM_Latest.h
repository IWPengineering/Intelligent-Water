/*
 * File:   EEPROM_Latest.h
 * Author: Nathan Chaney
 *
 * Created on June 3, 2014, 9:45 AM in Mahadaga, Burkina Faso
 */

/* Includes *******************************************************************/

#include "EEPROM_Latest.h"


/* Function Definitions *******************************************************/

void EEreadPassword(void);
void EEwritePassword(void);
void EEreadPowerAlloc(void);
void EEwritePowerAlloc(void);
void EEreadDate(void);
void EEwriteDate(void);
void EEreadEmerButton(void);
void EEwriteEmerButton(void);
void EEreadResetTime(void);
void EEwriteResetTime(void);
void EEreadAlarm(void);
void EEwriteAlarm(void);
void EEreadTotals(void);
void EEwriteTotals(void);
void EEreadPowerUsed(void);
void EEwritePowerUsed(void);
void EEreadHL(void);
void EEwriteHL(void);
void EEreadRelay(void);
void EEwriteRelay(void);

unsigned char findPowerToRead(void);
unsigned char findPowerToWrite(void);

void EEreadAll(void);


/* Variable Definitions *******************************************************/

// password is 231321
int __attribute__ ((space(eedata))) EEpassword0 = '2';
int __attribute__ ((space(eedata))) EEpassword1 = '3';
int __attribute__ ((space(eedata))) EEpassword2 = '1';
int __attribute__ ((space(eedata))) EEpassword3 = '3';
int __attribute__ ((space(eedata))) EEpassword4 = '2';
int __attribute__ ((space(eedata))) EEpassword5 = '1';
int __attribute__ ((space(eedata))) EEpowerAlloc = 750;
int __attribute__ ((space(eedata))) EEyear = 14;
int __attribute__ ((space(eedata))) EEmonth = 1;
int __attribute__ ((space(eedata))) EEday = 1;
int __attribute__ ((space(eedata))) EEemerButton = 0;
int __attribute__ ((space(eedata))) EEresetTime = 700;
int __attribute__ ((space(eedata))) EEaudibleAlarm = 0;
int __attribute__ ((space(eedata))) EEalarmOnePower = 0;
int __attribute__ ((space(eedata))) EEalarmTwoPower = 0;
int __attribute__ ((space(eedata))) EEtotalUsedH = 0;
int __attribute__ ((space(eedata))) EEtotalUsedL = 0;
int __attribute__ ((space(eedata))) EEpreviousDayUsedH = 0;
int __attribute__ ((space(eedata))) EEpreviousDayUsedL = 0;
int __attribute__ ((space(eedata))) EEpowerUsed1H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed1L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed2H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed2L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed3H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed3L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed4H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed4L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed5H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed5L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed6H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed6L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed7H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed7L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed8H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed8L = 0;
int __attribute__ ((space(eedata))) EEhighLow = 0xFF;
int __attribute__ ((space(eedata))) EErelay = 0xFF;

extern char passwordSet[6];
extern unsigned long powerAllocated;
extern unsigned char timeYear, timeMonth, timeDay;
extern char emerButtonEnable;
extern int emerButtonAlloc;
extern unsigned char resetHour, resetMinute;
extern char audibleAlarm, alarmOnePower, alarmTwoPower, alarmOneEnabled, alarmTwoEnabled;
extern unsigned long totalUsed, previousDayUsed;
extern unsigned long powerUsedMW;
extern char isHigh;
extern char relayActive;

/* Functions ******************************************************************/

void EEreadAll(void) {
    EEreadPassword();
    EEreadPowerAlloc();

    readTime();
    if(timeYear < 15)
        EEreadDate();

    EEreadEmerButton();
    EEreadResetTime();
    EEreadAlarm();
    EEreadTotals();
    EEreadPowerUsed();
    EEreadHL();
    EEreadRelay();
}

void EEreadPassword(void) {
    while (_WR) continue;

    TBLPAG = __builtin_tblpage(&EEpassword0);
    unsigned int offset = __builtin_tbloffset(&EEpassword0);
    passwordSet[0] = (char) __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEpassword1);
    offset = __builtin_tbloffset(&EEpassword1);
    passwordSet[1] = (char) __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEpassword2);
    offset = __builtin_tbloffset(&EEpassword2);
    passwordSet[2] = (char) __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEpassword3);
    offset = __builtin_tbloffset(&EEpassword3);
    passwordSet[3] = (char) __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEpassword4);
    offset = __builtin_tbloffset(&EEpassword4);
    passwordSet[4] = (char) __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEpassword5);
    offset = __builtin_tbloffset(&EEpassword5);
    passwordSet[5] = (char) __builtin_tblrdl(offset);
}

void EEwritePassword(void) {

    // char 0
    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEpassword0);
    unsigned int offset = __builtin_tbloffset(&EEpassword0);
    __builtin_tblwtl(offset, passwordSet[0]);

    asm volatile ("disi #5");
    __builtin_write_NVM();

    // char 1
    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEpassword1);
    offset = __builtin_tbloffset(&EEpassword1);
    __builtin_tblwtl(offset, passwordSet[1]);

    asm volatile ("disi #5");
    __builtin_write_NVM();

    // char 2
    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEpassword2);
    offset = __builtin_tbloffset(&EEpassword2);
    __builtin_tblwtl(offset, passwordSet[2]);

    asm volatile ("disi #5");
    __builtin_write_NVM();

    // char 3
    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEpassword3);
    offset = __builtin_tbloffset(&EEpassword3);
    __builtin_tblwtl(offset, passwordSet[3]);

    asm volatile ("disi #5");
    __builtin_write_NVM();

    // char 4
    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEpassword4);
    offset = __builtin_tbloffset(&EEpassword4);
    __builtin_tblwtl(offset, passwordSet[4]);

    asm volatile ("disi #5");
    __builtin_write_NVM();

    // char 5
    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEpassword5);
    offset = __builtin_tbloffset(&EEpassword5);
    __builtin_tblwtl(offset, passwordSet[5]);

    asm volatile ("disi #5");
    __builtin_write_NVM();
}

void EEreadPowerAlloc(void){
    while (_WR) continue;

    TBLPAG = __builtin_tblpage(&EEpowerAlloc);
    unsigned int offset = __builtin_tbloffset(&EEpowerAlloc);
    powerAllocated = (unsigned long) __builtin_tblrdl(offset);
}

void EEwritePowerAlloc(void){

    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEpowerAlloc);
    unsigned int offset = __builtin_tbloffset(&EEpowerAlloc);
    __builtin_tblwtl(offset, powerAllocated);

    asm volatile ("disi #5");
    __builtin_write_NVM();
}

void EEreadDate(void){
    while (_WR) continue;

    TBLPAG = __builtin_tblpage(&EEyear);
    unsigned int offset = __builtin_tbloffset(&EEyear);
    unsigned char tempYear = (unsigned char) __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEmonth);
    offset = __builtin_tbloffset(&EEmonth);
    unsigned char tempMonth = (unsigned char) __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEday);
    offset = __builtin_tbloffset(&EEday);
    unsigned char tempDay = (unsigned char) __builtin_tblrdl(offset);


    writeTime(tempYear, tempMonth, tempDay, 0, 0);
}
void EEwriteDate(void){

    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEyear);
    unsigned int offset = __builtin_tbloffset(&EEyear);
    __builtin_tblwtl(offset, timeYear);

    asm volatile ("disi #5");
    __builtin_write_NVM();

    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEmonth);
    offset = __builtin_tbloffset(&EEmonth);
    __builtin_tblwtl(offset, timeMonth);

    asm volatile ("disi #5");
    __builtin_write_NVM();

    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEday);
    offset = __builtin_tbloffset(&EEday);
    __builtin_tblwtl(offset, timeDay);

    asm volatile ("disi #5");
    __builtin_write_NVM();
}

void EEreadEmerButton() {
    while (_WR) continue;

    TBLPAG = __builtin_tblpage(&EEemerButton);
    unsigned int offset = __builtin_tbloffset(&EEemerButton);
    int tempEmerAlloc = __builtin_tblrdl(offset);

    if (tempEmerAlloc) {
        emerButtonEnable = 1;
        emerButtonAlloc = tempEmerAlloc;
    } else {
        emerButtonEnable = 0;
        emerButtonAlloc = 0;
    }
}

void EEwriteEmerButton() {
    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEemerButton);
    unsigned int offset = __builtin_tbloffset(&EEemerButton);
    __builtin_tblwtl(offset, emerButtonAlloc);

    asm volatile ("disi #5");
    __builtin_write_NVM();
}

void EEreadResetTime() {
    while (_WR) continue;

    TBLPAG = __builtin_tblpage(&EEresetTime);
    unsigned int offset = __builtin_tbloffset(&EEresetTime);
    unsigned int tempResetTime = __builtin_tblrdl(offset);

    resetMinute = tempResetTime % 100;
    resetHour = tempResetTime / 100;
}

void EEwriteResetTime() {
    while (_WR) continue;
    NVMCON = 0x4004;

    unsigned int tempResetTime = resetMinute + (100 * resetHour);

    TBLPAG = __builtin_tblpage(&EEresetTime);
    unsigned int offset = __builtin_tbloffset(&EEresetTime);
    __builtin_tblwtl(offset, tempResetTime);

    asm volatile ("disi #5");
    __builtin_write_NVM();
}

void EEreadAlarm() {
    while (_WR) continue;

    TBLPAG = __builtin_tblpage(&EEaudibleAlarm);
    unsigned int offset = __builtin_tbloffset(&EEaudibleAlarm);
    audibleAlarm = (char) __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEalarmOnePower);
    offset = __builtin_tbloffset(&EEalarmOnePower);
    alarmOnePower = (char) __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEalarmTwoPower);
    offset = __builtin_tbloffset(&EEalarmTwoPower);
    alarmTwoPower = (char) __builtin_tblrdl(offset);

    if (alarmOnePower)
        alarmOneEnabled = 1;
    else
        alarmOneEnabled = 0;

    if (alarmTwoPower)
        alarmTwoEnabled = 1;
    else
        alarmTwoEnabled = 0;
}

void EEwriteAlarm() {
    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEaudibleAlarm);
    unsigned int offset = __builtin_tbloffset(&EEaudibleAlarm);
    __builtin_tblwtl(offset, audibleAlarm);

    asm volatile ("disi #5");
    __builtin_write_NVM();


    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEalarmOnePower);
    offset = __builtin_tbloffset(&EEalarmOnePower);
    __builtin_tblwtl(offset, alarmOnePower);

    asm volatile ("disi #5");
    __builtin_write_NVM();


    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEalarmTwoPower);
    offset = __builtin_tbloffset(&EEalarmTwoPower);
    __builtin_tblwtl(offset, alarmTwoPower);

    asm volatile ("disi #5");
    __builtin_write_NVM();
}

void EEreadTotals() {

    unsigned int tempTotalUsedH, tempTotalUsedL, tempPreviousDayUsedH, tempPreviousDayUsedL;

    while (_WR) continue;

    TBLPAG = __builtin_tblpage(&EEtotalUsedH);
    unsigned int offset = __builtin_tbloffset(&EEtotalUsedH);
    tempTotalUsedH = __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEtotalUsedL);
    offset = __builtin_tbloffset(&EEtotalUsedL);
    tempTotalUsedL = __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEpreviousDayUsedH);
    offset = __builtin_tbloffset(&EEpreviousDayUsedH);
    tempPreviousDayUsedH = __builtin_tblrdl(offset);

    TBLPAG = __builtin_tblpage(&EEpreviousDayUsedL);
    offset = __builtin_tbloffset(&EEpreviousDayUsedL);
    tempPreviousDayUsedL = __builtin_tblrdl(offset);

    totalUsed = (((unsigned long) tempTotalUsedH) << 16) + tempTotalUsedL;
    previousDayUsed = (((unsigned long) tempPreviousDayUsedH) << 16) + tempPreviousDayUsedL;
}

void EEwriteTotals() {

    unsigned int tempTotalUsedH, tempTotalUsedL, tempPreviousDayUsedH, tempPreviousDayUsedL;

    tempTotalUsedH = totalUsed >> 16;
    tempTotalUsedL = totalUsed & 0xFFFF;
    tempPreviousDayUsedH = previousDayUsed >> 16;
    tempPreviousDayUsedL = previousDayUsed & 0xFFFF;


    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEtotalUsedH);
    unsigned int offset = __builtin_tbloffset(&EEtotalUsedH);
    __builtin_tblwtl(offset, tempTotalUsedH);

    asm volatile ("disi #5");
    __builtin_write_NVM();



    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEtotalUsedL);
    offset = __builtin_tbloffset(&EEtotalUsedL);
    __builtin_tblwtl(offset, tempTotalUsedL);

    asm volatile ("disi #5");
    __builtin_write_NVM();


    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEpreviousDayUsedH);
    offset = __builtin_tbloffset(&EEpreviousDayUsedH);
    __builtin_tblwtl(offset, tempPreviousDayUsedH);

    asm volatile ("disi #5");
    __builtin_write_NVM();


    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEpreviousDayUsedL);
    offset = __builtin_tbloffset(&EEpreviousDayUsedL);
    __builtin_tblwtl(offset, tempPreviousDayUsedL);

    asm volatile ("disi #5");
    __builtin_write_NVM();
}

void EEreadPowerUsed() {
    unsigned char EEpowerSelect;
    unsigned int tempPowerH, tempPowerL, offset;

    while (_WR) continue;
    
    EEpowerSelect = findPowerToRead();
    
    switch (EEpowerSelect) {
        case 0:
            tempPowerH = tempPowerL = 0;
            break;

        case 1:
            TBLPAG = __builtin_tblpage(&EEpowerUsed1H);
            offset = __builtin_tbloffset(&EEpowerUsed1H);
            tempPowerH = __builtin_tblrdl(offset);
            TBLPAG = __builtin_tblpage(&EEpowerUsed1L);
            offset = __builtin_tbloffset(&EEpowerUsed1L);
            tempPowerL = __builtin_tblrdl(offset);
            break;

        case 2:
            TBLPAG = __builtin_tblpage(&EEpowerUsed2H);
            offset = __builtin_tbloffset(&EEpowerUsed2H);
            tempPowerH = __builtin_tblrdl(offset);
            TBLPAG = __builtin_tblpage(&EEpowerUsed2L);
            offset = __builtin_tbloffset(&EEpowerUsed2L);
            tempPowerL = __builtin_tblrdl(offset);
            break;

        case 3:
            TBLPAG = __builtin_tblpage(&EEpowerUsed3H);
            offset = __builtin_tbloffset(&EEpowerUsed3H);
            tempPowerH = __builtin_tblrdl(offset);
            TBLPAG = __builtin_tblpage(&EEpowerUsed3L);
            offset = __builtin_tbloffset(&EEpowerUsed3L);
            tempPowerL = __builtin_tblrdl(offset);
            break;

        case 4:
            TBLPAG = __builtin_tblpage(&EEpowerUsed4H);
            offset = __builtin_tbloffset(&EEpowerUsed4H);
            tempPowerH = __builtin_tblrdl(offset);
            TBLPAG = __builtin_tblpage(&EEpowerUsed4L);
            offset = __builtin_tbloffset(&EEpowerUsed4L);
            tempPowerL = __builtin_tblrdl(offset);
            break;

        case 5:
            TBLPAG = __builtin_tblpage(&EEpowerUsed5H);
            offset = __builtin_tbloffset(&EEpowerUsed5H);
            tempPowerH = __builtin_tblrdl(offset);
            TBLPAG = __builtin_tblpage(&EEpowerUsed5L);
            offset = __builtin_tbloffset(&EEpowerUsed5L);
            tempPowerL = __builtin_tblrdl(offset);
            break;

        case 6:
            TBLPAG = __builtin_tblpage(&EEpowerUsed6H);
            offset = __builtin_tbloffset(&EEpowerUsed6H);
            tempPowerH = __builtin_tblrdl(offset);
            TBLPAG = __builtin_tblpage(&EEpowerUsed6L);
            offset = __builtin_tbloffset(&EEpowerUsed6L);
            tempPowerL = __builtin_tblrdl(offset);
            break;

        case 7:
            TBLPAG = __builtin_tblpage(&EEpowerUsed7H);
            offset = __builtin_tbloffset(&EEpowerUsed7H);
            tempPowerH = __builtin_tblrdl(offset);
            TBLPAG = __builtin_tblpage(&EEpowerUsed7L);
            offset = __builtin_tbloffset(&EEpowerUsed7L);
            tempPowerL = __builtin_tblrdl(offset);
            break;

        case 8:
            TBLPAG = __builtin_tblpage(&EEpowerUsed8H);
            offset = __builtin_tbloffset(&EEpowerUsed8H);
            tempPowerH = __builtin_tblrdl(offset);
            TBLPAG = __builtin_tblpage(&EEpowerUsed8L);
            offset = __builtin_tbloffset(&EEpowerUsed8L);
            tempPowerL = __builtin_tblrdl(offset);
    }

    powerUsedMW = (((unsigned long) tempPowerH) << 16) + tempPowerL;
}

void EEwritePowerUsed() {
    unsigned int offset;
    unsigned int tempPowerH = powerUsedMW >> 16;
    unsigned int tempPowerL = powerUsedMW & 0xFFFF;
    unsigned char EEpowerSelect = findPowerToWrite();

    switch (EEpowerSelect) {
        case 1:
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed1H);
            offset = __builtin_tbloffset(&EEpowerUsed1H);
            __builtin_tblwtl(offset, tempPowerH);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed1L);
            offset = __builtin_tbloffset(&EEpowerUsed1L);
            __builtin_tblwtl(offset, tempPowerL);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed2H);
            offset = __builtin_tbloffset(&EEpowerUsed2H);
            __builtin_tblwtl(offset, 0xFFFF);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            break;

        case 2:
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed2H);
            offset = __builtin_tbloffset(&EEpowerUsed2H);
            __builtin_tblwtl(offset, tempPowerH);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed2L);
            offset = __builtin_tbloffset(&EEpowerUsed2L);
            __builtin_tblwtl(offset, tempPowerL);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed3H);
            offset = __builtin_tbloffset(&EEpowerUsed3H);
            __builtin_tblwtl(offset, 0xFFFF);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            break;

        case 3:
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed3H);
            offset = __builtin_tbloffset(&EEpowerUsed3H);
            __builtin_tblwtl(offset, tempPowerH);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed3L);
            offset = __builtin_tbloffset(&EEpowerUsed3L);
            __builtin_tblwtl(offset, tempPowerL);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed4H);
            offset = __builtin_tbloffset(&EEpowerUsed4H);
            __builtin_tblwtl(offset, 0xFFFF);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            break;

        case 4:
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed4H);
            offset = __builtin_tbloffset(&EEpowerUsed4H);
            __builtin_tblwtl(offset, tempPowerH);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed4L);
            offset = __builtin_tbloffset(&EEpowerUsed4L);
            __builtin_tblwtl(offset, tempPowerL);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed5H);
            offset = __builtin_tbloffset(&EEpowerUsed5H);
            __builtin_tblwtl(offset, 0xFFFF);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            break;

        case 5:
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed5H);
            offset = __builtin_tbloffset(&EEpowerUsed5H);
            __builtin_tblwtl(offset, tempPowerH);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed5L);
            offset = __builtin_tbloffset(&EEpowerUsed5L);
            __builtin_tblwtl(offset, tempPowerL);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed6H);
            offset = __builtin_tbloffset(&EEpowerUsed6H);
            __builtin_tblwtl(offset, 0xFFFF);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            break;

        case 6:
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed6H);
            offset = __builtin_tbloffset(&EEpowerUsed6H);
            __builtin_tblwtl(offset, tempPowerH);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed6L);
            offset = __builtin_tbloffset(&EEpowerUsed6L);
            __builtin_tblwtl(offset, tempPowerL);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed7H);
            offset = __builtin_tbloffset(&EEpowerUsed7H);
            __builtin_tblwtl(offset, 0xFFFF);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            break;

        case 7:
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed7H);
            offset = __builtin_tbloffset(&EEpowerUsed7H);
            __builtin_tblwtl(offset, tempPowerH);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed7L);
            offset = __builtin_tbloffset(&EEpowerUsed7L);
            __builtin_tblwtl(offset, tempPowerL);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed8H);
            offset = __builtin_tbloffset(&EEpowerUsed8H);
            __builtin_tblwtl(offset, 0xFFFF);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            break;

        case 8:
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed8H);
            offset = __builtin_tbloffset(&EEpowerUsed8H);
            __builtin_tblwtl(offset, tempPowerH);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed8L);
            offset = __builtin_tbloffset(&EEpowerUsed8L);
            __builtin_tblwtl(offset, tempPowerL);
            asm volatile ("disi #5");
            __builtin_write_NVM();
            while (_WR) continue;
            NVMCON = 0x4004;
            TBLPAG = __builtin_tblpage(&EEpowerUsed1H);
            offset = __builtin_tbloffset(&EEpowerUsed1H);
            __builtin_tblwtl(offset, 0xFFFF);
            asm volatile ("disi #5");
            __builtin_write_NVM();
    }
}

void EEreadHL() {
    while (_WR) continue;

    TBLPAG = __builtin_tblpage(&EEhighLow);
    unsigned int offset = __builtin_tbloffset(&EEhighLow);
    isHigh = (char) __builtin_tblrdl(offset);
}

void EEwriteHL() {
    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EEhighLow);
    unsigned int offset = __builtin_tbloffset(&EEhighLow);
    __builtin_tblwtl(offset, isHigh);

    asm volatile ("disi #5");
    __builtin_write_NVM();
}

void EEreadRelay() {
    while (_WR) continue;

    TBLPAG = __builtin_tblpage(&EErelay);
    unsigned int offset = __builtin_tbloffset(&EErelay);
    relayActive = (char) __builtin_tblrdl(offset);
}

void EEwriteRelay() {
    while (_WR) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage(&EErelay);
    unsigned int offset = __builtin_tbloffset(&EErelay);
    __builtin_tblwtl(offset, relayActive);

    asm volatile ("disi #5");
    __builtin_write_NVM();
}

unsigned char findPowerToRead() {

    unsigned int tempPower;
    unsigned char EEpowerSelect;

    TBLPAG = __builtin_tblpage(&EEpowerUsed1H);
    unsigned int offset = __builtin_tbloffset(&EEpowerUsed1H);
    tempPower = __builtin_tblrdl(offset);

    if (tempPower == 0xFFFF) {
        EEpowerSelect = 8;
    } else {
        TBLPAG = __builtin_tblpage(&EEpowerUsed2H);
        unsigned int offset = __builtin_tbloffset(&EEpowerUsed2H);
        tempPower = __builtin_tblrdl(offset);

        if (tempPower == 0xFFFF) {
            EEpowerSelect = 1;
        } else {
            TBLPAG = __builtin_tblpage(&EEpowerUsed3H);
            unsigned int offset = __builtin_tbloffset(&EEpowerUsed3H);
            tempPower = __builtin_tblrdl(offset);

            if (tempPower == 0xFFFF) {
                EEpowerSelect = 2;
            } else {
                TBLPAG = __builtin_tblpage(&EEpowerUsed4H);
                unsigned int offset = __builtin_tbloffset(&EEpowerUsed4H);
                tempPower = __builtin_tblrdl(offset);

                if (tempPower == 0xFFFF) {
                    EEpowerSelect = 3;
                } else {
                    TBLPAG = __builtin_tblpage(&EEpowerUsed5H);
                    unsigned int offset = __builtin_tbloffset(&EEpowerUsed5H);
                    tempPower = __builtin_tblrdl(offset);

                    if (tempPower == 0xFFFF) {
                        EEpowerSelect = 4;
                    } else {
                        TBLPAG = __builtin_tblpage(&EEpowerUsed6H);
                        unsigned int offset = __builtin_tbloffset(&EEpowerUsed6H);
                        tempPower = __builtin_tblrdl(offset);

                        if (tempPower == 0xFFFF) {
                            EEpowerSelect = 5;
                        } else {
                            TBLPAG = __builtin_tblpage(&EEpowerUsed7H);
                            unsigned int offset = __builtin_tbloffset(&EEpowerUsed7H);
                            tempPower = __builtin_tblrdl(offset);

                            if (tempPower == 0xFFFF) {
                                EEpowerSelect = 6;
                            } else {
                                TBLPAG = __builtin_tblpage(&EEpowerUsed8H);
                                unsigned int offset = __builtin_tbloffset(&EEpowerUsed8H);
                                tempPower = __builtin_tblrdl(offset);

                                if (tempPower == 0xFFFF) {
                                    EEpowerSelect = 7;
                                } else {
                                    EEpowerSelect = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return EEpowerSelect;
}

unsigned char findPowerToWrite() {
    unsigned char temp = findPowerToRead();
    temp = (temp % 8) + 1;
    return temp;
}
/* 
 * File:   EEPROM test main.c
 * Author: Jacqui
 *
 * Created on August 10, 2015, 9:33 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xc.h>
#include <string.h>
#include "p24FV32KA302.h"

/* Function Definitions *******************************************************/

unsigned long EEPROMreadLong();
unsigned int EEPROMreadInt(void);
void EEPROMwriteLong(unsigned long);
void EEPROMwriteInt(unsigned int);


/* Variable Definitions *******************************************************/

int __attribute__ ((space(eedata))) ee_addr;


/* Functions ******************************************************************/

unsigned int EEPROMreadInt(void) {
    while(_WR);// wait for write or erase to finish

    // Set up a pointer to the EEPROM location to be read
    TBLPAG = __builtin_tblpage(&ee_addr);
    unsigned int offset = __builtin_tbloffset(&ee_addr);
    // Read the EEPROM data
    return __builtin_tblrdl(offset);
}

void EEPROMwriteInt(unsigned int data) {
    while(_WR);
//    int __attribute__ ((space(eedata))) eeData = 0x1234; // Global variable located in EEPROM
    // Set up NVMCON to write one word of data EEPROM
    NVMCON = 0x4004;
    // Set up a pointer to the EEPROM location to be written
    TBLPAG = __builtin_tblpage(&ee_addr);
    unsigned int offset = __builtin_tbloffset(&ee_addr);
    // Write Data Value To Holding Latch
    __builtin_tblwtl(offset, data);
    // Disable Interrupts For 5 Instructions
    asm volatile ("disi #5");
    // Issue Unlock Sequence & Start Write Cycle
    __builtin_write_NVM();
}

/*
 * 
 */
int main(int argc, char** argv) {

    return (EXIT_SUCCESS);
}


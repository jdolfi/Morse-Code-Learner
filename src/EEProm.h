#ifndef EEPROM_H
#define EEPROM_H

// much straight from the datasheet

typedef unsigned char BYTE;
typedef unsigned int WORD;

void IapIdle();
BYTE IapReadByte(WORD addr);

//-----------------------------------------------
#define CMD_IDLE 0		//Stand-By
#define CMD_READ 1		//IAP Byte-Read
#define CMD_PROGRAM 2	//IAP Byte-Program
#define CMD_ERASE 3		//IAP Sector-Erase

//#define ENABLE_IAP 0x80 //if SYSCLK<30MHz
//#define ENABLE_IAP 0x81 //if SYSCLK<24MHz
//#define ENABLE_IAP 0x82 //if SYSCLK<20MHz
//#define ENABLE_IAP 0x83 //if SYSCLK<12MHz
#define ENABLE_IAP 0x84 //if SYSCLK<6MHz
//#define ENABLE_IAP 0x85 //if SYSCLK<3MHz
//#define ENABLE_IAP 0x86 //if SYSCLK<2MHz
//#define ENABLE_IAP 0x87 //if SYSCLK<1MHz
//Start address for STC15 series MCU EEPROM

// end of datasheet~ish code

void IapIdle();
BYTE IapReadByte(WORD addr);
void IapProgramByte(WORD addr, BYTE dat);
void IapEraseSector(WORD addr);

void LoadNvSettingsFromEeprom();
void StoreNvSettingsToEeprom();
void LoadNvMacroFromEeprom(unsigned char macroNum);
void StoreNvMacroToEeprom(unsigned char macroNum);

#endif

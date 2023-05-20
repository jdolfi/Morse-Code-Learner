#include "stc15_1.h"
#include "EEProm.h"
#include "NvSettings.h"
#include "KeyTask.h"

// much straight from the datasheet

/*----------------------------
Disable ISP/IAP/EEPROM function
Make MCU in a safe state
----------------------------*/
void IapIdle()
{
 IAP_CONTR = 0; 	//Close IAP function
 IAP_CMD = 0; 		//Clear command to standby
 IAP_TRIG = 0; 		//Clear trigger register
 IAP_ADDRH = 0x80; 	//Data ptr point to non-EEPROM area
 IAP_ADDRL = 0; 	//Clear IAP address to prevent misuse
}
/*----------------------------
Read one byte from ISP/IAP/EEPROM area
Input: addr (ISP/IAP/EEPROM address)
Output:Flash data
----------------------------*/
BYTE IapReadByte(WORD addr)
{
	BYTE dat; 				//Data buffer
	IAP_CONTR = ENABLE_IAP;	//Open IAP function, and set wait time
	IAP_CMD = CMD_READ; 	//Set ISP/IAP/EEPROM READ command
	IAP_ADDRL = addr; 		//Set ISP/IAP/EEPROM address low
	IAP_ADDRH = addr >> 8; 	//Set ISP/IAP/EEPROM address high
	IAP_TRIG = 0x5a; 		//Send trigger command1 (0x5a)
	IAP_TRIG = 0xa5; 		//Send trigger command2 (0xa5)
	__asm__(" nop"); 		//MCU will hold here until ISP/IAP/EEPROM

	//operation complete
	dat = IAP_DATA; //Read ISP/IAP/EEPROM data
	IapIdle(); //Close ISP/IAP/EEPROM function
	return dat; //Return Flash data
}

/*----------------------------
Program one byte to ISP/IAP/EEPROM area
Input: addr (ISP/IAP/EEPROM address)
 dat (ISP/IAP/EEPROM data)
Output:-
----------------------------*/
void IapProgramByte(WORD addr, BYTE dat)
{
	IAP_CONTR = ENABLE_IAP;	//Open IAP function, and set wait time
	IAP_CMD = CMD_PROGRAM; 	//Set ISP/IAP/EEPROM PROGRAM command
	IAP_ADDRL = addr; 		//Set ISP/IAP/EEPROM address low
	IAP_ADDRH = addr >> 8;	//Set ISP/IAP/EEPROM address high
	IAP_DATA = dat; 		//Write ISP/IAP/EEPROM data
	IAP_TRIG = 0x5a; 		//Send trigger command1 (0x5a)
	IAP_TRIG = 0xa5; 		//Send trigger command2 (0xa5)
	__asm__(" nop"); 	//MCU will hold here until ISP/IAP/EEPROM
		
	//operation complete
	IapIdle();
}
/*----------------------------
Erase one sector area
Input: addr (ISP/IAP/EEPROM address)
Output:-
----------------------------*/
void IapEraseSector(WORD addr)
{
	IAP_CONTR = ENABLE_IAP;	//Open IAP function, and set wait time
	IAP_CMD = CMD_ERASE; 	//Set ISP/IAP/EEPROM ERASE command
	IAP_ADDRL = addr; 		//Set ISP/IAP/EEPROM address low
	IAP_ADDRH = addr >> 8;	//Set ISP/IAP/EEPROM address high
	IAP_TRIG = 0x5a; 		//Send trigger command1 (0x5a)
	IAP_TRIG = 0xa5; 		//Send trigger command2 (0xa5)
	__asm__(" nop"); 		//MCU will hold here until ISP/IAP/EEPROM
	
	//operation complete
	IapIdle();
}

void LoadNvSettingsFromEeprom()
{
	unsigned char* nvRam = (unsigned char*)nvSettings ;
	
	for (int i=0; i<(NV_SIZE*2); i++)
	{
		nvRam[i] = IapReadByte(SETTINGS_ADDRESS+i);
	}
}

void StoreNvSettingsToEeprom()
{
	unsigned char* nvRam = (unsigned char*)nvSettings ;
	
	IapEraseSector(SETTINGS_ADDRESS); 
	for (int i=0; i<(NV_SIZE*2); i++)
	{
		IapProgramByte(SETTINGS_ADDRESS+i,nvRam[i]);
	}	
}

void LoadNvMacroFromEeprom(unsigned char macroNum)
{ 
	unsigned short eeAddress ;
	
	if (macroNum == 3)
		eeAddress = MACRO3_ADDRESS ;
	else if (macroNum == 2)
		eeAddress = MACRO2_ADDRESS ;
	else
		eeAddress = MACRO1_ADDRESS ;
	
	
	for (int i=0; i<MACRO_MEM_SIZE; i++)
	{
		macroMem[i] = IapReadByte(eeAddress+i);
	}
	
	if ( macroMem[0] > (MACRO_MEM_SIZE*4) )
		macroMem[0] = 0;
	
}

void StoreNvMacroToEeprom(unsigned char macroNum)
{
	unsigned short eeAddress ;
	
	if (macroNum == 3)
		eeAddress = MACRO3_ADDRESS ;
	else if (macroNum == 2)
		eeAddress = MACRO2_ADDRESS ;
	else
		eeAddress = MACRO1_ADDRESS ;
	
	IapEraseSector(eeAddress); 
	for (int i=0; i<MACRO_MEM_SIZE; i++)
	{
		IapProgramByte(eeAddress+i,macroMem[i]);
	}	
}

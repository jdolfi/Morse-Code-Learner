#ifndef KEY_TASK_H
#define KEY_TASK_H

#include "NvSettings.h"

typedef enum
{
	KEY_STATE_IDLE			= 0x00,
	KEY_STATE_STRAIGHT_IDLE		= 0x01,
	KEY_STATE_STRAIGHT_DOWN		= 0x02,	
	
	KEY_STATE_CHAR_IDLE		= 0x03,
	KEY_STATE_CHAR_START		= 0x04,
	KEY_STATE_CHAR_DIT		= 0x05,
	KEY_STATE_CHAR_DAH		= 0x06,
	KEY_STATE_CHAR_DXX_GAP		= 0x07,
	KEY_STATE_CHAR_LETTER_GAP	= 0x08,
	KEY_STATE_CHAR_SPACE		= 0x09,
	
	KEY_STATE_IAMBIC_IDLE 		= 0x0A,
	KEY_STATE_IAMBIC_DIT		= 0x0B,
	KEY_STATE_IAMBIC_DAH		= 0x0C,
	KEY_STATE_IAMBIC_DIT_GAP	= 0x0D,
	KEY_STATE_IAMBIC_DAH_GAP	= 0x0E,
	
	KEY_STATE_MACRO_IDLE		= 0x10,
	KEY_STATE_MACRO_PLAY		= 0x11,
	KEY_STATE_MACRO_WAIT		= 0x12,
	KEY_STATE_MACRO_DIT		= 0x13,
	KEY_STATE_MACRO_DAH		= 0x14,
	KEY_STATE_MACRO_DXX_GAP		= 0x15,	
	KEY_STATE_MACRO_LETTER_GAP	= 0x16,	
	KEY_STATE_MACRO_WORD_GAP	= 0x17,	
	
	
} KEY_STATE_ENUM ;

void KeyTask();
void ConfigureKeyForOperate();
void ConfigureKeyForMacro();
void ConfigureKeyForMessages();
void ConfigureKeyForSettings();

void SetKeyCharCode(unsigned char code);

void StartMacroRecord();
void StopMacroRecord();

#define MACRO_MEM_SIZE 30
extern unsigned char macroMem[MACRO_MEM_SIZE];

#endif

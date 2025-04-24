#ifndef KEY_TASK_H
#define KEY_TASK_H

#include "NvSettings.h"

typedef enum
{
	KEY_STATE_DISABLE			= 0x00,
	KEY_STATE_IDLE				= 0x01,
	KEY_STATE_STRAIGHT_IDLE		= 0x02,
	KEY_STATE_STRAIGHT_DOWN		= 0x03,	
	
	KEY_STATE_IAMBIC_IDLE		= 0x04,
	KEY_STATE_IAMBIC_DIT		= 0x05,
	KEY_STATE_IAMBIC_DAH		= 0x06,
	KEY_STATE_IAMBIC_DIT_GAP	= 0x07,
	KEY_STATE_IAMBIC_DAH_GAP	= 0x08,
	
} KEY_STATE_ENUM ;

void KeyTask();
void KeyTaskEnable();
void KeyTaskDisable();

#endif

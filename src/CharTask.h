#ifndef CHAR_TASK_H
#define CHAR_TASK_H

typedef enum
{
	CHAR_STATE_DISABLE		= 0x01,
	CHAR_STATE_IDLE			= 0x02,
	CHAR_STATE_START		= 0x03,
	CHAR_STATE_DIT			= 0x04,
	CHAR_STATE_DAH			= 0x05,
	CHAR_STATE_DXX_GAP		= 0x06,
	CHAR_STATE_LETTER_GAP	= 0x07,
	CHAR_STATE_SPACE		= 0x08,
		
} CHAR_STATE_ENUM ;

void CharTask();
void CharTaskEnable();
void CharTaskDisable();
void CharTaskSetMsg(unsigned char const * msg);

#endif

#ifndef SPELL_TASK_H
#define SPELL_TASK_H

#include "SpellTable.h"

typedef enum
{
	SPELL_STATE_IDLE = 0x00,
	SPELL_STATE_WAIT = 0x01,
} SPELL_STATE_ENUM ;

void SetSpellMsg(unsigned char const * msg);
void SpellTask();

#endif

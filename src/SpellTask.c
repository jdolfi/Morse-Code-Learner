#include "Signals.h"
#include "KeyTask.h"
#include "SpellTask.h"

static SPELL_STATE_ENUM spellState = SPELL_STATE_IDLE;
static unsigned char const *pMessage = 0;

void SetSpellMsg(unsigned char const * msg)
{
	pMessage = msg ;
	spellState = SPELL_STATE_IDLE ;
}

void SpellTask()
{
	switch (spellState)
	{
		case SPELL_STATE_IDLE:
			if (pMessage)
			{
				if (*pMessage == MSG_END)
				{
					pMessage = 0;
					SetSignal(SIGNAL_MSG_END);
				}
				else
				{
					spellState = SPELL_STATE_WAIT ;	
					SetKeyCharCode(*pMessage) ;
					pMessage++;
				}
			}
		break ;
		
		case SPELL_STATE_WAIT:
			if (SignalIsSet(SIGNAL_CHAR_END))
			{
				ClearSignal(SIGNAL_CHAR_END);
				spellState = SPELL_STATE_IDLE ;
			}
		break ;
	}
	
}

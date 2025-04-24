#include "CharTask.h"
#include "SpellTable.h"
#include "Signals.h"
#include "NvSettings.h"
#include "OutControl.h"
#include "HwAbstraction.h"

static unsigned char 	charMask = 0;
static unsigned char	charCode = 0;
CHAR_STATE_ENUM charState = CHAR_STATE_DISABLE ;

static unsigned short  charUnitTimerValue ;
static unsigned short  charToneTimerValue;
static unsigned char  charTickCount ;
static unsigned char const *charMessage = 0;

void CharTaskSetMsg(unsigned char const * msg)
{
	charMessage = msg ;
	charState = CHAR_STATE_IDLE ;
}

void CharTaskEnable()
{
	charToneTimerValue = calcToneTimerValue(1200);
	charUnitTimerValue = calcUnitTimerValue(15);
	OutInitialize(OUT_MODE_OFF);
	
	charState = CHAR_STATE_IDLE ;
}

void CharTaskDisable()
{
	charState = CHAR_STATE_DISABLE ;
}

void CharTask()
{
	
	switch (charState)
	{
		case CHAR_STATE_DISABLE:
		break ;
						
		case CHAR_STATE_IDLE:
			if (charMessage)
				charCode = *charMessage ;
			
			if (charCode == MSG_END)
			{
				SBIT(signalMsgEnd);
				charCode=CHAR_NULL;
				charMessage=0;
			}
			
			if (charCode != CHAR_NULL)
			{	
				charMessage++;
				
				charMask = 0x80 ;
				if ( !(charMask & charCode) )
				{
					charState = CHAR_STATE_SPACE;
					break;
				}
				
				while (charMask & charCode)
					charMask = charMask >> 1 ;

				HwRestartUnitTimer(charUnitTimerValue);
				CBIT(charUnitTick);					
				
				charState = CHAR_STATE_START;
			}
		break;
		
		case CHAR_STATE_START:
			charTickCount=0;
			
			charMask = charMask >> 1 ;
			if (!charMask)
			{
				charCode = 0;
				
				charState = CHAR_STATE_LETTER_GAP ;
				break ;
			}
			
			OutEnable(charToneTimerValue);
							
			if (charMask & charCode)
				charState = CHAR_STATE_DAH ;
			else
				charState = CHAR_STATE_DIT ;
		
		break ;

		case CHAR_STATE_DIT:
			if (charUnitTick)
			{
				CBIT(charUnitTick);
				OutDisable();
				
				charState = CHAR_STATE_DXX_GAP ;
			}			
		break;
		
		case CHAR_STATE_DAH:
			if (charUnitTick)
			{
				CBIT(charUnitTick);
				
				charTickCount++ ;
				if (charTickCount >= 3)
				{
					OutDisable();
					charState = CHAR_STATE_DXX_GAP ;
				}
			}
		break;

		case CHAR_STATE_DXX_GAP:
			if (charUnitTick)
			{
				CBIT(charUnitTick);
				charState = CHAR_STATE_START ;
			}
		break ;

		case CHAR_STATE_LETTER_GAP:
			if (charUnitTick)
			{
				CBIT(charUnitTick);
				
				charTickCount++ ;
				if (charTickCount >= 2)
					charState = CHAR_STATE_IDLE ;
			}
		break ;
		
		case CHAR_STATE_SPACE:
			if (charUnitTick)
			{
				CBIT(charUnitTick);
				if (!charCode)
					charState = CHAR_STATE_IDLE ;
				else
					charCode-- ;
			}		
		break ;
		
	}
	
}

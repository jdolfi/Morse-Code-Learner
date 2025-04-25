#include "PlayTask.h"
#include "HwAbstraction.h"
#include "Signals.h"
#include "NvSettings.h"
#include "OutControl.h"
#include "MacroRecord.h"
#include "LedTask.h"
#include "ButtonTask.h"

static unsigned char  unitTickCount = 0;
static unsigned short playToneTimerValue =0;
static unsigned short playUnitTimerValue =0;

extern unsigned char  macroMem[MACRO_MEM_SIZE];

static unsigned char macroMemIdx = 0;
static unsigned char macroElemIdx = 0;
static unsigned char macroElemByte = 0;
static unsigned char macroElemCount = 0;
static unsigned char macroReplayCount = 0;
static unsigned char macroAbortCount = 0;

static PLAY_STATE_ENUM playState = PLAY_STATE_DISABLE ;

void PlayTaskEnable()
{	  		
	if (playState != PLAY_STATE_DISABLE)
		return;
		
	playUnitTimerValue = calcUnitTimerValue(nvSettings[NV_WPM]);
	playToneTimerValue = calcToneTimerValue(nvSettings[NV_TONE]);
	playState = PLAY_STATE_IDLE ;
}

void PlayTask()
{
	if (playState != PLAY_STATE_DISABLE)
	{
		if (signalDitLatch || signalDahLatch || 
			HwPB0Pressed() || HwPB1Pressed() || HwPB2Pressed() || HwPB3Pressed())
		{
			OutOff();
			macroMem[0]=0;

			playState = PLAY_STATE_ABORT;
		}
	}
	
	switch (playState)
	{		
		case PLAY_STATE_DISABLE:
		break;

		case PLAY_STATE_ABORT:
			if (signalReplayTick)
			{
				CBIT(signalReplayTick);				
				macroAbortCount++ ;

				// wait half second
				if (macroAbortCount > 2 )
				{	
					macroAbortCount=0;
					OutOff();
					playState = PLAY_STATE_DISABLE ;					
				}
			}		
		break;		

		case PLAY_STATE_IDLE:
			if (macroMem[0])
			{
				macroMemIdx = 1;
				macroElemIdx = 0;
				macroElemCount = macroMem[0];
				macroElemByte = macroMem[macroMemIdx++];
				playState = PLAY_STATE_PLAY ;
				
				HwRestartUnitTimer(playUnitTimerValue);
				CBIT(playUnitTick);					
			}
		break ;
		
		case PLAY_STATE_PLAY:
			if (!macroElemCount)
			{
				macroReplayCount=0;
				playState = PLAY_STATE_WAIT ;
			}			
			else
			{
				unitTickCount = 0;
				switch (macroElemByte & 0xC0)
				{
					case 0xC0:	// word gap
						playState = PLAY_STATE_WORD_GAP ;
					break ;
					
					case 0x80:	// letter gap
						playState = PLAY_STATE_LETTER_GAP ;
					break ;
					
					case 0x40:	// dah
						playState = PLAY_STATE_DAH ;
						OutEnable(playToneTimerValue);												
					break ;
					
					case 0x00:	// dit
						playState = PLAY_STATE_DIT ;
						OutEnable(playToneTimerValue);												
					break ;
				}				
				
				macroElemCount--;
				if (macroElemIdx<3)
				{
					macroElemByte <<= 2;
					macroElemIdx++ ;
				}
				else
				{
					macroElemIdx=0;
					macroElemByte = macroMem[macroMemIdx++];
				}
			}
		break ;
		
		case PLAY_STATE_WAIT:
		
			hfLed(LED_GREEN, LED_OFF);

			if (!nvSettings[NV_REPLAY])
			{
				OutOff();
				playState = PLAY_STATE_DISABLE ;
			}
			else if (signalReplayTick)
			{
				CBIT(signalReplayTick);
				macroReplayCount++ ;
				// 2 seconds / nvSettings[NV_REPLAY]
				if (macroReplayCount > 8*nvSettings[NV_REPLAY] )  
					playState = PLAY_STATE_IDLE ;
			}
		break ;
		
		case PLAY_STATE_DIT:
			if (playUnitTick)
			{
				CBIT(playUnitTick);
				OutDisable();
				
				playState = PLAY_STATE_DXX_GAP ;
			}			
		break;		
		
		case PLAY_STATE_DAH:
			if (playUnitTick)
			{
				CBIT(playUnitTick);
				
				unitTickCount++ ;
				if (unitTickCount >= 3)
				{
					OutDisable();
					playState = PLAY_STATE_DXX_GAP ;
				}
			}		
		break;		

		case PLAY_STATE_DXX_GAP:
			if (playUnitTick)
			{
				CBIT(playUnitTick);				
				playState = PLAY_STATE_PLAY ;
			}
		break ;

		case PLAY_STATE_LETTER_GAP:
			if (playUnitTick)
			{
				CBIT(playUnitTick);
				
				unitTickCount++ ;
				if (unitTickCount >= 2)
				{		
					unitTickCount = 0;			
					playState = PLAY_STATE_PLAY ;
				}
			}		
		break ;
		
		case PLAY_STATE_WORD_GAP:
			if (playUnitTick)
			{
				CBIT(playUnitTick);
				
				unitTickCount++ ;
				if (unitTickCount >= 4)
					playState = PLAY_STATE_PLAY ;
			}		
		break ;
	}
	
}

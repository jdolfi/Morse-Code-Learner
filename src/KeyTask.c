#include "NvSettings.h"
#include "OutControl.h"
#include "MacroRecord.h"
#include "HwAbstraction.h"
#include "Signals.h"
#include "KeyTask.h"

static KEY_STATE_ENUM	keyState = KEY_STATE_DISABLE;
static unsigned char	unitTickCount = 0;
static unsigned char	iambicACount = 0;

static unsigned short 	keyToneTimerValue;
static unsigned short 	keyUnitTimerValue;
static KEY_TYPE_ENUM	keyType ;

void KeyTaskEnable()
{
	keyUnitTimerValue = calcUnitTimerValue(nvSettings[NV_WPM]);
	keyToneTimerValue = calcToneTimerValue(nvSettings[NV_TONE]);
	keyType = nvSettings[NV_KEY_TYPE];	
	
	OutOff();
	keyState = KEY_STATE_IDLE ;
}

void KeyTaskDisable()
{
	OutOff();
	keyState = KEY_STATE_DISABLE ;	
}

void KeyTask()
{
	
	switch (keyState)
	{
				
		case KEY_STATE_DISABLE:
		break ;
		
		case KEY_STATE_IDLE:
																		
			switch (keyType)
			{
				case IAMBIC_A_KEY:
				case IAMBIC_B_KEY:
					keyState = KEY_STATE_IAMBIC_IDLE;
				break;
				
				default:
					keyState = KEY_STATE_STRAIGHT_IDLE;
				break ;
			}
						
		break;

		case KEY_STATE_STRAIGHT_IDLE:		
			if ( HwDahPressed() || HwDitPressed() )
			{				
				keyState = KEY_STATE_STRAIGHT_DOWN ;
				OutEnable(keyToneTimerValue);
			}	
		break ;

		case KEY_STATE_STRAIGHT_DOWN:
			if ( !HwDahPressed() && !HwDitPressed() )
			{
				keyState = KEY_STATE_IDLE ;
				OutDisable();
			}
		break ;
		
		case KEY_STATE_IAMBIC_IDLE:
		
			if (keyUnitTick)
			{
				CBIT(keyUnitTick);
				unitTickCount++;
			}		
		
			if ( HwDitPressed() )
			{
				keyState = KEY_STATE_IAMBIC_DIT;
				MacroRecordGap(unitTickCount);
			}
			else if ( HwDahPressed() )
			{
				keyState = KEY_STATE_IAMBIC_DAH;
				MacroRecordGap(unitTickCount);
			}
			else
				break ;

			OutEnable(keyToneTimerValue);
			HwRestartUnitTimer(keyUnitTimerValue);
			CBIT(keyUnitTick);
			unitTickCount=0;
			iambicACount=0;
				
		break ;
				
		case KEY_STATE_IAMBIC_DIT:
			if (keyUnitTick)
			{
				CBIT(keyUnitTick);
				OutDisable();				
				
				keyState = KEY_STATE_IAMBIC_DIT_GAP ;
				MacroRecordSymbol(MACRO_SYMBOL_DIT);
			}			
		break;
		
		case KEY_STATE_IAMBIC_DAH:
			if (keyUnitTick)
			{
				CBIT(keyUnitTick);
				
				unitTickCount++ ;
				if (unitTickCount >= 3)
				{
					OutDisable();					
					
					keyState = KEY_STATE_IAMBIC_DAH_GAP ;
					MacroRecordSymbol(MACRO_SYMBOL_DAH);
				}
			}
		break;
				
		case KEY_STATE_IAMBIC_DIT_GAP:
		case KEY_STATE_IAMBIC_DAH_GAP:
			if (keyUnitTick)
			{
				CBIT(keyUnitTick);	
				
				unitTickCount = 0;				
				iambicACount++ ;			

				if (keyState == KEY_STATE_IAMBIC_DIT_GAP)
					CBIT(signalDitLatch) ;
				else if (keyState == KEY_STATE_IAMBIC_DAH_GAP)
					CBIT(signalDahLatch) ;

				if ( !HwDahPressed() && !HwDitPressed() && 
					( keyType == IAMBIC_A_KEY) && (iambicACount>=2) )
				{
					CBIT(signalDahLatch);
					CBIT(signalDitLatch);
					keyState = KEY_STATE_IDLE ;
					break ;
				}
				else if ( (keyState == KEY_STATE_IAMBIC_DAH_GAP) && signalDitLatch)
					keyState = KEY_STATE_IAMBIC_DIT;
				else if ( signalDahLatch )
					keyState = KEY_STATE_IAMBIC_DAH;
				else if ( signalDitLatch )
					keyState = KEY_STATE_IAMBIC_DIT;
				else
				{
					keyState = KEY_STATE_IDLE ;			
					break;
				}

				OutEnable(keyToneTimerValue);
			}
		break;

	}
	
}

#include "HwAbstraction.h"
#include "Signals.h"
#include "LedTask.h"
#include "NvSettings.h"
#include "KeyTask.h"

static unsigned char	latchedKeys = 0;

static KEY_STATE_ENUM	keyState = KEY_STATE_IDLE;
static unsigned char	unitTickCount = 0;
static unsigned char	iambicACount = 0;
static unsigned char	keyTimeoutCount = 0;

static unsigned short 	keyToneTimerValue ;
static unsigned short 	keyUnitTimerValue ;
static OUT_MODE_ENUM 	keyOutMode ;
static KEY_TYPE_ENUM	keyType ;

static unsigned char 	charMask = 0;
static unsigned char	charCode = 0;

#define MACRO_MEM_SIZE 30
unsigned char	macroMem[MACRO_MEM_SIZE] = {
	// Bens Best Bent Wire
	52,
	0b01000000,	// b
	0b10001001,	// -e n
	0b00100000,	// n- s
	0b00110100,	// s_ b
	0b00001000,	// b-e
	0b10000000,	// -s
	0b10011101,	//-t_ b
	0b00000010,	// b-
	0b00100100,	// e-n
	0b10011100,	// -t_ w
	0b01011000,	// w- i
	0b00100001,	// i- r
	0b00100011, // r-e_
};

static unsigned char memIdx = 0;
static unsigned char elemIdx = 0;
static unsigned char elemByte = 0;
static unsigned char elemCount = 0;
static unsigned char macroIdle = 0;
static unsigned char macroRecording = 0;

void StartMacroRecord()
{	
	elemIdx = 0;
	elemByte = 0;
	elemCount=0;
	memIdx=1;
	
	macroIdle = 1 ;
}

void StopMacroRecord()
{
	if (elemCount > 1)
	{
		while (elemIdx < 3)
		{
			elemByte <<= 2;
			elemIdx++;
		}
		macroMem[memIdx++] = elemByte;
		macroMem[0] = elemCount ;
	}
		
	macroRecording = 0;
	macroIdle = 0;
}

void SetMacroMem(unsigned char bits)
{
	bits &= 0x03;
	
	if (macroIdle)
	{
		if ( ( bits == 0 ) || (bits == 1) )
			macroRecording = 1;
	}
	
	if (!macroRecording)
		return ;

	elemByte |= bits; 	
	elemCount++ ;
		
	if (elemIdx < 3)
	{
		elemByte <<= 2 ;	
		elemIdx++ ;
	}
	else
	{ 
		macroMem[memIdx++] = elemByte;
		if (memIdx > MACRO_MEM_SIZE)
			StopMacroRecord();
		elemByte = 0;
		elemIdx = 0 ;
	}

}

void SetMacroGap(unsigned char tickCount)
{
	if (!macroRecording)
		return ;
		
	while (tickCount >= 6)
	{
		SetMacroMem(3);
		tickCount -= 6;
	}
	
	while (tickCount >= 2)
	{
		SetMacroMem(2);
		tickCount -= 2;
	}
	
}

void SetKeyCharCode(unsigned char code)
{
	charCode = code ;
}

void KeyDown()
{
	if (keyOutMode == OUT_MODE_OFF)
	{
		hfSolid(LED_GREEN);	
//		uvSolid(LED_OFF);
		HwKeyerOut(0);		
	}
	else if (keyOutMode == OUT_MODE_HF)
	{
		hfSolid(LED_RED);
		uvSolid(LED_OFF);
		HwKeyerOut(1);
	}
	else if (keyOutMode == OUT_MODE_UV)
	{
		hfSolid(LED_GREEN);
		uvSolid(LED_RED);
		HwKeyerOut(1);		
	}

	keyTimeoutCount = 0; // hotKey until timeout
	HwStartToneTimer(keyToneTimerValue);	
}

void KeyUp()
{
	if (keyOutMode == OUT_MODE_OFF)
	{
		hfSolid(LED_OFF);	
//		uvSolid(LED_OFF);
		HwKeyerOut(0);
	}
	else if (keyOutMode == OUT_MODE_HF)
	{
		hfSolid(LED_GREEN);
		uvSolid(LED_OFF);
		HwKeyerOut(0);
	}
	else if (keyOutMode == OUT_MODE_UV)
	{
		hfSolid(LED_OFF);
		uvSolid(LED_RED);
		HwKeyerOut(1);
	}	

	HwStopToneTimer();	
}

void KeyOff()
{
	if (keyOutMode == OUT_MODE_OFF)
	{
		hfSolid(LED_OFF);	
		//uvSolid(LED_OFF);
	}
	else if (keyOutMode == OUT_MODE_HF)
	{
		hfSolid(LED_GREEN);
		uvSolid(LED_OFF);
	}
	else if (keyOutMode == OUT_MODE_UV)
	{
		hfSolid(LED_OFF);
		uvSolid(LED_GREEN);
	}	

	keyState = KEY_STATE_IDLE ;
	latchedKeys=0;
	charCode=0;
	charMask=0;
	HwKeyerOut(0);
	HwStopToneTimer();	
}

void ConfigureKeyForOperate()
{
	keyUnitTimerValue = calcUnitTimerValue(nvSettings[NV_WPM]);
	keyToneTimerValue = calcToneTimerValue(nvSettings[NV_TONE]) ;
	keyType = nvSettings[NV_KEY_TYPE];	
	keyOutMode = nvSettings[NV_OUT_MODE];
	
	KeyOff();
}

void ConfigureKeyForMacro()
{
	keyUnitTimerValue = calcUnitTimerValue(nvSettings[NV_WPM]);
	keyToneTimerValue = calcToneTimerValue(nvSettings[NV_TONE]) ;
	keyType = nvSettings[NV_KEY_TYPE];	
	keyOutMode = nvSettings[NV_OUT_MODE];
	
	KeyOff();
	keyState = KEY_STATE_MACRO_IDLE ;
}

void ConfigureKeyForMessages()
{
	keyToneTimerValue = calcToneTimerValue(1200);
	keyUnitTimerValue = calcUnitTimerValue(15);
	keyType = CHAR_KEY;
	keyOutMode = OUT_MODE_OFF;

	KeyOff();
}

void ConfigureKeyForSettings()
{
	keyUnitTimerValue = calcUnitTimerValue(nvSettings[NV_WPM]);
	keyToneTimerValue = calcToneTimerValue(nvSettings[NV_TONE]) ;
	keyType = nvSettings[NV_KEY_TYPE];	
	keyOutMode = OUT_MODE_OFF;
	
	KeyOff();
	uvLed(LED_RED, LED_OFF);
}

void KeyTask()
{
	latchedKeys |= HwKeysPressed() ;

	// uv timeout and general key output watchdog
	if (SignalIsSet(SIGNAL_KEY_TICK))
	{
		ClearSignal(SIGNAL_KEY_TICK);
		keyTimeoutCount++ ;
		if (keyTimeoutCount > 200)
		{
			KeyOff();
			keyTimeoutCount=0;
		}
	}
	
	switch (keyState)
	{
		case KEY_STATE_IDLE:
						
			switch (keyType)
			{
				case IAMBIC_A_KEY:
				case IAMBIC_B_KEY:
					keyState = KEY_STATE_IAMBIC_IDLE;
				break;
				
				case CHAR_KEY:
					keyState = KEY_STATE_CHAR_IDLE ;
				break ;
				
				default:
					keyState = KEY_STATE_STRAIGHT_IDLE;
				break ;
			}
						
		break;

		case KEY_STATE_MACRO_IDLE:
			if (macroMem[0])
			{
				memIdx = 1;
				elemIdx = 0;
				elemCount = macroMem[0];
				elemByte = macroMem[memIdx++];
				keyState = KEY_STATE_MACRO_PLAY ;
				
				HwRestartUnitTimer(keyUnitTimerValue);
				ClearSignal(SIGNAL_UNIT_TICK);					
			}
		break ;
		
		case KEY_STATE_MACRO_PLAY:
			if (!elemCount)
				keyState = KEY_STATE_IDLE ;
			else
			{
				unitTickCount = 0;
				switch (elemByte & 0xC0)
				{
					case 0xC0:	// word gap
						keyState = KEY_STATE_MACRO_WORD_GAP ;
					break ;
					
					case 0x80:	// letter gag
						keyState = KEY_STATE_MACRO_LETTER_GAP ;
					break ;
					
					case 0x40:	// dah
						keyState = KEY_STATE_MACRO_DAH ;
						KeyDown();												
					break ;
					
					case 0x00:	// dit
						keyState = KEY_STATE_MACRO_DIT ;
						KeyDown();												
					break ;
				}				
				
				elemCount--;
				if (elemIdx<3)
				{
					elemByte <<= 2;
					elemIdx++ ;
				}
				else
				{
					elemIdx=0;
					elemByte = macroMem[memIdx++];
				}
			}
		break ;
		
		case KEY_STATE_MACRO_DIT:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);
				KeyUp();
				
				keyState = KEY_STATE_MACRO_DXX_GAP ;
			}			
		break;		
		
		case KEY_STATE_MACRO_DAH:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);
				
				unitTickCount++ ;
				if (unitTickCount >= 3)
				{
					KeyUp();
					keyState = KEY_STATE_MACRO_DXX_GAP ;
				}
			}		
		break;		

		case KEY_STATE_MACRO_DXX_GAP:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);				
				keyState = KEY_STATE_MACRO_PLAY ;
			}
		break ;

		case KEY_STATE_MACRO_LETTER_GAP:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);
				
				unitTickCount++ ;
				if (unitTickCount >= 2)
				{		
					unitTickCount = 0;			
					keyState = KEY_STATE_MACRO_PLAY ;
				}
			}		
		break ;
		
		case KEY_STATE_MACRO_WORD_GAP:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);
				
				unitTickCount++ ;
				if (unitTickCount >= 4)
					keyState = KEY_STATE_MACRO_PLAY ;
			}		
		break ;
				
		case KEY_STATE_CHAR_IDLE:
			if (charCode)
			{
				charMask = 0x80 ;
				if ( !(charMask & charCode) )
				{
					keyState = KEY_STATE_CHAR_SPACE;
					break;
				}
				
				while (charMask & charCode)
					charMask = charMask >> 1 ;

				HwRestartUnitTimer(keyUnitTimerValue);
				ClearSignal(SIGNAL_UNIT_TICK);					
				
				keyState = KEY_STATE_CHAR_START;
			}
		break;
		
		case KEY_STATE_CHAR_START:
			charMask = charMask >> 1 ;
		
			if (!charMask)
			{
				charCode = 0;
				unitTickCount=0;
				keyState = KEY_STATE_CHAR_LETTER_GAP ;
				break ;
			}
			
			KeyDown();
			unitTickCount=0;
							
			if (charMask & charCode)
				keyState = KEY_STATE_CHAR_DAH ;
			else
				keyState = KEY_STATE_CHAR_DIT ;
		
		break ;

		case KEY_STATE_CHAR_DIT:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);
				KeyUp();
				
				keyState = KEY_STATE_CHAR_DXX_GAP ;
			}			
		break;
		
		case KEY_STATE_CHAR_DAH:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);
				
				unitTickCount++ ;
				if (unitTickCount >= 3)
				{
					KeyUp();
					keyState = KEY_STATE_CHAR_DXX_GAP ;
				}
			}
		break;

		case KEY_STATE_CHAR_DXX_GAP:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);				
				keyState = KEY_STATE_CHAR_START ;
			}
		break ;

		case KEY_STATE_CHAR_LETTER_GAP:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);
				
				unitTickCount++ ;
				if (unitTickCount >= 2)
				{
					keyState = KEY_STATE_CHAR_IDLE ;
					SetSignal(SIGNAL_CHAR_END);					
				}
			}
		break ;
		
		case KEY_STATE_CHAR_SPACE:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);
				if (!charCode)
				{
					keyState = KEY_STATE_CHAR_IDLE ;
					SetSignal(SIGNAL_CHAR_END);	
				}
				else
					charCode-- ;
			}		
		break ;
		
		case KEY_STATE_STRAIGHT_IDLE:		
			if ( HwKeysPressed() )
			{
				keyState = KEY_STATE_STRAIGHT_DOWN ;
				KeyDown();
			}	
		break ;

		case KEY_STATE_STRAIGHT_DOWN:
			if ( !HwKeysPressed() )
			{
				keyState = KEY_STATE_IDLE ;
				KeyUp();
			}
		break ;
		
		case KEY_STATE_IAMBIC_IDLE:
		
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);
				unitTickCount++;
			}
		
			if ( latchedKeys & DIT_PRESSED )
			{
				keyState = KEY_STATE_IAMBIC_DIT;
				SetMacroGap(unitTickCount);
			}
			else if ( latchedKeys & DAH_PRESSED )
			{
				keyState = KEY_STATE_IAMBIC_DAH;
				SetMacroGap(unitTickCount);
			}
			else
				break ;

			KeyDown();
			HwRestartUnitTimer(keyUnitTimerValue);
			ClearSignal(SIGNAL_UNIT_TICK);
			unitTickCount=0;
			iambicACount=0;
				
		break ;
				
		case KEY_STATE_IAMBIC_DIT:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);
				KeyUp();				
				
				keyState = KEY_STATE_IAMBIC_DIT_GAP ;
				SetMacroMem(0);
			}			
		break;
		
		case KEY_STATE_IAMBIC_DAH:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				ClearSignal(SIGNAL_UNIT_TICK);
				
				unitTickCount++ ;
				if (unitTickCount >= 3)
				{
					KeyUp();					
					
					keyState = KEY_STATE_IAMBIC_DAH_GAP ;
					SetMacroMem(1);
				}
			}
		break;
				
		case KEY_STATE_IAMBIC_DIT_GAP:
		case KEY_STATE_IAMBIC_DAH_GAP:
			if (SignalIsSet(SIGNAL_UNIT_TICK))
			{
				unitTickCount = 0;
				ClearSignal(SIGNAL_UNIT_TICK);	
				iambicACount++ ;			

				if (keyState == KEY_STATE_IAMBIC_DIT_GAP)
					latchedKeys &= ~DIT_PRESSED ;
				else if (keyState == KEY_STATE_IAMBIC_DAH_GAP)
					latchedKeys &= ~DAH_PRESSED ;			
						
				latchedKeys |= HwKeysPressed() ;				

				if ( !HwKeysPressed() && ( keyType == IAMBIC_A_KEY) && (iambicACount>=2) )
				{
					latchedKeys = 0;
					keyState = KEY_STATE_IDLE ;
					break ;
				}
				else if ( (keyState == KEY_STATE_IAMBIC_DAH_GAP) && ( latchedKeys & DIT_PRESSED ))
					keyState = KEY_STATE_IAMBIC_DIT;
				else if ( latchedKeys & DAH_PRESSED )
					keyState = KEY_STATE_IAMBIC_DAH;
				else if ( latchedKeys & DIT_PRESSED )
					keyState = KEY_STATE_IAMBIC_DIT;					
				else
				{
					keyState = KEY_STATE_IDLE ;			
					break;
				}

				KeyDown();
			}
		break;

	}
	
}

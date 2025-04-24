#include "MacroRecord.h"
#include "Signals.h"
#include "NvSettings.h"

unsigned char  macroMem[MACRO_MEM_SIZE];

static unsigned char macroMemIdx = 0;
static unsigned char macroElemIdx = 0;
static unsigned char macroElemByte = 0;
static unsigned char macroElemCount = 0;
static unsigned char macroIdle = 0;
static unsigned char macroRecording = 0;

void MacroRecordStart()
{	
	macroElemIdx = 0;
	macroElemByte = 0;
	macroElemCount=0;
	macroMemIdx=1;
	
	macroIdle = 1 ;
}

void MacroRecordStop()
{
	if (macroElemCount > 1)
	{
		while (macroElemIdx < 3)
		{
			macroElemByte <<= 2;
			macroElemIdx++;
		}
		macroMem[macroMemIdx++] = macroElemByte;
		macroMem[0] = macroElemCount ;
	}
		
	macroRecording = 0;
	macroIdle = 0;
}

void MacroRecordSymbol(unsigned char bits)
{
	bits &= 0x03;
	
	if (macroIdle)
	{
		if ( ( bits == MACRO_SYMBOL_DAH ) || (bits == MACRO_SYMBOL_DIT) )
			macroRecording = 1;
	}
	
	if (!macroRecording)
		return ;

	macroElemByte |= bits; 	
	macroElemCount++ ;
		
	if (macroElemIdx < 3)
	{
		macroElemByte <<= 2 ;	
		macroElemIdx++ ;
	}
	else
	{ 
		macroMem[macroMemIdx++] = macroElemByte;
		if (macroMemIdx > MACRO_MEM_SIZE)
			MacroRecordStop();
		macroElemByte = 0;
		macroElemIdx = 0 ;
	}
}

void MacroRecordGap(unsigned char tickCount)
{
		
	while (tickCount >= 6)
	{
		MacroRecordSymbol(MACRO_SYMBOL_WORD_GAP);
		tickCount -= 6;
	}
	
	while (tickCount >= 2)
	{
		MacroRecordSymbol(MACRO_SYMBOL_CHAR_GAP);
		tickCount -= 2;
	}
	
	if (tickCount == 1)
		MacroRecordSymbol(MACRO_SYMBOL_CHAR_GAP);
}


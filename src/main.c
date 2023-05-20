#include "HwAbstraction.h"
#include "KeyTask.h"
#include "ModeTask.h"
#include "SpellTask.h"
#include "ButtonTask.h"
#include "LedTask.h"

// SDCC requires interrupt prototypes in same file as main
// Actual interrupt routines in HwAbstraction.c
//Timer 0 ISR, Tone Timer 
void HwT0Int(void) __interrupt 1 ;	
//Timer 1 ISR, Unit Timer - measures a dit unit = 1.2/wpm
void HwT1Int(void) __interrupt 3 ;	
//Timer 2 ISR, (Heart) Beat Timer - 10ms
void HwT2Int(void) __interrupt 12;	

void main()
{
	HwInit();
	HwGlobalIntEnable();
		
	while (1)
	{
		HwSpkrMgr();
		KeyTask();
		LedTask();
		ButtonTask();
		ModeTask();
		SpellTask();
	}
	
}


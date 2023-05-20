#include "HwAbstraction.h"
#include "KeyTask.h"
#include "ModeTask.h"
#include "SpellTask.h"
#include "ButtonTask.h"
#include "LedTask.h"

//Timer 0 ISR, Tone Timer 
void HwT0Int(void) __interrupt 1 ;	// SDCC requires interrupt prototypes in same file as main
//Timer 1 ISR, Unit Timer - measures a dit unit
void HwT1Int(void) __interrupt 3 ;	// SDCC requires interrupt prototypes in same file as main
//Timer 2 ISR, (Heart) Beat Timer - 10ms
void HwT2Int(void) __interrupt 12;	// SDCC requires interrupt prototypes in same file as main

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


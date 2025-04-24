#include "NvSettings.h"
#include "OutControl.h"
#include "HwAbstraction.h"
#include "Signals.h"
#include "LedTask.h"

__near unsigned char outTimeoutCount=0;
__near static OUT_MODE_ENUM outMode ;

void OutTimer()
{
	// uv timeout and general key output watchdog
	if (signalOutTick)
	{
		CBIT(signalOutTick);
		outTimeoutCount++ ;
		if (outTimeoutCount > 200) // 2 seconds
		{
			outTimeoutCount=0;			
			OutOff();
		}
	}	
}

void OutInitialize(OUT_MODE_ENUM pMode)
{
	outTimeoutCount = 0;
	outMode = pMode;	
}

void OutEnable(unsigned short toneTimerValue)
{
	if (outMode == OUT_MODE_OFF)
	{
		hfSolid(LED_GREEN);	
		HwKeyerOut(0);		
	}
	else if (outMode == OUT_MODE_HF)
	{
		hfSolid(LED_RED);
		uvSolid(LED_OFF);
		HwKeyerOut(1);
	}
	else if (outMode == OUT_MODE_UV)
	{
		hfSolid(LED_GREEN);
		uvSolid(LED_RED);
		HwKeyerOut(1);		
	}

	HwSideToneOn(toneTimerValue);
	outTimeoutCount=0;
}

void OutDisable()
{
	if (outMode == OUT_MODE_OFF)
	{
		hfSolid(LED_OFF);	
		HwKeyerOut(0);
	}
	else if (outMode == OUT_MODE_HF)
	{
		hfSolid(LED_GREEN);
		uvSolid(LED_OFF);
		HwKeyerOut(0);
	}
	else if (outMode == OUT_MODE_UV)
	{
		hfSolid(LED_OFF);
		uvSolid(LED_RED);
		HwKeyerOut(1);
	}	

	HwSideToneOff();	
}

void OutOff()
{
	if (outMode == OUT_MODE_OFF)
	{
		hfSolid(LED_OFF);	
	}
	else if (outMode == OUT_MODE_HF)
	{
		hfSolid(LED_GREEN);
		uvSolid(LED_OFF);
	}
	else if (outMode == OUT_MODE_UV)
	{
		hfSolid(LED_OFF);
		uvSolid(LED_GREEN);
	}	
	
	CBIT(signalDahLatch);
	CBIT(signalDitLatch);
	HwKeyerOut(0);
	HwSideToneOff();	
}


#include "HwAbstraction.h"
#include "Signals.h"
#include "LedTask.h"

static LED_COLOR_ENUM hfColor1 ;
static LED_COLOR_ENUM hfColor2 ;
static LED_COLOR_ENUM uvColor1 ;
static LED_COLOR_ENUM uvColor2 ;

static LED_STATE_ENUM ledState ;

void hfLed(LED_COLOR_ENUM color1, LED_COLOR_ENUM color2)
{
	hfColor1 = color1;
	hfColor2 = color2;
	if (color1 == color2)
		HwSetHfLed(color1);
}

void uvLed(LED_COLOR_ENUM color1, LED_COLOR_ENUM color2)
{
	uvColor1 = color1;
	uvColor2 = color2;
	if (color1 == color2)
		HwSetUvLed(color1);	
}

void LedTask()
{
	if (!SignalIsSet(SIGNAL_LED_TICK))
		return;
		
	ClearSignal(SIGNAL_LED_TICK);
	
	switch (ledState)
	{
		case LED_STATE_1:
			HwSetHfLed(hfColor1);
			HwSetUvLed(uvColor1);
			ledState = LED_STATE_2;
		break;
	
		case LED_STATE_2:
			HwSetHfLed(hfColor2);
			HwSetUvLed(uvColor2);		
			ledState = LED_STATE_1;
		break;
	}
	
}



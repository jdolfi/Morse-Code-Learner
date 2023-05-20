#ifndef SIGNALS_H
#define SIGNALS_H

typedef enum
{
	SIGNAL_UNIT_TICK		=0x0001,
	SIGNAL_CHAR_END			=0x0002,
	SIGNAL_MSG_END			=0x0004,
	SIGNAL_BUTTON_TICK		=0x0008,
	SIGNAL_LED_TICK			=0x0010,
	SIGNAL_KEY_TICK			=0x0020,
	
	SIGNAL_PB0_SHORT		= 0x0100,
	SIGNAL_PB0_LONG			= 0x0200,
	SIGNAL_PB1_SHORT		= 0x0400,
	SIGNAL_PB1_LONG			= 0x0800,
	SIGNAL_PB2_SHORT		= 0x1000,
	SIGNAL_PB2_LONG			= 0x2000,
	SIGNAL_PB3_SHORT		= 0x4000,
	SIGNAL_PB3_LONG			= 0x8000,
	
	SIGNAL_PB_ALL			= 0xFF00,

} _SIGNALS_ENUM ;

void SetSignal(_SIGNALS_ENUM paramSignal);
void ClearSignal(_SIGNALS_ENUM paramSignal);
unsigned char SignalIsSet(_SIGNALS_ENUM paramSignal);
unsigned char SignalNotSet(_SIGNALS_ENUM paramSignal);

extern volatile unsigned short signals;

#endif

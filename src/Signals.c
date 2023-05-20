#include "Signals.h"

volatile unsigned short signals ;

void SetSignal(_SIGNALS_ENUM paramSignal)
{
	signals |= paramSignal ;
}

void ClearSignal(_SIGNALS_ENUM paramSignal)
{
	signals &= ~paramSignal ;	
}

unsigned char SignalIsSet(_SIGNALS_ENUM paramSignal)
{
	return ( (signals & paramSignal) != 0);
}

unsigned char SignalNotSet(_SIGNALS_ENUM paramSignal)
{
	return ( !(signals & paramSignal) );
}

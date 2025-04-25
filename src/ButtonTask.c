#include "HwAbstraction.h"
#include "Signals.h"
#include "ButtonTask.h"

static unsigned char but0Count = 0;
static BUTTON_STATE_ENUM but0State = BUT_STATE_IDLE;
static unsigned char but1Count = 0;
static BUTTON_STATE_ENUM but1State = BUT_STATE_IDLE;
static unsigned char but2Count = 0;
static BUTTON_STATE_ENUM but2State = BUT_STATE_IDLE;
static unsigned char but3Count = 0;
static BUTTON_STATE_ENUM but3State = BUT_STATE_IDLE;

void Button0Task()
{		
	switch (but0State)
	{
		case BUT_STATE_IDLE:
			if ( HwPB0Pressed() )
			{
				but0State = BUT_STATE_DOWN ;
				but0Count=1;
			}
		break ;
		
		case BUT_STATE_DOWN:
			if ( HwPB0Pressed() )
			{					
				if (but0Count < 100)
					but0Count++;
				else if (but0Count == 100)
				{
					SBIT(signalPB0Long);
					but0Count++;
				}
			}
			else
			{
				if (but0Count < 100)
					SBIT(signalPB0Short);					
				but0Count = 0;
				but0State = BUT_STATE_RELEASE ;
			}
		break ;
		
		case BUT_STATE_RELEASE:
			but0Count++ ;
			if (but0Count > 5)
			{
				but0Count = 0;
				but0State = BUT_STATE_IDLE ;
			}
		break;
	}		
	
}

void Button1Task()
{
	switch (but1State)
	{
		case BUT_STATE_IDLE:
			if ( HwPB1Pressed() )
			{
				but1State = BUT_STATE_DOWN ;
				but1Count=1;
			}
		break ;
		
		case BUT_STATE_DOWN:
			if ( HwPB1Pressed() )
			{					
				if (but1Count < 100)
					but1Count++;
				else if (but1Count == 100)
				{
					SBIT(signalPB1Long);
					but1Count++;
				}
			}
			else
			{
				if (but1Count < 100)
					SBIT(signalPB1Short);					
				but1Count = 0;
				but1State = BUT_STATE_RELEASE ;
			}
		break ;
		
		case BUT_STATE_RELEASE:
			but1Count++ ;
			if (but1Count > 5)
			{
				but1Count = 0;
				but1State = BUT_STATE_IDLE ;
			}
		break;
	}		
	
}


void Button2Task()
{
	switch (but2State)
	{
		case BUT_STATE_IDLE:
			if ( HwPB2Pressed() )
			{
				but2State = BUT_STATE_DOWN ;
				but2Count=1;
			}
		break ;
		
		case BUT_STATE_DOWN:
			if ( HwPB2Pressed() )
			{					
				if (but2Count < 100)
					but2Count++;
				else if (but2Count == 100)
				{
					SBIT(signalPB2Long);
					but2Count++;
				}
			}
			else
			{
				if (but2Count < 100)
					SBIT(signalPB2Short);					
				but2Count = 0;
				but2State = BUT_STATE_RELEASE ;
			}
		break ;
		
		case BUT_STATE_RELEASE:
			but2Count++ ;
			if (but2Count > 5)
			{
				but2Count = 0;
				but2State = BUT_STATE_IDLE ;
			}
		break;
	}		
}

void Button3Task()
{	
	switch (but3State)
	{
		case BUT_STATE_IDLE:
			if ( HwPB3Pressed() )
			{
				but3State = BUT_STATE_DOWN ;
				but3Count=1;
			}
		break ;
		
		case BUT_STATE_DOWN:
			if ( HwPB3Pressed() )
			{					
				if (but3Count < 100)
					but3Count++;
				else if (but3Count == 100)
				{
					SBIT(signalPB3Long);
					but3Count++;
				}
			}
			else
			{
				if (but3Count < 100)
					SBIT(signalPB3Short);					
				but3Count = 0;
				but3State = BUT_STATE_RELEASE ;
			}
		break ;
		
		case BUT_STATE_RELEASE:
			but3Count++ ;
			if (but3Count > 5)
			{
				but3Count = 0;
				but3State = BUT_STATE_IDLE ;
			}
		break;
	}		
	
}

void ButtonTask()
{
	if (!signalButtonTick)
		return ;
		
	CBIT(signalButtonTick);

	Button0Task();
	Button1Task();
	Button2Task();
	Button3Task();
}


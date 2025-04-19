#include "stc15_1.h"
#include "HwAbstraction.h"
#include "Signals.h"
#include "KeyTask.h"

static volatile unsigned char spkrOuts = 0;
static unsigned char spkrMemory = 11;
static unsigned char ledTimerCount=0;
static unsigned char replayTimerCount=0;
static unsigned char speakerVolume=0;

// Processor Oscillator runs at 6Mhz
// This is divided by 2 for system clock at 3Mhz
// Timers are all using 3MHZ/12 = 250Khz timebase
// Timers counting up, interrupting at rollover

//Timer 0 ISR, Tone Timer
void HwT0Int(void) __interrupt 1
{
	if (spkrOuts == TONE_OUT_SPKR1)
		P1_4 = !P1_4;
	else if (spkrOuts == TONE_OUT_SPKR2)
		P1_7 = !P1_7;
	else if (spkrOuts <= 0x07)
		P2_1 = !P2_1;
}

//Timer 1 ISR, Unit Timer - measures a dit unit
void HwT1Int(void) __interrupt 3
{
	SetSignal(SIGNAL_UNIT_TICK);
}

//Timer 2 ISR, (Heart) Beat Timer - 10ms
void HwT2Int(void) __interrupt 12
{
	SetSignal(SIGNAL_BUTTON_TICK);
	SetSignal(SIGNAL_KEY_TICK);
	
	ledTimerCount++;
	if (ledTimerCount > 30)	// 300ms
	{
		ledTimerCount=0;
		SetSignal(SIGNAL_LED_TICK);
	}
	
	replayTimerCount++;
	if (replayTimerCount > 25)	// 250ms
	{
		replayTimerCount=0;
		SetSignal(SIGNAL_REPLAY_TICK);
	}
	
}

unsigned char HwKeysPressed()
{
	unsigned char pressed = 0;
	
	if (!P2_5)
	  pressed |= DAH_PRESSED ;
	if (!P2_6)
	  pressed |= DIT_PRESSED ;
	  
	return pressed ;
}

void HwStartToneTimer(unsigned short timerValue)
{
	
	TH0 = timerValue >> 8 ;			
	TL0 = timerValue & 0x00FF ;
	TR0 = 1;
	ET0 = 1;
}

void HwStopToneTimer()
{
	TR0 = 0;
	ET0 = 0;
	
	P1_4 = 1;  // pulse outputs
	P1_6 = 1;
	P1_7 = 1;
	P2_1 = 1;
}

// Restart the dit unit timer at the beginning of a key press
void HwRestartUnitTimer(unsigned short timerValue)
{

	ET1 = 0;
	TR1 = 0;
	
	TH1 = timerValue >> 8 ;			
	TL1 = timerValue & 0x00FF ;
	TR1 = 1;
	ET1 = 1;	
}

void HwInit()
{
	P0M0 = 0x00; 
	P0M1 = 0x00;
	P1M0 = 0x00; // P1_7,P1_6,P1_4 spkr outs push-pull
	P1M1 = 0x00;
	P2M0 = 0x02;
	P2M1 = 0x00;
	P3M0 = 0x00; // P3_3 is sourcing power to the earphone, push-pull is way too loud
	P3M1 = 0x00;
	P4M0 = 0x00;
	P4M1 = 0x00;
	P5M0 = 0x00;
	P5M1 = 0x00;
	P6M0 = 0x00;
	P6M1 = 0x00;
	P7M0 = 0x00;
	P7M1 = 0x00;
	
	// Run a 6MHz RC oscillator, divide by 2 for a system clock of 3MHz
	CLK_DIV = 0x01;
	
#define T2R_BIT 0x10	
#define ET2_BIT 0x04 ;
	
	// 10ms, 65536 - (.01 * 250000) = 0xF63C Heartbeat timer for switches and such
	T2H = 0xF6 ;			
	T2L = 0x3C ;
	AUXR |= T2R_BIT ;
	IE2 |= ET2_BIT ;
}

void HwGlobalIntEnable()
{
	EA=1;
}

void HwGlobalIntDisable()
{
	EA=0;
}

void HwSetUvLed(LED_COLOR_ENUM ledMode)
{
	if (ledMode == LED_OFF)
	{
		P0_2 = 1;
		P0_3 = 1;
	}
	else if (ledMode == LED_RED)	
	{
		P0_2 = 0;
		P0_3 = 1;		
	}
	else if (ledMode == LED_GREEN)	
	{
		P0_2 = 1;
		P0_3 = 0;		
	}
	else if (ledMode == LED_MIX)	
	{
		P0_2 = 0;
		P0_3 = 0;		
	}
}

void HwSetHfLed(LED_COLOR_ENUM ledMode)
{
	if (ledMode == LED_OFF)
	{
		P0_0 = 1;
		P0_1 = 1;
	}
	else if (ledMode == LED_RED)	
	{
		P0_0 = 0;
		P0_1 = 1;		
	}
	else if (ledMode == LED_GREEN)	
	{
		P0_0 = 1;
		P0_1 = 0;		
	}
	else if (ledMode == LED_MIX)	
	{
		P0_0 = 0;
		P0_1 = 0;		
	}
}

void HwKeyerOut(unsigned char keyerOn)
{
	if (keyerOn)
		P1_1 = 1;
	else
		P1_1 = 0;
}

void SetSpeakerVolume(char pVol)
{
	speakerVolume = pVol ;
}

void HwSpkrMgr()
{		
	if (P3_4 == spkrMemory)
		return ;
		
	spkrMemory = P3_4 ;
		
	// P3_4 goes low on plugin of headphone
	if (P3_4)
	{		
		spkrOuts = speakerVolume ;
		
		P1M0 = 0x00;
		if (spkrOuts == TONE_OUT_SPKR2)
			P1M0 |= 0x80;	// P1_7
		
	}
	else
	{
		spkrOuts = 0x07 - nvSettings[NV_EAR_VOL] ;

		// All headphone attenuators NOT bypassed
		P3_5 = 0;	// 0b0010  10K
		P3_6 = 0;	// 0b0100  30K
		P3_7 = 0;	// 0b0001  4.7K		

		if (spkrOuts & 0x01)
			P3_7 = 1;	// 0b0001 4.7K
		if (spkrOuts & 0x02)
			P3_5 = 1;	// 0b0010 10K
		if (spkrOuts & 0x04)
			P3_6 = 1;	// 0b0100 30K

		P2_0 = 1;	// 0b1000 Enable		
	}		
}

unsigned char HwButtonsPressed()
{
	unsigned char retVal = 0;
	
	if (!P3_2)
		retVal |= PWR_BUTTON ;
	if (!P2_2)
		retVal |= HF_BUTTON ;	
	if (!P2_3)
		retVal |= TONE_BUTTON ;
	if (!P2_4)
		retVal |= VOL_BUTTON ;	
		
	return retVal ;
}

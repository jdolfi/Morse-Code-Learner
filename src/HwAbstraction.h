typedef enum
{
	LED_OFF		= 0x00,
	LED_RED		= 0x01,
	LED_GREEN	= 0x02,
	LED_MIX		= 0x03,
	
} LED_COLOR_ENUM ;

typedef enum
{

	TONE_OUT_EAR0  = 0x00,
	TONE_OUT_EAR1  = 0x01,
	TONE_OUT_EAR2  = 0x02,
	TONE_OUT_EAR3  = 0x03,
	TONE_OUT_EAR4  = 0x04,
	TONE_OUT_EAR5  = 0x05,
	TONE_OUT_EAR6  = 0x06,
	TONE_OUT_EAR7  = 0x07,
	TONE_OUT_SPKR1 = 0x10,
	TONE_OUT_SPKR2 = 0x20,
	TONE_OUT_SPKR3 = 0x30,
	TONE_OUT_OFF   = 0xFF,
	
} TONE_OUT_ENUM ;

void HwInit();
void HwGlobalIntEnable();
void HwGlobalIntDisable();
void HwSideToneOn(unsigned short timerValue);
void HwSideToneOff();
void HwRestartUnitTimer(unsigned short timerValue);
void HwSetHfLed(unsigned char on);
void HwSetUvLed(LED_COLOR_ENUM ledMode);
void HwKeyerOut(unsigned char keyerOn);
void HwLatchKeys();
char HwPB0Pressed();
char HwPB1Pressed();
char HwPB2Pressed();
char HwPB3Pressed();
char HwDitPressed();
char HwDahPressed();
void SetSpeakerVolume(char pVol);
void HwSpkrMgr();


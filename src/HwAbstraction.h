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

typedef enum
{
	NO_BUTTON   = 0x00 ,
	PWR_BUTTON  = 0x01,
	HF_BUTTON   = 0x02,
	TONE_BUTTON = 0x04,
	VOL_BUTTON  = 0x08,
	
} BUTTON_ENUM ;

typedef enum
{
	DIT_PRESSED  = 0x01,
	DAH_PRESSED  = 0x02,
	BOTH_PRESSED = 0x03,
	
} KEYS_PRESSED_ENUM ;

void HwInit();
void HwGlobalIntEnable();
void HwGlobalIntDisable();
void HwStartToneTimer(unsigned short timerValue);
void HwStopToneTimer();
void HwRestartUnitTimer(unsigned short timerValue);
void HwSetHfLed(unsigned char on);
void HwSetUvLed(LED_COLOR_ENUM ledMode);
void HwKeyerOut(unsigned char keyerOn);
unsigned char HwKeysPressed();
unsigned char HwButtonsPressed();
void HwSpkrMgr();


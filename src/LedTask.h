typedef enum
{
	LED_STATE_1		= 0x00,
	LED_STATE_2		= 0x01,

} LED_STATE_ENUM ;

void LedTask();
void hfLed(LED_COLOR_ENUM color1, LED_COLOR_ENUM color2);
void uvLed(LED_COLOR_ENUM color1, LED_COLOR_ENUM color2);

#define uvSolid(LED_COLOR) (uvLed(LED_COLOR, LED_COLOR))
#define hfSolid(LED_COLOR) (hfLed(LED_COLOR, LED_COLOR))

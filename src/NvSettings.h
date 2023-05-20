#ifndef NV_SETTINGS_H
#define NV_SETTINGS_H

typedef enum 
{
	STRAIGHT_KEY = 0x00,
	IAMBIC_A_KEY = 0x01,
	IAMBIC_B_KEY = 0x02,
	CHAR_KEY	 = 0x03,
	
} KEY_TYPE_ENUM ;

typedef enum 
{
	OUT_MODE_OFF = 0x00,
	OUT_MODE_HF  = 0x01,
	OUT_MODE_UV  = 0x02,
	
} OUT_MODE_ENUM ;

typedef enum
{
	NV_WPM		= 0x00,
	NV_TONE 	= 0x01,
	NV_KEY_TYPE	= 0x02,
	NV_OUT_MODE	= 0x03,	
	NV_SPKR_VOL	= 0x04,
	NV_EAR_VOL	= 0x05,
	NV_SIZE		= 0x06,
	
}	NV_SETTINGS_ENUM ;

typedef struct
{
	unsigned short low;
	unsigned short high;
	unsigned char  step;
	
} RANGE_STRUCT ;

extern unsigned short nvSettings[NV_SIZE];
extern const RANGE_STRUCT nvRanges[NV_SIZE];
void ValidateNvSettings();

unsigned short calcToneTimerValue(unsigned short freq);
unsigned short calcUnitTimerValue(unsigned char wpm);

#endif

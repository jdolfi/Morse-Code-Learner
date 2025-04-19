#include "NvSettings.h"

unsigned short nvSettings[NV_SIZE] = 
{
	18,					// NV_WPM, lowest = 5WPM	
	650,				// NV_TONE
	IAMBIC_B_KEY,		// NV_KEY_TYPE
	OUT_MODE_UV,		// NV_OUT_MODE	
	0x10,				// NV_SPKR_VOL
	7,					// NV_EAR_VOL
	5,					// NV_REPLAY
} ;

const RANGE_STRUCT nvRanges[NV_SIZE] =
{
	{5,55,2},			// NV_WPM	
	{500,1000,25},		// NV_TONE
	{0,2,1},			// NV_KEY_TYPE
	{0,2,1},			// NV_OUT_MODE	
	{0x00,0x20,0x10},	// NV_SPKR_VOL
	{1,7,1},			// NV_EAR_VOL 
	{0,10,1},			// NV_REPLAY
} ;

unsigned short calcToneTimerValue(unsigned short freq)
{
	unsigned long longValue = 250000 ;
	longValue /= freq ;
	longValue /= 2 ;
	longValue = 0xFFFF - longValue ;
	
	return (unsigned short)longValue ;
}

unsigned short calcUnitTimerValue(unsigned char wpm)
{
	unsigned long longValue = 25000 ;
	longValue *= 12 ;
	longValue /= wpm ;
	longValue = 0xFFFF - longValue ;
	
	return (unsigned short)longValue ;	
}

void ValidateNvSettings()
{
	for (int i=0; i<NV_SIZE; i++)
	{
		if ( (nvSettings[i] < nvRanges[i].low) || (nvSettings[i] > nvRanges[i].high) )
			nvSettings[i] = nvRanges[i].low;
	}
}

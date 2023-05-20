#ifndef EEPROM_H
#define EEPROM_H

typedef enum
{
	BUT_STATE_IDLE 	= 0x00,
	BUT_STATE_DOWN 	= 0x01,
	BUT_STATE_RELEASE = 0x02,

} BUTTON_STATE_ENUM ;

void ButtonTask();

#endif

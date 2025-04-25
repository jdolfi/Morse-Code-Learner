#include "HwAbstraction.h"
#include "Signals.h"
#include "ModeTask.h"
#include "LedTask.h"
#include "NvSettings.h"
#include "KeyTask.h"
#include "OutControl.h"
#include "CharTask.h"
#include "Messages.h"
#include "EEProm.h"
#include "MacroRecord.h"
#include "PlayTask.h"
#include "ButtonTask.h"

static OP_STATE_ENUM opState = MODE_STATE_STARTUP ;

unsigned char SignalCheck(unsigned char* theSignal)
{
	if (*theSignal)
	{
		CBIT(*theSignal);
		return 1 ;
	}	
	
	return 0;
}

void MinimalSpeakerVolume()
{
	char minVolume = TONE_OUT_SPKR1;
	if (nvSettings[NV_SPKR_VOL] > minVolume)
		minVolume = nvSettings[NV_SPKR_VOL];
		
	SetSpeakerVolume(minVolume);
}

void ModeTaskEnableSettings()
{	
	uvLed(LED_GREEN, LED_OFF);
	OutInitialize(OUT_MODE_OFF);
}

unsigned char adjustSetting(unsigned char settingId)
{
	unsigned short newSetting;
	unsigned char wasAdjusted = 0;
	
	if ( SignalCheck(&signalPB1Short) )
	{
		CharTaskEnable();
		wasAdjusted = 0x01;
		newSetting = nvSettings[settingId] - nvRanges[settingId].step ;		
		if ( (newSetting >= nvRanges[settingId].low) && (newSetting <= nvRanges[settingId].high) )
		{
			nvSettings[settingId] = newSetting ;		
			CharTaskSetMsg(eMsg) ;
		}
		else
			CharTaskSetMsg(ttMsg) ;
	}

	if ( SignalCheck(&signalPB2Short) )
	{
		CharTaskEnable();
		wasAdjusted = 0x01;
		newSetting = nvSettings[settingId] + nvRanges[settingId].step ;		
		if ( (newSetting >= nvRanges[settingId].low) && (newSetting <= nvRanges[settingId].high) )
		{
			nvSettings[settingId] = newSetting ;				
			CharTaskSetMsg(iMsg) ;
			
		}
		else
			CharTaskSetMsg(ttMsg) ;		
	}
	
	return wasAdjusted ;
}
	
void ModeTask()
{
	static unsigned char toggleRecord = 0;
	
	switch(opState)
	{
		case MODE_STATE_STARTUP:
			opState = MODE_STATE_STARTUP_WAIT ;
			LoadNvSettingsFromEeprom();
			ValidateNvSettings();
		break ;
		
		case MODE_STATE_STARTUP_WAIT:
			opState = MODE_STATE_OPERATE_MSG;
		break ;
		
		case MODE_STATE_EXIT_SETTINGS:
			opState = MODE_STATE_OPERATE_MSG;
			StoreNvSettingsToEeprom();			
		break ;
		
		case MODE_STATE_OPERATE_MSG:
			SetSpeakerVolume(nvSettings[NV_SPKR_VOL]);
			opState = MODE_STATE_OPERATE_MAIN_WAIT;
			CharTaskEnable();			
			CharTaskSetMsg(operateMsg) ;			
		break;
		
		case MODE_STATE_OPERATE_MAIN_WAIT:
			if ( signalMsgEnd )
			{
				CBIT(signalMsgEnd);
				opState = MODE_STATE_OPERATE_MAIN;
				OutInitialize(nvSettings[NV_OUT_MODE]);
				KeyTaskEnable();
			}
		break;
		
		case MODE_STATE_OPERATE_MAIN:
			//if (SignalCheck(&signalPB0Short))
			
			if (SignalCheck(&signalPB0Long))
			{
				opState = MODE_STATE_WPM_MSG ;
				MinimalSpeakerVolume();
			}
				
			else if (SignalCheck(&signalPB1Short))
			{
				LoadNvMacroFromEeprom(1);
				PlayTaskEnable();	
			}
			else if (SignalCheck(&signalPB1Long))
			{
				MinimalSpeakerVolume();
				ModeTaskEnableSettings();
				MacroRecordStart();
				opState = MODE_STATE_MACRO_STORE1 ;
			}
			
			else if (SignalCheck(&signalPB2Short))
			{
				LoadNvMacroFromEeprom(2);
				PlayTaskEnable();	
			}
			else if (SignalCheck(&signalPB2Long))
			{
				MinimalSpeakerVolume();
				ModeTaskEnableSettings();
				MacroRecordStart();
				opState = MODE_STATE_MACRO_STORE2 ;
			}

			else if (SignalCheck(&signalPB3Short))
			{
				LoadNvMacroFromEeprom(3);
				PlayTaskEnable();	
			}
			else if (SignalCheck(&signalPB3Long))
			{
				MinimalSpeakerVolume();
				ModeTaskEnableSettings();
				MacroRecordStart();
				opState = MODE_STATE_MACRO_STORE3 ;
			}
		break ;

		case MODE_STATE_MACRO_STORE1:
			if ( SignalCheck(&signalPB1Short) || SignalCheck(&signalPB1Long) )
			{
				opState = MODE_STATE_OPERATE_MSG;
				MacroRecordStop();
				StoreNvMacroToEeprom(1);
			}
		break ;

		case MODE_STATE_MACRO_STORE2:
			if ( SignalCheck(&signalPB2Short) || SignalCheck(&signalPB2Long) )
			{
				opState = MODE_STATE_OPERATE_MSG;
				MacroRecordStop();
				StoreNvMacroToEeprom(2);
			}
		break ;

		case MODE_STATE_MACRO_STORE3:
			if ( SignalCheck(&signalPB3Short) || SignalCheck(&signalPB3Long) )
			{
				opState = MODE_STATE_OPERATE_MSG;
				MacroRecordStop();
				StoreNvMacroToEeprom(3);
			}
		break ;
		
		case MODE_STATE_WPM_MSG:
			opState = MODE_STATE_WPM_MAIN_WAIT;
			CharTaskEnable();					
			CharTaskSetMsg(wpmMsg) ;		
		break ;

		case MODE_STATE_WPM_MAIN_WAIT:
			if ( signalMsgEnd )
			{
				CBIT(signalMsgEnd);
				opState = MODE_STATE_WPM_MAIN;
				ModeTaskEnableSettings();
			}
		break ;
		
		case MODE_STATE_WPM_MAIN:
			if ( adjustSetting(NV_WPM) )
			{
				KeyTaskEnable();
				opState = MODE_STATE_WPM_MAIN_WAIT;
			}

			if (SignalCheck(&signalPB0Short))
				opState = MODE_STATE_TONE_MSG ;

			if (SignalCheck(&signalPB0Long))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (SignalCheck(&signalPB3Short))
				opState = MODE_STATE_WPM_MSG;
		break ;		
		
		case MODE_STATE_TONE_MSG:
			opState = MODE_STATE_TONE_MAIN_WAIT;
			CharTaskEnable();
			CharTaskSetMsg(toneMsg) ;		
			
		break ;

		case MODE_STATE_TONE_MAIN_WAIT:
			if ( signalMsgEnd )
			{
				CBIT(signalMsgEnd);
				opState = MODE_STATE_TONE_MAIN;
				ModeTaskEnableSettings();
			}
		break ;
		
		case MODE_STATE_TONE_MAIN:
			if ( adjustSetting(NV_TONE) )
			{
				KeyTaskEnable();			
				opState = MODE_STATE_TONE_MAIN_WAIT;
			}

			if (SignalCheck(&signalPB0Short))
				opState = MODE_STATE_KEY_MSG ;

			if (SignalCheck(&signalPB0Long))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (SignalCheck(&signalPB3Short))
				opState = MODE_STATE_TONE_MSG;
		break ;		
		
		case MODE_STATE_KEY_MSG:
			opState = MODE_STATE_KEY_MAIN_WAIT;
			CharTaskEnable();
			CharTaskSetMsg(keyMsg) ;		
			
		break ;
		
		case MODE_STATE_KEY_VAL_MSG:
			if ( signalMsgEnd )
			{
				CBIT(signalMsgEnd);
		
				opState = MODE_STATE_KEY_MAIN_WAIT;
				CharTaskEnable();
				switch (nvSettings[NV_KEY_TYPE])
				{
					case IAMBIC_A_KEY:
						CharTaskSetMsg(&keyIambicAMsg[3]) ;
					break ;
					
					case IAMBIC_B_KEY:
						CharTaskSetMsg(&keyIambicBMsg[3]) ;
					break ;
					
					default:
						CharTaskSetMsg(&keyStraightMsg[3]) ;
					break ;
				}		
			}
		break ;

		case MODE_STATE_KEY_MAIN_WAIT:
			if ( signalMsgEnd )
			{
				CBIT(signalMsgEnd);
				opState = MODE_STATE_KEY_MAIN;
				ModeTaskEnableSettings();
			}
		break ;
		
		case MODE_STATE_KEY_MAIN:
			if ( adjustSetting(NV_KEY_TYPE) )
				opState = MODE_STATE_KEY_VAL_MSG;

			if (SignalCheck(&signalPB0Short))
				opState = MODE_STATE_OUT_MSG ;

			if (SignalCheck(&signalPB0Long))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (SignalCheck(&signalPB3Short))
			{				
				opState = MODE_STATE_KEY_MAIN_WAIT;
				CharTaskEnable();
				switch (nvSettings[NV_KEY_TYPE])
				{
					case IAMBIC_A_KEY:
						CharTaskSetMsg(keyIambicAMsg) ;
					break ;
					
					case IAMBIC_B_KEY:
						CharTaskSetMsg(keyIambicBMsg) ;
					break ;
					
					default:
						CharTaskSetMsg(keyStraightMsg) ;
					break ;
				}
			}
		break ;		

		case MODE_STATE_OUT_MSG:
			opState = MODE_STATE_OUT_MAIN_WAIT;
			CharTaskEnable();
			CharTaskSetMsg(outMsg) ;		
			
		break ;
		
		case MODE_STATE_OUT_VAL_MSG:
			if ( signalMsgEnd )
			{
				CBIT(signalMsgEnd);
		
				opState = MODE_STATE_OUT_MAIN_WAIT;
				CharTaskEnable();
				switch (nvSettings[NV_OUT_MODE])
				{
					case OUT_MODE_HF:
						CharTaskSetMsg(&outHfMsg[3]) ;
					break ;
					
					case OUT_MODE_UV:
						CharTaskSetMsg(&outUvMsg[3]) ;
					break ;
					
					default:
						CharTaskSetMsg(&outOffMsg[3]) ;
					break ;
				}		
			}
		break ;

		case MODE_STATE_OUT_MAIN_WAIT:
			if ( signalMsgEnd )
			{
				CBIT(signalMsgEnd);
				opState = MODE_STATE_OUT_MAIN;
				ModeTaskEnableSettings();
			}
		break ;
		
		case MODE_STATE_OUT_MAIN:
			if ( adjustSetting(NV_OUT_MODE) )
				opState = MODE_STATE_OUT_VAL_MSG;

			if (SignalCheck(&signalPB0Short))
				opState = MODE_STATE_SPKR_MSG ;

			if (SignalCheck(&signalPB0Long))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (SignalCheck(&signalPB3Short))
			{				
				opState = MODE_STATE_OUT_MAIN_WAIT;
				CharTaskEnable();
				switch (nvSettings[NV_OUT_MODE])
				{
					case OUT_MODE_HF:
						CharTaskSetMsg(outHfMsg) ;
					break ;
					
					case OUT_MODE_UV:
						CharTaskSetMsg(outUvMsg) ;
					break ;
					
					default:
						CharTaskSetMsg(outOffMsg) ;
					break ;
				}
			}
		break ;		

		case MODE_STATE_SPKR_MSG:
			opState = MODE_STATE_SPKR_MAIN_WAIT;
			CharTaskEnable();
			CharTaskSetMsg(spkrMsg) ;		
			
		break ;

		case MODE_STATE_SPKR_MAIN_WAIT:
			if ( signalMsgEnd )
			{
				CBIT(signalMsgEnd);
				opState = MODE_STATE_SPKR_MAIN;
				ModeTaskEnableSettings();
			}
		break ;
		
		case MODE_STATE_SPKR_MAIN:
			if ( adjustSetting(NV_SPKR_VOL) )
			{
				SetSpeakerVolume(nvSettings[NV_SPKR_VOL]);
				opState = MODE_STATE_SPKR_MAIN_WAIT;
			}

			if (SignalCheck(&signalPB0Short))
				opState = MODE_STATE_EAR_MSG ;

			if (SignalCheck(&signalPB0Long))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (SignalCheck(&signalPB3Short))
				opState = MODE_STATE_SPKR_MSG;
		break ;		

		case MODE_STATE_EAR_MSG:
			opState = MODE_STATE_EAR_MAIN_WAIT;
			CharTaskEnable();
			CharTaskSetMsg(earMsg) ;		
			
		break ;

		case MODE_STATE_EAR_MAIN_WAIT:
			if ( signalMsgEnd )
			{
				CBIT(signalMsgEnd);
				opState = MODE_STATE_EAR_MAIN;
				ModeTaskEnableSettings();
			}
		break ;
		
		case MODE_STATE_EAR_MAIN:
			if ( adjustSetting(NV_EAR_VOL) )
				opState = MODE_STATE_EAR_MAIN_WAIT;

			if (SignalCheck(&signalPB0Short))
				opState = MODE_STATE_REPLAY_MSG ;

			if (SignalCheck(&signalPB0Long))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (SignalCheck(&signalPB3Short))
				opState = MODE_STATE_EAR_MSG;
		break ;	

		case MODE_STATE_REPLAY_MSG:
			opState = MODE_STATE_REPLAY_MAIN_WAIT;
			CharTaskEnable();
			CharTaskSetMsg(replayMsg) ;		
			
		break ;

		case MODE_STATE_REPLAY_MAIN_WAIT:
			if ( signalMsgEnd )
			{
				CBIT(signalMsgEnd);
				opState = MODE_STATE_REPLAY_MAIN;
				ModeTaskEnableSettings();
			}
		break ;
		
		case MODE_STATE_REPLAY_MAIN:
			if ( adjustSetting(NV_REPLAY) )
				opState = MODE_STATE_REPLAY_MAIN_WAIT;

			if (SignalCheck(&signalPB0Short))
				opState = MODE_STATE_WPM_MSG ;

			if (SignalCheck(&signalPB0Long))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (SignalCheck(&signalPB3Short))
				opState = MODE_STATE_REPLAY_MSG;
		break ;	
							
	}

}

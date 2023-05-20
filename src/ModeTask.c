#include "HwAbstraction.h"
#include "Signals.h"
#include "ModeTask.h"
#include "NvSettings.h"
#include "KeyTask.h"
#include "SpellTask.h"
#include "Messages.h"
#include "EEProm.h"

static OP_STATE_ENUM opState = MODE_STATE_STARTUP ;

unsigned char checkSignal(_SIGNALS_ENUM pressId)
{
	if (SignalIsSet(pressId))
	{
		ClearSignal(pressId);
		return 1 ;
	}	
	
	return 0;
}

unsigned char adjustSetting(unsigned char settingId)
{
	unsigned short newSetting;
	unsigned char wasAdjusted = 0;
	
	if ( checkSignal(SIGNAL_PB1_SHORT) )
	{
		ConfigureKeyForMessages();
		wasAdjusted = 0x01;
		newSetting = nvSettings[settingId] - nvRanges[settingId].step ;		
		if ( (newSetting >= nvRanges[settingId].low) && (newSetting <= nvRanges[settingId].high) )
		{
			nvSettings[settingId] = newSetting ;		
			SetSpellMsg(eMsg) ;
		}
		else
			SetSpellMsg(ttMsg) ;
	}

	if ( checkSignal(SIGNAL_PB2_SHORT) )
	{
		ConfigureKeyForMessages();
		wasAdjusted = 0x01;
		newSetting = nvSettings[settingId] + nvRanges[settingId].step ;		
		if ( (newSetting >= nvRanges[settingId].low) && (newSetting <= nvRanges[settingId].high) )
		{
			nvSettings[settingId] = newSetting ;				
			SetSpellMsg(iMsg) ;
			
		}
		else
			SetSpellMsg(ttMsg) ;		
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
			opState = MODE_STATE_OPERATE_MAIN_WAIT;
			ConfigureKeyForMessages();			
			SetSpellMsg(operateMsg) ;			
		break;
		
		case MODE_STATE_OPERATE_MAIN_WAIT:
			if ( SignalIsSet(SIGNAL_MSG_END) )
			{
				ClearSignal(SIGNAL_MSG_END);
				opState = MODE_STATE_OPERATE_MAIN;
				ConfigureKeyForOperate();
			}
		break;
		
		case MODE_STATE_OPERATE_MAIN:
			//if (checkSignal(SIGNAL_PB0_SHORT))
			
			if (checkSignal(SIGNAL_PB0_LONG))
				opState = MODE_STATE_WPM_MSG ;
				
			else if (checkSignal(SIGNAL_PB1_SHORT))
			{
				LoadNvMacroFromEeprom(1);
				ConfigureKeyForMacro();	
			}
			else if (checkSignal(SIGNAL_PB1_LONG))
			{
				ConfigureKeyForSettings();
				StartMacroRecord();
				opState = MODE_STATE_MACRO_STORE1 ;
			}
			
			else if (checkSignal(SIGNAL_PB2_SHORT))
			{
				LoadNvMacroFromEeprom(2);
				ConfigureKeyForMacro();	
			}
			else if (checkSignal(SIGNAL_PB2_LONG))
			{
				ConfigureKeyForSettings();
				StartMacroRecord();
				opState = MODE_STATE_MACRO_STORE2 ;
			}

			else if (checkSignal(SIGNAL_PB3_SHORT))
			{
				LoadNvMacroFromEeprom(3);
				ConfigureKeyForMacro();	
			}
			else if (checkSignal(SIGNAL_PB3_LONG))
			{
				ConfigureKeyForSettings();
				StartMacroRecord();
				opState = MODE_STATE_MACRO_STORE3 ;
			}
		break ;

		case MODE_STATE_MACRO_STORE1:
			if ( checkSignal(SIGNAL_PB1_SHORT) || checkSignal(SIGNAL_PB1_LONG) )
			{
				opState = MODE_STATE_OPERATE_MSG;
				StopMacroRecord();
				StoreNvMacroToEeprom(1);
			}
		break ;

		case MODE_STATE_MACRO_STORE2:
			if ( checkSignal(SIGNAL_PB2_SHORT) || checkSignal(SIGNAL_PB2_LONG) )
			{
				opState = MODE_STATE_OPERATE_MSG;
				StopMacroRecord();
				StoreNvMacroToEeprom(2);
			}
		break ;

		case MODE_STATE_MACRO_STORE3:
			if ( checkSignal(SIGNAL_PB3_SHORT) || checkSignal(SIGNAL_PB3_LONG) )
			{
				opState = MODE_STATE_OPERATE_MSG;
				StopMacroRecord();
				StoreNvMacroToEeprom(3);
			}
		break ;
		
		case MODE_STATE_WPM_MSG:
			opState = MODE_STATE_WPM_MAIN_WAIT;
			ConfigureKeyForMessages();					
			SetSpellMsg(wpmMsg) ;		
		break ;

		case MODE_STATE_WPM_MAIN_WAIT:
			if ( SignalIsSet(SIGNAL_MSG_END) )
			{
				ClearSignal(SIGNAL_MSG_END);
				opState = MODE_STATE_WPM_MAIN;
				ConfigureKeyForSettings();				
			}
		break ;
		
		case MODE_STATE_WPM_MAIN:
			if ( adjustSetting(NV_WPM) )
				opState = MODE_STATE_WPM_MAIN_WAIT;

			if (checkSignal(SIGNAL_PB0_SHORT))
				opState = MODE_STATE_TONE_MSG ;

			if (checkSignal(SIGNAL_PB0_LONG))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (checkSignal(SIGNAL_PB3_SHORT))
				opState = MODE_STATE_WPM_MSG;
		break ;		
		
		case MODE_STATE_TONE_MSG:
			opState = MODE_STATE_TONE_MAIN_WAIT;
			ConfigureKeyForMessages();
			SetSpellMsg(toneMsg) ;		
			
		break ;

		case MODE_STATE_TONE_MAIN_WAIT:
			if ( SignalIsSet(SIGNAL_MSG_END) )
			{
				ClearSignal(SIGNAL_MSG_END);
				opState = MODE_STATE_TONE_MAIN;
				ConfigureKeyForSettings();
			}
		break ;
		
		case MODE_STATE_TONE_MAIN:
			if ( adjustSetting(NV_TONE) )
				opState = MODE_STATE_TONE_MAIN_WAIT;

			if (checkSignal(SIGNAL_PB0_SHORT))
				opState = MODE_STATE_KEY_MSG ;

			if (checkSignal(SIGNAL_PB0_LONG))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (checkSignal(SIGNAL_PB3_SHORT))
				opState = MODE_STATE_TONE_MSG;
		break ;		
		
		case MODE_STATE_KEY_MSG:
			opState = MODE_STATE_KEY_MAIN_WAIT;
			ConfigureKeyForMessages();
			SetSpellMsg(keyMsg) ;		
			
		break ;
		
		case MODE_STATE_KEY_VAL_MSG:
			if ( SignalIsSet(SIGNAL_MSG_END) )
			{
				ClearSignal(SIGNAL_MSG_END);
		
				opState = MODE_STATE_KEY_MAIN_WAIT;
				ConfigureKeyForMessages();
				switch (nvSettings[NV_KEY_TYPE])
				{
					case IAMBIC_A_KEY:
						SetSpellMsg(&keyIambicAMsg[3]) ;
					break ;
					
					case IAMBIC_B_KEY:
						SetSpellMsg(&keyIambicBMsg[3]) ;
					break ;
					
					default:
						SetSpellMsg(&keyStraightMsg[3]) ;
					break ;
				}		
			}
		break ;

		case MODE_STATE_KEY_MAIN_WAIT:
			if ( SignalIsSet(SIGNAL_MSG_END) )
			{
				ClearSignal(SIGNAL_MSG_END);
				opState = MODE_STATE_KEY_MAIN;
				ConfigureKeyForSettings();
			}
		break ;
		
		case MODE_STATE_KEY_MAIN:
			if ( adjustSetting(NV_KEY_TYPE) )
				opState = MODE_STATE_KEY_VAL_MSG;

			if (checkSignal(SIGNAL_PB0_SHORT))
				opState = MODE_STATE_OUT_MSG ;

			if (checkSignal(SIGNAL_PB0_LONG))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (checkSignal(SIGNAL_PB3_SHORT))
			{				
				opState = MODE_STATE_KEY_MAIN_WAIT;
				ConfigureKeyForMessages();
				switch (nvSettings[NV_KEY_TYPE])
				{
					case IAMBIC_A_KEY:
						SetSpellMsg(keyIambicAMsg) ;
					break ;
					
					case IAMBIC_B_KEY:
						SetSpellMsg(keyIambicBMsg) ;
					break ;
					
					default:
						SetSpellMsg(keyStraightMsg) ;
					break ;
				}
			}
		break ;		

		case MODE_STATE_OUT_MSG:
			opState = MODE_STATE_OUT_MAIN_WAIT;
			ConfigureKeyForMessages();
			SetSpellMsg(outMsg) ;		
			
		break ;
		
		case MODE_STATE_OUT_VAL_MSG:
			if ( SignalIsSet(SIGNAL_MSG_END) )
			{
				ClearSignal(SIGNAL_MSG_END);
		
				opState = MODE_STATE_OUT_MAIN_WAIT;
				ConfigureKeyForMessages();
				switch (nvSettings[NV_OUT_MODE])
				{
					case OUT_MODE_HF:
						SetSpellMsg(&outHfMsg[3]) ;
					break ;
					
					case OUT_MODE_UV:
						SetSpellMsg(&outUvMsg[3]) ;
					break ;
					
					default:
						SetSpellMsg(&outOffMsg[3]) ;
					break ;
				}		
			}
		break ;

		case MODE_STATE_OUT_MAIN_WAIT:
			if ( SignalIsSet(SIGNAL_MSG_END) )
			{
				ClearSignal(SIGNAL_MSG_END);
				opState = MODE_STATE_OUT_MAIN;
				ConfigureKeyForSettings();
			}
		break ;
		
		case MODE_STATE_OUT_MAIN:
			if ( adjustSetting(NV_OUT_MODE) )
				opState = MODE_STATE_OUT_VAL_MSG;

			if (checkSignal(SIGNAL_PB0_SHORT))
				opState = MODE_STATE_SPKR_MSG ;

			if (checkSignal(SIGNAL_PB0_LONG))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (checkSignal(SIGNAL_PB3_SHORT))
			{				
				opState = MODE_STATE_OUT_MAIN_WAIT;
				ConfigureKeyForMessages();
				switch (nvSettings[NV_OUT_MODE])
				{
					case OUT_MODE_HF:
						SetSpellMsg(outHfMsg) ;
					break ;
					
					case OUT_MODE_UV:
						SetSpellMsg(outUvMsg) ;
					break ;
					
					default:
						SetSpellMsg(outOffMsg) ;
					break ;
				}
			}
		break ;		

		case MODE_STATE_SPKR_MSG:
			opState = MODE_STATE_SPKR_MAIN_WAIT;
			ConfigureKeyForMessages();
			SetSpellMsg(spkrMsg) ;		
			
		break ;

		case MODE_STATE_SPKR_MAIN_WAIT:
			if ( SignalIsSet(SIGNAL_MSG_END) )
			{
				ClearSignal(SIGNAL_MSG_END);
				opState = MODE_STATE_SPKR_MAIN;
				ConfigureKeyForSettings();
			}
		break ;
		
		case MODE_STATE_SPKR_MAIN:
			if ( adjustSetting(NV_SPKR_VOL) )
				opState = MODE_STATE_SPKR_MAIN_WAIT;

			if (checkSignal(SIGNAL_PB0_SHORT))
				opState = MODE_STATE_EAR_MSG ;

			if (checkSignal(SIGNAL_PB0_LONG))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (checkSignal(SIGNAL_PB3_SHORT))
				opState = MODE_STATE_SPKR_MSG;
		break ;		

		case MODE_STATE_EAR_MSG:
			opState = MODE_STATE_EAR_MAIN_WAIT;
			ConfigureKeyForMessages();
			SetSpellMsg(earMsg) ;		
			
		break ;

		case MODE_STATE_EAR_MAIN_WAIT:
			if ( SignalIsSet(SIGNAL_MSG_END) )
			{
				ClearSignal(SIGNAL_MSG_END);
				opState = MODE_STATE_EAR_MAIN;
				ConfigureKeyForSettings();
			}
		break ;
		
		case MODE_STATE_EAR_MAIN:
			if ( adjustSetting(NV_EAR_VOL) )
				opState = MODE_STATE_EAR_MAIN_WAIT;

			if (checkSignal(SIGNAL_PB0_SHORT))
				opState = MODE_STATE_WPM_MSG ;

			if (checkSignal(SIGNAL_PB0_LONG))
				opState = MODE_STATE_EXIT_SETTINGS ;
				
			if (checkSignal(SIGNAL_PB3_SHORT))
				opState = MODE_STATE_EAR_MSG;
		break ;	
							
	}

}

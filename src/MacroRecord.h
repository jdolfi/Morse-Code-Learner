#define MACRO_MEM_SIZE 50
extern unsigned char macroMem[MACRO_MEM_SIZE];

typedef enum
{
	MACRO_SYMBOL_DIT		= 0x00,
	MACRO_SYMBOL_DAH		= 0x01,
	MACRO_SYMBOL_CHAR_GAP	= 0x02,
	MACRO_SYMBOL_WORD_GAP	= 0x03,
	
} MACRO_SYMBOLS_ENUM ;

void MacroRecordStart();
void MacroRecordStop();
void MacroRecordSymbol(unsigned char bits);
void MacroRecordGap(unsigned char tickCount);

#ifndef SIGNALS_H
#define SIGNALS_H

extern unsigned char keyUnitTick ;
extern unsigned char charUnitTick ;
extern unsigned char playUnitTick ;
extern unsigned char signalButtonTick ;
extern unsigned char signalLedTick;
extern unsigned char signalOutTick;
extern unsigned char signalReplayTick;

extern unsigned char signalMsgEnd;

extern unsigned char signalDahLatch;
extern unsigned char signalDitLatch;

extern unsigned char signalPB0Short;
extern unsigned char signalPB0Long;

extern unsigned char signalPB1Short;
extern unsigned char signalPB1Long;

extern unsigned char signalPB2Short;
extern unsigned char signalPB2Long;

extern unsigned char signalPB3Short;
extern unsigned char signalPB3Long;

#define SBIT(bit) (bit=1)
#define CBIT(bit) (bit=0)


#endif


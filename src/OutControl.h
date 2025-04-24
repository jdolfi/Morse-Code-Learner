#ifndef OUT_CONTROL_H
#define OUT_CONTROL_H

void OutTimer();
void OutInitialize(OUT_MODE_ENUM pMode);
void OutEnable(unsigned short toneTimerValue);
void OutDisable();
void OutOff();

#endif



# Morse-Code-Learner
"Morse Code Learner" experimental firmware written with sdcc.  

!!!!!!! THERE IS NO GOING BACK TO THE ORIGINAL FIRMWARE !!!!!!

!!!!!!!                  AND                            !!!!!!

!!!!!!!        This may BRICK your device               !!!!!!

!!!!!!!            Use at your own risk                 !!!!!!

Buttons have new functions.  From left to right buttons are:

M - Menu - enter settings mode, exit settings mode to operate mode

D - Down - Decrease setting .

U - Up - Increase setting ..

R - Recall - setting name and sometimes value

Long press of the Menu button brings you to keyer settings which themselves are annunciated in morse code.  The first setting to be annunciated is WPM.  Short press the menu button to scroll through other settings.  Long press the menu button again to exit settings mode and return to operate mode.  Operate mode is indicated by the break (=) _..._  Red UV led flashes in settings mode.

WPM - keyer speed in wpm

TONE - keyer tone

KEY - key mode - straight, iambic A, iambic B - choices are annunciated when changed

OUT - function of the "OUT" port - HF (key the dits/dahs), UV (PTT an HT for simulated CW), or OFF - choices are annunciated when changed

SPKR - Speaker volume 1,2

EAR - Ear volume 1,2,3

In operating mode, when idle, the leds indicate the output mode.  No leds means out mode off.
While keying the HF led will code in red in HF mode, green otherwise.  The UV led will be solid red while PTT active.

In operating mode the D, U, R keys are memory keyers.  To record a message in each memory, long push corresponding button until UV flashes red.  Then record with paddle keyer.  Promptly short or long press the button when finished to finish recording.  A short press of the D,U,R buttons will play back the corresponding recorded message.

There is no power down function implemented for now.  Just unplug it :)

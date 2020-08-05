system clock = HSE  8MHz Crystal

This program blinks PC13 LED @ 0.5Hz by pulling the COUNTFLAG bit in system timer's (systick) control and status register CSR, this bit which is set every time the counter reachs zero and cleared after reloading automaticallly the "reload value" at the next clock tic .
Notes:
- loading any value in the "current value" register clears the countflag bit.
- Reading the CSR clears the countflag bit.

the definition of systick registers are in "perih.h"

Systick initialization steps:
 - Disable systick by writing 0 to CSR (bit0=0)
 - put the "Reload value" in Reload value register RVR 
   to calculate this value use the formula: Reload value = (Interval(s) x Clck(Hz) ) - 1
 - clear Current value register this clears also the COUNTFLAG bit.
 - enable the systick timer by setting bit0 in CSR; in the same time u can enable the interrupt (set bit1)
   and choose the clock source (bit2=1 -> clock = system clock, bit2=0 -> clock = system clock/8 )

======================================================

this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D blinker.bin -R
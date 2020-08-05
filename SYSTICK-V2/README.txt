system clock = HSE  8MHz Crystal 

This program blinks PC13 LED @ 0.5Hz using the interrupt of the system timer (systick).
the definition of systick registers are in "perih.h"
The processor goes in sleep mode waiting for interrupt.


Systick initialization steps:
 - Disable systick by writing 0 to control and status register CSR (bit0=0)
 - put the "Reload value" in Reload value register RVR 
   to calculate this value use the formula: Reload value = (Interval(s) x Clck(Hz) ) - 1
 - clear Current value register this clears also the COUNTFLAG bit.
 - enable the systick timer by setting bit0 in CSR; in the same time u can enable the interrupt (set bit1)
   and choose the clock source (bit2=1 -> clock = system clock, bit2=0 -> clock = system clock/8 )

the vector table :
  .word _RAMEND  ;@ loaded automaticcaly in r13 (SP) at reset
	.word reset_handler
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word SysTick_handler

NOTE: 
systick interrupt is an internal Cortex exception and is handled in the system registers, it's permanently
enabled so no explicit action required to enable it within the NVIC.
To enable the SysTick interrupt just set bit1 in control register (SysTick->CTR).
 
======================================
this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D blinker.bin -R
 
 

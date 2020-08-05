
V2:
 - Implement delay_ms() using systick interrupt.
 - Draw bitmap image
 - Play animation

------------------------------------------------------------------------------------
V1:
system clock =  PLL 72MHz using 8Mhz crystal

This program blinks  PC13 LED @ 0.5Hz using the systick interrupt  .
and  Outputs VGA signals:
	- HSYNC 35,16KHz at PA8 (Timer1 chanal2 output)
	- VSYNC 56Hz at PA1 (Timer2 chanal2 output) 
	
It draws an empty rectangle on the border and a filled rectangle in the midle of screen.

The processor goes in sleep mode waiting for systick interrupt.

when systick interrupt occurs the processor handels it. inside the handler PC13 is toggled.

 
=================

this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D vga.bin -R
 

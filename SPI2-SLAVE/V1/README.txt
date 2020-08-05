
system clock =  PLL 72MHz using 8Mhz crystal

This program blinks  PC13 LED @1Hz using the systick interrupt.
and waits for a byte from SPI2, configured as slave, using pulling or interrupt.
Then it sends the received byte via UART1.

The processor goes in sleep mode waiting for systick interrupt.

when systick interrupt occurs the processor handels it. inside the handler PC13 is toggled.

-----------------------------------------------
The master is an ATMEGA328P programmed to send some chars,  then it sends one char in an infinite loop via SPI.
It toggles a LEd connected on PB0 after each sent char.

Internal 8MHz clock
powerd by 3.3V 


============================================================

this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D spi.bin -R
 

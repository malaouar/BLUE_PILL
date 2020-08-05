system clock =  72Mhz PLL

This program blinks :
- PC13 LED @ 0.5Hz using the overflow interrupt of the timer1 .
- LED connected to PA8 @ 1Hz using the compare match interrupt CC1 of the timer1 . 

The processor goes in sleep mode waiting for interrupts.


Timer1 initialization steps:
 - Enable timer1 clock in RCC->APB2ENR register (bit11)
 - load (prescaler value - 1) in TIMER1->PSC register
 - load the interval value in TIMER1->ARR register 
 - load the compare value in TIMER1->CCR1 register
 - enable update overflow interrupt in TIMER1->DIER register (bit0).
 - enable compare match interrupt in TIMER1->DIER register (bit1).
 - enable timer1 overflow interrupt in NVIC (IRQn = 25)
 - enable timer1 compare match interrupt in NVIC (IRQn = 27)
 - start the counter: enable bit0 in TIMER1->CR1 register
 - overflow flag must be cleared (TIMER1->SR &= ~(1<<0)) after entering to timer1 oberflow interrupt handler 
 - compare flag must be cleared (TIMER1->SR &= ~(1<<1)) after entering to timer1 compare interrupt handler

=================
New crt0.s and linker script
vetors table file
peripeharls definition file "periph.h"



this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D timer1.bin -R
 
 

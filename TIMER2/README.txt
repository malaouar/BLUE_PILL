system clock = HSE  8MHz Crystal or 72Mhz PLL

This program blinks PC13 LED @ 0.5Hz using the interrupt of the timer2 .
The processor goes in sleep mode waiting for interrupt.


Timer2 initialization steps:
 - Enable timer2 clock in RCC->APB1ENR register (bit0)
 - load (prescaler value - 1) in TIMER2->PSC register
 - load the interval value in TIMER2->ARR register 
 - enable update interrupts in TIMER2->DIER register (bit0).
 - enable the counter in TIMER2->CR1 register (bit0)
 - enable timer2 interrupt in NVIC (IRQn = 28)
 - interrupt flag must be cleared (TIMER2->SR &= ~(1<<0)) after entering to timer2 interrupt handler 


=================
New crt0.s and linker script
vetors table file
peripeharls definition file "periph.h"



this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D timer2.bin -R
 
 

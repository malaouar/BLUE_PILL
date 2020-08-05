system clock =  72Mhz PLL
Nested interrupts, one interruption preempts another low level one.

This program blinks :
- PC13 LED @ 0.5Hz using the overflow interrupt of the timer1 (level 0-> Highest priority) .
- LED connected to PA8  @ 1Hz using the compare match interrupt CC1 of the timer1 ((level 15-> lowest priority, we ca, choose any level > 0). 

The processor goes in sleep mode waiting for interrupts.

when the compare match interrupt  occurs the processor handels it. inside the handler the processor is put  sleep mode waiting for overflow interrupt.

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

Config of interrupt priority level:
 - set  priority group from 0 to 7 by writing bits(10:8) --> PRIGROUP field, in SCB-> AIRCR register.
 the Priority Grouping Number is given in the Table:
 
Priority Group Number 	Split Point 	Group Prio Bits (x) 	Sub Prio Bits (y) 	Total Group Prio 	Total SubGroup Prio
0,1,2,3 	              [xxxx.0000] 	      [7-4] 	               N/A 	               16 	            1
4 	                    [xxx.y0000] 	      [7-5] 	               [4] 	                8 	            2
5 	                    [xx.yy0000] 	      [7-6] 	              [5-4] 	              4 	            4
6 	                    [x.yyy0000] 	       [7] 	                [6-4] 	              2 	            8
7 	                    [.yyyy0000] 	       N/A 	                [7-4] 	             N/A 	            16

 - if nesessary, encode the priority for a given interrupt with the given priority group, preemptive priority value, and subpriority value.
 - use this encoded value to set priority for the given interrupt in NVIC->IP register related to the given interrupt.
  
=================
New crt0.s and linker script
vetors table file
peripeharls definition file "periph.h"



this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D timer1.bin -R
 

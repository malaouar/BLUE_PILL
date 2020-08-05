
This program configures PLL for 72MHz using HSE 8MHz cristal clock.
Configure TIMER1 for update event interrupt and enable it in NVIC.
configure  USART1 for 115200 8N1 and PA9, PA10 .
Configure PC13 for LED
When the interrupt occurs it prints a char on USART and increments a volatile variable "counter" which is used by the "main" for loop. Last it clears the interrupt flag bit.

in the "main" for loop we check the value of "counter" and togle the LED.

--------------------
this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D tim1int.bin -R
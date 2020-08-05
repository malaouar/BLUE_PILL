USART1 with 72MHz PLL

1- setup for  using 8MHz Crystal + PLL 72MHz. 

2- There are a number of steps involved in initializing the uart.  The USART1 I/O is PA9 and PA10.  So GPIOA needs to be enabled and USART1 has to be enabled.  Now that GPIOA is enabled we can tell GPIOA that PA9 is going to use the alternate function output, USART1_TX.  For alternate function inputs the manual says to set them as normal inputs so we do that as well.  Now we initialize the uart following the procedure in the manual.

 we are running at 72MHz so some math:
 The baud rate for the receiver and transmitter (Rx and Tx) are both set to the same value as programmed in the Mantissa and Fraction values of USARTDIV.
 
     Tx/Rx baud =  fCK/(16*USARTDIV)

 fCK is the Input clock to the peripheral (PCLK1 for USART2, 3, 4, 5 or PCLK2 for USART1)
    
    //72000000/(16*115200) = 39.0625
    //0.0625 * 16 = 1
    //39 1/16 0x27


Only USART1 is clocked with PCLK2 (72 MHz max). Other USARTs are clocked with PCLK1 (36 MHz max).

This prgram prints a bunch of characters (12345678) , then go into an echo loop, receive a character then transmit it back.  What you type you get back.

this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D uart.bin -R
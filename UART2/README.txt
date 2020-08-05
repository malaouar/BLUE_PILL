USART2 with 72MHz PLL

1- setup for  using 8MHz Crystal + PLL 72MHz. 

2- There are a number of steps involved in initializing the uart.  The USART2 I/O is PA2 and PA3.  So GPIOA needs to be enabled and USART2 has to be enabled.  Now that GPIOA is enabled we can tell GPIOA that PA2 is going to use the alternate function output, USART2_TX.  For alternate function inputs the manual says to set them as normal inputs so we do that as well.  Now we initialize the uart following the procedure in the manual.

 USART2 is clocked with PCLK1 72MHz/2 = 36MHz so some math:
 The baud rate for the receiver and transmitter (Rx and Tx) are both set to the same value as programmed in the Mantissa and Fraction values of USARTDIV.
 
 

 
     Tx/Rx baud =  fCK/(16*USARTDIV)

 fCK is the Input clock to the peripheral (PCLK1 for USART2, 3, 4, 5 or PCLK2 for USART1)
    
    // calculate baud rate for 115200
    //36000000/(16*115200) = 36000000/1843200  = 19.53125 
 
	//  19 = 0x13
    //  0.53125 * 16 = 8.5 = approx val is 8
	
    PUT32(USART2_BRR ,0x0138); // DIVisor_Mantissa[15:4] = 0x13, DIVisor_Fraction[3:0] = 8


 for 9600:
    /* Divisor = 36000000/(16*9600)=234.375
     * The DIV Fractional part is 0.375 * 16 = 6
     * The Mantissa = 234.  The BRR is divided as follows 
     * Bits15:Bit4 = Mantissa
     * Bit3:Bit0 = Fractional part
     * These are combined as follows:
     * Mantissa << 4 + Fractional part
     * This is equivalent to dividing 36000000 by the desired baud rate
    */

Only USART1 is clocked with PCLK2 (72 MHz max). Other USARTs are clocked with PCLK1 (36 MHz max).

This prgram prints a bunch of characters (12345678) , then go into an echo loop, receive a character then transmit it back.  What you type you get back.

this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D uart.bin -R
#include "periph.h"


//======================================
void uart_init( void ){
    //assuming 72MHz clock, 115200 8N1
    unsigned int ra;

    ra = RCC->APB2ENR;
    ra |= 1<<2;   //Enable PORT A clock
    ra |= 1<<14;  //Enable USART1  clock
    RCC->APB2ENR = ra;

    //Configure PA9 (TX)  alternate function output push-pull
    //Configure PA10 (RX) as input floating
    ra = PORTA->CRH;
    ra &= ~(0xFF0);  // clear bits 11:8 and 7:4 (control PA10 and PA9 respectiveley)
    ra |= 0x490;   // PA10 CNF = 01 -> floating input, MODE = 00 -> input
                 // PA9 CNF = 10 -> Alternate function output Push-pull, MODE = 10 -> Output mode, max speed 2 MHz
    PORTA->CRH = ra; 

    
    // calculate baud rate
    //72000000/(16*115200) = 39.0625
    //0.0625 * 16 = 1
    //39 = 0x27
    USART1->BRR = 0x0271; // DIVisor_Mantissa[11:0] = 0x27, DIVisor_Fraction[3:0] = 1
    
    // Enable USART1 and TX,  RX
    USART1->CR1 = 0x200C;  // UE = 1 --> USART enable, TE= 1 -> Transmitter enable, RE=1 -> Receiver enable
    //USART1->CR2 = 0x0000;  // default value
    //USART1->CR3 = 0x0000;  // default value

}

//=======================================
char uart_getc( void ){ 
    while(1){
        if((USART1->SR) & 0x20) break; // if bit5 (RXNE) Read data register not empty = 1
    }
    return((USART1->DR) & 0xFF);
}

//=========================================
void uart_putc(char c ){
    while(1){
        if((USART1->SR) & 0x80) break; // if Bit7 (TXE) Transmit data register empty = 1
    }
    USART1->DR = c;
}

//========================
void uart_putstr(char * str){
    while(*str) uart_putc(*str++);
}

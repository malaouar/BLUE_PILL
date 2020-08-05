#include "periph.h"
#include "uart.h"


//======================================
void uart_init(void){ //USART2
    //assuming 72MHz clock, 115200 8N1
    unsigned int ra;

    ra = RCC->APB1ENR;
    ra |= 1<<17;  //Enable USART2  clock
    RCC->APB1ENR = ra;
	
    ra = RCC->APB2ENR;
    ra |= 1<<2;   //Enable PORT A clock
    RCC->APB2ENR = ra;

    //Configure PA2 (TX)  alternate function output push-pull
    //Configure PA3 (RX) as input floating
    ra = PORTA->CRL;
    ra &= ~(0xFF00);  // clear bits 15:12 and 11:8 (control PA3 and PA2 respectiveley)
    ra |= 0x4900;   // PA3 CNF = 01 -> floating input, MODE = 00 -> input
                 // PA2 CNF = 10 -> Alternate function output Push-pull, MODE = 10 -> Output mode, max speed 2 MHz
    PORTA->CRL = ra;

    
    // calculate baud rate
    //36000000/(16*115200) = 19.53..
    //0.53.. * 16 = 8...
    //19 = 0x13
    //USART2->BRR = 0x0271; // DIVisor_Mantissa[15:4] = 0x13, DIVisor_Fraction[3:0] = 8
	//Or SIMPLY
	USART2->BRR = 36000000/115200;
    
    // Enable USART2 and TX,  RX
    USART2->CR1 = 0x200C;  // UE = 1 --> USART enable, TE= 1 -> Transmitter enable, RE=1 -> Receiver enable
    // USART2->CR2 = 0x0000;  // default value
    // USART2->CR3 = 0x0000;  // default value

}


//=======================================
char uart_getc(void){

    while(1) if(USART2->SR & 0x20) break; // if bit5 (RXNE) Read data register not empty = 1
    return(USART2->DR & 0xFF);
}
//=========================================
void uart_putc(char c){

    while(1) if(USART2->SR & 0x80) break; // if Bit7 (TXE) Transmit data register empty = 1
    USART2->DR = c;
}

//=====================================
void uart_putstr(char *str){

    while(*str) uart_putc(*str++);
}
//=============================================
void hexstring(unsigned int d){
    //unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1){
        rb -= 4;
        rc = (d >> rb) & 0xF;
        if(rc > 9) rc += 0x37; else rc += 0x30;
        uart_putc(rc);
        if(rb == 0) break;
    }
    uart_putc(0x20);
    uart_putc(0x0D);
    uart_putc(0x0A);
}



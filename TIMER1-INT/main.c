
#include "periph.h"


volatile unsigned int counter;


//=============================================
void clock_init(void){ //72MHz
    unsigned int ra;

    //enable the external clock
    ra = RCC->CR;
    ra = ra|1<<16; //HSEON = 1
    RCC->CR = ra;

    //wait for HSE to settle
    while(1) if((RCC->CR) & 1<<17) break; // wait while HSERDY = 0  (HSE not ready)
    
    //setup pll source settings
    ra = RCC->CFGR;
    ra &= ~(0xF<<18);  // clear PLLMUL bits (21:18)
    ra |= (0x7<<18);  // multiplication factor = 9 (9x8 = 72MHz)
    ra &= ~(1<<17);  // clear PLLXTPRE bit
    ra |= (0<<17);  // PLLXTPRE = 0 (HSE clock not divided)
    ra &= ~(1<<16);  // clear PLLSRC bit
    ra |= (1<<16);  // PLLSRC = 1 (PLL entry clock = HSE)
    RCC->CFGR = ra;
    
    //enable the pll
    ra = RCC->CR;
    ra = ra|1<<24; //PLLON = 1 --> Turn ON the PLL
    RCC->CR = ra;
    
    //wait for pll to lock
    while(1) if((RCC->CR)&(1<<25)) break; //wait while PLLRDY = 0 (PLL not locked)
    
    //USB div/1.5, AHB not divided, APB2 not divided, APB1 div/2, MCO
    ra = RCC->CFGR;
    ra &= ~(0x7<<24);  // clear MCO bits (26:24)
    //ra |= (0x7<<24);  // MCO (clock output at PA8) = 72/2 = 36 MHz ( 111  --> PLL/2 )
    ra &= ~(0x1<<22);  // clear USBPRE 
    ra |= (0x0<<22);  // USBPRE = 0  -->  USB clock = PLL/1.5 = 72/1.5= 48MHz
    ra &= ~(0x7<<11);  // clear PPRE2
    ra |= (0x0<<11);  // PPRE2 = 000 -> HCLK not divided
    ra &= ~(0x7<<8);  // clear PPRE1
    ra |= (0x4<<8);  // PPRE1 = 100 -> HCLK/2
    ra &= ~(0xF<<4);  // clear HPRE
    ra |= (0x0<<4);  // HPRE = 0000 -> SYSCLK not divided
    RCC->CFGR = ra;
    
    //add wait states for the flash
    FLASH->ACR = 0x2; // Bits(2:0) LATENCY = 010 Two wait states, if 48 MHz =< SYSCLK =< 72 MHz
    
    //switch to the pll as the source
    ra = RCC->CFGR;
    ra &= ~(0x3<<0);  // clear SW
    ra |= (0x2<<0);  // SW = 10 --> SYSCLK source = PLL
    RCC->CFGR = ra;
    
    //wait for it 
    while(1) if((RCC->CFGR & 0xF) == 0xA) break;// read SWS + SW if value = A (1010) PLL used as system clock
}

//=========================================
static void uart_init(void){
    //assuming 72MHz clock, 115200 8N1
    unsigned int ra;

    ra = RCC->APB2ENR;
    ra|=1<<2;   //Enable PORT A clock
    ra|=1<<14;  //Enable USART1  clock
    RCC->APB2ENR = ra;

    //Configure PA9 (TX)  alternate function output push-pull
    //Configure PA10 (RX) as input floating
    ra = GPIOA->CRH;
    ra &= ~(0xFF0);  // clear bits 11:8 and 7:4 (control PA10 and PA9 respectiveley)
    ra |= 0x490;   // PA10 CNF = 01 -> floating input, MODE = 00 -> input
                 // PA9 CNF = 10 -> Alternate function output Push-pull, MODE = 10 -> Output mode, max speed 2 MHz
    GPIOA->CRH = ra;

    
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

//===========================================
static void uart_putc ( unsigned int c ){

    while(1) if(USART1->SR & 0x80) break;
    USART1->DR = c ;
}

//==========================================
static void hexstring ( unsigned int d ){
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_putc(rc);
        if(rb==0) break;
    }
    uart_putc(0x20);
    uart_putc(0x0D);
    uart_putc(0x0A);
}


//=================================
int main (void){
	
    unsigned int ra;
    unsigned int rc;

    clock_init();
    uart_init();
    hexstring(0x12345678);

	// enable clocks
    ra= RCC->APB2ENR;
    ra|=1<<11;  //TIM1
	ra|=1<<4;   // port c
    RCC->APB2ENR = ra;
	
    //config C13 output push-pull
    ra= GPIOC->CRH;
    ra&=~(3<<20);   //PC13
    ra|=1<<20;      //PC13
    ra&=~(3<<22);   //PC13
    ra|=0<<22;      //PC13
    GPIOC->CRH = ra;
	
	GPIOC->BSRR = 1<<(13+16); // LED ON

    counter=0;
    TIM1->CR1 = 0x00001; //enable timer1
    TIM1->DIER = 0x00001;  // enable Update interrupt
    NVIC->ISER[0] = 0x02000000;  // enable interrupt Nr 25 (TIMER1 Update)
	
	
    for(rc=0;rc<100;){
        if(counter>=1000){
            counter=0;
            hexstring(rc++);
			///*
			ra ^= 1<<13;
			GPIOC->ODR = ra;  // togle LED
			//*/
        }
    }
    TIM1->CR1 = 0x00000;  // Disable timer1
    TIM1->DIER = 0x00000; // clear interrupt enable bits


    return(0);
}

//===================================
void tim1_handler(void){
    counter++;
    uart_putc('l');
    TIM1->SR = 0; // clear interrupt flag bits
}
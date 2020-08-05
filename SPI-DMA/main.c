#include "periph.h"


//=============================================
static void clock_init( void ){
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
    ra |= (0x7<<24);  // MCO (clock output at PA8) = 72/2 = 36 MHz ( 111  --> PLL/2 )
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


//========================================================
int kmain( void ){ 
    unsigned int ra;
	char buf[32];

    clock_init();

    ra = RCC->APB2ENR; // Read APB2 peripheral clock enable register
    ra |= 1<<2;   //enable portA  CLK
    ra |= 1<<4;   //enable portc  CLK
    RCC->APB2ENR = ra;
    
    //configure PA8 to MCO : alternate function output push-pull
    ra = PORTA->CRH;
    ra &= ~(0x0F);  // clear bits(3:0) --> PA8 mode and CNF 
    ra |= 0x0B;  // bits(3:0) = 1011 --> MODE= 11 (Output mode, max speed 50 MHz), CNF= 10 (Alternate function output Push-pull)
    PORTA->CRH = ra;
	
    
    //PC13 config
    ra = PORTC->CRH; // Read PORT C configuration register high
    ra &= ~(3<<20);   //clear Bits(21:20) --> mode13
    ra |= 1<<20;      //mode13 = 01 --> Output mode, max speed 10 MHz
    ra &= ~(3<<22);   //clear Bits(23:22) --> CNF13
    ra |= 0<<22;      //CNF13 = 00 --> General purpose output push-pull
    PORTC->CRH = ra;

    // turn LED ON
    PORTC->BSRR = 1<<(13+16);  // PC13  = 0 --> LED ON 
	
	//fill source buffer with chars to send via SPI
	for(ra = 0; ra<32; ra++) buf[ra] = 0xAA;     
	

	//************  Init DMA ***********
	DMA_Init();
	
	// Transfer "count" bytes, decrements after each transfer 
	DMA1->CNDTR3 = 32;

	// source: Memory (or periheral) adress
	DMA1->CMAR3 = (uint32_t)&buf; // 

	DMA1->CCR3 |= 1<<0;  // enable DMA1  channel3

	//*********  Init SPI ************ 	 
	SPI_Init();	

	while(1) ;
 
    return(0);
}

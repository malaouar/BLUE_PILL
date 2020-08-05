#include "periph.h"

void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
//void sleep();
void timer2_init();
void nvic_enable(int IRQn);
void delay(volatile uint32_t s);
//=============================================
/* By default 8 MHz internal RC clock (HSI) is used
   we want the  8MHz External crystal (HSE) to be selected as the source clock */
void clock_8Mhz(void){
	RCC->CR |= (1 << 16); 	// Enable HSE (HSEON = 1)
	while (!(RCC->CR & (1 << 17))); 	// Wait untill HSE settles down (HSERDY = 0  --> HSE not ready)
	RCC->CFGR |= (1 << 0); // SW = 01 --> SYSCLK source = HSE
	//while((RCC->CFGR & 0xF) != 0x5)); // wait until (SWS + SW) = 5 (0101) --> HSE is used as system clock
}

//======================================
/*   we want the  72MHz PLL deriven by External crystal (HSE) to be selected as the source clock */

void clock_72Mhz(void){
	
	RCC->CR |= (1 << 16); // Enable HSE
	while (!(RCC->CR & (1 << 17))); // Wait untill HSE settles down
	
  //USB div/1.5, AHB not divided, APB2 not divided, APB1 div/2, MCO
  //RCC->CFGR  Reset value = 0x0000 0000 
  RCC->CFGR |= (0x7<<24);  // Bits(26:24) = 111  --> PLL/2 : MCO (clock output at PA8) = 72/2 = 36MHz
  RCC->CFGR |= (0x0<<22);  // Bit22 (USBPRE) = 0  -->  USB clock = PLL/1.5 = 72/1.5= 48MHz
  RCC->CFGR |= (0b0111 << 18); // bits(21:18) = 7 -> PLL multiplication factor = 9 (9x8 = 72MHz)
	RCC->CFGR |= (0<<17);  // Bit17 PLLXTPRE = 0 (HSE clock not divided)
	RCC->CFGR |= (1<<16);  // Bit16 PLLSRC = 1 (PLL entry clock = HSE)
  RCC->CFGR |= (0x0<<11);  // Bits(13:11) PPRE2 = 000 -> HCLK not divided
  RCC->CFGR |= (0x4<<8);  // Bits(10:8) PPRE1 = 100 -> HCLK/2
  RCC->CFGR |= (0x0<<4);  // Bits(7:4) HPRE = 0000 -> SYSCLK not divided
	
	//enable the pll
	RCC->CR |= (1 << 24); //PLLON = 1 --> Turn ON the PLL
  // Wait untill PLL settles down
	while (!(RCC->CR & (1 << 25))); //wait while PLLRDY = 0 (PLL not locked)

	// Necessary wait states for Flash for high speeds
	FLASH->ACR = 0x2; // Bits(2:0) LATENCY = 010 Two wait states necessary if 48 MHz =< SYSCLK =< 72 MHz
	
	// Finally, choose PLL as the system clock
	RCC->CFGR |= (0b10 << 0); // SW = 10 --> SYSCLK source = PLL
	//wait for it
  //while((RCC->CFGR) & 0xF)==0xA);// wait until (SWS + SW) = A (1010) ->  PLL used as system clock
}

//============================
unsigned int rb=0;

int kmain (){
    unsigned int ra;
    
    clock_8Mhz(); // Enable 8MHz HSE clock

    RCC->APB2ENR |= 1<<4; //enable port c
    
    //config PC13 as push-pull output
    ra = PORTC->CRH; // Read PORT C configuration register high
    ra &= ~(3<<20);   //clear Bits(21:20) --> mode13
    ra |= 1<<20;      //mode13 = 01 --> Output mode, max speed 10 MHz
    ra &= ~(3<<22);   //clear Bits(23:22) --> CNF13
    ra |= 0<<22;      //CNF13 = 00 --> General purpose output push-pull
    PORTC->CRH = ra;

    // Init  timer2
    timer2_init();
    //Then enable timer2 interrupt in NVIC
    nvic_enable(TIMER2_IRQ); 
    
   // dodo
   for(;;) {asm(" wfi") ;} // sleep forever waiting for interrupt

   // return(0);
}

//=========================================
/* Interrupt handler */
void TIM2_IRQ_handler(void){
  // interrupt flag must be cleared after enter to interrupt handler
	TIMER2->SR &= ~(1<<0); // clear UIF flag (cancel the interrupt)

  PORTC->ODR ^= (1 << 13); // toggle LED 
}


//====================
/* Enable given interrupt:
   Each ISER {0-7} holds 32 interrupts. Thus take mod32 of the given interrupt
     to choose the ISER number (ISER[0] for IRQn 0-31, and ISER[1] for IRQn 32-63 ..)
     Then, enable the given bit on that register based on the remainder of the mod.
 */
void nvic_enable(int IRQn){

	//if ( irq >= 44 )   return;
	NVIC->ISER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

//=======================
void timer2_init(){
  
  // first enable timer2 clck in RCC->APB1ENR register
  RCC->APB1ENR |= (1<<0) ; // enable timer2 clock
  
  // (799+1)x10000= 8000000 ---> blink @1Hz rate
	TIMER2->ARR = 10000 ; // Auto reload value = 10000 (When timer achieve ARR value, the UIF flag will be set)
                      //The counter is blocked while the auto-reload value is null.
	TIMER2->PSC = 799;  //PSC = 799 =>  prescaler = 800 ((prescaler= PSC + 1)) ===> Timer2 clck = 8 Mhz/800= 10Khz

	TIMER2->DIER |= (1<<0);  // enable update interrupts
	TIMER2->CR1 |= (1<<0); // enable the counter
}


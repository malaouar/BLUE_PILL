#include "periph.h"

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
    
    clock_72Mhz(); // Enable 8MHz HSE clock

    RCC->APB2ENR |= 1<<4; //enable port c
    RCC->APB2ENR |= 1<<2; //enable port A
    
    uart1_init();
    print("welcome to SEBSEB\r\n"); 
    hexstring(SCB->CPUID);// print HEX value of processor part number, version, and implementation information

    
    //config PC13 as push-pull output
    ra = PORTC->CRH; // Read PORT C configuration register high
    ra &= ~(3<<20);   //clear Bits(21:20) --> mode13
    ra |= 1<<20;      //mode13 = 01 --> Output mode, max speed 10 MHz
    ra &= ~(3<<22);   //clear Bits(23:22) --> CNF13
    ra |= 0<<22;      //CNF13 = 00 --> General purpose output push-pull
    PORTC->CRH = ra;

    //config PA8 as push-pull output
    ra = PORTA->CRH; // Read PORT C configuration register high
    ra &= ~(3<<0);   //clear Bits(1:0) --> mode8
    ra |= 1<<0;      //mode8 = 01 --> Output mode, max speed 10 MHz
    ra &= ~(3<<2);   //clear Bits(3:2) --> CNF8
    ra |= 0<<2;      //CNF8 = 00 --> General purpose output push-pull
    PORTA->CRH = ra;
    
    setPriGroup(3); // Priority Grouping = 3 --> 4bits for preemption and 0bits for subgroup
                    // we have 16 levels of priority (0 to 15) and NO subpriority level
                    
    //ra = encodePriority (3, 15, 0); // we don't need this cose Priority Grouping = 3
                                      // so simply priority = 0 to 15 with NO subpriority
    //setPriority(27, ra);                                  
                                      
    setPriority(27, 15); // set compare match priority level to 15 (lower then overflow which = 0 -> default)
        
    // enable timer1 interrupts in NVIC
    enableIrq(TIMER1_OV_IRQ); // enable overflow
    enableIrq(TIMER1_CC_IRQ); // enable compare match
    
    
    
    //Init  timer1 and start it
    timer1_init();

    
   // dodo
   for(;;) {asm(" wfi") ;} // sleep forever waiting for interrupt

   // return(0);
}

//=========================================
/* TIM1_Overflow Interrupt handler */
void TIM1_OV(void){
  // interrupt flag must be cleared after enter to interrupt handler
	TIMER1->SR &= ~(1<<0); // clear UIF flag (cancel the interrupt)

  PORTC->ODR ^= (1 << 13); // toggle LED PC13
  PORTA->ODR ^= (1 << 8); // toggle LED PA8
}
//=============================
/* TIM1_CaptureCompre Interrupt handler */
void TIM1_CC(void){
  // interrupt flag must be cleared after enter to interrupt handler
	TIMER1->SR &= ~(1<<1); // clear CC1IF flag (cancel the interrupt)
  PORTA->ODR ^= (1 << 8); // toggle LED PA8  
  asm(" wfi") ; // sleep  waiting for overflow interrupt
}

//=======================
void timer1_init(){
  
  // first enable timer1 clck in RCC->APB2ENR register
  RCC->APB2ENR |= (1<<11) ; // Bit11 = 1 --> enable timer1 clock
  
  // (7199+1)x10000= 72000000 ---> blink @1Hz rate
	TIMER1->ARR = 10000 ; // Auto reload value = 10000 (When timer achieve ARR value, the UIF flag will be set)
                      //The counter is blocked while the auto-reload value is null.
	TIMER1->PSC = 7199;  //PSC = 799 =>  prescaler = 7200 ((prescaler= PSC + 1)) ===> Timer2 clck = 72 Mhz/7200= 10Khz

  //TIMER1->CCMR1 = 0;  // output compare mode = timing, preload Disable
  TIMER1->CCR[0] =  5000  ; // Compare 1 value 

	TIMER1->DIER |= (1<<0);  // enable update (overflow) interrupt
	TIMER1->DIER |= (1<<1);  // enable compare1 interrupt
	
	TIMER1->CR1 |= (1<<0); // enable the counter
}


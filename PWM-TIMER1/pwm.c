#include "periph.h"

//=============================================
void clock_init( void ){ //72MHz
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

//***********************************************************************
void PWM_Init(){
	 uint16_t tmpcr1 = 0; 
	uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

	uint16_t TimerPeriod = 2048; //2048 original
	uint16_t Channel1Pulse = 144;		// pulse width 

	/**** Enable clocks for PORTA, TIMER1 & TIMER2********/
	RCC->APB2ENR |= (1<<2) | (1<<11);     // enable PORTA and TIMER1 clocks
	//RCC->APB1ENR |= (1<<0);     // enable clock for timer 2
	// Enable alternate function clock (NOT Necessary here !!). 
	//RCC->APB2ENR |= (1 << 0); // Bit0 = 1 ==> AFIO  enabled


	/* Configure PORTA Pin1 (TIM2 OC2) and 8  Pin8 (T1C1)  as alternate function output Push pull 50 MHz */
    // B=1011 ==> mode=11 (Output mode, max speed 50MHz), 
    //cnf=10 (Alternate function output Push-pull)
	//PORTA->CRL |= 0x000000b0; // PA1
	PORTA->CRH |= 0x0000000b; // PA8

	/* Select the Counter Mode */
	tmpcr1 = TIMER1->CR1;  
    tmpcr1 &= (uint16_t)(~((uint16_t)(0x0010 | 0x0060))); // clear DIR and CMS
    tmpcr1 |= 0x0000; // Counter Mode Up 
    /* Set the clock division */
    tmpcr1 &= (uint16_t)(~((uint16_t)0x0300));  // clear CKD
    tmpcr1 |= (uint32_t)0; // Clock Division;
    TIMER1->CR1 = tmpcr1;


  /* Set the Autoreload value */
  TIMER1->ARR = TimerPeriod; // Period 
 
  /* Set the Prescaler value */
  TIMER1->PSC = 0;    //Prescaler
    

    /* Set the Repetition Counter value */
    TIMER1->RCR = 0; //Repetition Counter

  /* Generate an update event to reload the Prescaler and the Repetition counter
     values immediately */
  TIMER1->EGR = (uint16_t)0x0001;   // TPSC Reload Mode Immediate;      
		
 /* Disable the Channel 1: Reset the CC1E Bit */
  TIMER1->CCER &= (uint16_t)(~(uint16_t)0x0001); // CCER_CC1E
  /* Get the TIMx CCER register value */
  tmpccer = TIMER1->CCER;
  /* Get the TIMx CR2 register value */
  tmpcr2 =  TIMER1->CR2;
  
  /* Get the TIMx CCMR1 register value */
  tmpccmrx = TIMER1->CCMR[0];
    
  /* Reset the Output Compare Mode Bits */
  tmpccmrx &= (uint16_t)(~((uint16_t)0x0070));  // CCMR1_OC1M
  tmpccmrx &= (uint16_t)(~((uint16_t)0x0003)); // CCMR1_CC1S

  /* Select the Output Compare Mode */
  tmpccmrx |= (uint16_t)0x0070;  // OCMode PWM2
  
  /* Reset the Output Polarity level */
  tmpccer &= (uint16_t)(~((uint16_t)0x0002)); // TCCER_CC1P
  /* Set the Output Compare Polarity */
  tmpccer |= 0x0002;  // OC Polarity low
  
  /* Set the Output State */
  tmpccer |= (uint16_t)0x0001; //Output State enable
      
    /* Reset the Output N Polarity level */
    tmpccer &= (uint16_t)(~((uint16_t)0x0008));  //  TCCER_CC1NP
    /* Set the Output N Polarity */
    tmpccer |=   (uint16_t)0x0000; // OCN Polarity high
    
    /* Reset the Output N State */
    tmpccer &= (uint16_t)(~((uint16_t)0x0004));  // CCER_CC1NE    
    /* Set the Output N State */
    tmpccer |= 0x0004;  // Output N_State enable
    
    /* Reset the Output Compare and Output Compare N IDLE State */
    tmpcr2 &= (uint16_t)(~((uint16_t)0x0100)); //TIM_CR2_OIS1
    tmpcr2 &= (uint16_t)(~((uint16_t)0x0200));  // TIM_CR2_OIS1N
    
    /* Set the Output Idle state */
    tmpcr2 |= (uint16_t)0x0000; //TIM_OCIdleState_Reset
    /* Set the Output N Idle state */
    tmpcr2 |= (uint16_t)0x0100;  //TIM_OCIdleState_Set

  
  /* Write to TIMx CR2 */
  TIMER1->CR2 = tmpcr2;
  
  /* Write to TIMx CCMR1 */
  TIMER1->CCMR[0] = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIMER1->CCR[0] = Channel1Pulse; // pulse width
 
  /* Write to TIMx CCER */ 
  TIMER1->CCER = tmpccer;		

   /* Enable the TIM Main Output */
    TIMER1->BDTR |= (uint16_t)0x8000;  //TIM_BDTR_MOE= Main Output enable

	/* Enable the TIM Counter */
    TIMER1->CR1 |= (uint16_t)0x0001;  //TIM_CR1_CEN = Counter enable   
}
//==========================================
void pc13_init(){
    unsigned int ra;

    ra = RCC->APB2ENR; // Read APB2 peripheral clock enable register
    ra |= 1<<4; //enable port c clock
    RCC->APB2ENR = ra;
    
    //config PC13 as push-pull output
    ra = PORTC->CRH; // Read PORT C configuration register high
    ra &= ~(3<<20);   //clear Bits(21:20) --> mode13
    ra |= 1<<20;      //mode13 = 01 --> Output mode, max speed 10 MHz
    ra &= ~(3<<22);   //clear Bits(23:22) --> CNF13
    ra |= 0<<22;      //CNF13 = 00 --> General purpose output push-pull
    PORTC->CRH = ra;
}

//=======================================
void systick_init(){
    // Init systick timer
    // Reload value = (interval(s) x clk(Hz)) - 1 
    // for clck = 72MHz/8= 9 and interval = 1s
    // reload val = 1x9000000 - 1 = 8999999
    SYSTICK->CSR = 0;  // Bit0 = 0 ==> systick disabled
    SYSTICK->RVR = 8999999; // Reload value 
    SYSTICK->CVR = 0; // Reset current value and clear COUNTFLAG
    SYSTICK->CSR = 3; // Bit0 = 1 ==> systick enabled
                      // bit1 = 1 ==> interrupt enabled
                      // bit2 = 0 ==> systick clock source =  system clock/8
}


//======================================

void kmain (void){
    
    clock_init(); // Enable 72MHz HSE clock
    PWM_Init();   // initialize PWM
    pc13_init(); // Init pc13 LED
    systick_init(); // init system timer
        
   for(;;) {asm(" wfi ");} // sleep forever waiting for interrupt

   // return(0);
}

//=============================================
void SysTick_handler(){
  
  PORTC->ODR ^= (1<< 13); // toggle LED
  
}
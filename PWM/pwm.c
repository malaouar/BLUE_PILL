#include "periph.h"

//=============================================
void clock_init(void){
    unsigned int ra;

    //enable the external clock
    ra = RCC->CR;
    ra |= 1<<16; //HSEON = 1
    RCC->CR = ra;

    //wait for HSE to settle
    while(1) if(RCC->CR & (1<<17)) break; // wait while HSERDY = 0  (HSE not ready)

    //  we want to use HSE as system clock
        //select HSE clock
        ra =  RCC->CFGR;
        ra &= ~(0x3<<0);  // clear SW
        ra |= (0x1<<0);  // SW = 01 --> SYSCLK source = HSE
        RCC->CFGR = ra;
        //wait for it
        while(1) if((RCC->CFGR & 0xF) == 0x5) break; // read SWS + SW if value = 5 (0101) HSE used as system clock
}

//***********************************************************************
void PWM_Init(){
  /**** Enable clocks for PORTA & TIMER2********/
  RCC->APB2ENR |= (1<<2);     // enable PORTA clock
  RCC->APB1ENR |= (1<<0);     // enable clock for timer 2
  // Enable alternate function clock (NOT Necessary here !!). 
	//RCC->APB2ENR |= (1 << 0); // Bit0 = 1 ==> AFIO  enabled


  /* Configure PORTA Pin1 (TIM2 OC2)  as alternate function output Push pull 50 MHz */
  PORTA->CRL |= 0x000000b0;  // B=1011 ==> mode=11 (Output mode, max speed 50MHz), 
                            //cnf=10 (Alternate function output Push-pull)

/******* Set timer 2 Period for PWM  **************  
   For a 50 Hz PWM: Period = (1/50Hz) = 0.02 Seconds  
  using the timer calculator with MCU Clock = 8MHz:
   Prescaler=3 ; Preload = 39999
***********************************************/

    TIMER2->ARR = 39999;    // Set Auto reload value (periode)
    TIMER2->PSC = 3;        // Set Prescalar value
    // Output Compare Mode, ENABLE Preload,PWM  mode1
    TIMER2->CCMR[0]  = 0x00006800; //bit15 (OC1CE: Output Compare 1 Clear Enable) = 0 => OC1Ref is not affected 
                               // by the ETRF Input
                               //bits(14:12) OC1M ( Output Compare2 mode) = 110 => PWM mode1 
                               //Bit11 (Output Compare 2 preload enable) = 1 ==>
                               // TIMx_CCR2 preload value is loaded in the active register at each update event.
                               //Bit10 (Output Compare 1 fast enable) = 0
                               //Bits(9:8) = 00 => CC2 channel is configured as output
                               
    TIMER2->EGR |= (1<<0);    //Bit0 UG (Update generation ENABLE) = 1 => Re-initialize the counter 
                            //and generates an update of the registers
    
    // Enable CH2 output and polarity active-high
    TIMER2->CCER = 0x00000010;   //bit4 CC2E (Capture/Compare2 output enable) =1 => channel 2 enabled; 
                               //bit5 CC2P Capture/Compare2 output polarity) =0 =>  active high
                               
    TIMER2->CR1 |= (1<<7);  // Bit7 ARPE (Auto-reload preload enable) = 1 => TIMx_ARR register is buffered
    TIMER2->CR1 |= (1<<0);  // Bit0 CEN (Counter enable) = 1 => ENABLE Timer counter     
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
    // for clck = 8MHz and interval = 1s
    // reload val = 1x8000000 - 1 = 7999999
    SYSTICK->CSR = 0;  // Bit0 = 0 ==> systick disabled
    SYSTICK->RVR = 7999999; // Reload value 
    SYSTICK->CVR = 0; // Reset current value and clear COUNTFLAG
    SYSTICK->CSR = 7; // Bit0 = 1 ==> systick enabled
                      // bit1 = 1 ==> interrupt enabled
                      // bit2 = 1 ==> systick clock source = system clock   (if 0 --> system clock/8)
}
//======================================
 unsigned int duty = 100;
 
void kmain (void){
    
    clock_init(); // Enable 8MHz HSE clock
    PWM_Init();   // initialize PWM
    pc13_init(); // Init pc13 LED
    systick_init(); // init system timer
    
    TIMER2->CCR[1]  = duty; // start with duty = 100
    
   for(;;) {asm(" wfi ");} // sleep forever waiting for interrupt

   // return(0);
}

//=============================================
void SysTick_handler(){
  
  PORTC->ODR ^= (1<< 13); // toggle LED
  
  duty += 100;
  if(duty >= 39999) duty =100;
  TIMER2->CCR[1]  = duty;   // SET duty cycle
}
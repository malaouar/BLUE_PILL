#include "periph.h"

void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
//void sleep();

//=============================================
static void clock_init ( void )
{
    unsigned int ra;

    //enable the external clock
    ra = GET32(RCC_CR);
    ra |= 1<<16; //HSEON = 1
    PUT32(RCC_CR,ra);

    //wait for HSE to settle
    while(1) if(GET32(RCC_CR) & (1<<17)) break; // wait while HSERDY = 0  (HSE not ready)

    //  we want to use HSE as system clock
        //select HSE clock
        ra = GET32(RCC_CFGR);
        ra &= ~(0x3<<0);  // clear SW
        ra |= (0x1<<0);  // SW = 01 --> SYSCLK source = HSE
        PUT32(RCC_CFGR,ra);
        //wait for it
        while(1) if((GET32(RCC_CFGR) & 0xF) == 0x5) break; // read SWS + SW if value = 5 (0101) HSE used as system clock
}

//======================================


unsigned int rb=0;

int kmain ( void )
{
    unsigned int ra;
    
    clock_init(); // Enable 8MHz HSE clock

    ra=GET32(RCC_APB2ENR); // Read APB2 peripheral clock enable register
    ra|=1<<4; //enable port c
    PUT32(RCC_APB2ENR,ra);

    
    //config PC13 as push-pull output
    ra = PORTC->CRH; // Read PORT C configuration register high
    ra &= ~(3<<20);   //clear Bits(21:20) --> mode13
    ra |= 1<<20;      //mode13 = 01 --> Output mode, max speed 10 MHz
    ra &= ~(3<<22);   //clear Bits(23:22) --> CNF13
    ra |= 0<<22;      //CNF13 = 00 --> General purpose output push-pull
    PORTC->CRH = ra;

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
    
   for(;;) {asm(" wfi ");} // sleep forever waiting for interrupt

   // return(0);
}

void SysTick_handler(){
  
  PORTC->ODR ^= (1<< 13); // toggle LED

/*
  if(rb == 0) {
     PORTC->BSRR |= 1<<(13+0);  //set bit13 in Port C bit set/reset register ==> set PC13  (LED OFF)
      rb = 1;
  }
  else{
     PORTC->BSRR |= 1<<(13+16);  ////set bit29 in Port C bit set/reset register ==> reset PC13  (LED ON)
     rb = 0;
  }
  */
}
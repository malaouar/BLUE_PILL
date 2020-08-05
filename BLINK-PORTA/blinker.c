#include "periph.h"

void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );


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
        return;
}

//======================================


int kmain ( void )
{
    unsigned int ra;
    
    clock_init(); // enable HSE crystal clock
    
    //ra=GET32(RCC_APB2ENR); // Read APB2 peripheral clock enable register
	ra= (unsigned int *)(RCC_APB2ENR);
    ra|=1<<2; //enable port A
    PUT32(RCC_APB2ENR,ra); 

    
    //config PA1 and PA7 as push-pull outputs
    ra = PORTA->CRL; // Read PORT A configuration register low
	/*------------------------------------------------------
	//PA1
    ra &= ~(3<<4);   //clear Bits(2:3) --> mode1
    ra |= 1<<2;      //mode1 = 01 --> Output mode, max speed 10 MHz
    ra &= ~(3<<6);   //clear Bits(4:5) --> CNF1
    ra |= 0<<3;      //CNF1 = 00 --> General purpose output push-pull
	//PA7
    ra &= ~(3<<28);   //clear Bits(28:29) --> mode7
    ra |= 1<<28;      //mode7 = 01 --> Output mode, max speed 10 MHz
    ra &= ~(3<<30);   //clear Bits(30:31) --> CNF7
    ra |= 0<<30;      //CNF13 = 00 --> General purpose output push-pull
	---------------------------------------------------------------*/
	
	ra &= ~((3<<4) | (3<<6) | (3<<28) | (3<<30)); // clear MODE and CNF bits for PA1 and PA7
	ra |= (1<<4) | 1<<28;  // make mode = 01 --> Output mode, max speed 10 MHz for PA1 and PA7
							// CNF = 0 already --> General purpose output push-pull
	
    PORTA->CRL = ra;
	
	ra = PORTA->CRH; // Read PORT A configuration register high
	ra &= ~((3<<0) | (3<<2));
	ra |= (1<<0);
	PORTA->CRH = ra;

    // Init systick timer
    // Reload value = (interval(s) x clk(Hz)) - 1
    // for clck = 8MHz and interval = 1s
    // reload val = 1x8000000 - 1 = 7999999
    SYSTICK->CSR = 0;  // Bit0 = 0 ==> systick disabled
    SYSTICK->RVR = 7999999; // Reload value 
    SYSTICK->CVR = 0; // Reset current value and clear COUNTFLAG
    SYSTICK->CSR = 5; // Bit0 = 1 ==> systick enabled
                      // bit1 = 0 ==> interrupt disabled
                      // bit2 = 1 ==> systick clock source = system clock   (if 0 --> system clock/8)
    
   for(;;){
      // wait while COUNTFLAG is not set
      // Reading the csr clears the countflag bit.
      while(1){ if(SYSTICK->CSR &(1<<16)) break; } 
      //PORTA->BSRR |= 1<<(1+0);  //set bit1 in PortA bit set/reset register (BSRR) ==> set PA1  (LED OFF)
	  //PORTA->BSRR |= 1<<(7+0);	//set bit7 in PortA bit set/reset register ==> set PA7  (LED OFF)
	  PORTA->BSRR |= (1<<(1+0) | 1<<(7+0) | 1<<(8+0));
      while(1){ if(SYSTICK->CSR &(1<<16)) break; }
      //PORTA->BSRR |= 1<<(1+16);  //set bit17 in PortA bit set/reset register ==> reset PA1  (LED ON)
	  //PORTA->BSRR |= 1<<(7+16);  //set bit23 in PortA bit set/reset register ==> reset PA1  (LED ON)
	  PORTA->BSRR |= (1<<(1+16) | 1<<(7+16) | 1<<(8+16));
    }

   // return(0);
}


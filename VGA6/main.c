#include "periph.h"
#include "video.h"
#include "gdi.h"
#include "uart.h"


volatile u32	Timing; //  delay variable for milliseconds
volatile unsigned int	n = 0;  // multipurpose var
volatile u32	flag = 0;	// flag for reception
volatile u32	DONE = 0;  // flag reception done


uint16_t *fb16p; // 16bit pointer 
char *fb8p;   // 8bit pointer 
uint16_t buf[1500]; // general purpose buffer

extern u8 img[];
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
    // for clck = 72MHz and interval = 1ms
    // reload val = 1x10(-3) x 72x10(+6) - 1 = 72000 - 1 = 71999
    SYSTICK->CSR = 0;  // Bit0 = 0 ==> systick disabled
    SYSTICK->RVR = 71999; // Reload value 
    SYSTICK->CVR = 0; // Reset current value and clear COUNTFLAG
    SYSTICK->CSR = 7; // Bit0 = 1 ==> systick enabled
                      // bit1 = 1 ==> interrupt enabled
                      // bit2 = 1 ==> systick clock source =  system clock
}


//=============================
void delay_ms(uint32_t dly) {

	Timing = dly;
	while (Timing > 0); // wait 
}

//======================================
void main(void){

	int n;
	
    clock_init(); // Enable 72MHz HSE clock
    pc13_init(); // Init pc13 LED
	PORTC->BSRR = 1 << (13 + 16); //LED ON

    systick_init(); // init system timer
	vidInit();
	
	// draw image	
	fb8p = fb;
	for(n=0; n<10000; n++) *fb8p++ = img[n];
	delay_ms(5000);
	
	//run text demo
	demo();

	
	
	//uart_init();
	//hexstring(0x12345678);
	//uart_putstr("Hello seb-seb!!\r\n");
	
	
	fb16p = buf; // let fb16p point the gp buffer
	fb8p = buf;
	
	for(;;) {asm(" nop ");}
     
   //for(;;) {asm(" wfi ");} // sleep forever waiting for interrupt
}

//=============================================
// systick interrupt handler every 1ms
void SysTick_handler(){
  
  if (Timing > 0) --Timing; // decrement timing variable
}

//==============================
void SPI2_IRQHandler(){
	
	uint16_t c;
	c = SPI2->DR;
	
	if(!DONE){
	
		// swap bytes: first received byte is the high byte in SPI2->DR, the second received byte is the low one
		asm ("rev16 %1,%0"
			  : "=r" (c)
			  : "r" (c));
			  
		*fb16p++ = c;  // save in buffer

		if(n++ == 10){// if 20 chars received
			*fb16p = 0; // zero terminated string
			drawTextEx(16,  160, fb8p, ROP_COPY);
			n = 0;  // reset counter			
			fb16p = buf;  // reset pointer
			fb8p = buf;
			DONE =1;
		}		
	}
	// else
	//hexstring(c); 
	PORTC->ODR ^= (1<< 13); // toggle LED
}
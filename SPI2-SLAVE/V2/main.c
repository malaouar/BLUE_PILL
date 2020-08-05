#include "periph.h"
#include "uart.h"

volatile unsigned int	Timing; //  delay variable for milliseconds
char buf[2];
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

//===============================================
void SPI2_init(){
	
	uint32_t ra;
	uint16_t tmpreg = 0;
	uint16_t SPI2_IRQn   = 36; // SPI2 global Interrupt
	
	
   // initialize clocks	
	//enable portB CLOCK
    ra = RCC->APB2ENR; // Read APB2 peripheral clock enable register
    ra |= 1<<3  ;   // set bit3  (PORTB) 
    RCC->APB2ENR = ra;  // write back to APB2 peripheral clock enable register
	//enable SPI CLOCK
    ra = RCC->APB1ENR; // Read APB1 peripheral clock enable register
    ra |= 1<< 14 ;   // set  bit14 (SPI2)
    RCC->APB1ENR = ra;  // write back to APB2 peripheral clock enable register

	// Enable alternate function clock
	RCC->APB2ENR |= (1 << 0); // Bit0 = 1 ==> AFIO  enabled	
	
	//configure MOSI, SCK and SS: CNF=01==> alternate function input floating, MODE= 00 
	// configure MISO: CNF=10==> alternate function output push-pull, MODE= 01 ==> 10MHz
    ra = PORTB->CRH;
    ra &= ~(0x0F<<28);  // clear bits(31:28) --> PB15 MOSI mode and CNF
    ra |= 0x04 << 28;  // bits(31:28) = 0100 --> MODE= 00, CNF= 01
    //ra &= ~(0x0F<<24);  // clear bits(27:24) --> PB14 MISO mode and CNF
    //ra |= 0x09 << 24;  // bits(27:24) = 1001 --> MODE= 01, CNF= 10 
    ra &= ~(0x0F<<20);  // clear bits(23:20) --> PB13 SCK mode and CNF
    ra |= 0x04 << 20;  // bits(23:20) = 0100 --> MODE= 00, CNF= 01
    ra &= ~(0x0F<<16);  // clear bits(19:16) --> PB12 SS mode and CNF
    ra |= 0x04 << 16;  // bits(19:16) = 0100 --> MODE= 00, CNF= 01 
    PORTB->CRH = ra;

	// Reset SPI2 Interface	   
	//RCC->APB1RSTR |= ((uint32_t)0x00004000);  // Enable SPI2 reset state
	//RCC->APB1RSTR &= ~((uint32_t)0x00004000); // Release SPI2 from reset state

	// Get the SPI2 CR1 value
	tmpreg = SPI2->CR1;
	  
	// Clear BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, MSTR, CPOL and CPHA bits
	tmpreg &= (uint16_t)0x3040;  // CR1_CLEAR_Mask
	  
	/* Configure SPI2: direction, NSS management, first transmitted bit, BaudRate prescaler
		 master/salve mode, CPOL and CPHA */	 

/*
  tmpreg |= (uint16_t)(Direction_2Lines_FullDuplex | Mode_Slave | DataSize_16b | CPOL_Low | CPHA_1Edge | NSS_Hard | BaudRatePrescaler_2 | FirstBit_MSB);
*/
				  
	//tmpreg |= (uint16_t)(0x0000 | 0x0000 | 0x0800 | 0x0000 |  0x0000 | 0x0200 |  0x0000 | 0x0000);
	//tmpreg |= (uint16_t)(0x0200 ); // bit9 = 1 => Software slave management enabled (default all other options)

	// bit11 (DFF)= 1 => 16-bit data, bit10 (RXONLY)= 1 => Receive-only mode, bit9 = 0 => Enable Hard NSS
	tmpreg |= (uint16_t)(0x0C00 ); //bit(11:9) = 110 => 16bit, receive only, hard NSS
				  
	// Write to SPI2 CR1
	SPI2->CR1 = tmpreg;
  
	// CRCPOLY Configuration
	//SPI2->CRCPR = 7; //  CRC Polynomial (default value)  

   // Enable the SPI2 RX  DMA request: Bit0 (RXDMAEN) in CR2
   //SPI2->CR2 |= (uint16_t)0x0001; // Bit0 = 1 => Rx buffer DMA enabled
	
   // Enable the SPI2 RX buffer not empty interrupt: Bit6 (RXNEIE) in CR2
   SPI2->CR2 |= 1 << 6; // Bit6 = 1 => RX buffer not empty interrupt enabled

    // Enable SPI2 interrupt in NVIC
    NVIC->ISER[SPI2_IRQn >> 5] = (uint32_t) (0x01 << (SPI2_IRQn & 0x1F));	
	  
	// Activate the SPI mode (Reset bit11 (I2SMOD bit) in I2SCFGR register) 
	SPI2->I2SCFGR &= (uint16_t)0xF7FF; // Bit11 = 0 => SPI Mode (Not I2S)	
		
	// Enable SPI2: set Bit 6 (SPE) in SPI2_CR1 
	SPI2->CR1 |= 1 << 6 ; // SPI2 enabled
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
    // for clck = 72MHz and interval = 1s
    // reload val = 1 x 9x10(+6) - 1 = 9000000 - 1 = 8999999
    SYSTICK->CSR = 0;  // Bit0 = 0 ==> systick disabled
    SYSTICK->RVR = 8999999; // Reload value 
    SYSTICK->CVR = 0; // Reset current value and clear COUNTFLAG
    SYSTICK->CSR = 3; // Bit0 = 1 ==> systick enabled
                      // bit1 = 1 ==> interrupt enabled
                      // bit2 = 1 ==> systick clock source =  system clock (0 system clock/8)
}


//=============================
void delay_ms(uint32_t dly){

	Timing = dly;
	while (Timing > 0); // wait 
}

//======================================
void main(void){

	char c;
	
    clock_init(); // Enable 72MHz HSE clock	
    pc13_init(); // Init pc13 LED
	PORTC->BSRR = 1 << (13 + 16); //LED ON

    systick_init(); // init system timer
	SPI2_init();
	uart_init();
	hexstring(0x12345678);
	uart_putstr("hello sebseb\r\n"); 

/*
	// for testing without using interrupt
	for(;;){	
		while(!(SPI2->SR & 1)); // wait until bit0 (RXNE) = 1
		c = SPI2->DR;
		uart_putc(c);				
	}
*/
     
   for(;;) {asm(" wfi ");} // sleep forever waiting for interrupt

   // return(0); 
}

//=============================================
// systick interrupt handler every 1ms 
void SysTick_handler(){
  
  //if (Timing > 0) --Timing; // decrement timing variable
  PORTC->ODR ^= (1<< 13); // toggle LED
}

//==============================
void SPI2_IRQHandler(){
	
	uint16_t c;
	c = SPI2->DR;
	uart_putc((char)(c >> 8)); //first received byte is the high byte in SPI2->DR
	uart_putc((char)(c & 0x00FF)); //second received byte is the low byte in SPI2->DR
}

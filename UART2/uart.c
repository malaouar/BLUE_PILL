
void PUT32 ( unsigned int, unsigned int );
unsigned int GET16 ( unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );
//===============   GPIOS  ========================
// Each PORT has 16 I/O (Px0 to Px15), x= A, B, C, .....
//------------------  PORT A ---------------------
#define PORT_A_BASE 0x40010800    // PORT A Base adress
#define PORT_A_CRL  (PORT_A_BASE+0x00)  // Port A configuration register low
#define PORT_A_CRH  (PORT_A_BASE+0x04)  // Port A configuration register high 
#define PORT_A_IDR  (PORT_A_BASE+0x08)  
#define PORT_A_ODR  (PORT_A_BASE+0x0C)
#define PORT_A_BSRR (PORT_A_BASE+0x10)  // Port A bit set/reset register
#define PORT_A_BRR  (PORT_A_BASE+0x14)
#define PORT_A_LCKR (PORT_A_BASE+0x18)

//------------------  PORT C ---------------------
#define PORT_C_BASE 0x40011000  // PORT C Base adress
#define PORT_C_CRL  (PORT_C_BASE+0x00) // Port C configuration register low
#define PORT_C_CRH  (PORT_C_BASE+0x04)  // Port C configuration register high 
#define PORT_C_IDR  (PORT_C_BASE+0x08)
#define PORT_C_ODR  (PORT_C_BASE+0x0C)
#define PORT_C_BSRR (PORT_C_BASE+0x10)  // Port C bit set/reset register
#define PORT_C_BRR  (PORT_C_BASE+0x14)
#define PORT_C_LCKR (PORT_C_BASE+0x18)



// =========== reset and clock control (RCC)  ==============
#define RCC_BASE 0x40021000        // Base adress
#define RCC_CR      (RCC_BASE+0x00)   // control register
#define RCC_CFGR    (RCC_BASE+0x04)   // configuration register
#define RCC_APB2ENR (RCC_BASE+0x18)   // APB2 peripheral clock enable register 
#define RCC_APB1ENR (RCC_BASE+0x1C)   // APB2 peripheral clock enable register

// bits in RCC_CR ( Clock control register)
#define HSEON 16 // High Speed External clock enable (0: HSE oscillator OFF, 1: HSE oscillator ON)
#define HSERDY 17  // External high-speed clock ready flag (0: HSE oscillator not ready, 1: HSE oscillator ready)
#define PLLON  24  //  PLL enable bit (0: PLL OFF, 1: PLL ON)
#define PLLRDY  25  //  PLL clock ready flag (0: PLL unlocked, 1: PLL locked)


// bits in RCC_CFGR (Clock configuration register)
// Bits 1:0 --> SW: System clock (SYSCLK) source ( 00: HSI, 01: HSE, 10: PLL)
// Bits 3:2 --> SWS: System clock source STATUS Set and cleared by hardware (00: HSI, 01: HSE, 10: PLL)
// Bits 7:4 --> HPRE: AHB prescaler (1000: SYSCLK2,...., 1111: SYSCLK/12)
// Bits 10:8 --> PPRE1: APB low-speed prescaler (APB1) (0xx: HCLK not divided, 100: HCLK/2, 101: HCLK/4, 110: HCLK/8, 111: HCLK/16)
// Bits 13:11 --> PPRE2: APB high-speed prescaler (APB2) (0xx: HCLK not divided, 100: HCLK/2, 101: HCLK/4, 110: HCLK/8, 111: HCLK/16)
// Bits 21:18 -->  PLLMUL: PLL multiplication factor
// Bits 26:24 --> MCO: Microcontroller clock output at pin PA8 (0xx: No clock, 100: System clock, 101: HSI, 110: HSE, 111: PLL/2)
#define PLLXTPRE 17 // HSE divider for PLL entry (0: HSE clock not divided, 1: HSE clock divided by 2)
#define PLLSRC 16 // PLL entry clock source (0: HSI clock/2, 1: HSE clock)
#define USBPRE  22 // USB prescaler (0: PLL clock is divided by 1.5, 1: PLL clock is not divided)

// bits in RCC_APB2ENR (APB2 peripheral clock enable register)
#define AFIOEN  0 // Alternate function IO clock enable (0: clock disabled, 1: clock enabled)

//============================================================
#define FLASH_ACR   0x40022000

//=======================================
#define USART2_BASE 0x40004400
#define USART2_SR   (USART2_BASE+0x00)  //Status register
#define USART2_DR   (USART2_BASE+0x04)  // Data register
#define USART2_BRR  (USART2_BASE+0x08)  // Baud rate register
#define USART2_CR1  (USART2_BASE+0x0C)  // Control register 1
#define USART2_CR2  (USART2_BASE+0x10)  // Control register 2
#define USART2_CR3  (USART2_BASE+0x14)  // Control register 3
#define USART2_GTPR (USART2_BASE+0x18)  // Guard time and prescaler register

//=============================================
static void clock_init ( void )
{
    unsigned int ra;

    //enable the external clock
    ra=GET32(RCC_CR);
    ra=ra|1<<16; //HSEON = 1
    PUT32(RCC_CR,ra);

    //wait for HSE to settle
    while(1) if(GET32(RCC_CR)&(1<<17)) break; // wait while HSERDY = 0  (HSE not ready)

    //setup pll source settings
    ra=GET32(RCC_CFGR);
    ra&=~(0xF<<18);  // clear PLLMUL bits (21:18)
    ra|= (0x7<<18);  // multiplication factor = 9 (9x8 = 72MHz)
    ra&=~(1<<17);  // clear PLLXTPRE bit
    ra|= (0<<17);  // PLLXTPRE = 0 (HSE clock not divided)
    ra&=~(1<<16);  // clear PLLSRC bit
    ra|= (1<<16);  // PLLSRC = 1 (PLL entry clock = HSE)
    PUT32(RCC_CFGR,ra);
    
    //enable the pll
    ra=GET32(RCC_CR);
    ra=ra|1<<24; //PLLON = 1 --> Turn ON the PLL
    PUT32(RCC_CR,ra);
    
    //wait for pll to lock
    while(1) if(GET32(RCC_CR)&(1<<25)) break; //wait while PLLRDY = 0 (PLL not locked)
    
    //USB div/1.5, AHB not divided, APB2 not divided, APB1 div/2, MCO
    ra=GET32(RCC_CFGR);
    ra&=~(0x7<<24);  // clear MCO bits (26:24)
    ra|= (0x7<<24);  // MCO (clock output at PA8) = 72/2 = 36 MHz ( 111  --> PLL/2 )
    ra&=~(0x1<<22);  // clear USBPRE 
    ra|= (0x0<<22);  // USBPRE = 0  -->  USB clock = PLL/1.5 = 72/1.5= 48MHz
    ra&=~(0x7<<11);  // clear PPRE2
    ra|= (0x0<<11);  // PPRE2 = 000 -> HCLK not divided
    ra&=~(0x7<<8);  // clear PPRE1
    ra|= (0x4<<8);  // PPRE1 = 100 -> HCLK/2
    ra&=~(0xF<<4);  // clear HPRE
    ra|= (0x0<<4);  // HPRE = 0000 -> SYSCLK not divided
    PUT32(RCC_CFGR,ra);
    
    //add wait states for the flash
    PUT32(FLASH_ACR,0x2); // Bits(2:0) LATENCY = 010 Two wait states, if 48 MHz =< SYSCLK =< 72 MHz
    
    //switch to the pll as the source
    ra=GET32(RCC_CFGR);
    ra&=~(0x3<<0);  // clear SW
    ra|= (0x2<<0);  // SW = 10 --> SYSCLK source = PLL
    PUT32(RCC_CFGR,ra);
    
    //wait for it
    while(1) if((GET32(RCC_CFGR)&0xF)==0xA) break;// read SWS + SW if value = A (1010) PLL used as system clock
}

//======================================
void uart_init(void){ //USART2
    //assuming 72MHz clock, 115200 8N1
    unsigned int ra;

    ra = GET32(RCC_APB1ENR);
    ra |= 1<<17;  //Enable USART2  clock
    PUT32(RCC_APB1ENR,ra);
	
    ra = GET32(RCC_APB2ENR);
    ra |= 1<<2;   //Enable PORT A clock
    PUT32(RCC_APB2ENR , ra);

    //Configure PA2 (TX)  alternate function output push-pull
    //Configure PA3 (RX) as input floating
    ra = GET32(PORT_A_CRL);
    ra &= ~(0xFF00);  // clear bits 15:12 and 11:8 (control PA3 and PA2 respectiveley)
    ra |= 0x4900;   // PA3 CNF = 01 -> floating input, MODE = 00 -> input
                 // PA2 CNF = 10 -> Alternate function output Push-pull, MODE = 10 -> Output mode, max speed 2 MHz
    PUT32(PORT_A_CRL , ra);
   
    // calculate BRR value for 115200 baud rate
    //36000000/(16*115200) = 36000000/1843200  = 19.53125
	//  19 = 0x13
    //  0.53125 * 16 = 8.5 = approx val is 8 	
    //PUT32(USART2_BRR ,0x0138); // DIVisor_Mantissa[15:4] = 0x13, DIVisor_Fraction[3:0] = 8

	// Or SIMPLY !!
	PUT32(USART2_BRR ,(36000000/115200));  // freq/bauderate
    
    // Enable USART2 and TX,  RX
    PUT32(USART2_CR1 , 0x200C);  // UE = 1 --> USART enable, TE= 1 -> Transmitter enable, RE=1 -> Receiver enable
    // USART2_CR2 = 0x0000;  // default value
    // USART2_CR3 = 0x0000;  // default value
}

//=======================================
char uart_getc(void){

    while(1) if(GET32(USART2_SR) & 0x20) break; // if bit5 (RXNE) Read data register not empty = 1
    return(GET32(USART2_DR) & 0xFF);
}
//=========================================
void uart_putc(char c){

    while(1) if(GET32(USART2_SR) & 0x80) break; // if Bit7 (TXE) Transmit data register empty = 1
    PUT32(USART2_DR , c);
}

//=====================================
void uart_putstr(char *str){

    while(*str) uart_putc(*str++);
}
//=============================================
void hexstring(unsigned int d){
    //unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1){
        rb -= 4;
        rc = (d >> rb) & 0xF;
        if(rc > 9) rc += 0x37; else rc += 0x30;
        uart_putc(rc);
        if(rb == 0) break;
    }
    uart_putc(0x20);
    uart_putc(0x0D);
    uart_putc(0x0A);
}


//=================================
int notmain ( void )
{
    char c;

    clock_init();
    uart_init();
    
    hexstring(0x12345678);
	uart_putstr("hello sebseb");
    
    while(1){
        c=uart_getc();
        uart_putc(c);
    }
    
    return(0);
}

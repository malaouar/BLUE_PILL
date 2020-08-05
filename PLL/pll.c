
void PUT32 ( unsigned int, unsigned int );
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
#define RCC_CFGR    (RCC_BASE+0x04)   // configuration register
#define RCC_APB2ENR (RCC_BASE+0x18)   // APB2 peripheral clock enable register 
#define RCC_CR      (RCC_BASE+0x00)   // control register

// bits in RCC_CR ( Clock control register)
#define HSEON 16 // High Speed External clock enable (0: HSE oscillator OFF, 1: HSE oscillator ON)
#define HSERDY 17  // External high-speed clock ready flag (0: HSE oscillator not ready, 1: HSE oscillator ready)
#define PLLON  24  //  PLL enable bit (0: PLL OFF, 1: PLL ON)
#define PLLRDY  25  //  PLL clock ready flag (0: PLL unlocked, 1: PLL locked)


// bits in RCC_CFGR (Clock configuration register)
// Bits 1:0 --> SW: System clock (SYSCLK) source ( 00: HSI, 01: HSE, 10: PLL)
// Bits 3:2 --> SWS: System clock source STATUS Set and cleared by hardware (00: HSI, 01: HSE, 10: PLL)
// Bits 7:4 --> HPRE: AHB prescaler (1000: SYSCLK/2,...., 1111: SYSCLK/12)
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
    
    
    /*
    // if we want to use HSE as system clock

        //select HSE clock
        ra=GET32(RCC_CFGR);
        ra&=~(0x3<<0);  // clear SW
        ra|= (0x1<<0);  // SW = 01 --> SYSCLK source = HSE
        PUT32(RCC_CFGR,ra);
        //wait for it
        while(1) if((GET32(RCC_CFGR)&0xF)==0x5) break; // read SWS + SW if value = 5 (0101) HSE used as system clock
    */
    
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


//========================================================
int notmain ( void )
{
    unsigned int ra;

    clock_init();

    ra=GET32(RCC_APB2ENR); // Read APB2 peripheral clock enable register
    ra|=1<<2;   //enable port A
    ra|=1<<4;   //enable port c
    PUT32(RCC_APB2ENR,ra);
    
    //configure PA8 to MCO : alternate function output push-pull
    ra=GET32(PORT_A_CRH);
    ra&=~(0x0F);  // clear bits(3:0) --> PA8 mode and CNF
    ra|=0x0B;  // bits(3:0) = 1011 --> MODE= 11 (Output mode, max speed 50 MHz), CNF= 10 (Alternate function output Push-pull)
    PUT32(PORT_A_CRH,ra);

    
    //PC13 config
    ra=GET32(PORT_C_CRH); // Read PORT C configuration register high
    ra&=~(3<<20);   //clear Bits(21:20) --> mode13
    ra|=1<<20;      //mode13 = 01 --> Output mode, max speed 10 MHz
    ra&=~(3<<22);   //clear Bits(23:22) --> CNF13
    ra|=0<<22;      //CNF13 = 00 --> General purpose output push-pull
    PUT32(PORT_C_CRH,ra);

    // turn LED ON
    PUT32(PORT_C_BSRR,1<<(13+16));  // PC13  = 0 --> LED ON

    return(0);
}

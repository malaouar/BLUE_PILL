
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

//===============   GPIO  ==========
#define GPIO_C_BASE 0x40011000  // GPIO C Base adress
#define GPIO_C_CRH   (GPIO_C_BASE + 0x04)// Port C configuration register high 
#define GPIO_C_BSRR  (GPIO_C_BASE + 0x10)  // Port C bit set/reset register 


// =========== reset and clock control (RCC)  ==============
#define RCC_BASE 0x40021000        // Base adress
#define RCC_APB2ENR (RCC_BASE+0x18)   //APB2 peripheral clock enable register 

int notmain ( void )
{
    unsigned int ra;
    unsigned int rx;
    
    ra=GET32(RCC_APB2ENR); // Read APB2 peripheral clock enable register
    ra|=1<<4; //enable port c
    PUT32(RCC_APB2ENR,ra);

    
    //PC13 config
    ra=GET32(GPIO_C_CRH); // Read PORT C configuration register high
    ra&=~(3<<20);   //clear Bits(21:20) --> mode13
    ra|=1<<20;      //mode13 = 01 --> Output mode, max speed 10 MHz
    ra&=~(3<<22);   //clear Bits(23:22) --> CNF13
    ra|=0<<22;      //CNF13 = 00 --> General purpose output push-pull
    PUT32(GPIO_C_CRH,ra);


    for(rx=0;;rx++)
    {
        PUT32(GPIO_C_BSRR,1<<(13+0));  // set PC13 in Port C bit set/reset register (LED OFF)
        for(ra=0;ra<600000;ra++) dummy(ra);
        PUT32(GPIO_C_BSRR,1<<(13+16));  // reset PC13  (LED ON)
        for(ra=0;ra<600000;ra++) dummy(ra); 
    }
    return(0);
}

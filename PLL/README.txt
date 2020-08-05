setup for  using 8MHz Crystal + PLL 72MHz. 
Output PLL/2 at PA8 pin (MCO)

this program must be loaded at 0x08002000  in flash so Load  it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D pll.bin -R

======================================== 
  example steps for using 8MHz Crystal + PLL 72MHz  :

- Set the Flash waitstate to 2 *(Ref. Manual Sec. 3)*
- Set the APB1 PCKL1 to not exceed 36MHz; set prescaleer to HCLK/2 (0b100) *(Ref. Manual Sec. 7)*
- Enable High Speed External clock. *(Ref. Manual Sec. 7)*
- Wait until the HSE clock is ready.
- Set PLL source to be HSE. *(Ref. Manual Sec. 7)*
- Set PLL Multiplier to 9 (8 MHz Crystal x 9 = 72MHz) *(Ref. Manual Sec. 7)*
- Enable the PLL. *(Ref. Manual Sec. 7)*
- Wait until the PLL is ready.
- Set clock source to use PLL. *(Ref. Manual Sec. 7)*
- Wait until PLL becomes clock source. *(Ref. Manual Sec. 7)*

void init_clock(void)
{
    // Conf clock : 72MHz using HSE 8MHz crystal w/ PLL X 9 (8MHz x 9 = 72MHz)
    FLASH->ACR      |= FLASH_ACR_LATENCY_2; //  Two wait states, per datasheet
    RCC->CFGR       |= RCC_CFGR_PPRE1_2;    //  prescale AHB1 = HCLK/2
    RCC->CR         |= RCC_CR_HSEON;        //  enable HSE clock
    while( !(RCC->CR & RCC_CR_HSERDY) );    //  wait for the HSEREADY flag
    
    RCC->CFGR       |= RCC_CFGR_PLLSRC;     //   set PLL source to HSE
    RCC->CFGR       |= RCC_CFGR_PLLMULL9;   //   multiply by 9
    RCC->CR         |= RCC_CR_PLLON;        //   enable the PLL
    while( !(RCC->CR & RCC_CR_PLLRDY) );    //   wait for the PLLRDY flag
    
    RCC->CFGR       |= RCC_CFGR_SW_PLL;     //   set clock source to pll

    while( !(RCC->CFGR & RCC_CFGR_SWS_PLL) );    //   wait for PLL as source
    
}

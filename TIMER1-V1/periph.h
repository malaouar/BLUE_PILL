#include <stdint.h>

//===================  reset and clock control (RCC)  ==========
typedef struct
{
	volatile uint32_t CR;       // clock control register,                 offset: 0x00 
	volatile uint32_t CFGR;     // clock configuration register,           offset: 0x04 
	volatile uint32_t CIR;      // clock interrupt register,               offset: 0x08 
	volatile uint32_t APB2RSTR; // APB2 peripheral reset register,         offset: 0x0C 
	volatile uint32_t APB1RSTR; // APB1 peripheral reset register,         offset: 0x10 
	volatile uint32_t AHBENR;   // AHB peripheral clock enable register,   offset: 0x14 
	volatile uint32_t APB2ENR;  // APB2 peripheral clock enable register,  offset: 0x18 
	volatile uint32_t APB1ENR;  // APB1 peripheral clock enable register,  offset: 0x1C 
	volatile uint32_t BDCR;     // backup domain control register,         offset: 0x20 
	volatile uint32_t CSR;      // control/status register,                offset: 0x24 
	volatile uint32_t AHBRSTR;  // AHB peripheral clock reset register,    offset: 0x28 
	volatile uint32_t CFGR2;    // clock configuration register 2,         offset: 0x2C 
} STM32_RCC;

#define RCC ((STM32_RCC *) 0x40021000) // RCC Base adress


//======================  GPIO ==================
typedef struct { 
  volatile uint32_t  CRL ;  // configuration register low
  volatile uint32_t  CRH ;  // configuration register high
  volatile uint32_t  IDR ;  // Input data register
  volatile uint32_t  ODR ;  // Ouput data register 
  volatile uint32_t  BSRR ; // Bit set/reset register
  volatile uint32_t  BRR ;  // Bit reset register 
  volatile uint32_t  LCKR ; // lock register
}  GPIO ;

#define PORTA ((GPIO *) 0x40010800)  // PORT A Base adress
#define PORTB ((GPIO *) 0x40010C00)  // PORT B Base adress
#define PORTC ((GPIO *) 0x40011000)  // PORT C Base adress
#define PORTD ((GPIO *) 0x40011400)  // PORT D Base adress

//===================== USART   ==================

typedef struct {
  volatile uint32_t SR;   //Status register
  volatile uint32_t DR;   // Data register
  volatile uint32_t BRR;  // Baud rate register
  volatile uint32_t CR1;  // Control register 1
  volatile uint32_t CR2;  // Control register 2
  volatile uint32_t CR3;  // Control register 3
  volatile uint32_t GTPR;  // Guard time and prescaler register
} USART ;

#define USART1 ((USART *) 0x40013800) // USART1 BASE adress

//======================   TIMERS ============
typedef struct {
	volatile uint32_t CR1;	    // 00  control register1 
	volatile uint32_t CR2;	    // 04  control register2 
	volatile uint32_t SMCR;	  // 08  slave mode control register 
	volatile uint32_t DIER;	  // 0c  DMA/Interrupt enable register 
	volatile uint32_t SR;	    // 10  status register 
	volatile uint32_t EGR;	    // 14  event generation register 
	volatile uint32_t CCMR[2];	// 18  capture/compare mode registers 1 @ 2
	volatile uint32_t CCER;	  // 20  capture/compare enable register 
	volatile uint32_t CNT;	    // 24  counter 
	volatile uint32_t PSC;	    // 28  prescaler 
	volatile uint32_t ARR;	    // 2c  auto-reload register 
	volatile uint32_t RCR;     // 30  repetition counter register
	volatile uint32_t CCR[4];	// 34  capture/compare registers 1 TO 4 
	volatile uint32_t BDTR;    // 44  break and dead-time register
	volatile uint32_t DCR;	    // 48  DMA control register 
	volatile uint32_t DMAR;	  // 4c  DMA address for full transfer 
} TIMER;

#define TIMER1	((TIMER *) 0x40012C00)  // TIMER1 base adress
#define TIMER2	((TIMER *) 0x40000000)
#define TIMER3	((TIMER *) 0x40000400)
#define TIMER4	((TIMER *) 0x40000800)

#define CHAN1		0
#define CHAN2		1
#define CHAN3		2
#define CHAN4		3

#define TIMER1_OV_IRQ 25
#define TIMER1_CC_IRQ 27
#define	TIMER2_IRQ	28
#define	TIMER3_IRQ	29

//======================  SYSTICK  =================
typedef struct {
  volatile uint32_t CSR;  // Control and status register
  volatile uint32_t RVR;  // Reload value register
  volatile uint32_t CVR;  // Current value register
  volatile uint32_t CLVR; // Calibration value register
} SYSTEMTIMER; 

#define SYSTICK (( SYSTEMTIMER *) 0xE000E010) // system timer base adress


//===================  NVIC ==================

typedef struct
{
  volatile uint32_t ISER[8];               // 0x000  Interrupt Set Enable Register 
        uint32_t RESERVED0[24];
  volatile uint32_t ICER[8];               // 0x080  Interrupt Clear Enable Register 
        uint32_t RSERVED1[24];
  volatile uint32_t ISPR[8];               // 0x100  Interrupt Set Pending Register 
        uint32_t RESERVED2[24];
  volatile uint32_t ICPR[8];               // 0x180  Interrupt Clear Pending Register 
        uint32_t RESERVED3[24];
  volatile uint32_t IABR[8];               // 0x200  Interrupt Active bit Register 
        uint32_t RESERVED4[56];
  volatile uint8_t  IP[240];               // 0x300  Interrupt Priority Register (8Bit wide) 
        uint32_t RESERVED5[644];
  volatile  uint32_t STIR;                 // 0xE00  Software Trigger Interrupt Register 
} STM32_NVIC ;

#define NVIC	((STM32_NVIC *) 0xe000e100) // NVIC base adress

//=============== FLASH =================

typedef struct {                                  
  volatile uint32_t  ACR;             // 0x00 Flash access control register          
  volatile uint32_t  KEYR;            // 0x04 Flash key register                  
  volatile uint32_t  OPTKEYR;         // 0x08 Flash option key register    
  volatile uint32_t  SR;              // 0x0C Status register      
  volatile uint32_t  CR;              // 0x10 Control register                                           
  volatile uint32_t  AR;              // 0x14 Flash address register     
  volatile uint32_t  RESERVED;
  volatile uint32_t  OBR;             // 0x1C Option byte register                                       
  volatile uint32_t  WRPR;            // 0x20 Write protection register             
} STM32_FLASH;   

#define FLASH ((STM32_FLASH *) 0x40022000) // FLASH base address
                                
#include <stdint.h>

//===================  reset and clock control (RCC)  ==========
typedef struct
{
	volatile uint32_t CR;       // 0x00 clock control register      
	volatile uint32_t CFGR;     // 0x04 clock configuration register  
	volatile uint32_t CIR;      // 0x08 clock interrupt register
	volatile uint32_t APB2RSTR; // 0x0C APB2 peripheral reset register
	volatile uint32_t APB1RSTR; // 0x10 APB1 peripheral reset register
	volatile uint32_t AHBENR;   // 0x14 AHB peripheral clock enable register
	volatile uint32_t APB2ENR;  // 0x18 APB2 peripheral clock enable register
	volatile uint32_t APB1ENR;  // 0x1C APB1 peripheral clock enable register
	volatile uint32_t BDCR;     // 0x20 backup domain control register
	volatile uint32_t CSR;      // 0x24 control/status register
	volatile uint32_t AHBRSTR;  // 0x28 AHB peripheral clock reset register
	volatile uint32_t CFGR2;    // 0x2C clock configuration register 2
} STM32_RCC;

#define RCC ((STM32_RCC *) 0x40021000) // RCC Base adress


//======================  GPIO ==================
typedef struct { 
  volatile uint32_t  CRL ;  // 0x00 configuration register low
  volatile uint32_t  CRH ;  // 0x04 configuration register high
  volatile uint32_t  IDR ;  // 0x08 Input data register
  volatile uint32_t  ODR ;  // 0x0C Ouput data register 
  volatile uint32_t  BSRR ; // 0x10 Bit set/reset register
  volatile uint32_t  BRR ;  // 0x14 Bit reset register 
  volatile uint32_t  LCKR ; // 0x18 lock register
}  GPIO ;

#define PORTA ((GPIO *) 0x40010800)  // PORT A Base adress
#define PORTB ((GPIO *) 0x40010C00)  // PORT B Base adress
#define PORTC ((GPIO *) 0x40011000)  // PORT C Base adress
#define PORTD ((GPIO *) 0x40011400)  // PORT D Base adress

//===================== USART   ==================

typedef struct {
  volatile uint32_t SR;   // 0x00 Status register
  volatile uint32_t DR;   // 0x04 Data register
  volatile uint32_t BRR;  // 0x08 Baud rate register
  volatile uint32_t CR1;  // 0x0C Control register 1
  volatile uint32_t CR2;  // 0x10 Control register 2
  volatile uint32_t CR3;  // 0x14 Control register 3
  volatile uint32_t GTPR;  // 0x18 Guard time and prescaler register
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
  volatile uint32_t ISER[8];    // 0x000  Interrupt Set Enable Register 
  uint32_t RESERVED0[24];
  volatile uint32_t ICER[8];    // 0x080  Interrupt Clear Enable Register 
  uint32_t RSERVED1[24];
  volatile uint32_t ISPR[8];    // 0x100  Interrupt Set Pending Register 
  uint32_t RESERVED2[24];
  volatile uint32_t ICPR[8];    // 0x180  Interrupt Clear Pending Register 
  uint32_t RESERVED3[24];
  volatile uint32_t IABR[8];    // 0x200  Interrupt Active bit Register 
  uint32_t RESERVED4[56];
  volatile uint8_t  IP[240];    // 0x300  Interrupt Priority Register (8Bit wide) 
  uint32_t RESERVED5[644];
  volatile  uint32_t STIR;      // 0xE00  Software Trigger Interrupt Register 
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

//============== SCB =====================
/*  Structure type to access the System Control Block (SCB).*/
typedef struct
{
  volatile uint32_t CPUID;       // 0x000 CPUID Base Register 
  volatile uint32_t ICSR;        // 0x004 Interrupt Control and State Register 
  volatile uint32_t VTOR;        // 0x008 Vector Table Offset Register 
  volatile uint32_t AIRCR;       // 0x00C Application Interrupt and Reset Control Register 
  volatile uint32_t SCR;         // 0x010 System Control Register 
  volatile uint32_t CCR;         // 0x014 Configuration Control Register 
  volatile uint8_t  SHP[12];     // 0x018 System Handlers Priority Registers (4-7, 8-11, 12-15) 
  volatile uint32_t SHCSR;       // 0x024 System Handler Control and State Register 
  volatile uint32_t CFSR;        // 0x028 Configurable Fault Status Register 
  volatile uint32_t HFSR;        // 0x02C HardFault Status Register 
  volatile uint32_t DFSR;        // 0x030 Debug Fault Status Register 
  volatile uint32_t MMFAR;       // 0x034 MemManage Fault Address Register 
  volatile uint32_t BFAR;        // 0x038 BusFault Address Register 
  volatile uint32_t AFSR;        // 0x03C Auxiliary Fault Status Register 
  volatile uint32_t PFR[2];      // 0x040 Processor Feature Register 
  volatile uint32_t DFR;         // 0x048 Debug Feature Register 
  volatile uint32_t ADR;         // 0x04C Auxiliary Feature Register 
  volatile uint32_t MMFR[4];     // 0x050 Memory Model Feature Register 
  volatile uint32_t ISAR[5];     // 0x060 Instruction Set Attributes Register 
  uint32_t RESERVED0[5];
  volatile uint32_t CPACR;       // 0x088 Coprocessor Access Control Register 
} STM32_SCB;

#define SCB ((STM32_SCB*) 0xE000ED00)

//================  AFIO ====================
typedef struct{
	volatile uint32_t EVCR;      // 0x00 
	volatile uint32_t MAPR;      // 0x04
	volatile uint32_t EXTICR1;   // 0x08 
	volatile uint32_t EXTICR2;   // 0x0C 
	volatile uint32_t EXTICR3;   // 0x10 
	volatile uint32_t EXTICR4;   // 0x14 
	volatile uint32_t MAPR2;     // 0x18 
} AFIO_type;

#define AFIO            ((AFIO_type  *)  0x40010000)  //  AFIO base address
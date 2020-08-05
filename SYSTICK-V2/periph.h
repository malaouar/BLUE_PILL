#include <stdint.h>


// =========== reset and clock control (RCC)  ==============
#define RCC_BASE 0x40021000        // Base adress
#define RCC_CFGR    (RCC_BASE+0x04)   // configuration register
#define RCC_APB2ENR (RCC_BASE+0x18)   // APB2 peripheral clock enable register 
#define RCC_CR      (RCC_BASE+0x00)   // control register

//==========================
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

//=======================================

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

//=======================================
typedef struct {
  volatile uint32_t CSR;  // Control and status register
  volatile uint32_t RVR;  // Reload value register
  volatile uint32_t CVR;  // Current value register
  volatile uint32_t CLVR; // Calibration value register
} SYSTEMTIMER; 

#define SYSTICK (( SYSTEMTIMER *) 0xE000E010) // system timer base adress

/*
 * Description: DMA memory to memory  transfer example to transfer 96 bytes of data.
				
 * DMA setup steps:
 *   1. Enable DMA clock from RCC->AHBENR register: bit0 = DMA1
 *   2. Setup DMA->CCRx register based on the requirements; (x = 1..7)
 *      e.g 32-bit mode, circular buffer, increment after each read/write ...
 *   3. Set source and destination addresses to CMARx and CPARx registers
 *   4. Set the number of byte to be transferred to CNDTRx register
 *   5. Enable DMA transfer: Activate the channel by setting the ENABLE bit in the DMA->CCRx register.
 *
 */

#include "periph.h"

//========================================
void DMA_Init(){ 
	
	uint32_t ra;

	// Enable DMA1 clock. Bit 0 in RCC AHBENR register
	//RCC->AHBENR |= (1 << 0);
	ra = RCC->AHBENR;
	ra |= (1 << 0);
	RCC->AHBENR = ra;
	
	

	// Enable memory to memory transfer on DMA1 channel 1
	DMA1->CCR1 |= (1 << 14);

	// Set DMA priority to very high
	DMA1->CCR1 |= (0b11 << 12);

	// Set memory transfer size to 8-bits 
	DMA1->CCR1 |= (0b00 << 10);  // default 8bits

	// Set peripheral transfer size to 8-bits
	DMA1->CCR1 |= (0b00 << 8);   // default 8bits

	// Enable memory increment mode
	DMA1->CCR1 |= (1 << 7);
	
	// Enable peripheral (here second memory zone) increment mode
	DMA1->CCR1 |= (1 << 6);
	
	// Data direction: 0 - Read from peripheral; 1 - Read from memory
	DMA1->CCR1 |= (1 << 4);
	

	
	/*
	ra = DMA1->CCR1;
	ra |= (1 << 14) | (0b11 << 12) | (0b00 << 10) | (0b00 << 8) | (1 << 7) | (1 << 6) | (1 << 4) ;
	DMA1->CCR1 = ra;
	
	*/
	
	
	/*---------------------------------------

	// Enable circular mode
	//   Cannot be used at the same time with memory mode
	//DMA1->CCR1 |= (1 << 5);

	
	// Enable transfer complete interrupt
	//DMA1->CCR1 |= (1 << 1);


	// Transfer "count" bytes, decrements after each transfer
	//DMA1->CNDTR1 = count;

	// Memory address (source)
	//DMA1->CMAR1 = (uint32_t)from;

	// Peripheral address (destination)
	//DMA1->CPAR1 = (volatile uint32_t)to;


	// Enable DMA
	//DMA1->CCR1 |= (1 << 0);
	
	------------------------------------*/ 

}

/*
 * Description: DMA memory to SPI1 example to transfer data  FROM A CIRCULAR BUFFER.
				
 * DMA setup steps:
 *   1. Enable DMA clock from RCC->AHBENR register: bit0 = DMA1
 *   2. Setup DMA->CCR3 register based on the requirements; 
 *      e.g 32-bit mode, circular buffer, increment after each read/write ...
 *   3. Set source and destination addresses to CMARx and CPARx registers
 *   4. Set the number of byte to be transferred to CNDTRx register
 *   5. Enable DMA transfer: Activate the channel by setting the ENABLE bit in the DMA->CCRx register.
 *
 */

#include "periph.h"

//========================================
void DMA_Init(){ 

	uint32_t tmpreg =0;
	
	// Enable DMA1 clock. Bit 0 in RCC AHBENR register
	RCC->AHBENR |= (1 << 0);	


	
	/* First Disable DMA1_Channel3 */
	  DMA1->CCR3 &= ~((uint16_t)0x0001) ; //DMA_CCR1_EN
	  
	/* Reset DMA1_Channel3 control register */
	  DMA1->CCR3  = 0;
	  
	/* Reset DMA1_Channel3 remaining bytes register */
	  DMA1->CNDTR3 = 0;
	  
	/* Reset DMA1_Channel3 peripheral address register */
	  DMA1->CPAR3  = 0;
	  
	/* Reset DMA1_Channel3 memory address register */
	  DMA1->CMAR3 = 0;
	  
	/* Reset interrupt pending bits for DMA1 Channel3 */
	DMA1->IFCR |=     ((uint32_t)(0x00000100 | 0x00000200 | 0x00000400 | 0x00000800));  // DMA1_Channel3_IT_Mask
	
	
	/*--------------------------- DMA1_Channel3 CCR Configuration -----------------*/
	  /* Get the DMA1_Channel3 CCR value */
	  tmpreg = DMA1->CCR3;
	  /* Clear MEM2MEM, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
	  tmpreg &= (uint32_t)0xFFFF800F;  // CCR_CLEAR_Mask
	  /* Configure DMA1_Channel3: data transfer, data size, priority level and mode */
	  
	/*
	  tmpreg |= DMA_DIR_PeripheralDST | DMA_Mode_Circular |
				DMA_PeripheralInc_Disable | DMA_MemoryInc_Enable |
				DMA_PeripheralDataSize_Byte | DMA_MemoryDataSize_Byte |
				DMA_Priority_Medium | DMA_M2M_Disable;
	*/
				
	tmpreg |= ((uint32_t)(0x00000010 | 0x00000020 |0x00000000 | 0x00000080 |0x00000000 | 0x00000000 |
				0x00001000 | 0x00000000));				

	  /* Write to DMA1_Channel3 CCR */ 
	  DMA1->CCR3 = tmpreg;

	/*--------------------------- DMA1_Channel3 CPAR Configuration ----------------*/
	  DMA1->CPAR3 = (uint32_t)&SPI1->DR;

}


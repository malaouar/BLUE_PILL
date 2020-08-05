#include "periph.h"
#include "video.h"


//========================================
void DMA_Init(){ 

	uint32_t tmpreg =0;
	uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;
	uint32_t DMA1_Channel3_IRQn = 13;
	
	// Enable DMA1 clock. Bit0 in RCC->AHBENR register
	RCC->AHBENR |= (1 << 0);	// Bit0 = 1


	
	/* First Disable DMA1_Channel3: reset EN flag (Bit0) in DMA1->CCR3 */
	  DMA1->CCR3 &= ~((uint16_t)0x0001) ; // Bit0 = 0 => DMA C3 disabled
	  
	/* Reset DMA1_Channel3 control register */
	  DMA1->CCR3  = 0;
	  
	/* Reset DMA1_Channel3 remaining bytes register */
	  DMA1->CNDTR3 = 0;
	  
	/* Reset DMA1_Channel3 peripheral address register */
	  DMA1->CPAR3  = 0;
	  
	/* Reset DMA1_Channel3 memory address register */
	  DMA1->CMAR3 = 0;
	  
	/* Reset interrupt pending bits for DMA1 Channel3 (Bits11:8) */
	DMA1->IFCR |=     (uint32_t)(0x00000F00);  // DMA1_Channel3_IT_Mask
	
	
	/*--------------------------- DMA1_Channel3 CCR Configuration -----------------*/
	  /* Get the DMA1_Channel3 CCR value */
	  tmpreg = DMA1->CCR3;
	  /* Clear MEM2MEM, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
	  tmpreg &= (uint32_t)0xFFFF800F;  // CCR_CLEAR_Mask
	  /* Configure DMA1_Channel3: data transfer, data size, priority level and mode */
	  
	/*
	  tmpreg |= DMA_DIR_PeripheralDST | DMA_Mode_Normal |
				DMA_PeripheralInc_Disable | DMA_MemoryInc_Enable |
				DMA_PeripheralDataSize_Byte | DMA_MemoryDataSize_Byte |
				DMA_Priority_Medium | DMA_M2M_Disable;
	*/
				
	tmpreg |= ((uint32_t)(0x00000010 | 0x00000000 |0x00000000 | 0x00000080 |0x00000000 | 0x00000000 |
				0x00001000 | 0x00000000));				

	  /* Write to DMA1_Channel3 CCR */ 
	  DMA1->CCR3 = tmpreg;

	// DMA1_Channel3 CPAR Configuration : destination peripheral address
	  DMA1->CPAR3 = (uint32_t)&SPI1->DR;
	  
	// Transfer "count" bytes, decrements after each transfer 
	DMA1->CNDTR3 = VTOTAL; // 52 BYTES

	// source: Memory (or periheral) adress
	DMA1->CMAR3 = (uint32_t)&fb; // 
//=========  Config interrupts priority=1, subprio=0 ===============	
/*  
    // Compute the Corresponding IRQ Priority     
    tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700))>> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;

    tmppriority = (uint32_t) (1 << tmppre);
    tmppriority |=  (0 & tmpsub);
    tmppriority = tmppriority << 0x04;
        
    NVIC->IP[DMA1_Channel3_IRQn] = tmppriority;

*/
    // Enable DMA1 Channel3 interrupt in NVIC
    NVIC->ISER[DMA1_Channel3_IRQn >> 5] = (uint32_t) (0x01 << (DMA1_Channel3_IRQn & 0x1F));
    // set  DMA1 channel3  Transfer complete interrupt TCIE flag (Bit1)
    DMA1->CCR3 |= 0x00000002; // Bit1 = 1 => Transfer complete interrupt enabled

	DMA1->CCR3 |= 1<<0;  // enable DMA1  channel3

}

//=================================================
void SPI_Init(){

	uint16_t tmpreg = 0;
	unsigned int ra;
		
	//enable portB and SPI CLOCKs
    ra = RCC->APB2ENR; // Read APB2 peripheral clock enable register
    ra |= 1<<3  | 1<< 12 ;   // set bit3  (PORTB) and bit12 (SPI)
    RCC->APB2ENR = ra;  // write back to APB2 peripheral clock enable register


	//configure PA7 (MOSI1): CNF= 10 ==> alternate function output push-pull, MODE= 11 ==> 50MHz
    ra = PORTA->CRL;
    ra &= ~(0x0F<<28);  // clear bits(31:28) --> PA8 mode and CNF
    ra |= 0x0B << 28;  // bits(31:28) = 1011 --> MODE= 11, CNF= 10 
    PORTA->CRL = ra;


	//Remap MOSI1 from PA7 to PB5
	// Configure PORTB Pin5  as alternate function output Push pull 50 MHz
	PORTB->CRL |= 0x00B00000;  
	
	// we must disable JTAG and SW on PORTB
	AFIO->MAPR &= 0xF0FFFFFF; // ~(7<<24) reset bits(26:24)
	AFIO->MAPR |= 0x04000000; // 4<<24  bits(26:24) = 100 => disable JTAG-DP and SW-DP

	// config AFIO_MAPR Bit0 (SPI1_REMAP)
	AFIO->MAPR |= 1<<0; // Bit0 = 1 => Remap (NSS/PA15, SCK/PB3, MISO/PB4, MOSI/PB5)	
	
/*---------------------------- SPIx CR1 Configuration ------------------------*/
  /* Get the SPIx CR1 value */
  tmpreg = SPI1->CR1;
  /* Clear BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, MSTR, CPOL and CPHA bits */
  tmpreg &= 0x3040;   // CR1_CLEAR_Mask
  /* Configure SPIx: direction, NSS management, first transmitted bit, BaudRate prescaler
     master/salve mode, CPOL and CPHA */
  /* Set BIDImode, BIDIOE and RxONLY bits according to SPI_Direction value */
  /* Set SSM, SSI and MSTR bits according to SPI_Mode and SPI_NSS values */
  /* Set LSBFirst bit according to SPI_FirstBit value */
  /* Set BR bits according to SPI_BaudRatePrescaler value */
  /* Set CPOL bit according to SPI_CPOL value */
  /* Set CPHA bit according to SPI_CPHA value */
/*
  tmpreg |= (uint16_t)(SPI_Direction | SPI_Mode | SPI_DataSize | SPI_CPOL | SPI_CPHA | SPI_NSS |				SPI_BaudRatePrescaler | SPI_FirstBit); 
*/
	
	// SPI_BaudRatePrescaler = 0x0008 ==> SPI CLK = 72MHz/4= 18MHz 
	tmpreg |= (uint16_t)(0x0000 | 0x0104 | 0x0000 | 0x0000 | 0x0000 | 0x0300 | 0x0008 | 0x0000);

  /* Write to SPI1 CR1 */
  SPI1->CR1 = tmpreg;
  
  // Enable DMA request whenever the Transmit buffer is empty (TXE flag in SPI status register SPI->SR is set).
  SPI1->CR2 |= 1<<1;  // set bit1 
  
  /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
  SPI1->I2SCFGR &= 0xF7FF; //SPI_Mode_Select  (clear bit11) 

/*---------------------------- SPIx CRCPOLY Configuration --------------------*/
  /* Write to SPIx CRCPOLY */
  //SPI1->CRCPR = 7;  // SPI_CRCPolynomial, default = 7  
//---------------------
	
	/* Enable SPI1 */
    SPI1->CR1 |= 0x0040;  // CR1_SPE_Set 
  
}


//====================================
void 	SPIDMA_Init(){

	DMA_Init();
	SPI_Init();
}
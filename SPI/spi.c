#include "periph.h"


//=================================================
void SPI_Init(){

	uint16_t tmpreg = 0;
	unsigned int ra;
 
    // Step 1: Initialize SPI
		
	//enable portA and SPI CLOCKs
    ra = RCC->APB2ENR; // Read APB2 peripheral clock enable register
    ra |= 1<<2  | 1<< 12 ;   // set bit2  (PORTA) and bit12 (SPI)
    RCC->APB2ENR = ra;  // write back to APB2 peripheral clock enable register
	
	//configure PA7 to MOSI and PA5 to SCK: CNF= 10 ==> alternate function output push-pull, MODE= 11 ==> 50MHz
    ra = PORTA->CRL;
    ra &= ~(0x0F<<28);  // clear bits(31:28) --> PA8 mode and CNF
    ra |= 0x0B << 28;  // bits(31:28) = 1011 --> MODE= 11, CNF= 10 
    ra &= ~(0x0F<<20);  // clear bits(23:20) --> PA5 mode and CNF
    ra |= 0x0B << 20;  // bits(23:20) = 1011 --> MODE= 11, CNF= 10 	
    PORTA->CRL = ra;

	
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
	
	// SPI_BaudRatePrescaler = 0x0020 ==> SPI CLK = 72MHz/32
	tmpreg |= (uint16_t)(0x0000 | 0x0104 | 0x0000 | 0x0000 | 0x0000 | 0x0300 | 0x0020 | 0x0000);

  /* Write to SPI1 CR1 */
  SPI1->CR1 = tmpreg;
  
  /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
  SPI1->I2SCFGR &= 0xF7FF; //SPI_Mode_Select

/*---------------------------- SPIx CRCPOLY Configuration --------------------*/
  /* Write to SPIx CRCPOLY */
  //SPI1->CRCPR = 7;  // SPI_CRCPolynomial, default = 7
//---------------------
	
	/* Enable SPI1 */
    SPI1->CR1 |= 0x0040;  // CR1_SPE_Set
  
}
 
//===========================================================
void SPI_send(uint8_t data){

    // Wait until transmit complete
    while (!(SPI1->SR & 0x0002));  // TXE flag
    // Write data to be transmitted to the SPI data register
    SPI1->DR = data;
} 


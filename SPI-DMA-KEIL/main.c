

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_spi.h"

#define LED_PIN  GPIO_Pin_13
#define LED_PORT GPIOC

void SPI_Config(){
	SPI_InitTypeDef SPI_InitStructure;
	DMA_InitTypeDef	DMA_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI_Cmd(SPI1, DISABLE);
	DMA_DeInit(DMA1_Channel3);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
	//DMA_InitStructure.DMA_MemoryBaseAddr = (u32) &fb;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 32;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // SCK=72MHz/4
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPI1, &SPI_InitStructure);	
	SPI_CalculateCRC(SPI1, DISABLE);	
	SPI1->CR2 |= SPI_I2S_DMAReq_Tx;
}


void main(){
	int8_t i;
	char fb[32];
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* SPI1, GPIOA, GPIOC and AFIO clocks enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);

	// Configure the GPIO pin for the LED (PC13)
	GPIO_InitStruct.GPIO_Pin = LED_PIN; // Configure Led pin
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;// Set Output Push-Pull
	 
	GPIO_Init(LED_PORT, &GPIO_InitStruct);
	 
	GPIO_ResetBits(LED_PORT, LED_PIN);  // PC13 LED ON
	
	for(i=0; i<32; i++) fb[i] = 0xAA;
	 
	SPI_Config();
	
	DMA1_Channel3->CCR &= ~1; // disable DMA
	DMA1_Channel3->CMAR = (uint32_t) &fb;
	DMA1_Channel3->CCR |= 1<<0;  // enable DMA  channel3
	
	SPI_Cmd(SPI1, ENABLE);	// enable SPI1
	
	while(1);
 
}						
						
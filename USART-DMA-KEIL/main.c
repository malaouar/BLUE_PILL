#include "stm32f10x.h"


/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;
uint8_t TxBuffer[16]={'l', 'a', 'o', 'u', 'a', 'r'};


/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void DMA_Configuration(void);


//===================================================
int main(void){
  /* System Clocks Configuration */
  RCC_Configuration();
  /* Configure the GPIO ports */
  GPIO_Configuration();
  /* Configure the DMA */
  DMA_Configuration();


  /* USART1 is configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;

  /* Configure USART1 */
  USART_Init(USART1, &USART_InitStructure);

  /* Enable USART1 DMA  TX request */
 USART_DMACmd(USART1,  USART_DMAReq_Tx, ENABLE);
 
  /* Enable USART1 TX DMA1 Channel */
 DMA_Cmd(DMA1_Channel4, ENABLE);

  /* Enable the USART*/
  USART_Cmd(USART1, ENABLE);

  while (1);
 
}

//==========================================================
void RCC_Configuration(void){
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  /* Enable USART1 Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

//=====================================
void GPIO_Configuration(void){
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure USARTy Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
}


//===========================================
void DMA_Configuration(void){
  DMA_InitTypeDef DMA_InitStructure;

  /* USARTy TX DMA1 Channel (triggered by USARTy Tx event) Config */
  DMA_DeInit(DMA1_Channel4);
	
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//0x40013804;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 16;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);

}
#include "periph.h"
#include "video.h"


volatile uint16_t vline = 0;	 /* The current line being drawn */
volatile uint32_t vflag = 0;	 /* When 1, the SPI DMA request can draw on the screen */
volatile uint32_t vdraw = 0;	 /* Used to increment vline every 3 drawn lines */
uint8_t fb[VID_VSIZE][VID_HSIZE+2];  /* Frame buffer */


//*****************************************************************************
//	This irq is generated at the end of the horizontal back porch.
//	Test if inside a valid vertical start frame (vflag variable), 
//	and start the DMA to output a single frame buffer line through the SPI device.
//*****************************************************************************
void TIM1_CC_IRQHandler(void){
	if (vflag) DMA1->CCR3 = 0x93;  // Enable DMA1 C5
	TIM1->SR = 0xFFFB; //reset TIMER1 CC2  interrupt flag
}

//*****************************************************************************
//	This irq is generated at the end of the vertical back porch.
//	Sets the 'vflag' variable to 1 (valid vertical frame).
//*****************************************************************************
void TIM2_IRQHandler(void){
	//vflag = 1;
	//TIM2->SR = 0xFFF7; //reset TIMER2 CC3  interrupt flag
}

void TIM4_IRQHandler(void){
	vflag = 1;
	TIM4->SR = 0xFFF7; //reset TIMER4 CC3  interrupt flag
}

//*****************************************************************************
//	This interrupt is generated at the end of every line.
//	It will increment the line number and set the corresponding line pointer
//	in the DMA register.
//*****************************************************************************
void DMA1_Channel3_IRQHandler(void){

	DMA1->IFCR |= (uint32_t)(0x00000F00 );  // reset DMA1 channel3 interrupt flags
	DMA1->CCR3 = 0x92;  // Disable DMA1 C3
	DMA1->CNDTR3 = VTOTAL;
	
	vdraw++;
	
	if (vdraw == 3){
		vdraw = 0;

		vline++;
		
		if (vline == VID_VSIZE){
			vdraw = vline = vflag = 0;
			DMA1->CMAR3 = (uint32_t) &fb[0][0];
		} else {
			DMA1->CMAR3 += VTOTAL;
		}
	}

}

//=============================================
/*
void DMA1_Channel5_IRQHandler(void){	
	DMA1->IFCR |= (uint32_t)(0x000F0000 );  // reset DMA1 channel5 interrupt flags
	DMA1->CCR5 = 0x92;  // Disable DMA1 C5
	DMA1->CNDTR5 = VTOTAL;
	
	vdraw++;
	
	if (vdraw == 3){
		vdraw = 0;

		vline++;
		
		if (vline == VID_VSIZE){
			vdraw = vline = vflag = 0;
			DMA1->CMAR5 = (uint32_t) &fb[0][0];
		} else {
			DMA1->CMAR5 += VTOTAL;
		}
	}
}
*/
//=============================  
void clearScreen(void){
	uint16_t x, y;

	for (y = 0; y < VID_VSIZE; y++){
		for (x = 0; x < VTOTAL; x++) fb[y][x] = 0;
		//for (x = 0; x < VTOTAL; x++) fb[y][x] = 0xAA;

	}
}

//============================
void vidInit(void){

	clearScreen();
	TIMS_Init();
	SPIDMA_Init();

}

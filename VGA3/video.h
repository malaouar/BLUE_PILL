
#ifndef	__VIDEO_H
#define	__VIDEO_H

#include <stdint.h>

#define VTOTAL			52	    // Total bytes to send through SPI 
#define	VID_HSIZE		50		// Horizontal resolution (in bytes)
#define	VID_VSIZE		200		// Vertical resolution (in lines)

#define	VID_PIXELS_X	(VID_HSIZE * 8)
#define	VID_PIXELS_Y	VID_VSIZE
#define	VID_PIXELS_XR	(VID_PIXELS_X + 16)
#define	VID_HSIZE_R		(VID_HSIZE + 2)

extern   uint8_t fb[VID_VSIZE][VID_HSIZE+2];	 /* Frame buffer */
extern volatile uint16_t vline;	 /* The current line being drawn */
extern volatile uint32_t vflag ;	 /* When 1, the SPI DMA request can draw on the screen */
extern volatile uint32_t vdraw;	 /* Used to increment vline every 3 drawn lines */

//	Function definitions

void	vidInit(void); 
void	clearScreen(void); 
void 	TIMS_Init();
void 	SPIDMA_Init();
void 	delay_ms(uint32_t dly);




#endif	// __VIDEO_H 



	/*
		SVGA 800x600 @ 56 Hz
		Vertical refresh	35.15625 kHz
		Pixel freq.			36.0 MHz
		
		1 system tick @ 72Mhz = 0,0138 us
	*/
	
	/*
		Horizontal timing
		-----------------
		
		Timer 1 period = 35156 Hz
		
		Timer 1 channel 1 generates a pulse for HSYNC each 28.4 us.
		28.4 us	= Visible area + Front porch + Sync pulse + Back porch.
		HSYNC is 2 us long, so the math to do is:
		2us / 0,0138us = 144 system ticks.
		
		Timer 1 channel 2 generates a pulse equal to HSYNC + back porch.
		This interrupt will fire the DMA request to draw on the screen if vflag == 1.
		Since firing the DMA takes more or less 800ns, we'll add some extra time.
		The math for HSYNC + back porch is:
		(2us + 3,55us - dma) / 0,0138us = +-350 system ticks
	
		Horizontal timing info
		----------------------

						Dots	us
		--------------------------------------------		
		Visible area	800		22.222222222222
		Front porch		24		0.66666666666667
		Sync pulse		72		2
		Back porch		128		3.5555555555556
		Whole line		1024	28.444444444444
	
	*/

	/*
		Vertical timing
		---------------
		
		Polarity of vertical sync pulse is positive.

						Lines
		------------------------------
		Visible area	600
		Front porch		1
		Sync pulse		2
		Back porch		22
		Whole frame		625
		
	*/
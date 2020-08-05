
#include <stdint.h>
#include "gdi.h"
#include "video.h"


//*****************************************************************************
//	Function gdiPoint(PGDI_RECT rc, u16 x, u16 y)
//
//	Show a point in x/y position using the current graphical mode stored in 
//	grMode variable
//
//	parameters:
//		x			X position
//		y			Y position
//		rop			Raster operation. See GDI_ROP_xxx defines
//
//	return:			none
//*****************************************************************************
void gdiPoint(PGDI_RECT rc, u16 x, u16 y, u16 rop) {

u16		w, r;
u8		m;

//	Test for point outside display area

	if (x >= VID_PIXELS_X || y >= VID_PIXELS_Y) return;

	w = x >> 3;
	r = x - (w << 3);

//	Prepare mask

	m = (0x80 >> r);

	switch(rop) {
		case GDI_ROP_COPY:		fb[y][w] |= m;
								break;
		case GDI_ROP_XOR:		fb[y][w] ^= m;
								break;
		case GDI_ROP_AND:		fb[y][w] &= m;
								break;
	}
}

//*****************************************************************************
//	Function gdiLine(i16 x1, i16 y1, i16 x2, i16 y2, u16 rop)
//
//	Draw line using Bresenham algorithm 
//
//	This function was taken from the book:
//	Interactive Computer Graphics, A top-down approach with OpenGL
//	written by Emeritus Edward Angel
//
//	parameters:
//		prc			Clipping rectangle
//		x1			X start position
//		y1			Y start position
//		x2			X end position
//		y2			Y end position
//		rop			Raster operation. See GDI_ROP_xxx defines
//
//	return			none
//*****************************************************************************
void gdiLine(PGDI_RECT prc, i16 x1, i16 y1, i16 x2, i16 y2, u16 rop) {

i16		dx, dy, i, e;
i16		incx, incy, inc1, inc2;
i16		x, y;

	dx = x2 - x1;
	dy = y2 - y1;

	if(dx < 0) dx = -dx;
	if(dy < 0) dy = -dy;
	incx = 1;
	if(x2 < x1) incx = -1;
	incy = 1;
	if(y2 < y1) incy = -1;
	x=x1;
	y=y1;

	if (dx > dy) {
		gdiPoint(prc, x, y, rop);
		e = 2*dy - dx;
		inc1 = 2 * ( dy -dx);
		inc2 = 2 * dy;
		for (i = 0; i < dx; i++) {
			if (e >= 0) {
				y += incy;
				e += inc1;
			}
			else {
				e += inc2;
			}
			x += incx;
			gdiPoint(prc, x, y, rop);
		}
	} else {
		gdiPoint(prc, x, y, rop);
		e = 2 * dx - dy;
		inc1 = 2 * (dx - dy);
		inc2 = 2 * dx;
		for(i = 0; i < dy; i++) {
			if (e >= 0) {
				x += incx;
				e += inc1;
			} else {
				e += inc2;
			}
			y += incy;
			gdiPoint(prc, x, y, rop);
		}
	}
}

//*****************************************************************************
//	Function gdiRectangle(i16 x1, i16 y1, i16 x2, i16 y2, u16 rop)
//
//	Draw rectangle
//
//	parameters:
//		x1			X start position
//		y1			Y start position
//		x2			X end position
//		y2			Y end position
//		rop			Raster operation. See GDI_ROP_xxx defines
//
//	return			none
//*****************************************************************************
void	gdiRectangle(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop) {

	gdiLine(NULL,x0,y0,x1,y0,rop);
	gdiLine(NULL,x0,y1,x1,y1,rop);
	gdiLine(NULL,x0,y0,x0,y1,rop);
	gdiLine(NULL,x1,y0,x1,y1,rop);
}

//========================================
void gdiRectangleFull(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop) {
	
	uint16_t x2,y2,x3,y3;
	uint8_t a;
	
	x3=x1-x0;
	y3=y1-y0;
	
	for(a=0;a<y3;a++){
	gdiLine(NULL,x0,y0+a,x0+x3,y0+a,rop);
	}
	
}


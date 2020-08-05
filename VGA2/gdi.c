

#include "gdi.h"
#include "video.h"


//***************************************************************************** 
//	Function gdiBitBlt(PGDI_RECT prc, i16 x, i16 y, i16 w, i16 h, pu8 bm, u16 rop)
//
//	Bit Block Transfer funcion. This function uses the STM32 Bit-Banding mode
//	to simplify the complex BitBlt functionality.
//
//	From Cortex STM32F10x Reference Manual (RM0008):
//	A mapping formula shows how to reference each word in the alias region to a 
//	corresponding bit in the bit-band region. The mapping formula is:
//	bit_word_addr = bit_band_base + (byte_offset x 32) + (bit_number × 4)
//	where:
//	bit_word_addr is the address of the word in the alias memory region that 
//	maps to the targeted bit.
//	bit_band_base is the starting address of the alias region
//	byte_offset is the number of the byte in the bit-band region that contains 
//	the targeted bit bit_number is the bit position (0-7) of the targeted bit.
//	Example:
//	The following example shows how to map bit 2 of the byte located at SRAM 
//	address 0x20000300 in the alias region:
//	0x22006008 = 0x22000000 + (0x300*32) + (2*4).
//	Writing to address 0x22006008 has the same effect as a read-modify-write 
//	operation on bit 2 of the byte at SRAM address 0x20000300.
//	Reading address 0x22006008 returns the value (0x01 or 0x00) of bit 2 of 
//	the byte at SRAM address 0x20000300 (0x01: bit set; 0x00: bit reset).
//
//	For further reference see the Cortex M3 Technical Reference Manual
//
//	Parameters:
//
//		prc			Clipping rectangle. All X/Y coordinates are inside "prc"
//					If "prc" is NULL, the coordinates will be the entire display
//					area
//		x			Bitmap X start position
//		y			Bitmap Y start position
//		w			Bitmap width, in pixels
//		y			Bitmap height, in pixels
//		bm			Pointer to te bitmap start position
//		rop			Raster operation. See GDI_ROP_xxx defines
//
//	return			none
//*****************************************************************************
void gdiBitBlt(PGDI_RECT prc, i16 x, i16 y, i16 w, i16 h, pu8 bm, u16 rop) {

u16		i, xz, xb, xt;
u32		wb;					// Width in bytes
u32		r;					// Start X position in bits (relative to x)
u32		k;				
u32		d;
u32		offs;
u8		c;
pu8		fbPtr;				// Pointer to the Frame Buffer Bit-Band area
pu8		fbBak;
u8		fb1;
u32		fb2;
u32		rp;
pu8		bmPtr;				// Pointer to the bitmap bits

//	Calculate clipping region

	if (prc != NULL) {
		x = prc->x + x;
		y = prc->y + y;
	}

//	Get total bitmap width in bytes

	wb = (u32) w >> 3;
	if ((wb << 3) < (u32) w) ++wb;

//	Get starting bit inside the first byte

	d = (u32) x >> 3;
	r  = ((u32) x - (d << 3));

//	Clip X

	if (prc == NULL) {
		if ((x + w) >= VID_PIXELS_X ) {
			xt =  VID_PIXELS_X - x;
		} else {
			xt = w;
		}
	} else {
		if ((x + w) >= (x + prc->w)) {
			xt = prc->w - x;
		} else {
			xt = w;
		}
	}

//	Draw bits

	for (i = 0; i < h; i++) {

//	Clip Y

		if ((i + y) > (VID_VSIZE - 1)) return;

//	Get offset to frame buffer in bit-banding mode

		offs = (((u32) x >> 3)) + ((u32) (y + i)  * VID_HSIZE_R);
		k = (u32) (&fb - 0x20000000);
		k += offs;
		fbPtr = (pu8) (0x22000000 + (k * 32) + ((7 - r) * 4));
		fbBak = (pu8) (0x22000000 + (k * 32) + 28);

//	Get offset to bitmap bits

		bmPtr = bm + ((u32) i * wb);
		xz = w;

		xb = 0;
		for (xz = 0; xz < xt; xz++) {
			fb1 = ((u32) fbPtr) & 0x000000E0;
			if (xb++ == 0) {
				c = *bmPtr;
				++bmPtr;
			}
			xb &= 0x07;
			(c & 0x80) ? (rp = 1) : (rp = 0);
			switch(rop) {
				case GDI_ROP_COPY:	*fbPtr = rp;		break;
				case GDI_ROP_XOR:	*fbPtr ^= rp;		break;
				case GDI_ROP_AND:	*fbPtr &= rp;		break;
				case GDI_ROP_OR:	*fbPtr |= rp;		break;
			}
			fbPtr -= 4;
			fb2 = ((u32) fbPtr) & 0x000000E0;
			if (fb1 != fb2) {
				fbPtr = fbBak + 32;
				fbBak = fbPtr;
			}
			c <<= 1;
		}
	}
}

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

//***********************************************************************
void gdiRectangleFull(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop) {
	
	uint16_t x3,y3;
	uint8_t a;
	
	x3=x1-x0;
	y3=y1-y0;
	
	for(a=0;a<y3;a++){
	gdiLine(NULL,x0,y0+a,x0+x3,y0+a,rop);
	}
	
}


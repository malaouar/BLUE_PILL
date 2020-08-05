

#include "gdi.h"  
#include "video.h"

 
extern const u8 	font[];

const u8	closeBm[] = { 0x7f, 0xC0,0x7f, 0xC0, 0x7f, 0xC0, 0x7f, 0xC0, 0x40, 
						0x40, 0x7f, 0xC0, 0x7f, 0xC0, 0x7f, 0xC0, 0x7f, 0xC0 
			};


u16	strLen(pu8 str) {

	int		i = 0;

	while (*str != 0) { 
		++i;
		++str;
	}
	return(i);
}

//*****************************************************************************
//	Function copyRect(PRECT rc1, PRECT rc2) 
//	Copy rectangle rc2 to rc1
//	parameters:
//		rc1			Destination rectangle
//		y			Source rectangle
//*****************************************************************************
void copyRect(PRECT rc1, PRECT rc2) {

	rc1->x = rc2->x;
	rc1->y = rc2->y;
	rc1->w = rc2->w;
	rc1->h = rc2->h;
}

//*****************************************************************************
//	Function bitBlt(PRECT prc, i16 x, i16 y, i16 w, i16 h, pu8 bm, u16 rop)
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
//		prc			Clipping rectangle. All X/Y coordinates are inside "prc"
//					If "prc" is NULL, the coordinates will be the entire display
//					area
//		x			Bitmap X start position
//		y			Bitmap Y start position
//		w			Bitmap width, in pixels
//		y			Bitmap height, in pixels
//		bm			Pointer to te bitmap start position
//		rop			Raster operation. See ROP_xxx defines
//*****************************************************************************
void bitBlt(PRECT prc, i16 x, i16 y, i16 w, i16 h, pu8 bm, u16 rop) {

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

		offs = (((u32) x >> 3)) + ((u32) (y + i)  * VID_HSIZE);
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
				case ROP_COPY:	*fbPtr = rp;		break;
				case ROP_XOR:	*fbPtr ^= rp;		break;
				case ROP_AND:	*fbPtr &= rp;		break;
				case ROP_OR:	*fbPtr |= rp;		break;
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
//	Function point(PRECT rc, u16 x, u16 y)
//	Show a point in x/y position using the current graphical mode stored in 
//	grMode variable
//	parameters:
//		x			X position
//		y			Y position
//		rop			Raster operation. See ROP_xxx defines
//*****************************************************************************
void point(PRECT rc, u16 x, u16 y, u16 rop) {

	u16		w, r;
	u8		m;

//	Test for point outside display area

	if (x >= VID_PIXELS_X || y >= VID_PIXELS_Y) return;

	w = x >> 3;
	r = x - (w << 3);

//	Prepare mask

	m = (0x80 >> r);

	switch(rop) {
		case ROP_COPY:		fb[y][w] |= m; break;
		case ROP_XOR:		fb[y][w] ^= m; break;
		case ROP_AND:		fb[y][w] &= m; break;
	}
}

//*****************************************************************************
//	Function line(i16 x1, i16 y1, i16 x2, i16 y2, u16 rop)
//	Draw line using Bresenham algorithm 
//	This function was taken from the book:
//	Interactive Computer Graphics, A top-down approach with OpenGL
//	written by Emeritus Edward Angel
//	parameters:
//		prc			Clipping rectangle
//		x1			X start position
//		y1			Y start position
//		x2			X end position
//		y2			Y end position
//		rop			Raster operation. See ROP_xxx defines
//*****************************************************************************
void line(PRECT prc, i16 x1, i16 y1, i16 x2, i16 y2, u16 rop) {

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
		point(prc, x, y, rop);
		e = 2*dy - dx;
		inc1 = 2 * ( dy -dx);
		inc2 = 2 * dy;
		for (i = 0; i < dx; i++) {
			if (e >= 0) {
				y += incy;
				e += inc1;
			}
			else { e += inc2;}
			x += incx;
			point(prc, x, y, rop);
		}
	} else {
		point(prc, x, y, rop);
		e = 2 * dx - dy;
		inc1 = 2 * (dx - dy);
		inc2 = 2 * dx;
		for(i = 0; i < dy; i++) {
			if (e >= 0) {
				x += incx;
				e += inc1;
			} else { e += inc2;}
			y += incy;
			point(prc, x, y, rop);
		}
	}
}

//*****************************************************************************
//	Function rectangle(i16 x1, i16 y1, i16 x2, i16 y2, u16 rop)
//	Draw rectangle
//	parameters:
//		x1			X start position
//		y1			Y start position
//		x2			X end position
//		y2			Y end position
//		rop			Raster operation. See ROP_xxx defines
//*****************************************************************************
void	rectangle(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop) {

	line(NULL,x0,y0,x1,y0,rop);
	line(NULL,x0,y1,x1,y1,rop);
	line(NULL,x0,y0,x0,y1,rop);
	line(NULL,x1,y0,x1,y1,rop);
}


//***********************************************************************
// Draw full rectangle
void rectangleFull(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop) {
	
	uint16_t x2,y2,x3,y3;
	uint8_t a;
	
	x3=x1-x0;
	y3=y1-y0;
	
	for(a=0;a<y3;a++) 	line(NULL,x0,y0+a,x0+x3,y0+a,rop);	
}

//*****************************************************************************
//	Function rectangleEx(PRECT rc, u16 rop)
//	Draw rectangle
//	parameters:
//		rc			Struct containing the rectangle parameters
//		rop			Raster operation. See ROP_xxx defines
//*****************************************************************************
void	rectangleEx(PRECT rc, u16 rop) {

	rectangle(rc->x, rc->y, rc->x + rc->w, rc->y + rc->h,rop);
}

//*****************************************************************************
//	Function circle(i16 x, i16 y, i16 r, u16 rop)
//	Draw circle. This function uses the integer-precision math
//	parameters:
//		x			Circle center X position
//		y			Circle center Y position
//		r			Radius
//		rop			Raster operation. See ROP_xxx defines
//*****************************************************************************
void	circle(u16 x, u16 y, u16 r, u16 rop) {

	i32		x1, y1;
	u16		a;

	for (a = 0; a < 360; a++) {		
		x1 = r * cos(a);
		y1 = r * sin(a);
		point(NULL, (x1 / 10000) + x,(y1 / 10000) + y,rop);
	}
}

//*****************************************************************************
//	Function drawText(PRECT prc, pu8 ptext, u16 style, u16 rop)
//	Draw text inside rectangle
//	parameters:
//		prc			Pointer to clipping rectangle
//		ptext		Pointer to text
//		style		Text style (see WINCAPTION_xx defines)
//		rop			Raster operation. See ROP_xxx defines
//*****************************************************************************
void drawText(PRECT prc, pu8 ptext, u16 style, u16 rop) {

	u16		l, i, pos, xp;
	u8		c;
	pu8		ptx;

	l = strLen(ptext) * FONT_WIDTH;
	switch(style) {
		case WINCAPTION_RIGHT:		
			if (l < prc->w) prc->x += (prc->w - l); break;
		case WINCAPTION_CENTER:		
			if (l < prc->w) prc->x += ((prc->w - l) / 2); break;
	}
	l = strLen(ptext);
	xp = 1;//prc->x;
	for (i = 0; i < l; i++) {
		c = *(ptext++);
		if (c >= FONT_OFFSET) {
			pos = (u16) (c - FONT_OFFSET) * FONT_BYTEWIDTH * FONT_HEIGHT;
			ptx = ((pu8) font) + pos;
			bitBlt(prc, xp, 0, FONT_WIDTH, FONT_HEIGHT, ptx, rop);
			xp += FONT_WIDTH;
			if (xp >= ((prc->x + prc->w) - FONT_WIDTH)) return;
		}
	}
}

//*****************************************************************************
//	Function drawTextEx(i16 x, i16 y, pu8 ptext, u16 rop)
//	Draw text in X/Y position using system font.
//	parameters:
//		x			X start position
//		y			Y start position
//		ptext		Pointer to text
//		rop			Raster operation. See ROP_xxx defines
//*****************************************************************************
void drawTextEx(i16 x, i16 y, pu8 ptext, u16 rop) {

	u16		l, i, pos, xp;
	u8		c;
	pu8		ptx;

	l = strLen(ptext);
	xp = x;
	for (i = 0; i < l; i++) {
		c = *(ptext++);
		if (c >= FONT_OFFSET) {
			pos = (u16) (c - FONT_OFFSET) * FONT_BYTEWIDTH * FONT_HEIGHT;
			ptx = ((pu8) font) + pos;
			bitBlt(NULL, xp, y, FONT_WIDTH, FONT_HEIGHT, ptx, rop);
			xp += FONT_WIDTH;
			if (xp >= VID_PIXELS_X) return;
		}
	}
}
//*****************************************************************************
//	Function drawWindow(PWINDOW pwin)
//	parameters:	pwin		Pointer to windows struct
//*****************************************************************************
void	drawWindow(PWINDOW pwin) {

	i16			i;
	RECT	rc, rt; 

	copyRect(&rc,&pwin->rc);
	if (pwin->style & WINCAPTION) {
		copyRect(&rt,&pwin->rc);
		rt.h = rt.y + 11;
		rt.x += 2;
		rt.y += 1;
		rc.h += 10;
		for (i = 0; i < 11; i++) {
			line(NULL,rc.x, rc.y + i, rc.x + rc.w, rc.y + i, ROP_COPY);
		}
		if (pwin->style & WINCLOSEICON) {
			bitBlt(&rc, rc.w - 9, 1, 10, 9, (pu8) closeBm, ROP_COPY);
			rt.w -= 11;
		} else {
			rt.w -= 1;
		}
		drawText(&rt,pwin->caption,pwin->style & WINCAPTION_MASK, ROP_XOR);
	}
	rectangleEx(&rc,ROP_COPY);
}

//********************************** MATH FUNCTIONS **********************
static const i16 SineTable[360] = {
    0,    174,    348,    523,    697,    871,   1045,   1218,   1391, 
	1564,   1736,   1908,   2079,   2249,   2419,   2588,   2756,   2923, 
	3090,   3255,   3420,   3583,   3746,   3907,   4067,   4226,   4383, 
	4539,   4694,   4848,   4999,   5150,   5299,   5446,   5591,   5735, 
	5877,   6018,   6156,   6293,   6427,   6560,   6691,   6819,   6946, 
	7071,   7193,   7313,   7431,   7547,   7660,   7771,   7880,   7986, 
	8090,   8191,   8290,   8386,   8480,   8571,   8660,   8746,   8829, 
	8910,   8987,   9063,   9135,   9205,   9271,   9335,   9396,   9455, 
	9510,   9563,   9612,   9659,   9702,   9743,   9781,   9816,   9848, 
	9876,   9902,   9925,   9945,   9961,   9975,   9986,   9993,   9998, 
	10000,   9998,   9993,   9986,   9975,   9961,   9945,   9925,   9902, 
	9876,   9848,   9816,   9781,   9743,   9702,   9659,   9612,   9563, 
	9510,   9455,   9396,   9335,   9271,   9205,   9135,   9063,   8987, 
	8910,   8829,   8746,   8660,   8571,   8480,   8386,   8290,   8191, 
	8090,   7986,   7880,   7771,   7660,   7547,   7431,   7313,   7193, 
	7071,   6946,   6819,   6691,   6560,   6427,   6293,   6156,   6018, 
	5877,   5735,   5591,   5446,   5299,   5150,   5000,   4848,   4694, 
	4539,   4383,   4226,   4067,   3907,   3746,   3583,   3420,   3255, 
	3090,   2923,   2756,   2588,   2419,   2249,   2079,   1908,   1736, 
	1564,   1391,   1218,   1045,    871,    697,    523,    348,    174, 
	0,   -174,   -348,   -523,   -697,   -871,  -1045,  -1218,  -1391, 
	-1564,  -1736,  -1908,  -2079,  -2249,  -2419,  -2588,  -2756,  -2923, 
	-3090,  -3255,  -3420,  -3583,  -3746,  -3907,  -4067,  -4226,  -4383, 
	-4539,  -4694,  -4848,  -4999,  -5150,  -5299,  -5446,  -5591,  -5735, 
	-5877,  -6018,  -6156,  -6293,  -6427,  -6560,  -6691,  -6819,  -6946, 
	-7071,  -7193,  -7313,  -7431,  -7547,  -7660,  -7771,  -7880,  -7986, 
	-8090,  -8191,  -8290,  -8386,  -8480,  -8571,  -8660,  -8746,  -8829, 
	-8910,  -8987,  -9063,  -9135,  -9205,  -9271,  -9335,  -9396,  -9455, 
	-9510,  -9563,  -9612,  -9659,  -9702,  -9743,  -9781,  -9816,  -9848, 
	-9876,  -9902,  -9925,  -9945,  -9961,  -9975,  -9986,  -9993,  -9998, 
	-10000,  -9998,  -9993,  -9986,  -9975,  -9961,  -9945,  -9925,  -9902, 
	-9876,  -9848,  -9816,  -9781,  -9743,  -9702,  -9659,  -9612,  -9563, 
	-9510,  -9455,  -9396,  -9335,  -9271,  -9205,  -9135,  -9063,  -8987, 
	-8910,  -8829,  -8746,  -8660,  -8571,  -8480,  -8386,  -8290,  -8191, 
	-8090,  -7986,  -7880,  -7771,  -7660,  -7547,  -7431,  -7313,  -7193, 
	-7071,  -6946,  -6819,  -6691,  -6560,  -6427,  -6293,  -6156,  -6018, 
	-5877,  -5735,  -5591,  -5446,  -5299,  -5150,  -5000,  -4848,  -4694, 
	-4539,  -4383,  -4226,  -4067,  -3907,  -3746,  -3583,  -3420,  -3255, 
	-3090,  -2923,  -2756,  -2588,  -2419,  -2249,  -2079,  -1908,  -1736, 
	-1564,  -1391,  -1218,  -1045,   -871,   -697,   -523,   -348,   -174
}; 

//*****************************************************************************
//	Function sin(u16 angle):  
//	Return sine value from 'angle' parameter in degrees. The returned value
//	is multiplied by 10000 (-10000..0..10000). The angle must be an integer
//	value from 0 to 359.
//*****************************************************************************
i16	sin(u16 angle){
	u16	a = (angle % 359);

	return SineTable[a];
}

//*****************************************************************************
//	Function cos(u16 angle):
//	Return cosine value from 'angle' parameter in degrees. The returned
//	value is multiplied by 10000 (-10000..0..10000). The angle must be an
//	integer value from 0 to 359
//*****************************************************************************
i16	cos(u16 angle){
	u16	a = ((angle + 90) % 359);

	return SineTable[a];
}

//**********************************************************************
// clear a given rectangle: x0, y0 top-left corner, x1,y1 botom-right corner
// NOTE: x0, x1  coordinates must be a multiple of 8.  

void clear_rectangle(u16 x0, u16 y0, u16 x1, u16 y1){
	
	u16 n, m;
	for(n=y0; n<y1; n++){
		for(m=(x0/8); m<(x1/8); m++) fb[n][m] = 0;	
	}	
}
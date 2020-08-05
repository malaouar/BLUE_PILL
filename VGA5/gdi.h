
#ifndef	__GDI_H
#define	__GDI_H



//	Types

typedef	unsigned int	u32;
typedef	int				i32;
typedef	unsigned short	u16;
typedef	short			i16;
typedef	unsigned char	u8;
typedef	char			i8;

typedef	u32				*pu32;
typedef	i32				*pi32;
typedef u16				*pu16;
typedef i16				*pi16;
typedef	u8				*pu8;
typedef i8				*pi8;

#define	NULL			0x0

i16		sin(u16 angle);		// Return sine value from 0 to +-10000
i16		cos(u16 angle);		// Return cosine value from 0 to +-10000

//	System font

#define	FONT_WIDTH		6		// Width in pixels
#define	FONT_HEIGHT		9		// Height in pixels
#define	FONT_BYTEWIDTH	1		// Width in bytes
#define	FONT_OFFSET		0x20

//	RASTER OPERATION

#define	ROP_COPY			0
#define	ROP_XOR				1
#define	ROP_AND				2
#define	ROP_OR				3

typedef  struct{
	i16		x;							// X position
	i16		y;							// Y position
	i16		w;							// Width
	i16		h;							// Height

} RECT, *PRECT;

#define	WINCAPTION			0x0001
#define	WINBORDER			0x0002
#define	WINCLOSEICON		0x0003

//	Text align mode 

#define	WINCAPTION_LEFT		0x0000
#define	WINCAPTION_CENTER	0x0010
#define	WINCAPTION_RIGHT	0x0020
#define	WINCAPTION_MASK		0x0030

typedef struct{
	u16			style;				// Mode, see WINxxx defines
	RECT		rc;					// Absolute rectangle
	pu8			caption;			// Caption text

} WINDOW, *PWINDOW;

typedef  struct{
	i16		w;							// Width in bits
	i16		h;							// Height in bits
	i16		wb;							// width in bytes
	i16		wh;							// Height in bytes
	pu8		*bm;						// Pointer to bitmap bits

} BITMAP, PBITMAP;

//	Function definitions

void	copyRect(PRECT rc1, PRECT rc2);
void	bitBlt(PRECT prc, i16 x, i16 y, i16 w, i16 h, pu8 bm, u16 rop);
void	point(PRECT rc, u16 x, u16 y, u16 rop);
void	line(PRECT prc, i16 x0, i16 y0, i16 x1, i16 y1, u16 rop);
void	rectangle(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop);
void 	rectangleFull(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop);
void	rectangleEx(PRECT rc, u16 rop);
void	circle(u16 x, u16 y, u16 r, u16 rop);
void	drawWindow(PWINDOW pwin);
void	drawTextEx(i16 x, i16 y, pu8 ptext, u16 rop);

#endif	// __H

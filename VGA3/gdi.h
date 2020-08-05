
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

#define	GDI_SYSFONT_WIDTH		6		// Width in pixels
#define	GDI_SYSFONT_HEIGHT		9		// Height in pixels
#define	GDI_SYSFONT_BYTEWIDTH	1		// Width in bytes
#define	GDI_SYSFONT_OFFSET		0x20

//	RASTER OPERATION

#define	GDI_ROP_COPY			0
#define	GDI_ROP_XOR				1
#define	GDI_ROP_AND				2
#define	GDI_ROP_OR				3

typedef  struct{
	i16		x;							// X position
	i16		y;							// Y position
	i16		w;							// Width
	i16		h;							// Height

} GDI_RECT, *PGDI_RECT;

#define	GDI_WINCAPTION			0x0001
#define	GDI_WINBORDER			0x0002
#define	GDI_WINCLOSEICON		0x0003

//	Text align mode 

#define	GDI_WINCAPTION_LEFT		0x0000
#define	GDI_WINCAPTION_CENTER	0x0010
#define	GDI_WINCAPTION_RIGHT	0x0020
#define	GDI_WINCAPTION_MASK		0x0030

typedef struct{
	u16				style;				// Mode, see GDI_WINxxx defines
	GDI_RECT		rc;					// Absolute rectangle
	pu8				caption;			// Caption text

} GDI_WINDOW, *PGDI_WINDOW;

typedef  struct{
	i16		w;							// Width in bits
	i16		h;							// Height in bits
	i16		wb;							// width in bytes
	i16		wh;							// Height in bytes
	pu8		*bm;						// Pointer to bitmap bits

} GDI_BITMAP, PGDI_BITMAP;

//	Function definitions

void	copyRect(PGDI_RECT rc1, PGDI_RECT rc2);
void	bitBlt(PGDI_RECT prc, i16 x, i16 y, i16 w, i16 h, pu8 bm, u16 rop);
void	point(PGDI_RECT rc, u16 x, u16 y, u16 rop);
void	line(PGDI_RECT prc, i16 x0, i16 y0, i16 x1, i16 y1, u16 rop);
void	rectangle(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop);
void 	rectangleFull(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop);
void	rectangleEx(PGDI_RECT rc, u16 rop);
void	circle(u16 x, u16 y, u16 r, u16 rop);
void	drawWindow(PGDI_WINDOW pwin);
void	drawTextEx(i16 x, i16 y, pu8 ptext, u16 rop);

#endif	// __GDI_H

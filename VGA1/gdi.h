

#ifndef	__GDI_H
#define	__GDI_H

#define	NULL			0x0 

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



//	RASTER OPERATION

#define	GDI_ROP_COPY			0
#define	GDI_ROP_XOR				1
#define	GDI_ROP_AND				2
#define	GDI_ROP_OR				3

typedef  struct
{
	uint16_t x;		// X position
	uint16_t y;		// Y position
	uint16_t w;		// Width
	uint16_t h;		// Height
}GDI_RECT, *PGDI_RECT;


//	Function definitions

void	gdiPoint(PGDI_RECT rc, u16 x, u16 y, u16 rop);
void	gdiLine(PGDI_RECT prc, i16 x0, i16 y0, i16 x1, i16 y1, u16 rop);
void	gdiRectangle(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop);
void 	gdiRectangleFull(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop);

#endif	// __GDI_H

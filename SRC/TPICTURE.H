/********************************************************************** 
 * 
 *  NAME:         TPICTURE.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  11.8.95  MHM     added headers 
 * 
 *********************************************************************/ 
 
// TPicture.h

#ifndef TPICTURE_H
#define TPICTURE_H

#ifndef __WINDOWS_H
	#include <windows.h>
#endif

//Class for paletted DIBs
class TPicture {
protected :
    friend class picfile;			// for loading pictures

	BITMAPINFO *Info;
	BYTE huge  *pPicture;
	HGLOBAL		Picture;

	int 		bytewidth;
    BYTE		BitsPerPixel;
	BYTE huge* *Pic_p;

	BITMAPINFO *PaintInfo;

	HPALETTE 	hPal;
	RGBTRIPLE *Entries;

// Methods
	void		DeletePalette();
	void		DestroyAll();

static int 			GetRealWidth    	(int width,int bitcount);
static HPALETTE		CreateUniformPalette(void);

static DWORD		GetPaletteSize		(int nBitCount);

static HPALETTE		GetPaletteFromDib	(BITMAPINFO* Info);
static BITMAPINFO*	CreatePaintInfo		(BITMAPINFO* Info);

public:
    TPicture(void);
	TPicture(int width,int height,int bitcount);
    TPicture(HGLOBAL DIB);

	void Init(void);
	void Init(int width,int height,int bitcount);
    void Init(HGLOBAL DIB);

	static TPicture*	Create(HGLOBAL);
	static TPicture*	Create(int width,int height,int bitcount);

	~TPicture();

	BOOL				IsOK(void);

	POINT 		GetResolution(void);
	inline BYTE	GetBitsPerPixel(void){return BitsPerPixel;};

	HPALETTE	GetPalette(){ return hPal;};
    void		SetPalette(HPALETTE);

	RGBTRIPLE	GetPixel(int x,int y);
	void		SetPixel(int x,int y,RGBTRIPLE&);

	BYTE		GetIndex(int x,int y);
	void		SetIndex(int x,int y,BYTE index);
	BYTE huge  *GetYLine(int y){return Pic_p[y];};

	void		Paint(HDC,RECT&,POINT viewres);

	void		Copy(TPicture*,RECT* DestClip=NULL);// Copy with stretch, not tested
	void		Clear();

static HGLOBAL		AllocateDib     	(int nWidth,int nHeight,int nBitCount);
};

/* Dib functions */

DWORD dwGetDibPaletteSize(BITMAPINFOHEADER *bihInfoHeader);
DWORD dwGetBitmapInfoSize(BITMAPINFOHEADER *bihInfoHeader);
BYTE huge *pGetDibDataPointer(BYTE huge *pDib);

#endif
 
 
/* End of file TPICTURE.H */ 
 

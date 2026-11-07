/********************************************************************** 
 * 
 *  NAME:         VIEWMAP.H 
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
 
// Viewmaps
#ifndef VIEWMAP_H
#define VIEWMAP_H

#ifndef __WINDOWS_H
	#include <windows.h>
#endif

class TPicture;
class Scanline_Screen;

#define Viewmap		0
#define DDBView		1
#define FastMap		2
#define GrayScale	3
#define Floyd3		4
#define Floyd2		5

extern LPSTR ViewmapNames[];

class TViewmap {
protected:
	int		Type;
	TPicture*	Picture;

    BOOL	bClear;
	POINT	ViewRes;
public:
	TViewmap(TPicture* Pic,POINT Res,BOOL Clear=FALSE);
	virtual ~TViewmap();
	static TViewmap* Create(TPicture* Pic,POINT Res,int Type,BOOL Clear = FALSE);

	virtual int			GetType();
	virtual HPALETTE	GetPalette();
	virtual POINT		GetResolution();
	virtual void		Resize(POINT Res);
	virtual void		Paint(HDC hDC,RECT& rcPaint);
	virtual RECT *		Invalidate(RECT*);
	virtual RECT *		InvalidateScanline(Scanline_Screen* sArea);
protected:
	static int Scale(int nSource,int SourceRes,int nDestRes);

};

class TDDBView : public TViewmap{
protected:
	HBITMAP View;
public:
	TDDBView(TPicture* Pic,POINT Res,BOOL Clear=FALSE);
	~TDDBView();

    virtual void Resize(POINT Res);
	virtual void DeleteView();
	virtual void CreateView();
	virtual void Paint(HDC hDC,RECT& rcPaint);
	virtual RECT *InvalidateScanline(Scanline_Screen* sArea);
};

class TFastMap : public TViewmap{
protected:
	TPicture *View;
    HPALETTE hPal;
public:
	TFastMap(TPicture* Pic,POINT Res,BOOL Clear = FALSE);
	~TFastMap();

	virtual HPALETTE GetPalette();
    virtual void Resize(POINT Res);
	virtual void DeleteView();
	virtual void CreateView();
	virtual void Paint(HDC hDC,RECT& rcPaint);
	virtual RECT *InvalidateScanline(Scanline_Screen* sArea);

protected:
	virtual HPALETTE CreatePalette(HPALETTE);

	int *source_x;
	WORD *source_rgb_x;

	POINT source;
	int *dest_x;
    POINT dest;

	virtual void InvalidateLine(int from,int to);
	virtual void InvalidateLine8to8(int from,int to);
	virtual void InvalidateLine24(int from,int to);
	virtual void InvalidateLine24to8(int from,int to);
};

class TGrayScale : public TFastMap {
public:
	TGrayScale(TPicture* Pic,POINT Res,BOOL Clear=FALSE);
	~TGrayScale();

protected:
	virtual void InvalidateLine24to8(int from,int to);
	virtual HPALETTE CreatePalette(HPALETTE);
};

class TFloyd2 : public TFastMap {
public:
	TFloyd2(TPicture* Pic,POINT Res,BOOL Clear=FALSE);
	~TFloyd2();

protected:
	virtual void InvalidateLine24to8(int from,int to);
};

#define TESTMAP_RMAX 8
#define TESTMAP_GMAX 8
#define TESTMAP_BMAX 4

class TFloyd3 : public TFastMap {
public:
	TFloyd3(TPicture* Pic,POINT Res,BOOL Clear=FALSE);
	~TFloyd3();

protected:
	virtual void InvalidateLine24to8(int from,int to);
	virtual HPALETTE CreatePalette(HPALETTE Source);

    static int fStaticsInitialized;
	static BYTE pRToIndex[256];
	static BYTE pGToIndex[256];
	static BYTE pBToIndex[256];

   	static BYTE pIndexToR[TESTMAP_RMAX];
	static BYTE pIndexToG[TESTMAP_GMAX];
	static BYTE pIndexToB[TESTMAP_BMAX];
};

#endif
 
/* End of file VIEWMAP.H */ 
 

/********************************************************************** 
 * 
 *  NAME:         TPICTURE.CPP 
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
 
// TPicture.cpp

#include <mem.h>
#include "tpicture.h"

/////////////////////////////////////////////////////
// TPicture
////////////////////////////////////////////////////

// Constructors
TPicture::TPicture(void)
{
    /* Create NULL TPicture */
	Init();
};

TPicture::TPicture(HGLOBAL Dib)
{
    /* Create TPicture from Dib */
	Init(Dib);
};

TPicture::TPicture(int width,int height,int bitcount)
{
    /* Create new TPicture */
	Init(width,height,bitcount);
};

// Initializers
void TPicture::Init(void)
{
    /* Init all values */
	Picture		= NULL;
	Info		= NULL;
	pPicture	= NULL;
	bytewidth	= 0;
	BitsPerPixel= 0;
    Pic_p		= NULL;
	hPal		= NULL;
    Entries		= NULL;
	PaintInfo	= NULL;
};

void TPicture::Init(HGLOBAL Dib)
{
    Init();
    if(Dib==NULL) return;

    Picture  = Dib;
    BYTE huge* pDib = (BYTE huge*)GlobalLock(Dib);
	pPicture = pGetDibDataPointer(pDib);
	Info 	 = (BITMAPINFO*)pDib;

	PaintInfo = CreatePaintInfo(Info);

	bytewidth = GetRealWidth((int)Info->bmiHeader.biWidth,Info->bmiHeader.biBitCount);
	BitsPerPixel = (BYTE)Info->bmiHeader.biBitCount;

	// Calculate y-line cache
    POINT Res = GetResolution();
    Pic_p = new BYTE huge *[Res.y];
	for(int y = 0 ; y < Res.y ; y++ )
		Pic_p[y]=&pPicture[(DWORD)y*bytewidth];

	// Also 24-Dibs use this as their constructor
	if(Info->bmiHeader.biBitCount==24)
		SetPalette(CreateUniformPalette());
	   else
		SetPalette(GetPaletteFromDib(Info));

	if((hPal==NULL)||(Entries==NULL))
    	DestroyAll();
};

void TPicture::Init(int width,int height,int bitcount)
{
	// Also TPicture24 uses this as its constructor ( TPicture(width,height,24))
    // First create DIB, then let TPicture(DIB) do the work

	HGLOBAL Pic = AllocateDib(width,height,bitcount);

    BITMAPINFO *TInfo = (BITMAPINFO*)GlobalLock(Pic);
 
    /* Fill in the BitmapInfo structure */
	if(TInfo!=NULL){
		TInfo->bmiHeader.biSize				= sizeof(BITMAPINFOHEADER);
		TInfo->bmiHeader.biWidth			= width;
		TInfo->bmiHeader.biHeight			= height;
		TInfo->bmiHeader.biPlanes			= 1;
		TInfo->bmiHeader.biBitCount 		= bitcount;
		TInfo->bmiHeader.biCompression	 	= BI_RGB;
		TInfo->bmiHeader.biSizeImage	 	= 0;
		TInfo->bmiHeader.biClrUsed	 		= 0;
		TInfo->bmiHeader.biClrImportant 	= 0;}

	GlobalUnlock(Pic);

    Init(Pic);
};

// Creators
TPicture* TPicture::Create(HGLOBAL hDIB)
{
	TPicture* Pic = new TPicture(hDIB);

    if(Pic!=NULL)
		if(Pic->IsOK()==FALSE){
			delete Pic;
			return NULL;}
    return Pic;
};

TPicture* TPicture::Create(int width,int height,int bitcount)
{
	TPicture* Pic = new TPicture(width,height,bitcount);

	if(Pic!=NULL)
		if(Pic->IsOK()==FALSE){
			delete Pic;
	        return NULL;}
	return Pic;
};

// Confusing, isn't it?

BOOL  TPicture::IsOK(void){ return(Picture!=NULL);};

void TPicture::Paint(HDC hDC,RECT& rcPaint,POINT ViewRes)
{
	POINT Res = GetResolution();

	HPALETTE hOldPal = SelectPalette (hDC, hPal, TRUE);
    RealizePalette (hDC);

	// Get clipping rectangle for window
	RECT Clip;			CopyRect(&Clip,&rcPaint);
    Clip.left  = (int)(((long)Clip.left  *Res.x)/ViewRes.x);
	Clip.top   = (int)(((long)Clip.top   *Res.y)/ViewRes.y);
	Clip.right = (int)(((long)Clip.right *Res.x)/ViewRes.x);
	Clip.bottom= (int)(((long)Clip.bottom*Res.y)/ViewRes.y);

	Clip.right = Clip.right -Clip.left;
	Clip.bottom = Clip.bottom- Clip.top;

	RECT ViewClip;		CopyRect(&ViewClip,&rcPaint);
	ViewClip.right = ViewClip.right - ViewClip.left;
	ViewClip.bottom= ViewClip.bottom - ViewClip.top;

	SetStretchBltMode (hDC, STRETCH_DELETESCANS);

	StretchDIBits(hDC
			,ViewClip.left					,ViewClip.top
			,ViewClip.right					,ViewClip.bottom
			,Clip.left          			,Res.y-Clip.top-Clip.bottom
			,Clip.right						,Clip.bottom
			,(void*)pPicture	,PaintInfo		,DIB_PAL_COLORS	,SRCCOPY);

	SelectPalette (hDC, hOldPal, TRUE);
};

void TPicture::SetPalette(HPALETTE hPalette)
{
	if(hPal!=NULL)
		DeletePalette();

	hPal=hPalette;

	if(hPal!=NULL){
		PALETTEENTRY *PalEntries = new PALETTEENTRY[256];
		int palsize = GetPaletteEntries(hPal,0,256,PalEntries);
		Entries  = new RGBTRIPLE[palsize];

		for(int i = 0 ; i < palsize ; i++ ){
			Entries[i].rgbtRed  = PalEntries[i].peRed;
			Entries[i].rgbtGreen= PalEntries[i].peGreen;
			Entries[i].rgbtBlue = PalEntries[i].peBlue;
			};
		delete PalEntries;
		};
};

void TPicture::Copy(TPicture* Source,RECT* DClip)
{
	if(Source == NULL) return;

	POINT SourceRes = Source->GetResolution();
    POINT DestRes = GetResolution();

	RECT* DestClip = NULL;
	RECT DefDestClip;

	if(DClip == NULL){
		DestClip = &DefDestClip;
		DestClip->left = 0;
		DestClip->top = 0;
		DestClip->right  = DestRes.x;
		DestClip->bottom = DestRes.y;
		}
	   else
    	DestClip = DClip;

	int *Source_x = new int[DestRes.x];

	int from = DestClip->left;
	int to = DestClip->right;

	for(int x=DestClip->left;x<DestClip->right;x++)
		Source_x[x]= (int)(((long)x*SourceRes.x)/DestRes.x); 

	for( int y=DestClip->top ; y<DestClip->bottom ; y++ ){
		int Source_y = (int)(((long)y*SourceRes.y)/DestRes.y);

		BYTE huge *Source_p = Source->Pic_p[Source_y];
		BYTE huge *Dest_p = Pic_p[y];

		if(GetBitsPerPixel()==24)
			if(Source->GetBitsPerPixel()==24){	// 24 -> 24
				for(x=from;x<to;x++){
					RGBTRIPLE*  To=(RGBTRIPLE*)&Dest_p[x*sizeof(RGBTRIPLE)];
					RGBTRIPLE*From=(RGBTRIPLE*)&Source_p[Source_x[x]*sizeof(RGBTRIPLE)];
					To->rgbtRed   = From->rgbtRed; 
					To->rgbtGreen = From->rgbtGreen;
					To->rgbtBlue  = From->rgbtBlue;
					}
				}
			   else {							// 24 -> paletted
				for(x=from;x<to;x++)
					SetPixel(x,y,Source->GetPixel(Source_x[x],Source_y));
				}
		   else
			if(Source->GetBitsPerPixel()==24){	// paletted -> 24
				for(x=from;x<to;x++){
					*(RGBTRIPLE*)&Dest_p[x*sizeof(RGBTRIPLE)]
					= Source->GetPixel(Source_x[x],Source_y);
					}
				}
		   	   else {
				for(x=from;x<to;x++)
					SetPixel(x,y,Source->GetPixel(Source_x[x],Source_y));
				};
		}

	delete Source_x;
};

// Clears the DIB
void TPicture::Clear()
{
    POINT Res = GetResolution();
	_fmemset(pPicture,bytewidth,Res.y);
};

POINT TPicture::GetResolution(void)
{
	POINT Resolution;
	Resolution.x= (int)Info->bmiHeader.biWidth;
	Resolution.y= (int)Info->bmiHeader.biHeight;
    return Resolution;
};

BYTE TPicture::GetIndex(int x,int y)
{
	BYTE Bits = GetBitsPerPixel();

    // 256-colors
	if(Bits==8)
		return (Pic_p[y])[x];

	int new_x = x;
	if(Bits==4)
		new_x >>= 1;
	   else
		new_x >>= 3;

	BYTE index =(Pic_p[y])[new_x];

    // 16-colors
	if(Bits==4){
		index >>= 4*(1-(x&1));
		index &=15;
		}
	   else {
    // 2-colors
		index >>=   (7-( x&7 ));
		index &= 1;
		}

	return index;
};

// Doesn't work with compressed DIBs(RLEs)
RGBTRIPLE	TPicture::GetPixel(int x,int y)
{
	BYTE Bits = GetBitsPerPixel();

    // 256-colors
	if(Bits==8)
		return Entries[(Pic_p[y])[x]];
			
    // truecolors
	if(Bits==24){
        RGBTRIPLE * color = (RGBTRIPLE*)&(Pic_p[y])[x*sizeof(RGBTRIPLE)];
        return *color;
        };

	int new_x = x;
	if(Bits==4)
		new_x >>= 1;
	   else
		new_x >>= 3;

	BYTE index = (Pic_p[y])[new_x];

    // 16-colors
	if(Bits==4){
		index >>= 4*(1-(x&1));
		index &=15;
		}
	   else {
    // 2-colors
		index >>=   (7-( x&7 ));
		index &= 1;
		}

	return Entries[index];
};

void TPicture::SetPixel(int x,int y,RGBTRIPLE& color)
{
    if(GetBitsPerPixel()==24){
		RGBTRIPLE *C=(RGBTRIPLE *)&(Pic_p[y])[x*sizeof(RGBTRIPLE)];
		C->rgbtRed   = color.rgbtRed;
		C->rgbtGreen = color.rgbtGreen;
		C->rgbtBlue  = color.rgbtBlue;
		};
};

// Doesn't work with compressed DIBs(RLEs)
void TPicture::SetIndex(int x,int y,BYTE index)
{
	// Convert to index to paletted dib
    BYTE Bits=GetBitsPerPixel(); 

    // 256-colors
	if(Bits==8){
		(Pic_p[y])[x] = index;
		return;
		};

	int new_x = x;
	if(Bits==4)// 16 colors 
		new_x >>= 1;
	   else		// 2 colors 
		new_x >>= 3;

	BYTE huge *address=&(Pic_p[y])[new_x];

    // 16-colors
	if(Bits==4){
		index <<= 4*(1-(x&1));
		*address &= (240 << 4*(1-(x&1)));
		*address |= index;
		}
	   else {
    // 2-colors
		index <<= 7-(x&7);
		*address &= (254 << (7-(x&7)));
		*address |=index;
		}
};

TPicture::~TPicture()
{
	DestroyAll();
};

/////////////////////////////////////////7
// Protected area

BITMAPINFO* TPicture::CreatePaintInfo(BITMAPINFO* Info)
{
    int nPalSize=256;

	BITMAPINFO *PaintInfo = (BITMAPINFO*)new
		BYTE[sizeof(BITMAPINFO)+nPalSize*sizeof(WORD)];
	if(PaintInfo == NULL) return NULL;

    // Copy Info --> PaintInfo
	_fmemcpy(PaintInfo,Info,sizeof(BITMAPINFO));

    // Use PaintInfos palette as identity palette  
    WORD *wPalette = (WORD*)PaintInfo->bmiColors;
	for(int i=0;i<nPalSize;i++)
		wPalette[i] = (WORD)i;

	return PaintInfo; 
};

HPALETTE TPicture::GetPaletteFromDib(BITMAPINFO* Info)
{
	// No palettes in truecolor bitmaps
	// (not necesserily true but who cares!)
	if(Info->bmiHeader.biBitCount==24) return NULL;

	int nPalSize;
	if(Info->bmiHeader.biClrUsed==0)
		nPalSize= 1 << Info->bmiHeader.biBitCount;
	   else
		nPalSize = (int)Info->bmiHeader.biClrUsed;

	LOGPALETTE *pPal=(LOGPALETTE*)LocalAlloc(LPTR,
				sizeof(LOGPALETTE)+ nPalSize * sizeof(PALETTEENTRY));
	if(pPal==NULL) return NULL;

	pPal->palNumEntries = nPalSize;
	pPal->palVersion	= 0x300;

	RGBQUAD* Palette = Info->bmiColors;

	for(int i=0;i<nPalSize;i++){				// Copy palette-entries
		pPal->palPalEntry[i].peRed   = Palette[i].rgbRed;    	
		pPal->palPalEntry[i].peGreen = Palette[i].rgbGreen;
		pPal->palPalEntry[i].peBlue  = Palette[i].rgbBlue;
		pPal->palPalEntry[i].peFlags = (BYTE)0;
		}

	HPALETTE hPal = CreatePalette(pPal);

	LocalFree((HLOCAL)pPal);

    return hPal;
};

HPALETTE TPicture::CreateUniformPalette(void)
{
	HPALETTE    hPal;
	LOGPALETTE* pPal;
    int			nPalSize=256; 

	pPal=(LOGPALETTE*)LocalAlloc(LPTR,
			sizeof(LOGPALETTE) + nPalSize * sizeof(PALETTEENTRY));

	if(pPal==NULL) return NULL;

	pPal->palNumEntries = nPalSize;
	pPal->palVersion	= 0x300;

	for(int i=0 ; i<nPalSize ; i++){
		pPal->palPalEntry[i].peRed   = ( (BYTE)i    &224);
		pPal->palPalEntry[i].peGreen = (((BYTE)i<<3)&224);
		pPal->palPalEntry[i].peBlue  = (((BYTE)i<<6)&224);
		pPal->palPalEntry[i].peFlags = (BYTE)0;
		}

    hPal = CreatePalette(pPal);

	LocalFree((HLOCAL)pPal);

    return hPal;
};

void TPicture::DeletePalette()
{
	if(hPal!=NULL){
		DeleteObject(hPal);		// Delete palette
        hPal		= NULL;};
	if(Entries!=NULL){
		delete Entries;
		Entries=NULL;}
};

HGLOBAL TPicture::AllocateDib(int nWidth,int nHeight,int nBitCount)
{                                
	DWORD dwDibSize =
		sizeof(BITMAPINFOHEADER) + GetPaletteSize(nBitCount) * sizeof(RGBQUAD) +
		(DWORD) GetRealWidth(nWidth,nBitCount) * (DWORD)nHeight;

	GlobalCompact(dwDibSize);
	return (HGLOBAL)GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,dwDibSize);
};

DWORD TPicture::GetPaletteSize(int nBitCount)
{
    DWORD dwColors;
	if(nBitCount<=8)
		dwColors = (DWORD)1L << nBitCount;
	   else
		dwColors = 256;
    return dwColors;
};

int  TPicture::GetRealWidth(int width,int bitcount)
{ return ((((width) * bitcount)+ 31)/32)* 4; };

void TPicture::DestroyAll()
{
	if(Picture!=NULL){
        GlobalUnlock(Picture);	// Delete DIB
		GlobalFree((HANDLE)Picture);
		Picture		= NULL;
		Info		= NULL;
		pPicture	= NULL;
		}
	if(PaintInfo!=NULL){
		delete PaintInfo;
		PaintInfo = NULL;
		}
	if(Pic_p!=NULL){
		delete Pic_p;
		Pic_p = NULL;
		}
	DeletePalette();
};

/* Given pointer to packed Dib, calculates the pointer to Dib data */
BYTE huge *pGetDibDataPointer(BYTE huge *pDib)
{
	BITMAPINFO *pbiInfo = (BITMAPINFO *)pDib;

	return &((BYTE huge*)pDib)[dwGetBitmapInfoSize(&pbiInfo->bmiHeader)];
};

/* Get size of BitmapInfo and palette */
DWORD dwGetBitmapInfoSize(BITMAPINFOHEADER *bihInfoHeader)
{
	return (sizeof(BITMAPINFOHEADER)
		+ dwGetDibPaletteSize(bihInfoHeader) * sizeof(RGBQUAD));
};

/* Count how many colors are there in bitmaps palette */
DWORD dwGetDibPaletteSize(BITMAPINFOHEADER *bihInfoHeader)
{
	DWORD dwColors;

	if((bihInfoHeader->biClrUsed == 0) &&(bihInfoHeader->biBitCount<=8))
		dwColors = (DWORD)1L << bihInfoHeader->biBitCount;
	   else
		dwColors = bihInfoHeader->biClrUsed;
	return dwColors;
};

 
 
/* End of file TPICTURE.CPP */ 
 

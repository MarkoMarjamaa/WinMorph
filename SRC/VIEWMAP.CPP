/**********************************************************************
 * 
 *  NAME:         VIEWMAP.CPP 
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
 
// Viewmap
#include "viewmap.h"
#include "tpicture.h"
#include "scanline.h"

LPSTR ViewmapNames[]={// Names of the viewmaps for user selection
	"No Viewmap",
	"Windows DDB",
	"Fast mapping",
	"Gray scale",
	"Floyd-Steinberg",
	NULL };

//////////////////////////////////////////////////////////7
// TViewmap
////////////////////////////////////////////////////////////
TViewmap::TViewmap(TPicture *Pic,POINT Res,BOOL Clear)
{
    Type	= Viewmap;
	Picture = Pic;
	ViewRes = Res;
	bClear	= Clear;
};

TViewmap::~TViewmap(){};

TViewmap* TViewmap::Create(TPicture* Pic,POINT Res,int Type,BOOL Clear)
{
    TViewmap* NewViewmap=NULL;
	switch(Type){
		case Viewmap  :NewViewmap = new TViewmap(Pic,Res,Clear);break;
		case DDBView  :NewViewmap = new TDDBView(Pic,Res,Clear);break;
        case FastMap  :NewViewmap = new TFastMap(Pic,Res,Clear);break;
		case GrayScale:NewViewmap = new TGrayScale(Pic,Res,Clear);break;
		case Floyd3   :NewViewmap = new TFloyd3(Pic,Res,Clear);break;
		};
    return NewViewmap;
};

int		TViewmap::GetType()		{ return Type;};
POINT	TViewmap::GetResolution() { return ViewRes;};
HPALETTE TViewmap::GetPalette() { return Picture->GetPalette();};

void	TViewmap::Resize(POINT Res){	ViewRes = Res;};

void TViewmap::Paint(HDC hDC,RECT& rcPaint)
{
	Picture->Paint(hDC,rcPaint,ViewRes);
};

RECT *TViewmap::Invalidate(RECT* rOrigRect)
{
	RECT rRealRect;

	POINT pOrigRes = Picture->GetResolution();

	if(rOrigRect == NULL){
		rOrigRect = &rRealRect;

		rRealRect.left		= 0;
		rRealRect.right		= pOrigRes.x;
		rRealRect.top		= 0;
		rRealRect.bottom	= pOrigRes.y;
        }

	Scanline_Screen Screen(pOrigRes);

	Screen.DrawRectangle(*rOrigRect);

	return InvalidateScanline(&Screen);
};

RECT *TViewmap::InvalidateScanline(Scanline_Screen* sOrigArea)
{
	static RECT rViewRect;

	POINT pOrigRes = Picture->GetResolution();

	RECT rOrigRect = sOrigArea->GetExtents();

	rViewRect.left   = Scale(rOrigRect.left  , pOrigRes.x, ViewRes.x);
	rViewRect.right  = Scale(rOrigRect.right , pOrigRes.x, ViewRes.x);
	rViewRect.top    = Scale(rOrigRect.top   , pOrigRes.y, ViewRes.y);
	rViewRect.bottom = Scale(rOrigRect.bottom, pOrigRes.y, ViewRes.y);

    return &rViewRect;
};

int TViewmap::Scale(int nSource,int nSourceRes,int nDestRes)
{
	return (int)((((float)nSource + 0.5) / nSourceRes) * nDestRes);
};

/////////////////////////////////////////////////////////
// TDDBView
/////////////////////////////////////////////////////////

TDDBView::TDDBView(TPicture*Pic,POINT Res,BOOL Clear):
	TViewmap(Pic,Res,Clear)
{
	Type = DDBView;
	View = NULL;
};

TDDBView::~TDDBView()
{
	DeleteView();
};

void TDDBView::Resize(POINT Res)
{
	DeleteView();
	ViewRes = Res;
    CreateView();
};

void TDDBView::DeleteView()
{
	if(View!=NULL){
		DeleteObject(View);
		View = NULL;
		};
};

void TDDBView::CreateView()
{
#ifdef DEBUG
	if(View!=NULL){
		MessageBox(NULL,"Viewmap:View should be null",NULL,MB_OK|MB_TASKMODAL);
		DeleteView();}
#endif

	HDC hDC = CreateCompatibleDC(NULL);
	if (hDC == NULL) return;

	int DC_Bpp = GetDeviceCaps(hDC,BITSPIXEL);
	int DC_Planes = GetDeviceCaps(hDC,PLANES);

	View = CreateBitmap(ViewRes.x,ViewRes.y,DC_Planes,DC_Bpp,NULL);
	
	DeleteDC(hDC);
	
	if(bClear){
		HDC hDC = CreateCompatibleDC(NULL);
		if(hDC == NULL)
        	return;

		HBITMAP OldBitmap = (HBITMAP)SelectObject(hDC,View);
		BitBlt(hDC,0,0,ViewRes.x,ViewRes.y,NULL,0,0,BLACKNESS);
		SelectObject(hDC,OldBitmap);

		DeleteDC(hDC);

		bClear = FALSE;
		}
	   else
		Invalidate(NULL);// Paint the DDB
};

void TDDBView::Paint(HDC hDC,RECT& rcPaint)
{
    if(View==NULL) CreateView();

	HDC MemDC = CreateCompatibleDC (hDC);
    if (MemDC==NULL)	return;

	RECT ViewClip;		CopyRect(&ViewClip,&rcPaint);
	ViewClip.right = ViewClip.right - ViewClip.left;
	ViewClip.bottom= ViewClip.bottom - ViewClip.top;

	HPALETTE hOldPal1 = SelectPalette (MemDC, GetPalette(), TRUE);
	HPALETTE hOldPal2 = SelectPalette (hDC  , GetPalette(), TRUE);
	RealizePalette(hDC);

	HBITMAP hOldBitmap = (HBITMAP)SelectObject (MemDC, View);
	 BitBlt (hDC
			,ViewClip.left	,ViewClip.top	,ViewClip.right	,ViewClip.bottom
			,MemDC			,ViewClip.left	,ViewClip.top	,SRCCOPY);
	SelectObject (MemDC, hOldBitmap);
   
	if(hOldPal1)	SelectPalette(MemDC, hOldPal1, TRUE);
	if(hOldPal2)	SelectPalette(  hDC, hOldPal2, TRUE);
	DeleteDC (MemDC);
};

RECT* TDDBView::InvalidateScanline(Scanline_Screen* sOrigArea)
{
	static RECT rViewRect;

	if(View==NULL)
		CreateView();

	HDC hDC = CreateCompatibleDC(NULL);
	if (hDC == NULL)
		return NULL;

	HBITMAP OldBitmap = (HBITMAP)SelectObject(hDC,View);
	HPALETTE hOldPal  = SelectPalette (hDC, GetPalette(), TRUE);
	RealizePalette (hDC);

	POINT pOrigRes = Picture->GetResolution();

	RECT rOrigRect = sOrigArea->GetExtents();

	rViewRect.left   = Scale(rOrigRect.left  , pOrigRes.x, ViewRes.x);
	rViewRect.right  = Scale(rOrigRect.right , pOrigRes.x, ViewRes.x);
	rViewRect.top    = Scale(rOrigRect.top   , pOrigRes.y, ViewRes.y);
	rViewRect.bottom = Scale(rOrigRect.bottom, pOrigRes.y, ViewRes.y);

	Picture->Paint(hDC,rViewRect,ViewRes);

	SelectPalette (hDC, hOldPal, TRUE);
	SelectObject(hDC,OldBitmap);

	DeleteDC(hDC);

    return &rViewRect;
};

			   
////////////////////////////////////////////////////////7
// TFastMap
/////////////////////////////////////////////////////////
#include "scanline.h"

TFastMap::TFastMap(TPicture*Pic,POINT Res,BOOL Clear):
	TViewmap(Pic,Res,Clear)
{
	Type = FastMap;
	View = NULL;

	source_x = NULL;
	source_rgb_x = NULL;

	dest_x = NULL;
};

TFastMap::~TFastMap()
{
	DeleteView();
};

void TFastMap::Resize(POINT Res)
{
	DeleteView();
	ViewRes = Res;
    CreateView();
};

void TFastMap::DeleteView()
{
	if(View!=NULL){
		delete View;
		View = NULL;
		};

	if(source_x!=NULL){
		delete source_x;
		source_x = NULL;
		}

	if(source_rgb_x!=NULL){
		delete source_rgb_x;
		source_rgb_x = NULL;
		}

	if(dest_x!=NULL){
		delete dest_x;
		dest_x = NULL;
		}
};

void TFastMap::CreateView()
{
#ifdef DEBUG
	if(View!=NULL){
		MessageBox(NULL,"Viewmap:View should be null",NULL,MB_OK|MB_TASKMODAL);
		DeleteView();}
#endif

	// Create scaling tables
	POINT pOrigRes = Picture->GetResolution();

	// Scale table View --> Orig
	source_x = new int[ViewRes.x+10];
	for(int x = 0 ; x < (ViewRes.x+10) ; x++ ){
		source_x[x] = Scale(x,ViewRes.x,pOrigRes.x);
		if(source_x[x]>=pOrigRes.x)
			source_x[x] = pOrigRes.x - 1;
		}

    // Scale table View --> Orig RGB 
    // Multiply scale table View --> Orig with sizeof(RGBTRIPLE)
	source_rgb_x = new WORD[ViewRes.x+10];
	for(x = 0 ; x < (ViewRes.x+10) ; x++ )
		source_rgb_x[x] = source_x[x] * sizeof(RGBTRIPLE);

	// Scale table Orig --> View
	dest_x = new int[pOrigRes.x+10];
	for(x = 0 ; x < (pOrigRes.x+10) ; x++ ){
        dest_x[x] = Scale(x,pOrigRes.x,ViewRes.x);
//		dest_x[x] = (int)( (x * (long)ViewRes.x + (pOrigRes.x>>1) ) / pOrigRes.x );
		if(dest_x[x]>=ViewRes.x)
			dest_x[x] = ViewRes.x - 1;
		}

	HDC hDC = CreateCompatibleDC(NULL);
	int Bits = GetDeviceCaps(hDC,BITSPIXEL) * GetDeviceCaps(hDC,PLANES);
	if(Bits<=8)
		Bits=8;
	   else
	   	Bits=24;
	DeleteDC(hDC);

	// Create the actual viewmap
    View = TPicture::Create(ViewRes.x,ViewRes.y,Bits);

	View->SetPalette(CreatePalette(Picture->GetPalette()));

	// Invalidate if needed
	if(!bClear)
		Invalidate(NULL);
    bClear = FALSE;
};

HPALETTE TFastMap::CreatePalette(HPALETTE Source)
{
	// Set palette stuff
	int nPalSize = 256;

	LOGPALETTE* pPal=(LOGPALETTE*)LocalAlloc(LPTR,
			sizeof(LOGPALETTE) + nPalSize * sizeof(PALETTEENTRY));

	GetPaletteEntries(Source,0,nPalSize,pPal->palPalEntry);
	pPal->palNumEntries = nPalSize;
	pPal->palVersion = 0x300;

	HPALETTE hPalette = ::CreatePalette(pPal);

	LocalFree((HLOCAL)pPal);

	return hPalette;
};

void TFastMap::Paint(HDC hDC,RECT& rcPaint)
{
	if(View==NULL)
		CreateView();

    View->Paint(hDC,rcPaint,ViewRes);
};

HPALETTE TFastMap::GetPalette()
{
	if(View==NULL)
		return Picture->GetPalette();
	return View->GetPalette();
};

RECT *TFastMap::InvalidateScanline(Scanline_Screen* sOrigArea)
{
	static RECT rViewRect;

	if(View==NULL)
		CreateView();

	POINT pOrigRes = Picture->GetResolution();

	RECT rOrigRect = sOrigArea->GetExtents();

	rViewRect.left   = Scale(rOrigRect.left  , pOrigRes.x, ViewRes.x);
	rViewRect.right  = Scale(rOrigRect.right , pOrigRes.x, ViewRes.x);
	rViewRect.top    = Scale(rOrigRect.top   , pOrigRes.y, ViewRes.y);
	rViewRect.bottom = Scale(rOrigRect.bottom, pOrigRes.y, ViewRes.y);

	if(rViewRect.top<0)
		rViewRect.top = 0;
	if(rViewRect.bottom>=ViewRes.y)
		rViewRect.bottom = ViewRes.y - 1;

	for( int y = rViewRect.top ; y < rViewRect.bottom ; y++ ){
        dest.y = ViewRes.y - 1 - y;
        source.y = Scale(dest.y,ViewRes.y,pOrigRes.y);
		//source.y = (int)((dest.y * (long)pOrigRes.y) / ViewRes.y );

		int nLine = pOrigRes.y - 1 - source.y;
        if((nLine>=0)&&(nLine<pOrigRes.y)){
			POINT pLine = sOrigArea->Get_Scanline(nLine);
			if(pLine.y>-1)
 				InvalidateLine( dest_x[pLine.x] , dest_x[pLine.y]);
			}			
		}

	return &rViewRect;
};

void TFastMap::InvalidateLine(int from,int to)
{
	if(View->GetBitsPerPixel()==24)
        InvalidateLine24(from,to);
	   else
		if(Picture->GetBitsPerPixel()!=24)
			InvalidateLine8to8(from,to);
		   else
           	InvalidateLine24to8(from,to);
};

void TFastMap::InvalidateLine8to8(int from,int to)
{
	BYTE huge *Pic_p = Picture->GetYLine(source.y);
	BYTE huge*View_p = View   ->GetYLine(dest.y);
	int *Source_x = source_x;

	// We know that the View has 8 bits per pixel
	if(Picture->GetBitsPerPixel()==8)// Special case for speed
		for(int x = from ; x < to ; x++ )
			View_p[x] = Pic_p[Source_x[x]];
	else
		for(int x = from ; x < to ; x++ )
			View_p[x] = Picture->GetIndex(source_x[x],source.y);

};

void TFastMap::InvalidateLine24(int from,int to)
{
	BYTE huge*View_p = View->GetYLine(dest.y);
	TPicture *Pic = Picture;
	int    y  = source.y;
	int  *s_x = source_x;

	for(int x = from ; x < to ; x++ ){
		*(RGBTRIPLE*)&View_p[x*sizeof(RGBTRIPLE)]
			= Pic->GetPixel(s_x[x],y);
		}
};

void TFastMap::InvalidateLine24to8(int from,int to)
{
	BYTE huge * Pic_p = Picture->GetYLine(source.y);
	BYTE huge *View_p = View   ->GetYLine(dest.y);

	WORD *Source_RGB_x = source_rgb_x;

	for(int x = from ; x < to ; x++ ){
		RGBTRIPLE *From = (RGBTRIPLE*)&(Pic_p[Source_RGB_x[x]]);

        BYTE Index = (From->rgbtRed&224) +
				    ((From->rgbtGreen&224)>>3)+
				     (From->rgbtBlue>>6);
		View_p[x] = Index;
		}
};

/////////////////////////////////////////////////////////////////
// Gray scale
/////////////////////////////////////////////////////////////////

TGrayScale::TGrayScale(TPicture*Pic,POINT Res,BOOL Clear):
	TFastMap(Pic,Res,Clear)
{
	Type = GrayScale;
};

TGrayScale::~TGrayScale()
{
};

void TGrayScale::InvalidateLine24to8(int from,int to)
{
	BYTE huge  *Pic_p = Picture->GetYLine(source.y);
	BYTE huge *View_p = View   ->GetYLine(dest.y);
	WORD *Source_RGB_x = source_rgb_x;

	for(int x = from ; x < to ; x++ ){
		RGBTRIPLE *From = (RGBTRIPLE*)&Pic_p[Source_RGB_x[x]];
		BYTE Index = (From->rgbtRed  >>1) +
				     (From->rgbtGreen>>2) +
				     (From->rgbtBlue >>2) ;
		View_p[x] = Index;
		}
};

HPALETTE TGrayScale::CreatePalette(HPALETTE Source)
{
	int nPalSize = 256;

	LOGPALETTE *pPal = (LOGPALETTE*)LocalAlloc(LPTR,
			sizeof(LOGPALETTE) + nPalSize * sizeof(PALETTEENTRY));

	pPal->palNumEntries = nPalSize;
	pPal->palVersion	= 0x300;

	PALETTEENTRY* Entry = pPal->palPalEntry;

	if(Picture->GetBitsPerPixel()<=8){
		GetPaletteEntries(Source,0,nPalSize,pPal->palPalEntry);
		for(int i = 0 ; i < nPalSize ; i++ ){
			BYTE Gray = (BYTE)(	(Entry[i].peRed  >>1) +
								(Entry[i].peGreen>>2) +
								(Entry[i].peBlue >>2) );
			Entry[i].peRed   = Gray;
			Entry[i].peGreen = Gray;
			Entry[i].peBlue  = Gray;
			Entry[i].peFlags = NULL;
			}
		}
       else {
		for(int i = 0 ; i < nPalSize ; i++ ){
			Entry[i].peRed   = i;
			Entry[i].peGreen = i;
			Entry[i].peBlue  = i;
			Entry[i].peFlags = NULL;
			}
		}

    HPALETTE hPalette = ::CreatePalette(pPal);

    LocalFree((HLOCAL)pPal);

	return hPalette;
};

//////////////////////////////////////////////////////////7
// Floyd-Steinberg dithering
///////////////////////////////////////////////////////////7

TFloyd2::TFloyd2(TPicture*Pic,POINT Res,BOOL Clear):
	TFastMap(Pic,Res,Clear)
{
	Type = Floyd2;
};

TFloyd2::~TFloyd2(){};

void TFloyd2::InvalidateLine24to8(int from,int to)
{
	static BYTE Red  = 0;
	static BYTE Green= 0;
    static BYTE Blue = 0;
	
    // Calculate in advance
    BYTE huge  *Pic_p = Picture->GetYLine(source.y);
	BYTE huge *View_p = View   ->GetYLine(dest.y);
	WORD *Source_RGB_x = source_rgb_x;

	BYTE Index; 
	for(int x = from ; x < to ; x++ ){
		RGBTRIPLE *Color = (RGBTRIPLE*)&Pic_p[Source_RGB_x[x]];

asm{les bx,DWORD PTR Color			// Load Color pointer to bx
	mov al,ss:BYTE PTR Red			//Red   += Color.rgbtRed;
	add al,es:[bx+2]
	jnc R1							//if(Red  >255) Red  = 255;
	mov al,255}
R1:asm{mov dl,al					//Index = (Red&224);
	sal dx,3
	and al,31						//Red  &= 31;
   	mov ss:BYTE PTR Red,al
	mov al,ss:BYTE PTR Green		//Green += Color.rgbtGreen;
	add al,es:[bx+1]
	jnc G1							//if(Green>255) Green= 255;
	mov al,255}
G1:asm{mov dl,al					//Index += ((Green&224)>>3);
	sal dx,3
	and al,31						//Green &= 31;
	mov ss:BYTE PTR Green,al}
asm{mov al,ss:BYTE PTR Blue			//Blue  += Color.rgbtBlue;
	add al,es:[bx+0]
	jnc B1							//if(Blue >255) Blue = 255;
	mov al,255}
B1:asm{mov dl,al					//Index +=((Blue&192)>>6);
	sal dx,2
	mov BYTE PTR Index,dh   		//Save index
	and al,63						//Blue  &= 63;
	mov ss:BYTE PTR Blue,al}

	View_p[x]=Index;
	}
}; 
 
//////////////////////////////////////////////////////////7
// TFloyd3
///////////////////////////////////////////////////////////7

#define TESTMAP_RMAX 8
#define TESTMAP_GMAX 8
#define TESTMAP_BMAX 4

int TFloyd3::fStaticsInitialized = FALSE;
BYTE TFloyd3::pRToIndex[256];
BYTE TFloyd3::pGToIndex[256];
BYTE TFloyd3::pBToIndex[256];

BYTE TFloyd3::pIndexToR[TESTMAP_RMAX];
BYTE TFloyd3::pIndexToG[TESTMAP_GMAX];
BYTE TFloyd3::pIndexToB[TESTMAP_BMAX];

TFloyd3::TFloyd3(TPicture*Pic,POINT Res,BOOL Clear):
	TFastMap(Pic,Res,Clear)
{
	Type = Floyd3;

	if(!TFloyd3::fStaticsInitialized){
		for(int iColor = 0 ; iColor < 128 ; iColor ++ ){
			TFloyd3::pRToIndex[iColor] = (BYTE)((iColor * (TESTMAP_RMAX -1 )) / 127);
			TFloyd3::pGToIndex[iColor] = (BYTE)((iColor * (TESTMAP_GMAX -1 )) / 127);
			TFloyd3::pBToIndex[iColor] = (BYTE)((iColor * (TESTMAP_BMAX -1 )) / 127);
		};

		for( iColor = 128 ; iColor < 256 ; iColor ++ ){
			TFloyd3::pRToIndex[iColor] = (BYTE)(TESTMAP_RMAX - 1);
			TFloyd3::pGToIndex[iColor] = (BYTE)(TESTMAP_GMAX - 1);
			TFloyd3::pBToIndex[iColor] = (BYTE)(TESTMAP_BMAX - 1);
		};

		for(int iIndex = 0 ; iIndex < TESTMAP_RMAX ; iIndex ++ )
			TFloyd3::pIndexToR[iIndex] = (BYTE)((iIndex * 127 )/(TESTMAP_RMAX - 1 ));

		for( iIndex = 0 ; iIndex < TESTMAP_GMAX ; iIndex ++ )
			TFloyd3::pIndexToG[iIndex] = (BYTE)((iIndex * 127 )/(TESTMAP_GMAX - 1 ));

		for( iIndex = 0 ; iIndex < TESTMAP_BMAX ; iIndex ++ )
			TFloyd3::pIndexToB[iIndex] = (BYTE)((iIndex * 127 )/(TESTMAP_BMAX - 1 ));

		TFloyd3::fStaticsInitialized = TRUE;
    };
};

TFloyd3::~TFloyd3(){};

void TFloyd3::InvalidateLine24to8(int from,int to)
{
	static BYTE Red  = 0;
	static BYTE Green= 0;
    static BYTE Blue = 0;
	
    // Calculate in advance
    BYTE huge  *Pic_p = Picture->GetYLine(source.y);
	BYTE huge *View_p = View   ->GetYLine(dest.y);
	WORD *Source_RGB_x = source_rgb_x;

	for(int x = from ; x < to ; x++ ){
		RGBTRIPLE *Color = (RGBTRIPLE*)&Pic_p[Source_RGB_x[x]];
		WORD bGreenIndex,bBlueIndex;
		WORD bRedIndex;
		BYTE Index;

		Red  += (Color->rgbtRed>>1);
		Green+= (Color->rgbtGreen>>1);
		Blue += (Color->rgbtBlue>>1);

		bRedIndex = TFloyd3::pRToIndex[Red];
		Red -= TFloyd3::pIndexToR[bRedIndex];
		Index = ((BYTE)bRedIndex)<<5;

        /* Green can use the same maps than Red */
		bGreenIndex = TFloyd3::pGToIndex[Green];
		Green -= TFloyd3::pIndexToG[bGreenIndex];
		Index += ((BYTE)bGreenIndex)<<2;

		bBlueIndex = TFloyd3::pBToIndex[Blue];
		Blue -= TFloyd3::pIndexToB[bBlueIndex];
		Index += (BYTE)bBlueIndex;

		View_p[x] = Index;
	}
};

HPALETTE TFloyd3::CreatePalette(HPALETTE Source)
{
	int nPalSize = 256;

	LOGPALETTE *pPal = (LOGPALETTE*)LocalAlloc(LPTR,
			sizeof(LOGPALETTE) + nPalSize * sizeof(PALETTEENTRY));

	pPal->palNumEntries = nPalSize;
	pPal->palVersion	= 0x300;

	PALETTEENTRY* Entry = pPal->palPalEntry;

	if(Picture->GetBitsPerPixel()<=8){
		GetPaletteEntries(Source,0,nPalSize,pPal->palPalEntry);
		}
       else {
		for(int i = 0 ; i < nPalSize ; i++ ){
			Entry[i].peRed   = (((i/TESTMAP_BMAX)/TESTMAP_GMAX)	* 255 ) / (TESTMAP_RMAX -1);
			Entry[i].peGreen = (((i/TESTMAP_BMAX)%TESTMAP_GMAX)	* 255 ) / (TESTMAP_GMAX -1);
			Entry[i].peBlue  = (( i              %TESTMAP_BMAX)	* 255 ) / (TESTMAP_BMAX -1);
			Entry[i].peFlags = NULL;
			}
		}

    HPALETTE hPalette = ::CreatePalette(pPal);

    LocalFree((HLOCAL)pPal);

	return hPalette;
};

/* End of file VIEWMAP.CPP */
 

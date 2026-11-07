/********************************************************************** 
 * 
 *  NAME:         SCANLINE.CPP 
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
 
// Scanline.cpp
#include "scanline.h"
#include "common.h"

extern HINSTANCE hInstance;

////////////////////////////////////////////////////
// Scanline_Screen
////////////////////////////////////////////////////7

Scanline_Screen::Scanline_Screen(POINT res)
{
	lpLineFunc = (LINEDDAPROC)MakeProcInstance(
		(FARPROC) Scanline_Screen::LineFunc, hInstance );

	Res = res;

	Scanline = new POINT[Res.y];

    Clear();
};

void Scanline_Screen::Clear(void)
{
	// Reset scanlines
	for(int y = 0 ; y < Res.y ; y++ ){
		Scanline[y].x = Res.x;
		Scanline[y].y = -1;
		};

    Extents.left   = Res.x;
	Extents.right  = -1;
	Extents.top    = Res.y;
	Extents.bottom = -1;
};

POINT Scanline_Screen::Get_Scanline(int y){	return Scanline[y];};

RECT  Scanline_Screen::GetExtents(){ return Extents;};

Scanline_Screen::~Scanline_Screen()
{
	FreeProcInstance((FARPROC)lpLineFunc );

	delete Scanline;
};

void FAR PASCAL Scanline_Screen::LineFunc( int X, int Y, LPSTR lpData )
{
	((Scanline_Screen*)lpData)->DrawPoint(X,Y);
};


void Scanline_Screen::DrawPoint(int X,int Y)
{
	if(Y<0)			return;
    if(Y>=Res.y)	return;

	Extents.top    = min(Extents.top,Y);
    Extents.bottom = max(Extents.bottom,Y+1);

	POINT* pScanline = &Scanline[Y];

	if(X < 0)		X = 0;
    if(X > Res.x)	X = Res.x;

	if (X<pScanline->x){
		pScanline->x = X;
        Extents.left = min(Extents.left,pScanline->x);
		};

	if ((X+1)>pScanline->y){
		pScanline->y  = X + 1;
		Extents.right = max(Extents.right,pScanline->y);
        };
};

void Scanline_Screen::DrawLine(POINT pStart,POINT pEnd)
{
	DrawLine(pStart.x,pStart.y,pEnd.x,pEnd.y);	
};

void Scanline_Screen::DrawLine(int x1,int y1,int x2,int y2)
{
	LineDDA(x1,y1,x2,y2,lpLineFunc,(LPARAM)this);
};


void Scanline_Screen::DrawRectangle(RECT rRect)
{
	//Draw rectangle
	DrawLine(rRect.left   ,rRect.top,rRect.left   ,rRect.bottom-1);
	DrawLine(rRect.right-1,rRect.top,rRect.right-1,rRect.bottom-1);
};

void Scanline_Screen::DrawTriangle(POINT p1,POINT p2,POINT p3)
{
	// Draw triangle
	DrawLine(p1,p2);
	DrawLine(p2,p3);
	DrawLine(p3,p1);
};

 
 
/* End of file SCANLINE.CPP */ 
 

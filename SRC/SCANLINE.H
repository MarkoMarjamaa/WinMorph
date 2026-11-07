/********************************************************************** 
 * 
 *  NAME:         SCANLINE.H 
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
 
#ifndef SCANLINE_H
#define SCANLINE_H

#include "common.h"
#ifndef __WINDOWS_H
	#include <windows.h>
#endif

//////////////////////////////////////////////////////
// Scanline_screen
// class for getting the scanlines for triangles
///////////////////////////////////////////////////////

class Scanline_Screen
{
	POINT Res;
	POINT* Scanline;
    RECT Extents;
	LINEDDAPROC		 lpLineFunc;
	static void FAR PASCAL LineFunc( int X, int Y, LPSTR lpData );

public:
	Scanline_Screen			(POINT Res);
	~Scanline_Screen		();

    void	Clear			(void);

	void	DrawPoint		(int x,int y);
	void	DrawLine		(POINT pStart,POINT pEnd);
	void	DrawLine		(int x1,int y1,int x2,int y2);
	void	DrawRectangle	(RECT rRect);
	void	DrawTriangle	(POINT p1,POINT p2,POINT p3);
	POINT	Get_Scanline	(int y);
	RECT	GetExtents		(void);
};

#endif 
 
/* End of file SCANLINE.H */ 
 

/**********************************************************************
 * 
 *  NAME:         ODBUTTON.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          29.02.96 MHM created
 *********************************************************************/

#include "odbutton.h"

TODButton::TODButton(PTWindowsObject pParent, int nResourceId,
	LPSTR szTitle, int x, int y, int w, int h,BOOL IsDefault,
	 LPSTR ResId1, LPSTR ResId2, LPSTR ResId3,LPSTR ResId4)
	:	TButton(pParent, nResourceId, szTitle, x, y, w, h,IsDefault)
{
	bmBitmap[ODBUTTON_NORMAL  ] = NULL;
	bmBitmap[ODBUTTON_PRESSED ] = NULL;
	bmBitmap[ODBUTTON_FOCUSED ] = NULL;
	bmBitmap[ODBUTTON_DISABLED] = NULL;

	SetButton(ODBUTTON_NORMAL   , ResId1);
	SetButton(ODBUTTON_PRESSED  , ResId2);
	SetButton(ODBUTTON_FOCUSED  , ResId3);
	SetButton(ODBUTTON_DISABLED , ResId4);

	Attr.Style |= BS_OWNERDRAW;
}

TODButton::~TODButton()
{
	// Delete the buttons.
	if(bmBitmap[ODBUTTON_NORMAL ])
		DeleteObject(bmBitmap[ODBUTTON_NORMAL ]);
	if(bmBitmap[ODBUTTON_PRESSED])
		DeleteObject(bmBitmap[ODBUTTON_PRESSED]);
	if(bmBitmap[ODBUTTON_FOCUSED])
		DeleteObject(bmBitmap[ODBUTTON_FOCUSED]);
	if(bmBitmap[ODBUTTON_DISABLED])
		DeleteObject(bmBitmap[ODBUTTON_DISABLED]);
}

int TODButton::GetState(DRAWITEMSTRUCT far &DrawInfo)
{
    int nState;
    // If button is disabled
	if(DrawInfo.itemState & ODS_DISABLED)
		nState = ODBUTTON_DISABLED;
	else
	if(DrawInfo.itemState & ODS_SELECTED)
		nState = ODBUTTON_PRESSED;
	else
	if(DrawInfo.itemState & ODS_FOCUS)
		nState = ODBUTTON_FOCUSED;
	else
		nState = ODBUTTON_NORMAL;

    return nState; 
};

void TODButton::ODADrawEntire(DRAWITEMSTRUCT far &DrawInfo)
{
	DrawButton(DrawInfo);
}

void TODButton::ODASelect(DRAWITEMSTRUCT far &DrawInfo)
{
	DrawButton(DrawInfo);
}

void TODButton::ODAFocus(DRAWITEMSTRUCT far &DrawInfo)
{
	DrawButton(DrawInfo);
}

void TODButton::DrawButton(DRAWITEMSTRUCT far &DrawInfo)
{
    HBITMAP hBitmap = bmBitmap[GetState(DrawInfo)]; 

	// Get device contexts for the button and the bitmap.
	HDC     hMemDC = CreateCompatibleDC(DrawInfo.hDC);

	// Select the bitmap into the memory DC.
	HBITMAP	hOldBitmap = (HBITMAP)SelectObject(hMemDC,hBitmap);

	BITMAP	bmBitmap;
	GetObject(hBitmap, sizeof(bmBitmap), (LPSTR) &bmBitmap);

	// Draw the button.
/*	BitBlt(DrawInfo.hDC,
		DrawInfo.rcItem.left, 	DrawInfo.rcItem.top,
		bmBitmap.bmWidth, 		bmBitmap.bmHeight,
		hMemDC, 0, 0,SRCCOPY);                    */

	StretchBlt(DrawInfo.hDC,
		DrawInfo.rcItem.left, 	DrawInfo.rcItem.top,
		DrawInfo.rcItem.right - DrawInfo.rcItem.left,
		DrawInfo.rcItem.bottom - DrawInfo.rcItem.top,
		hMemDC,
		0, 0,
		bmBitmap.bmWidth, 		bmBitmap.bmHeight,
		SRCCOPY);

	// Clean off
	SelectObject(DrawInfo.hDC, hOldBitmap);
	DeleteDC(hMemDC);
};

void TODButton::SetButton(int nButton,LPSTR ResId)
{
	// Get a handle to the application's instance.
	HINSTANCE hInstance = GetApplication()->hInstance;

    // If old bitmap exists
	if(bmBitmap[nButton])
        // delete it 
		DeleteObject(bmBitmap[nButton]);

    // load new bitmap
	bmBitmap[nButton] = LoadBitmap(hInstance, ResId);

	if(HWindow!=NULL)
    	InvalidateRect(HWindow,NULL,FALSE);
};

/* End of file ODBUTTON.CPP */
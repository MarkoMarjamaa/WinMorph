/**********************************************************************
 * 
 *  NAME:         PICTUREW.CPP 
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
 
#include "picturew.h"   // main class
#include "tpicture.h"   // class TPicture
#include "getint.h"     // class GetPoint
#include "pictoptw.h"	// class TPictureOptionsWnd
#include "viewmap.h"
#include "filefunc.h"
#include "morphcfg.h"
#include "morphwin.h"
																		
TPictureWindow::TPictureWindow(PTWindowsObject AParent,LPSTR Name)
  : TWindow(AParent,Name)
{
	Attr.Style      = WS_OVERLAPPEDWINDOW |  WS_CHILD | WS_VISIBLE;
	Attr.Style     |= WS_HSCROLL | WS_VSCROLL;
	WindowTitle     = Name;

	Scroller        = new TScroller(this, 1, 1, 0, 0);
	Scroller->XLine    = 8;
	Scroller->YLine   = 8;

	Dib = NULL;
	Owns_DIB = FALSE;

	bStretchDibToWindow     = TRUE;
	bAutoUpdateViewmaps     = FALSE;

	ViewMap = NULL;
	ViewMapType = Floyd3;
	ViewRes.x = 100;
	ViewRes.y = 100;
	bClear = FALSE;
};

TPictureWindow::~TPictureWindow()
{
	DeleteBitmap();
};

void TPictureWindow::LoadCfg(wmfstream& file)
{
	 do {
		// Read block start
		if(!GetStartOfBlock(file))         break;

		while(!file.eof()){
			char szKeyWord[80];
			//Read keyword
			ReadKeyword(file,szKeyWord,sizeof(szKeyWord));

            // Check for end of block 
			LPSTR szNullKeyWord[] = {NULL};
			int nKeyword = IdentifyKeyword(szNullKeyWord,szKeyWord);

			if(nKeyword == ENDOFBLOCK)    break;

            // Extract label separator 
			if(GetCharacter(file,'=')==FALSE){
				file.vErrorMessage("Can't find label separator");
				// Couldn't find '=', the label separator
				nKeyword = UNDEFINED;
		 	};

			// Process keywords
			nKeyword = LoadCfgKeyword(file,szKeyWord);

			// If keyword is not identified
			if(nKeyword == UNDEFINED){
				// show an error message
				file.vErrorMessage("Can't identify keyword");
				};
			};
		// No errors encountered 
		}
	while(FALSE);

	InvalidateRect(HWindow,NULL,FALSE);
};

int TPictureWindow::LoadCfgKeyword(wmfstream& file,LPSTR szKeyWord)
{
	LPSTR szKeyWords[] = {"StretchDib",NULL};

	// and identify it
	int nKeyword = IdentifyKeyword(szKeyWords,szKeyWord);

    // Process keyword
	switch(nKeyword){
	case 0:/* StretchWindow */
		switch(IdentifyKeyword(file,szOnOffValues)){
			case ON  : SetStretchToWindow(TRUE)  ; break;
			case OFF : SetStretchToWindow(FALSE) ; break;
			default:break; // Error
				};
		break;

	default:// Error
    	break;
	};

	// Return results
	return nKeyword;
};

void TPictureWindow::SaveCfg(wmfstream& file)
{
	 // Save start of block
	file << " (\n";

    // Save keywords
    SaveCfgKeyword(file);

	// Save end of block
	file << "\t)\n";
};

void TPictureWindow::SaveCfgKeyword(wmfstream& file)
{
	// Save ShowTriangles
	file << "\tStretchDib"  << " = ";
	if(bStretchDibToWindow==TRUE)
		file << szOnOffValues[ON];
		else
		file << szOnOffValues[OFF];
	file << "\n";
};


LPSTR TPictureWindow::GetClassName(){ return "TPictureWindow";};

void TPictureWindow::GetWindowClass(WNDCLASS& wc)
{
	TWindow::GetWindowClass(wc);
	wc.hIcon = LoadIcon(GetApplication()->hInstance,"Icon");
}

void TPictureWindow::CMStretchToWindow(RTMessage)
{
	SetStretchToWindow(!bStretchDibToWindow);
};

void TPictureWindow::SetStretchToWindow(BOOL State)
{
	bStretchDibToWindow = State;
	if(bStretchDibToWindow)   	// Changed to stretchmode
		ViewRes = GetViewSize();    // Stretch to window
	AdjustScroller();				// Set scrollers if needed

	UpdateMenu(TRUE);
};

void TPictureWindow::DefCommandProc(RTMessage Msg)
{
	float fZoom = 0.0;

	switch(Msg.Message){
		case WM_COMMAND:{
			if(Msg.WParam==CM_ACTUAL_VIEW)
				fZoom = 1.0;
			if(	(Msg.WParam >= CM_ZOOM_IN)&& (Msg.WParam <  CM_ZOOM_IN + 20))
            	fZoom = (float)(Msg.WParam - CM_ZOOM_IN );

			if(	(Msg.WParam >= CM_ZOOM_OUT)&&(Msg.WParam <  CM_ZOOM_OUT + 20))
				fZoom = 1.0/(float)(Msg.WParam - CM_ZOOM_OUT);

			if(	(Msg.WParam >= CM_VIEWMAP_TYPE)&&(Msg.WParam <  CM_VIEWMAP_TYPE + 20))
				ChangeViewMapType(Msg.WParam - CM_VIEWMAP_TYPE - 1 );
		};
	};

	if(fZoom!=0.0){
        POINT pRes = GetResolution();

		if(pRes.x != 0){
			pRes.x = (int)((float)pRes.x * fZoom);
			pRes.y = (int)((float)pRes.y * fZoom);

			ChangeViewRes(pRes,bStretchDibToWindow);
    	}
	};

	TWindow::DefCommandProc(Msg);
};

HMENU TPictureWindow::GetMenu(void)
{
    /* If parent has been defined, use it's menu */
	if(Parent)
		return ((TMorphWnd*)Parent)->GetViewMenu();
	else
    	return ::GetMenu(HWindow);
};

void TPictureWindow::ActivationResponse(WORD Activated,BOOL IsIconified)
{
	TWindow::ActivationResponse(Activated,IsIconified);
	if(Activated==TRUE)
		PostMessage(Parent->HWindow,WM_QUERYNEWPALETTE,0,0L);

    /* Update MDI menu */
    UpdateMenu(Activated);
};

// Enable/Disable one menu item
void TPictureWindow::SetMenuItem(int nMenuItem,BOOL bState)
{
	EnableMenuItem(GetMenu(),nMenuItem,
		(bState ? (MF_ENABLED) : (MF_DISABLED | MF_GRAYED)) | MF_BYCOMMAND);
};

void TPictureWindow::UpdateMenuList(BOOL fActivate, int nSubMenuPos, int nItemPos)
{
	HMENU hSubMenu  = GetSubMenu(GetMenu(),nSubMenuPos);

	EnableMenuItem(GetMenu(),nSubMenuPos,
		(fActivate ? (MF_ENABLED) : (MF_DISABLED | MF_GRAYED)) | MF_BYPOSITION);

    if(fActivate){
	    /* Clear checkmarks */
		for(int nPos = 0 ; nPos < 20 ; nPos ++ )
			if(GetMenuState(hSubMenu,nPos,MF_BYPOSITION ) & MF_CHECKED)
				CheckMenuItem(hSubMenu,nPos,MF_UNCHECKED | MF_BYPOSITION);

		CheckMenuItem(hSubMenu,nItemPos,MF_CHECKED | MF_BYPOSITION);
        };
};

void TPictureWindow::UpdateMenu(BOOL fActivate)
{
	/* Set Stretch */
	CheckMenuItem(GetMenu(),CM_STRETCH_DIB,
	  (bStretchDibToWindow ? MF_CHECKED : MF_UNCHECKED ) | MF_BYCOMMAND);
	SetMenuItem(CM_STRETCH_DIB	,fActivate );

	/* Update Viewmap menu */
    UpdateMenuList(fActivate,6,ViewMapType);
};

void  TPictureWindow::WMPaletteChanged(TMessage& TM)
{
	int iEntriesRealized = 0;

	if ((TM.WParam!=(WPARAM)HWindow)// This window is not active
		&&(GetBitmap()!=NULL)		// There is a picture to show
		&&!IsIconic(HWindow)){		// Non-Iconified window

		UpdateViewmap();

		HDC hDC        = GetDC(HWindow);
		HPALETTE hpalT = SelectPalette(hDC,GetViewmap()->GetPalette(),TRUE);
		iEntriesRealized = RealizePalette(hDC);
		SelectPalette(hDC,hpalT,TRUE);
		ReleaseDC(HWindow,hDC);
		if(iEntriesRealized > 0)
			InvalidateRect(HWindow,NULL,TRUE);
    };

    TM.Result = iEntriesRealized;
};

void TPictureWindow::WMQueryNewPalette(TMessage& TM)
{
    int iEntriesRealized = 0;

	if((GetBitmap()!=NULL)			// There's a picture to show
		&&!IsIconic(HWindow)){

		UpdateViewmap();

		HDC hDC        = GetDC(HWindow);
	 	HPALETTE hpalT = SelectPalette(hDC,GetViewmap()->GetPalette(),FALSE);
	  	iEntriesRealized = RealizePalette(hDC);
		SelectPalette(hDC,hpalT,TRUE);
		ReleaseDC(HWindow,hDC);

		if(iEntriesRealized > 0)
			InvalidateRect(HWindow,NULL,TRUE);
	};

	TM.Result = iEntriesRealized;
};

void TPictureWindow::Paint(HDC hdc,PAINTSTRUCT _FAR &ps)
{
	if(IsIconic(HWindow)) return;

	ps.rcPaint.left  += (int)Scroller->XPos;
	ps.rcPaint.right += (int)Scroller->XPos;
	ps.rcPaint.top   += (int)Scroller->YPos;
	ps.rcPaint.bottom+= (int)Scroller->YPos;

	if(GetBitmap() == NULL){
		// Clear window(paint white)
		BitBlt(hdc,
			ps.rcPaint.left,  ps.rcPaint.top,
			ps.rcPaint.right, ps.rcPaint.bottom,
		  NULL,0,0,WHITENESS);
		}
		 else {
		UpdateViewmap();
		GetViewmap()->Paint(hdc,ps.rcPaint);
		}
};

POINT	  TPictureWindow::GetResolution(void)
{
	POINT pRes = {0,0};
    if(GetBitmap())
		return GetBitmap()->GetResolution();
	else
    	return pRes;
};

void TPictureWindow::SetBitmap(TPicture* NewDib,BOOL fNewOwnsDib,TViewmap* viewmap,BOOL ClearViewmap)
{
	DeleteBitmap();      // delete the old picture and viewmap

	Dib = NewDib;
	Owns_DIB = fNewOwnsDib;

	if(Dib)
		SetViewRes(Dib->GetResolution());

	bClear = ClearViewmap; 
	ViewMap = viewmap;

	InvalidateRect(HWindow, NULL, FALSE);
    /* Tried this, but it invalidates palettes too much */
	PostMessage(Parent->HWindow,WM_QUERYNEWPALETTE,0,0L);
};

TPicture* TPictureWindow::GetBitmap(void){ return Dib;};

TViewmap* TPictureWindow::GetViewmap(void){ return ViewMap;};
POINT TPictureWindow::GetViewRes(){ return ViewRes;};

void TPictureWindow::DeleteBitmap()
{
	if((Owns_DIB)&&(Dib!=NULL)){
		delete Dib;
		Dib = NULL;
		};     

	if(ViewMap!=NULL){
		delete ViewMap;
		ViewMap = NULL;
		};
};

void TPictureWindow::SetViewRes(POINT NewRes)
{
	if(!bStretchDibToWindow){
        // Check that viewres has changed
		if((ViewRes.x!=NewRes.x)||(ViewRes.y!=NewRes.y)){
			ViewRes = NewRes;

			// Change windows size so we can get a new size from GETMINMAX
			RECT WindowRect;
			GetWindowRect(HWindow,&WindowRect);
			SetWindowPos(HWindow,(HWND)NULL,0,0,
				WindowRect.right  - WindowRect.left,
				WindowRect.bottom - WindowRect.top,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

			AdjustScroller();
			};
		};
};

void TPictureWindow::Invalidate(RECT* ClipRect)
{
	if(IsIconic(HWindow)){
        // Do not update now, but when window is back in view
		DeleteViewmap();
	} else {
		UpdateViewmap();
		GetViewmap()->Invalidate(ClipRect);

		InvalidateRect(HWindow,ClipRect,FALSE);
    };
};

// Get size of client area
POINT TPictureWindow::GetViewSize()
{
	RECT ClientRect;  
	POINT Screen;
	GetClientRect(HWindow, &ClientRect);
	Screen.x = ClientRect.right - ClientRect.left;
	Screen.y = ClientRect.bottom- ClientRect.top;

	// Substract the space taken by scrollers
	DWORD dwWinStyle = GetWindowLong (HWindow, GWL_STYLE);
	if(dwWinStyle & WS_VSCROLL)   Screen.x += GetSystemMetrics (SM_CYHSCROLL);
	if(dwWinStyle & WS_HSCROLL)   Screen.y += GetSystemMetrics (SM_CXHSCROLL);

	return Screen;
};

// Adjust scrollers
void TPictureWindow::AdjustScroller()
{
	POINT Outside;

	POINT ClientSize = GetViewSize();
	Outside.x = ViewRes.x - ClientSize.x;
	Outside.y = ViewRes.y - ClientSize.y;

	if((Outside.x > 0)||(Outside.y > 0)){
		Outside.x += GetSystemMetrics (SM_CYHSCROLL);
		Outside.y += GetSystemMetrics (SM_CXHSCROLL);
	};

	Scroller->ScrollTo(0, 0);
	Scroller->SetRange(Outside.x,Outside.y );

	InvalidateRect(HWindow, NULL, FALSE);
}

void TPictureWindow::WMSize(TMessage& Msg)
{
	static BOOL bInSize = FALSE;

	// Check the semaphore, return if it's set.
	if (bInSize==TRUE)   return;

	bInSize = TRUE;

	TWindow::WMSize(Msg);
	if ( (Msg.WParam != SIZEICONIC) )
		if(bStretchDibToWindow){
			// Set new ViewRes
			ViewRes = GetViewSize();
			InvalidateRect(HWindow,NULL,FALSE);
		} else
           	AdjustScroller();

	bInSize = FALSE;

	// Update viewres
	SetViewRes(ViewRes);
}


// Calculate the size of the window frames 
POINT TPictureWindow::CalculateFrameSize(void)
{
    POINT FrameSize;
	RECT WindowRect;
	GetWindowRect(HWindow,&WindowRect);
	
    POINT pClient = GetViewSize();
	FrameSize.x = ( WindowRect.right  - WindowRect.left ) - pClient.x;
	FrameSize.y = ( WindowRect.bottom - WindowRect.top  ) - pClient.y;

	return FrameSize;    
};
 
// Get stretch min and max values
void TPictureWindow::WMGetMinMax(RTMessage Msg)
{
    // if picture is in stretch mode 
	if(bStretchDibToWindow){
    	// use default min + mix size
		DefWndProc(Msg);
	} else {
		POINT FrameSize = CalculateFrameSize();

		MINMAXINFO FAR*lpmmi = (MINMAXINFO FAR*) Msg.LParam;
	    // Maximum window size is ViewRes + frames
		lpmmi->ptMaxSize.x      = ViewRes.x + FrameSize.x;
		lpmmi->ptMaxSize.y      = ViewRes.y + FrameSize.y;
		lpmmi->ptMaxTrackSize.x = ViewRes.x + FrameSize.x;
		lpmmi->ptMaxTrackSize.y = ViewRes.y + FrameSize.y;

		Msg.Result = 0;
    };
};


/////////////////////////////////////////////////////////
// Functions to change the attributes of windows
//////////////////////////////////////////////////////////

void TPictureWindow::CMOptions(RTMessage)
{
	TPic_Opt Opt;
	 // Set the default values
	Opt.ZoomRes = ViewRes;
	if(GetBitmap())
		Opt.Resolution = GetResolution();
	   else 
		Opt.Resolution = ViewRes;

	Opt.bStretchDibToWindow = bStretchDibToWindow;
	Opt.bAutoUpdateViewmaps = bAutoUpdateViewmaps;
	Opt.ViewmapNames = ViewmapNames;
	Opt.ViewmapType  = ViewMapType;

	// Execute the dialog
	if(GetModule()->
		ExecDialog( new TPictureOptionsWnd(Parent,&Opt))==IDOK){
		// Check what has been changed

        // Viewmap type has been changed
		ChangeViewMapType(Opt.ViewmapType);
		ChangeViewRes(Opt.ZoomRes,Opt.bStretchDibToWindow);
    };
};

void TPictureWindow::ChangeViewMapType(int nNewType)
{
	// If Viewmap type has been changed
	if(ViewMapType!=nNewType){
		ViewMapType = nNewType;
		InvalidateRect(HWindow,NULL,FALSE);
    	UpdateMenu(TRUE);
	};
};


void TPictureWindow::ChangeViewRes(POINT pNewRes,BOOL bNewStretch)
{
	if(bNewStretch!=bStretchDibToWindow){
		bStretchDibToWindow = bNewStretch;
		if(bStretchDibToWindow)   	// Changed to stretchmode
			ViewRes = GetViewSize();    // Stretch to window
			else
			ViewRes = pNewRes;      // Changed to scrollers
		AdjustScroller();				// Set scrollers if needed
	}               

	POINT FrameSize = CalculateFrameSize();
	SetWindowPos(HWindow,(HWND)NULL,0,0,
		pNewRes.x + FrameSize.x,
		pNewRes.y + FrameSize.y,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

	SetViewRes(pNewRes);
};

void TPictureWindow::UpdateViewmap()
{
	if(GetBitmap()==NULL) return;

    // if viewmap exists
	if(GetViewmap())
		if(GetViewmap()->GetType()!=ViewMapType)
			DeleteViewmap();
		   else {
			POINT Res = GetViewmap()->GetResolution();
			if((ViewRes.x!=Res.x)||(ViewRes.y!=Res.y))
				GetViewmap()->Resize(ViewRes);// Just resize
			}
	if(GetViewmap()==NULL)
		CreateViewmap();
};

void TPictureWindow::CreateViewmap()
{
	SetViewmap(TViewmap::Create
		(GetBitmap(),ViewRes,ViewMapType,bClear));

	bClear = FALSE;
};

void TPictureWindow::DeleteViewmap()
{
	bClear = FALSE;

    // Delete viewmap
	TViewmap *vmView = FetchViewmap();
	if(vmView)
    	delete vmView;
};

TViewmap* TPictureWindow::FetchViewmap()
{
	TViewmap* Value = ViewMap;
	ViewMap = NULL;
	return Value;
};

void TPictureWindow::SetViewmap(TViewmap* NewMap)
{
	DeleteViewmap();

	ViewMap = NewMap;
};
 
 
/* End of file PICTUREW.CPP */ 
 

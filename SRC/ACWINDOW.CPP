/**********************************************************************
 * 
 *  NAME:         ACWINDOW.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          16.12.95 MHM renamed Toolbox to ACWindow 
 *********************************************************************/
 
#include <assert.h>
#include <button.h>
#include <static.h>
#include <scrollba.h>

#include "acwindow.h"	
#include "morphwin.h"	// class TMorphWnd
#include "accontex.h"	// class ACContext
#include "acmorph.h"	// class ACMorphContext
#include "odbutton.h"	// class ODButton
#include "morphpic.h"

#define MAXTEXT_LEN 9

TACScrollBar::TACScrollBar(PTWindowsObject AParent, int AnId,
	int X, int Y, int W, int H, BOOL IsHScrollBar, PTModule AModule):
	TScrollBar(AParent,AnId ,X,Y,W,H, IsHScrollBar,AModule)
{
};

void TACScrollBar::SBLineUp(RTMessage Msg)
{
	TScrollBar::SBLineUp(Msg);

	((TACWindow*)Parent)->SetFrame(GetPosition());
};

void TACScrollBar::SBLineDown(RTMessage Msg)
{
	TScrollBar::SBLineDown(Msg);

	((TACWindow*)Parent)->SetFrame(GetPosition());
};

void TACScrollBar::SBPageUp(RTMessage Msg)
{
	TScrollBar::SBPageUp(Msg);

	((TACWindow*)Parent)->SetFrame(GetPosition());
};

void TACScrollBar::SBPageDown(RTMessage Msg)
{
	TScrollBar::SBPageDown(Msg);

	((TACWindow*)Parent)->SetFrame(GetPosition());
};

void TACScrollBar::SBThumbPosition(RTMessage Msg)
{
	TScrollBar::SBThumbPosition(Msg);

	((TACWindow*)Parent)->SetFrame(GetPosition());
};

void TACScrollBar::SBThumbTrack(RTMessage Msg)
{
	TScrollBar::SBThumbTrack(Msg);

	((TACWindow*)Parent)->SetFrame(GetPosition());
};

void TACScrollBar::SBTop(RTMessage Msg)
{
	TScrollBar::SBTop(Msg);

	((TACWindow*)Parent)->SetFrame(GetPosition());
};

void TACScrollBar::SBBottom(RTMessage Msg)
{
	TScrollBar::SBBottom(Msg);

	((TACWindow*)Parent)->SetFrame(GetPosition());
};

//////////////////////////////////////7

TACWindow::TACWindow(PTWindowsObject AParent,LPSTR szTitle,HMENU hNewMenu) :
	TWindow(AParent,szTitle)
{
	AC = NULL;

	// Get menu handle from main window
	hACMenu = hNewMenu;

	Attr.Style = WS_CAPTION | WS_BORDER | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;

    // This window isn't a MDI window's child but it's sibling   
	SetFlags(WB_MDICHILD,FALSE);

    // Set window width
	Attr.W = 380 + GetSystemMetrics(SM_CXBORDER);
	Attr.H =  60 + GetSystemMetrics(SM_CYCAPTION);

    // Set scrollbar width
	SetRect(&rScrollBar,0,0,380,20);
	nScrollButtonWidth = GetSystemMetrics(SM_CXHSCROLL);
	nScrollThumbWidth = GetSystemMetrics(SM_CXHTHUMB);

	SetRect(&rMarkBar,
		 rScrollBar.left + nScrollButtonWidth
		,rScrollBar.bottom
		,rScrollBar.right - nScrollButtonWidth
		,rScrollBar.bottom + ( rScrollBar.bottom - rScrollBar.top ) );

	sbFrames = new TACScrollBar(this,CM_ACFRAMES
			, rScrollBar.left	, rScrollBar.top
			, rScrollBar.right  - rScrollBar.left
			, rScrollBar.bottom - rScrollBar.top , TRUE);

	new TODButton(this,CM_ACMARKIN,"Mark In"
			, rScrollBar.left	, rScrollBar.bottom
			, nScrollButtonWidth
			, rScrollBar.bottom - rScrollBar.top
			, FALSE
			, "BM_MARKIN_NORMAL"	, "BM_MARKIN_PRESSED"
			, "BM_MARKIN_FOCUSED"	, "BM_MARKIN_DISABLED");
	new TODButton(this,CM_ACMARKOUT,"Mark Out"
			, rScrollBar.right - nScrollButtonWidth
			, rScrollBar.bottom
			, nScrollButtonWidth
			, rScrollBar.bottom - rScrollBar.top
			, FALSE
			, "BM_MARKOUT_NORMAL"	, "BM_MARKOUT_PRESSED"
			, "BM_MARKOUT_FOCUSED"	, "BM_MARKOUT_DISABLED");

	btnPlay = new TODButton(this,CM_ACPLAY  ,">"
		,  0, 40, 30, 20,  FALSE
		, "BM_PLAY_NORMAL"	, "BM_PLAY_PRESSED"
		, "BM_PLAY_FOCUSED"	, "BM_PLAY_DISABLED");
	new TODButton(this,CM_ACSTOP  ,"O"
		, 30, 40, 30, 20,  FALSE
		, "BM_STOP_NORMAL"	, "BM_STOP_PRESSED"
		, "BM_STOP_FOCUSED"	, "BM_STOP_DISABLED");

	new TODButton(this,CM_ACPREVMARK,"\216<"
		, 80, 40, 30, 20,  FALSE
		, "BM_PREVMARK_NORMAL"	, "BM_PREVMARK_PRESSED"
		, "BM_PREVMARK_FOCUSED"	, "BM_PREVMARK_DISABLED");
	new TODButton(this,CM_ACNEXTMARK,">\201"
		,110, 40, 30, 20,  FALSE
		, "BM_NEXTMARK_NORMAL"	, "BM_NEXTMARK_PRESSED"
		, "BM_NEXTMARK_FOCUSED"	, "BM_NEXTMARK_DISABLED");

	new TButton(this,CM_ACMORPH	 ,"Morph"
		,rScrollBar.right - 80 - 80, 40, 80, 20,  FALSE);
	new TButton(this,CM_ACCONFIG ,"Config"
		,rScrollBar.right - 80, 40, 80, 20,  FALSE);
};

void TACWindow::SetupWindow()
{
	TWindow::SetupWindow();

	// Disable all controls
	UpdatePlayState();

	UpdateFrameChangers();
};

LPSTR TACWindow::GetClassName(){ return "WinMorph_ACWindow";};

void TACWindow::GetWindowClass( WNDCLASS& WndClass )
{
  TWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH)COLOR_BTNFACE+1;
};

void TACWindow::DefCommandProc(RTMessage Msg)
{
	// Hey, we got the neighbours mail !!
	// First we get the neightbours address
	HWND NextWindow = GetNextWindow(HWindow,GW_HWNDNEXT);
    	
	// Then we get the neighbours attention
	SetFocus(NextWindow);

	// ... and repost the mail
	PostMessage(NextWindow,WM_COMMAND,Msg.WParam,Msg.LParam);
};

void TACWindow::Paint(HDC hDC,PAINTSTRUCT _FAR &)
{
    // Paint borders for MarkBar
	Rectangle(hDC	, rMarkBar.left		, rMarkBar.top
					, rMarkBar.right	, rMarkBar.bottom);

	if(AC){
        // Select gray color for rectangle inside
		HBRUSH hBrushTemp =(HBRUSH)
			SelectObject(hDC,CreateSolidBrush(PALETTERGB(196,196,196)));

        int nMarkBarWidth = rMarkBar.right - rMarkBar.left - nScrollThumbWidth;
		int nMarkInLine,nMarkOutLine;

		if(AC->nFrames>1){
			nMarkInLine = 
					rMarkBar.left + (int)( nMarkBarWidth *				
					(AC->nMarkIn     /(float)(AC->nFrames-1)));
			nMarkOutLine= 
					rMarkBar.left + (int)( nMarkBarWidth *
					((AC->nMarkOut+1)/(float)(AC->nFrames-1)));
			}
		   else {
			nMarkInLine  = rMarkBar.left;
			nMarkOutLine = rMarkBar.left + nMarkBarWidth;
			};

		if(AC->nMarkOut == AC->nFrames -1 )
        	nMarkOutLine += nScrollThumbWidth;

        // Paint MarkIn & MarkOut
		Rectangle(hDC	, nMarkInLine , rMarkBar.top
						, nMarkOutLine, rMarkBar.bottom );

        // Paint frame marks
		for(int nFrame = 1 ; nFrame < AC->nFrames ; nFrame ++ ){
			int nXline = rMarkBar.left + (int)( nMarkBarWidth *
						(nFrame/(float)(AC->nFrames-1)));
			MoveTo(hDC,nXline,rMarkBar.top);
			LineTo(hDC,nXline,rMarkBar.bottom);
			};

		DeleteObject(SelectObject(hDC,hBrushTemp));
		};
};

void TACWindow::Cm_ACPlay(RTMessage)
{
	switch(AC->nPlayState){
		case ACPLAY_STOPPED:
			AC->Play();
			break;
		case ACPLAY_PLAYING:
			AC->SetPause(TRUE);
			break;
		case ACPLAY_PAUSED:
			AC->SetPause(FALSE);
			break;
        };

	// Update play buttons in ACW
	UpdatePlayState();

	UpdateFrameChangers();
};

void TACWindow::Cm_ACMorph(RTMessage)
{
	if(AC->nPlayState==ACPLAY_PAUSED)
        // Continue morphing
		AC->SetPause(FALSE);
       else
		// Start morphing
		AC->Morph();

	// Update play buttons in ACW
	UpdatePlayState();

	UpdateFrameChangers();
};

void TACWindow::Cm_ACStop(RTMessage)
{
	AC->Stop();
};

void TACWindow::Cm_ACPrevMark(RTMessage)
{
	int nNewFrame = AC->nFrame;

	if(nNewFrame > AC->nMarkOut)
        // If frame is past MarkOut, rewind to MarkOut
		nNewFrame = AC->nMarkOut;
	   else
		if(nNewFrame > AC->nMarkIn)
			// If frame is between MarkIn and MarkOut, rewind to MarkIn
			nNewFrame = AC->nMarkIn;
		   else
            // If frame is before MarkIn, rewind to start
			nNewFrame = 0;

    // Set frame for ACContext
	AC->SetFrameN(nNewFrame);
};

void TACWindow::Cm_ACNextMark(RTMessage)
{
	int nNewFrame = AC->nFrame;

	if(nNewFrame < AC->nMarkIn)
        // If frame is before MarkIn, wind to MarkIn
		nNewFrame = AC->nMarkIn;
	   else
		if(nNewFrame < AC->nMarkOut)
			// If frame is between MarkIn and MarkOut, wind to MarkOut
			nNewFrame = AC->nMarkOut;
		   else
            // If frame is after MarkOut, wind to end
			nNewFrame = AC->nFrames - 1;

    // Set frame for ACContext
	AC->SetFrameN(nNewFrame);

    SetFocus(AC->GetParent()->HWindow);
};

void TACWindow::Cm_ACMarkIn(RTMessage)
{
	assert(AC!=NULL);

	// Set new MarkIn to AC
	AC->nMarkIn = AC->nFrame;

    AC->nMarkOut = max(AC->nMarkIn,AC->nMarkOut);

	// Update ACWindow
	InvalidateRect(HWindow,&rMarkBar,FALSE);
};

void TACWindow::Cm_ACMarkOut(RTMessage)
{
	assert(AC!=NULL);

    // Set new MarkOut to AC
	AC->nMarkOut = AC->nFrame;

	AC->nMarkIn = min(AC->nMarkIn,AC->nMarkOut);

    // Update ACWindow
	InvalidateRect(HWindow,&rMarkBar,FALSE);
};

void TACWindow::Cm_ACFirstFrame(RTMessage)
{
	AC->SetFrameN(0);
};

void TACWindow::Cm_ACLastFrame(RTMessage)
{
	AC->SetFrameN(AC->nFrames - 1);
};

void TACWindow::CM_Config(RTMessage)
{
	AC->Config();
};

void TACWindow::SetFrame(int nNewFrame)
{
    assert(AC!=NULL);

	// Set new frame only if it's changed. Otherwise the frame changing
	//would loop between AC,ACW and the scroller forever

	// Set frame for ACContect
	if(AC->nFrame!=nNewFrame)
		AC->SetFrameN(nNewFrame);

    // Set frame for the scroller
	if(sbFrames->GetPosition()!=nNewFrame){
        // Set scroller position  
		sbFrames->SetPosition(nNewFrame);
		};

	// Update Frames/Frames - window
    UpdateFrameRatio();
};

void TACWindow::EnableControl(int nControl,BOOL fState)
{
    // If button is disabled 
	if(!fState){
        // give focus to another window
		HWND NextWindow = GetNextWindow(HWindow,GW_HWNDNEXT);    	

		SetFocus(NextWindow);
    }

    // Switch controls state
	EnableWindow(GetDlgItem(this->HWindow,nControl),fState);

    // Switch menu items state
	EnableMenuItem(hACMenu , nControl , MF_BYCOMMAND |
		(fState? MF_ENABLED : MF_DISABLED | MF_GRAYED ));
};

void TACWindow::SetACContext(ACMorphContext *NewAC)
{
	// Detach ACW from old AC
	if(AC)
		AC->Detach();

	AC = NewAC;

    // Attach new AC to ACW 
	if(AC){
    	AC->Attach(this);
		UpdateContext();
		};
};

void TACWindow::UpdateContext()
{
    assert(AC!=NULL);

	UpdatePlayState();

	UpdateFrameChangers();

	// Set range of the scroll bar
	sbFrames->SetRange(0,AC->nFrames - 1);

	// Update Frame/Frames - indicators
    UpdateFrame();

	// Update Mark Bar frame
	InvalidateRect(HWindow,&rMarkBar,FALSE);
};

void TACWindow::UpdateFrame()
{
	// Set right frame to scrollbar
    SetFrame(AC->nFrame);

	UpdateFrameRatio();
};


void TACWindow::UpdateFrameRatio()
{
	// Update Frame number
    char szFrame[30]="";
	char szCaption[80];

	if(AC)
		if(AC->nFrames>0)
			wsprintf(szFrame," : Frame %d/%d",AC->nFrame + 1,AC->nFrames);

	strcpy(szCaption,"Projector");
    strcat(szCaption,szFrame);
    SetCaption(szCaption);
};

void TACWindow::SetPlayPause(void)
{
	BOOL bShowPause = FALSE;	

	if(AC)
		if(AC->nPlayState == ACPLAY_PLAYING)
			bShowPause = TRUE;

	if(bShowPause){
		btnPlay->SetButton(ODBUTTON_NORMAL  , "BM_PAUSE_NORMAL");
		btnPlay->SetButton(ODBUTTON_PRESSED , "BM_PAUSE_PRESSED");
		btnPlay->SetButton(ODBUTTON_FOCUSED , "BM_PAUSE_FOCUSED");

		ModifyMenu(hACMenu,CM_ACPLAY,MF_BYCOMMAND | MF_STRING ,CM_ACPLAY,(LPSTR)"Pause\bP");
		}
	   else {
		btnPlay->SetButton(ODBUTTON_NORMAL  , "BM_PLAY_NORMAL");
		btnPlay->SetButton(ODBUTTON_PRESSED , "BM_PLAY_PRESSED");
		btnPlay->SetButton(ODBUTTON_FOCUSED , "BM_PLAY_FOCUSED");

		ModifyMenu(hACMenu,CM_ACPLAY,MF_BYCOMMAND | MF_STRING ,CM_ACPLAY,(LPSTR)"Play\bP");
		};
};

void TACWindow::UpdatePlayState()
{
	BOOL fPlayButton	= FALSE;
	BOOL fMorphButton	= FALSE;
    BOOL fStopButton	= FALSE;
	BOOL fConfigButton  = FALSE;

	// Change button and menuitem
	SetPlayPause();

	if(AC)
		if(AC->nFrames > 0){
			// Normal AC context
			fPlayButton	= TRUE;
			fMorphButton= FALSE;

			switch(AC->nPlayState){
				case ACPLAY_STOPPED:
					fMorphButton = AC->CanMorph() &&
						(((TMorphWnd*)Parent)->GetMorphing()==0);
					break;
				case ACPLAY_PLAYING:
					break;
				case ACPLAY_PAUSED:
					if(AC->nMorphing){
							fPlayButton = FALSE;
							fMorphButton = TRUE;
                            };
					break;
				};

			fStopButton   = ( AC->nPlayState != ACPLAY_STOPPED );
			fConfigButton = ( AC->nPlayState == ACPLAY_STOPPED );

            AC->GetParent()->UpdateMenu(TRUE);

		};

	EnableControl(CM_ACMORPH, fMorphButton);
	EnableControl(CM_ACSTOP , fStopButton);
	EnableControl(CM_ACPLAY , fPlayButton);
	EnableControl(CM_ACCONFIG,fConfigButton);
};

void TACWindow::UpdateFrameChangers()
{
	// If default null context, disable all
    // If morphing , disable all
	BOOL bEnabled = FALSE;

	if(AC)// Check that AC isn't NULL
		if(!AC->nMorphing)
			if(AC->nFrames > 0)
				bEnabled = TRUE;

	EnableControl(CM_ACFRAMES , bEnabled);
	EnableControl(CM_ACMARKIN , bEnabled);
	EnableControl(CM_ACMARKOUT, bEnabled);

	EnableControl(CM_ACPREVMARK,bEnabled);
	EnableControl(CM_ACNEXTMARK,bEnabled);

	// Switch menu items state
	EnableMenuItem(hACMenu , CM_ACPREVFRAME , MF_BYCOMMAND |
		(bEnabled ? MF_ENABLED : MF_DISABLED | MF_GRAYED ));
	// Switch menu items state
	EnableMenuItem(hACMenu , CM_ACNEXTFRAME , MF_BYCOMMAND |
		(bEnabled ? MF_ENABLED : MF_DISABLED | MF_GRAYED ));

	// Switch menu items state
	EnableMenuItem(hACMenu , CM_ACFIRSTFRAME , MF_BYCOMMAND |
		(bEnabled ? MF_ENABLED : MF_DISABLED | MF_GRAYED ));
	// Switch menu items state
	EnableMenuItem(hACMenu , CM_ACLASTFRAME , MF_BYCOMMAND |
		(bEnabled ? MF_ENABLED : MF_DISABLED | MF_GRAYED ));
};

/* End of file ACWINDOW.CPP */ 
 

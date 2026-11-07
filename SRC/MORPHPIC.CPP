/**********************************************************************
 * 
 *  NAME:         MORPHPIC.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          11. 8.95 MHM added headers 
 *          20. 9.95 MHM removed TMorphEditWnd to separate file
 *********************************************************************/ 

#include "morphpic.h"
#include "common.h"
#include "acanim.h"		// class ACAnimContext 
#include "tpicture.h"	// class TPicture
#include "viewmap.h"	// class TViewmap
#include "morphwin.h"	// class MorphWnd
#include "morphcfg.h"	// class TMorphConfig
#include "geom2d.h"		// class Point2d

//////////////////////////////////////////////////////////////
// TMorphPictureWnd 
//////////////////////////////////////////////////////////////

TMorphPictureWnd::TMorphPictureWnd(PTWindowsObject AParent,LPSTR Name,
		ACAnimContext *NewAC)
	  : TPictureWindow(AParent,Name)
{
	ViewFrame = 0;
	Frames = 0;

	AC = NewAC;
    AC->SetParent(this);
};

TMorphPictureWnd::~TMorphPictureWnd()
{
	delete AC;
};

void TMorphPictureWnd::SetupWindow(void)
{
	TPictureWindow::SetupWindow();

	FramesChanged();
};

BOOL TMorphPictureWnd::CanMorph(void)	{ return FALSE; };

void TMorphPictureWnd::UpdateTitle(void)
{
	char NewTitle[80];

	if(AC->nPlayState!=ACPLAY_STOPPED)
		wsprintf(NewTitle,"%s:%d\0",AC->GetStateName(),ViewFrame+1);
	   else
		wsprintf(NewTitle,"%s:%d\0",WindowTitle,ViewFrame+1);

	SetCaption(NewTitle);
};

void TMorphPictureWnd::FramesChanged(void)
{
	int nNewFrames = GetNewFrames();

    // If the frames is changed
    if(Frames != nNewFrames){
        // Set new frames
		Frames = nNewFrames;

        // Set new viewframe
		SetViewFrame(0);

		// Inform AC that the number of frames has been changed
		AC->SetFrames(Frames);
		};
};

ACContext *TMorphPictureWnd::GetACContext(void)
{
	return AC;
};

int TMorphPictureWnd::GetNewFrames(void)
{
	return 1;
};

void TMorphPictureWnd::ActivationResponse(WORD Activated,BOOL IsIconified)
{
	TPictureWindow::ActivationResponse(Activated,IsIconified);
	/*if(Activated==TRUE)*/
		// Update AC Window's context
	((TMorphWnd*)Parent)->UpdateACContext();
};

void TMorphPictureWnd::SetViewFrame(int nNewFrame)
{
    // Check if last frame was requested
	if(nNewFrame == LAST_FRAME)
        nNewFrame = GetFrames()-1;

    // Set new frame
    ViewFrame = nNewFrame;

    // Update window title
	UpdateTitle();

	// Update window
    if(HWindow)
		InvalidateRect(HWindow,NULL,FALSE);
};

void  TMorphPictureWnd::TimerTick(RTMessage)
{
	assert(AC!=NULL);

	AC->PlayFrame();
};

int	 TMorphPictureWnd::GetFrames()	{ return Frames; };

void  TMorphPictureWnd::InvalidateScanline(Scanline_Screen* Area)
{
    // If window is minimized as an icon, 
	if(IsIconic(HWindow)){
        // Repaint window only when it's maximized
		DeleteViewmap();
		}
       else {
		UpdateViewmap();

        // Repaint Viewmap
		RECT *rViewArea = GetViewmap()->InvalidateScanline(Area);

        assert(rViewArea->left>= 0);
		assert(rViewArea->top >= 0);

		RECT rClientArea;
		rClientArea.left  = rViewArea->left   - ((int)Scroller->XPos /*+ MorphConfig.PointSize*/);
		rClientArea.right = rViewArea->right  - ((int)Scroller->XPos /*- MorphConfig.PointSize*/);
		rClientArea.top   = rViewArea->top    - ((int)Scroller->YPos /*+ MorphConfig.PointSize*/);
		rClientArea.bottom= rViewArea->bottom - ((int)Scroller->YPos /*- MorphConfig.PointSize*/);

		if(rClientArea.left< 0){
			assert(rClientArea.left>= 0);
			};

		assert(rClientArea.top >= 0);

		// Repaint window
		InvalidateRect(HWindow,&rClientArea,FALSE);
		};
};

/* End of file MORPHPIC.CPP */


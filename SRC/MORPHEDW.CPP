/**********************************************************************
 * 
 *  NAME:         MORPHEDW.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  20.9.95  MHM     created 
 * 
 *********************************************************************/ 

#include "morphedw.h"
#include "morphwin.h"	// class MorphWnd
#include "morphcfg.h"	// class TMorphConfig
#include "acanim.h"		// class TACAnimContext
#include "itemlist.h"	// class ItemList
#include "animatio.h"
#include "animndlg.h"
#include "viewmap.h"

TMorphEditWnd::TMorphEditWnd(PTWindowsObject AParent,LPSTR Name,
	ACAnimContext *NewAC)
  : TAnimationWnd(AParent,Name,NewAC)
{
	MorphBlock		= 0;
	AnimBlock		= 0;
	fAnimationFramesUsed = FALSE;

	fShowPoints		= TRUE;
    fShowTriangles	= TRUE;

	MorphFrame		= 0;
	fTracking		= FALSE;

	ViewPoints		= NULL;
	nViewPointsAllocated = 0;
	nViewpoints		= 0;

	SetAnimation(((TMorphWnd*)Parent)->GetMorphmesh()->GetAnimation(AnimBlock),FALSE);
};

BOOL TMorphEditWnd::CanMorph(void)
{ return fAnimationFramesUsed && (AnimBlock==MorphResult);};

void TMorphEditWnd::~TMorphEditWnd()
{
	nViewpoints = 0;
	ViewPointsResize(0);

	if(AC->nPlayState != ACPLAY_STOPPED){
		AC->Stop();
    }
};

void TMorphEditWnd::WMMenuInit(RTMessage Msg)
{
	BOOL bMenuState = TRUE;

	if(AC)
		bMenuState = ( AC->nPlayState == ACPLAY_STOPPED );

	EnableMenuItem((HMENU)Msg.WParam,SC_CLOSE,
		((bMenuState ? (MF_ENABLED) : (MF_DISABLED | MF_GRAYED)) | MF_BYCOMMAND));
};

int TMorphEditWnd::LoadCfgKeyword(wmfstream& file,LPSTR szKeyWord)
{
	LPSTR szKeyWords[] = {	"ShowPoints",	"ShowTriangles",
		"UseAnimFrames",	"MorphBlock",	"AnimBlock",		NULL};

	// and identify it
	int nKeyword = IdentifyKeyword(szKeyWords,szKeyWord);

    // Process keyword
	switch(nKeyword){
	case 0:/* ShowPoints */
		switch(IdentifyKeyword(file,szOnOffValues)){
			case ON  : fShowPoints = TRUE  ; break;
			case OFF : fShowPoints = FALSE ; break;
			default:break; // Error
				};
        break;
	case 1:/* ShowTriangles */
		switch(IdentifyKeyword(file,szOnOffValues)){
			case ON  : fShowTriangles = TRUE  ; break;
			case OFF : fShowTriangles = FALSE ; break;
			default:break; // Error
				};
		break;
	case 2:/* UseAnimFrames */
		switch(IdentifyKeyword(file,szOnOffValues)){
			case ON  : fAnimationFramesUsed = TRUE  ; break;
			case OFF : fAnimationFramesUsed = FALSE ; break;
			default:break; // Error
				};
		break;
	case 3:/* MorphBlock */
		MorphBlock = IdentifyKeyword(file,TMorphmesh::MeshNames);
		break;
	case 4:/* AnimBlock */
		AnimBlock = IdentifyKeyword(file,TMorphmesh::szPictureNames);
		break;
	case UNDEFINED:
		nKeyword = TAnimationWnd::LoadCfgKeyword(file,szKeyWord);
        break;
	default:// Error
    	break;
	};

	Frames = 0;	/* Force FramesChanged to update */
	FramesChanged();
	UpdateTitle();
	UpdateMenu(TRUE);

	// Return results
	return nKeyword;
};

void TMorphEditWnd::SaveCfgKeyword(wmfstream& file)
{
	TAnimationWnd::SaveCfgKeyword(file);

	// Save ShowPoints
	file << "\tShowPoints"  << " = ";
	file << szOnOffValues[fShowPoints?ON:OFF];
	file << "\n";

	// Save ShowTriangles
	file << "\tShowTriangles"  << " = ";
	file << szOnOffValues[fShowTriangles?ON:OFF];
	file << "\n";

	// Save UseAnimFrames
	file << "\tUseAnimFrames"  << " = ";
	file << szOnOffValues[fAnimationFramesUsed?ON:OFF];
	file << "\n";

    // Save MorphBlock
	file << "\tMorphBlock"  << " = ";
	file << TMorphmesh::MeshNames[MorphBlock];
	file << "\n";

	// Save AnimhBlock
	file << "\tAnimBlock"  << " = ";
	file << TMorphmesh::szPictureNames[AnimBlock];
	file << "\n";
};

void TMorphEditWnd::DefCommandProc(RTMessage Msg)
{
	switch(Msg.Message){
		case WM_COMMAND:{
			if(	(Msg.WParam >= CM_MORPHBLOCK)&& (Msg.WParam <  CM_MORPHBLOCK + 20)){
				SetMorphBlock( Msg.WParam - CM_MORPHBLOCK - 1);
				};
			if(	(Msg.WParam >= CM_ANIMATION)&&(Msg.WParam <  CM_ANIMATION + 20)){
				SetAnimBlock(Msg.WParam - CM_ANIMATION - 1 );
				};
		};
	};

	TAnimationWnd::DefCommandProc(Msg);
};

void TMorphEditWnd::CMUseAnimFrames(RTMessage)
{
	fAnimationFramesUsed = !fAnimationFramesUsed;
    Frames = 0;
    FramesChanged();
	UpdateMenu(TRUE);
};

void TMorphEditWnd::CMDeleteView(TMessage&)
{
	SendMessage(HWindow,WM_FIRST + WM_CLOSE,0,0);
};

void TMorphEditWnd::SetMorphBlock(int nBlock)
{
	MorphBlock = nBlock;
    Frames = 0;
	FramesChanged();
	UpdateMenu(TRUE);
};

void TMorphEditWnd::SetAnimBlock(int nBlock)
{
	AnimBlock = nBlock;
    Frames = 0;
	SetAnimation(((TMorphWnd*)Parent)->GetMorphmesh()->GetAnimation(AnimBlock),FALSE);
	UpdateMenu(TRUE);
};

void TMorphEditWnd::UpdateTitle()
{
	char NewTitle[120];

	// If user is moving a selection
	if(fTracking){
        // show coordinates in title
		long x = (long)(pfDragRelative.x * 100000.0);
		long y = (long)(pfDragRelative.y * 100000.0);
		wsprintf(NewTitle,"(0.%0.5ld,0.%0.5ld)",x,y);

		SetCaption(NewTitle);
		}
	   else {
		if(AC->nPlayState==ACPLAY_STOPPED){
			LPSTR pszAnimNames[] = {"Start","Result","End","Background"};
			LPSTR pszMorphNames[] = {"Start","Result","End"};

			wsprintf(NewTitle,"[Anim:%s][Morph:%s]:%d\0",
				pszAnimNames[AnimBlock],
				pszMorphNames[MorphBlock],ViewFrame+1);
			SetCaption(NewTitle);
		} else
			// Normal title
			TAnimationWnd::UpdateTitle();
    }

};

void TMorphEditWnd::SetupWindow(void)
{
	TAnimationWnd::SetupWindow();

	ViewPointsUpdate(((TMorphWnd*)Parent)->GetMorphmesh()->GetPoints());
};

void TMorphEditWnd::Paint(HDC hDC, PAINTSTRUCT& ps)
{
	if (IsIconic(HWindow)) return;

    // Normal paint
	TAnimationWnd::Paint(hDC,ps);

    // Paint Morphmesh
	((TMorphWnd*)Parent)->GetMorphmesh()->Paint(hDC,
		MorphBlock,MorphFrame,ViewPoints,
		fShowPoints,   fShowTriangles);
};

int TMorphEditWnd::GetNewFrames(void)
{
	// If Window uses Animations frames
	if(fAnimationFramesUsed)
        // use Animationwindows function
		return TAnimationWnd::GetNewFrames();
	   else {
        // otherwise get Frames from Morphmesh
		return ((TMorphWnd*)Parent)->GetMorphmesh()->GetFrames(MorphBlock);
		};
};

void TMorphEditWnd::SetViewRes(POINT pNewRes)
{
	TPictureWindow::SetViewRes(pNewRes);

    // Update Viewpoints
	ViewPointsScale();
};

void TMorphEditWnd::SetViewFrame(int nNewFrame)
{
	TAnimationWnd::SetViewFrame(nNewFrame);

    // Adjust MorphFrame
	if(ViewFrame == 0)
		MorphFrame = 0;
       else
		MorphFrame = (int)
			(((long)ViewFrame*
				(((TMorphWnd*)Parent)->GetMorphmesh()->GetFrames(MorphBlock)-1))
								/(GetFrames()-1));

    // Update Viewpoints
	ViewPointsScale();
};

void TMorphEditWnd::CMNew(RTMessage)
{
	// Set default values
    TAnimationOpt Options={{320,200},1};

    // If there's an earlier animation, use it's dimensions
	if(GetAnimation()){
		Options.Res    = GetAnimation()->GetResolution();
		Options.Frames = GetAnimation()->GetFrames();
        };

    // Open New Animation window
	if(GetModule()->ExecDialog( new
			TAnimationNewDlg(this,&Options))==IDOK)

		((TMorphWnd*)Parent)->GetMorphmesh()->
			PictureNew(AnimBlock,Options.Res,Options.Frames);
};

void TMorphEditWnd::CMOpen(RTMessage)
{ ((TMorphWnd*)Parent)->GetMorphmesh()->PictureLoad(MorphBlock,NULL); };
void TMorphEditWnd::CMSave(RTMessage)
{ ((TMorphWnd*)Parent)->GetMorphmesh()->PictureSave(MorphBlock); };
void TMorphEditWnd::CMSaveAs(RTMessage)
{ ((TMorphWnd*)Parent)->GetMorphmesh()->PictureSaveAs(MorphBlock); };

void TMorphEditWnd::AnimationChanged(int nPicture,Animation *pNewAnim)
{
	if(nPicture==AnimBlock)
		SetAnimation(pNewAnim,FALSE);
};

void TMorphEditWnd::AnimationFrameChanged(int nPicture,int nFrame)
{
	if(nPicture==AnimBlock)
		delete FetchViewmap(nFrame);
};

void TMorphEditWnd::InvalidateAnimFrame(int nPicture,int nFrame)
{
	if(nPicture==AnimBlock)
        if(nFrame==Anim_Frame)
			InvalidateRect(HWindow,NULL,FALSE);
};

void TMorphEditWnd::CMCut(RTMessage)
{
	((TMorphWnd*)Parent)->GetMorphmesh()->Cut(
		(MorphConfig.nTriangulation==ON_EDITION),MorphBlock,MorphFrame);
};

void TMorphEditWnd::CMCopy(RTMessage)
{
	((TMorphWnd*)Parent)->GetMorphmesh()->Copy(MorphBlock,MorphFrame,MorphFrame+1);
};

void TMorphEditWnd::CMCopyFrame(RTMessage)
{
	// Get pointer to Morphmesh
	TMorphmesh *Morphmesh = ((TMorphWnd*)Parent)->GetMorphmesh();

	((TMorphWnd*)Parent)->GetMorphmesh()->Copy(MorphBlock,
		(AC->nMarkIn *Morphmesh->GetFrames(MorphBlock))/AC->nFrames,
		(AC->nMarkOut*Morphmesh->GetFrames(MorphBlock))/AC->nFrames + 1);
};

void TMorphEditWnd::CMPaste(RTMessage)
{
	((TMorphWnd*)Parent)->GetMorphmesh()->Paste(
		(MorphConfig.nTriangulation==ON_EDITION),MorphBlock,MorphFrame);
};

void TMorphEditWnd::CMClear(RTMessage)
{
	if(((TMorphWnd*)Parent)->GetMorphing()==0){
        // Delete point
		((TMorphWnd*)Parent)->GetMorphmesh()->SelectionPointsDelete(
			(MorphConfig.nTriangulation==ON_EDITION),MorphBlock,MorphFrame);
		};
};

void TMorphEditWnd::CMTriangulate(TMessage&)
{
	((TMorphWnd*)Parent)->GetMorphmesh()->SelectionTriangulate(MorphBlock,MorphFrame);
};

void TMorphEditWnd::CMTriangulateAll(TMessage&)
{
	((TMorphWnd*)Parent)->GetMorphmesh()->TriangulateAll(MorphBlock,MorphFrame);
};

void TMorphEditWnd::CMAttachPoints(TMessage&){};

void TMorphEditWnd::CMDetachPointsOneFrame(TMessage&)
{
	// Get pointer to Morphmesh
	TMorphmesh *Morphmesh = ((TMorphWnd*)Parent)->GetMorphmesh();

	// Just one frame
	Morphmesh->SelectionDetachPoints(MorphBlock,MorphFrame,MorphFrame+1);
};

void TMorphEditWnd::CMDetachPointsSelectedFrames(TMessage&)
{
	// Get pointer to Morphmesh
	TMorphmesh *Morphmesh = ((TMorphWnd*)Parent)->GetMorphmesh();

	// Selected frames
	Morphmesh->SelectionDetachPoints(MorphBlock,
		(AC->nMarkIn *Morphmesh->GetFrames(MorphBlock))/AC->nFrames,
		(AC->nMarkOut*Morphmesh->GetFrames(MorphBlock))/AC->nFrames+1);
};

void TMorphEditWnd::CMDetachPointsOneBlock(TMessage&)
{
	// Get pointer to Morphmesh
	TMorphmesh *Morphmesh = ((TMorphWnd*)Parent)->GetMorphmesh();

	// One block
	Morphmesh->SelectionDetachPoints(MorphBlock,-1,-1);
};

void TMorphEditWnd::CMDetachPointsAllBlocks(TMessage&)
{
	// Get pointer to Morphmesh
	TMorphmesh *Morphmesh = ((TMorphWnd*)Parent)->GetMorphmesh();

	// All blocks
	Morphmesh->SelectionDetachPoints(-1,-1,-1);
};

void TMorphEditWnd::UpdateMenu(BOOL fActivate)
{
	TAnimationWnd::UpdateMenu(fActivate);

    /* Set Show points */
	CheckMenuItem(GetMenu(),CM_SHOW_POINTS,
	  (fShowPoints ? MF_CHECKED : MF_UNCHECKED ) | MF_BYCOMMAND);
	SetMenuItem(CM_SHOW_POINTS		,fActivate );

    /* Set Show triangles */
	CheckMenuItem(GetMenu(),CM_SHOW_TRIANGLES,
	  (fShowTriangles ? MF_CHECKED : MF_UNCHECKED ) | MF_BYCOMMAND);
	SetMenuItem(CM_SHOW_TRIANGLES	,fActivate );

	/* Set UseAnimationFrames */
	BOOL bMenuState = fActivate && (AC->nPlayState==ACPLAY_STOPPED);

	CheckMenuItem(GetMenu(),CM_USE_ANIMFRAMES,
	  (fAnimationFramesUsed ? MF_CHECKED : MF_UNCHECKED ) | MF_BYCOMMAND);
	SetMenuItem(CM_USE_ANIMFRAMES	,bMenuState );

	/* Set UseAnimationFrames */
	SetMenuItem(CM_DELETE_VIEW	,bMenuState );

	/* Update Morphblock */
	UpdateMenuList(bMenuState,7,MorphBlock);
	/* Update Animation */
	UpdateMenuList(bMenuState,8,AnimBlock);
};

void TMorphEditWnd::CMShowPoints(TMessage&)
{
	fShowPoints = !fShowPoints;

    // Update menu
    UpdateMenu(TRUE);

    // Repaint window
    InvalidateRect(HWindow,NULL,FALSE);
};

void TMorphEditWnd::CMShowTriangles(TMessage&)
{
	fShowTriangles = !fShowTriangles;

    // Update menu
	UpdateMenu(TRUE);

    // Repaint window
	InvalidateRect(HWindow,NULL,FALSE);
};

// Give mouse coordinates in window as parameter and get logical coordinates
// for the viewport 
POINT TMorphEditWnd::GetLogical(POINT p)
{
    // Add scroller position
	p.x += (int)Scroller->XPos;
	p.y += (int)Scroller->YPos;

    // Is this possible ?
/*	if(p.x >= ViewRes.x) p.x = ViewRes.x - 1;
	if(p.y >= ViewRes.y) p.y = ViewRes.y - 1;

	if(p.x < 0) p.x = 0;
	if(p.y < 0) p.y = 0;
*/
	return p;	
};

void TMorphEditWnd::WMLButtonDown(TMessage& Msg)
{
	// If this window is animating or morphing is in progress
	if((AC->nPlayState!=ACPLAY_STOPPED)||
		(((TMorphWnd*)Parent)->GetMorphing()>0)){
		// don't do anything
		TWindow::WMLButtonDown(Msg);
		return;
		};

	// Get pointer to Morphmesh
	TMorphmesh *Morphmesh = ((TMorphWnd*)Parent)->GetMorphmesh();

    // Get viewport coordinates
	pDragStart = GetLogical(MAKEPOINT(Msg.LParam));

    // Scale coordinates to 0.0 - 1.0
	pfDragRelative.x = 0.0;
	pfDragRelative.y = 0.0;

	pfDragStart.x = ((double)pDragStart.x)/ViewRes.x;
	pfDragStart.y = ((double)pDragStart.y)/ViewRes.y;

	int nMoveMode = NORMAL_MOVE;

	// If 'Z' was pressed
	if(GetKeyState('Z')<0)
    	nMoveMode = SCALING;
	if(GetKeyState('R')<0)
    	nMoveMode = ROTATION;

	if(nMoveMode==NORMAL_MOVE){
		// Check if there's a point under cursor
		int nPoint = GetPointIn(pDragStart);
		// If there were no points
		if(nPoint == NO_POINTS){

			Morphmesh->PointNew(pfDragStart,
				(MorphConfig.nTriangulation==ON_EDITION),MorphBlock,MorphFrame);
	        // Get the index for the new point 
			nPoint = GetPointIn(pDragStart);
			};

	    // If CTRL is not down, remove other selections
		if(GetKeyState(VK_CONTROL)>-1)
			// Clear old selections
			Morphmesh->SelectionClear();

		// Set/Reset point as selection
		Morphmesh->SelectionToggle(nPoint);
	};

	// Start dragging
	if(GetKeyState(VK_MENU)>-1){
		if(GetKeyState(VK_SHIFT)>-1)
			// Just one frame
			Morphmesh->SelectionMoveStart(nMoveMode,MorphBlock,MorphFrame,MorphFrame,MorphFrame);
		else 
            // Selected frames
			Morphmesh->SelectionMoveStart(nMoveMode,MorphBlock,
				(AC->nMarkIn *Morphmesh->GetFrames(MorphBlock))/AC->nFrames,
				MorphFrame,
				(AC->nMarkOut*Morphmesh->GetFrames(MorphBlock))/AC->nFrames);
	} else {
		if(GetKeyState(VK_SHIFT)>-1)
			// One block
			Morphmesh->SelectionMoveStart(nMoveMode,MorphBlock,-1,MorphFrame,-1);
		else
			// All blocks
			Morphmesh->SelectionMoveStart(nMoveMode,-1,-1,MorphFrame,-1);
	};

	// User starts dragging the mouse
	fTracking = TRUE;
    fDragStarted = FALSE;
    // Update title to tracking state
	UpdateTitle();
	// Takes over execution(runs own messageloop)
	TWindow::WMLButtonDown(Msg);
	fTracking = FALSE;
    // Update title to normal state
    UpdateTitle();

	// Stop dragging
	Morphmesh->SelectionMoveEnd();
};

void TMorphEditWnd::WMMouseMove(TMessage& Msg)
{
	if(fTracking){
        // Get viewport coordinates
		POINT pDragPoint = GetLogical(MAKEPOINT(Msg.LParam));

        // Change to relative coordinates
		pDragPoint.x -= pDragStart.x;
		pDragPoint.y -= pDragStart.y;

		if(!fDragStarted)
			if((abs(pDragPoint.x) >= MorphConfig.nPointDragStart)||
				(abs(pDragPoint.y) >= MorphConfig.nPointDragStart))
                fDragStarted = TRUE;

		if(fDragStarted){
	        // Scale coordinates to 0.0 - 1.0
			pfDragRelative.x = ((double)pDragPoint.x)/ViewRes.x;
			pfDragRelative.y = ((double)pDragPoint.y)/ViewRes.y;

	        // Move point
			((TMorphWnd*)Parent)->GetMorphmesh()->SelectionMove(pfDragStart,pfDragRelative);
		}

        // Update tracking title
		UpdateTitle();
		};
};

//////////////////////////////////////////////////
// ViewPoints
/////////////////////////////////////////////////////

void TMorphEditWnd::ViewPointsUpdate(int nNewPoints)
{
    // Delete old points
	nViewpoints = 0;
	ViewPointsResize(0);

    // Add new points
	for(int nIndex = 0 ; nIndex <  nNewPoints ; nIndex++ )
		ViewPointNew();

    // Update window
    if(HWindow)
		InvalidateRect(HWindow,NULL,FALSE);
};

void TMorphEditWnd::ViewPointNew(void)
{
    // Allocate more points if needed
	if(nViewpoints==nViewPointsAllocated)
		ViewPointsResize(nViewPointsAllocated+25);
	if(nViewpoints==nViewPointsAllocated)
    	return;		// Couldn't allocate more memory

	nViewpoints++;

	ViewPointUpdate(nViewpoints-1);
};

void TMorphEditWnd::ViewPointUpdate(int nPoint)
{
	ViewPointScale(nPoint);
	// Update window
	ViewPointInvalidate(nPoint);

};

void TMorphEditWnd::ViewPointInvalidate(int nPoint)
{
	static RECT R;

	R.left  = ViewPoints[nPoint].x;
	R.top   = ViewPoints[nPoint].y;
	R.right = ViewPoints[nPoint].x + 1;
	R.bottom= ViewPoints[nPoint].y + 1;

	R.left  -= ((int)Scroller->XPos + MorphConfig.nPointSize + MorphConfig.nKnotSize );
	R.right -= ((int)Scroller->XPos - MorphConfig.nPointSize - MorphConfig.nKnotSize );
	R.top   -= ((int)Scroller->YPos + MorphConfig.nPointSize + MorphConfig.nKnotSize );
	R.bottom-= ((int)Scroller->YPos - MorphConfig.nPointSize - MorphConfig.nKnotSize );

    // Update window
	InvalidateRect(HWindow,&R,FALSE);
};

void TMorphEditWnd::ViewPointDelete(int nPoint)
{
	assert(nPoint!=NO_POINTS);
	assert(nPoint<nViewpoints);

    // Update window
	ViewPointInvalidate(nPoint);

	// Delete point
	nViewpoints--;
	ViewPoints[nPoint] = ViewPoints[nViewpoints];

    // Free points if not needed
	if((nViewPointsAllocated-nViewpoints)>25)
		ViewPointsResize(nViewPointsAllocated-25);
};

void TMorphEditWnd::ViewPointsResize(int nNewSize)
{
	assert(nNewSize >= nViewpoints);

	POINT *NewViewPoints = NULL;

    do {
		// Assure that nNewsize is a positive number
		nNewSize = max(nNewSize,0);

		if(nNewSize > 0){
			NewViewPoints = new POINT [nNewSize];
			if(!NewViewPoints){
				assert(FALSE);
				break;
                };

			// Copy old viewpoints
			if(ViewPoints)
				for(int i = 0 ; i < min(nNewSize,nViewpoints) ; i++)
					NewViewPoints[i] = ViewPoints[i];	
			};

		// Assign new viewpoints
		SWAP(POINT *,ViewPoints,NewViewPoints);
		SWAP(int,nViewPointsAllocated,nNewSize);
        }
	while(FALSE);

    // Delete ViewpointArray not needed
	if(NewViewPoints)
		delete NewViewPoints;
};

void TMorphEditWnd::ViewPointScale(int nPoint)
{
	// Get Point and scale it
	ViewPoints[nPoint] =
		((TMorphWnd*)Parent)->
			GetMorphmesh()->
				Point(MorphBlock,nPoint)->
					Scale(MorphFrame,ViewRes);
};

void TMorphEditWnd::ViewPointsScale(void)
{
    // Scale all points
	for(int i = 0 ; i < nViewpoints ; i++ )
		ViewPointScale(i);

    // Update window
    if(HWindow)
		InvalidateRect(HWindow,NULL,FALSE);
};

// Checks if pointindex i is inside POINT+-PointSize
BOOL TMorphEditWnd::IsPointIn(int pIndex,POINT pArea)
{
	if(pArea.x< ViewPoints[pIndex].x - MorphConfig.nPointDragSize) return FALSE;
	if(pArea.y< ViewPoints[pIndex].y - MorphConfig.nPointDragSize) return FALSE;
	if(pArea.x> ViewPoints[pIndex].x + MorphConfig.nPointDragSize) return FALSE;
	if(pArea.y> ViewPoints[pIndex].y + MorphConfig.nPointDragSize) return FALSE;

	return TRUE;
};

// Finds a first viewpoint that encloses point
int TMorphEditWnd::GetPointIn(POINT nPoint)
{
    for(int nIndex = 0 ; nIndex < nViewpoints ; nIndex++)
		if(IsPointIn(nIndex,nPoint)==TRUE)
			return nIndex;
	return NO_POINTS;
};

// Selection functions

void TMorphEditWnd::ConnectedRectAddPoints(ItemList *SelectionList)
{
	// Clear selected rectangle from Morphwindow
	rConnectedRect.left  = 16384;
	rConnectedRect.top   = 16384;
	rConnectedRect.right =    -1;
	rConnectedRect.bottom=    -1;

	for(int nIndex = 0 ; nIndex < SelectionList->GetItems() ; nIndex ++ ){
		int nConnected = SelectionList->GetItem(nIndex);
        // If point is connected
        if((nConnected < SL_SELECTED) && (nConnected > 0 ))
			// Add new point to Morphwindows selected rectangle
			RectAddPoint(rConnectedRect,nIndex);
		};
};

void TMorphEditWnd::RectAddPoint(RECT& rRect,int nPoint)
{
	POINT pPoint = ViewPoints[nPoint];

    // Add point to selection rectangle
	rRect.left  = min(rRect.left  ,pPoint.x);
	rRect.right = max(rRect.right ,pPoint.x);
	rRect.top   = min(rRect.top   ,pPoint.y);
	rRect.bottom= max(rRect.bottom,pPoint.y);
};

void TMorphEditWnd::SelectedRectAddPoints(RECT& rInvalidate,ItemList *SelectionList)
{
	// Add selection points
	for(int nIndex = 0; nIndex < SelectionList->GetItems() ; nIndex++){
        if(SelectionList->GetFlag(nIndex,SL_SELECTED)){
			// Invalidate all objects connecting with Selection only in one frame
			// In morphwindows SelPoint will be a viewpoint
			RectAddPoint(rInvalidate,nIndex);
			};
        };
};

void TMorphEditWnd::MoveRectToWindowCoordinates(RECT& rRect)
{
	// Move to window coordinates
	rRect.left  -= ( MorphConfig.nPointSize + MorphConfig.nKnotSize + (int)Scroller->XPos);
	rRect.right += ( MorphConfig.nPointSize + MorphConfig.nKnotSize - (int)Scroller->XPos + 1 );
	rRect.top   -= ( MorphConfig.nPointSize + MorphConfig.nKnotSize + (int)Scroller->YPos);
	rRect.bottom+= ( MorphConfig.nPointSize + MorphConfig.nKnotSize - (int)Scroller->YPos + 1 );
};

void TMorphEditWnd::ViewPointScale(ItemList *SelectionList)
{
	for(int nIndex = 0; nIndex < SelectionList->GetItems() ; nIndex++)
   	    if(SelectionList->GetFlag(nIndex,SL_SELECTED))
			// Move selected point
			ViewPointScale(nIndex);
};

void TMorphEditWnd::SelectionMoved(int nSelectionBlock,ItemList *SelectionList)
{
	if(nSelectionBlock==MorphBlock){
        // Init update rectangle with selected rect
		RECT rInvalidate = rConnectedRect;

		// Add 'before'- rectangle to updating
		SelectedRectAddPoints(rInvalidate,SelectionList);

        // Update moved selection viewpoints
		ViewPointScale(SelectionList);

		// Add 'after'- rectangle to updating
		SelectedRectAddPoints(rInvalidate,SelectionList);

        // Add scroller pos + point sizes
		MoveRectToWindowCoordinates(rInvalidate);

		// Update window with rectangle
		InvalidateRect(HWindow,&rInvalidate,FALSE);
		};
};

/* End of file MORPHEDW.CPP */

/**********************************************************************
 * 
 *  NAME:         MORPHWIN.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          11. 8.95 MHM added headers 
 *           6. 9.95 MHM removed Interpolation button
 *  1.9466  10.12.95 MHM removed ModulePath etc., new CreateModulePath 
 *  1.9471  11.12.95 MHM renamed morphstr.h to morphmsh.h
 *********************************************************************/
 
#ifndef MORPHWIN_H
#define MORPHWIN_H

#ifndef __WINDOWS_H
	#include <windows.h>
#endif

#ifndef __OWL_H
	#include <owl.h>
#endif

#include "winmorph.rch"
#include "geom2d.h"
#include "tstruct.h"

class TMorphEditWnd; 	// morphedw.h
class TMorphPictureWnd; // morphpic.h
class TAnimationWnd; 	// animwnd.h
class TACWindow;        // acwindow.h
class ACContext;		// accontex.h
class ACMorphContext;	// acmorph.h
class MorphPoint;    	// tstruct.h
class TMorphmesh;		// morphmsh.h
class TMorphmeshUndo;
class ItemList;			// itemlist.h
class Animation;
class TClipBoard;

// For nMorphing and nPlaying semaphores
#define	LOCK_START	+1
#define LOCK_END	-1

#define IterateMorphWnds(Index)                                                		\
	for(TWindow *Last = NULL,*Index = ((ActiveChild )?(TWindow*)ActiveChild->Next(): NULL) ;     \
			(Last != ActiveChild) && Index ;                                                   \
            Last = Index,                                                           \
			Index = (TWindow *)Index->Next() )                                      \
		if(Index->isA() == morphwndClass)

class TMorphWnd : public TMDIFrame
{
	friend ACMorphContext; // So it has access to get the right resources
protected:
	int      nMorphing;
	int      nPlaying;

	LPSTR			  szProgName;
	TMorphPictureWnd *ResultWnd;
	TACWindow     	 *ACWindow;
	ACMorphContext	 *ACDefault;
	TMorphmeshUndo  *Morphmesh;
	HMENU hViewMenu;

	TClipBoard *pClipBoard;

	void  LoadMorphmesh			(LPSTR szFilename);

	BOOL  IsMorphWindowActive	(void);
	int   GetMorphWindowID		(TWindow *Window);
	void  SendMessageToAllChildren (UINT message,WPARAM wParam,LPARAM lParam);
	LPSTR CreateModulePath		(LPSTR szFilename);

	void  UpdateMenuState		(void);
	void  UpdateUndoMenuItem	(int nMenuItem,LPSTR szMenuName,BOOL fEnabled,LPSTR szUndoName,LPSTR szKeyName);
	void  SetMenuItem			(int nMenuItem,BOOL bState);
	void  FileMenuUpdate		(void);
	void  UpdateTitle			(void);

	void  LoadMeshPictures		(void);
	void  SavePictureToMorphmesh(int nWindow);
public:
	TMorphWnd(LPSTR szTitle);
	~TMorphWnd();

	virtual void  InitClientWindow	(void);
	virtual void  SetupWindow		(void);
	virtual void  GetWindowClass	(WNDCLASS& wc);
	virtual LPSTR GetClassName		(void);
	virtual BOOL  CanClose			(void);

	virtual void DefCommandProc		(RTMessage);

	virtual void WMQueryNewPalette	(RTMessage)=[WM_FIRST + WM_QUERYNEWPALETTE];
	virtual void WMPaletteChanged	(RTMessage)=[WM_FIRST + WM_PALETTECHANGED];

	TMorphmesh*  GetMorphmesh		(void){ return (TMorphmesh*)Morphmesh; };
	void  SetMorphmesh				(TMorphmeshUndo* NewMorph);
	void  SavePicturesToMorphmesh	(void);
	TMorphEditWnd *MakeMorphWindow	(void);

	void  SetClipBoard				(TClipBoard *pNewClipBoard);
	TClipBoard *GetClipBoard		(void) { return pClipBoard; };

	void  UpdateACContext			(void);
	void  UpdateUndoMenu			(void);
	void  UpdateViewMenu			(void);
	void  UpdateSelectionMenu		(void);
	void  UpdatePasteMenu			(void);

	void  SetMorphing(int nLock){ nMorphing += nLock;UpdateMenuState();};
    int   GetMorphing(void)		{ return nMorphing;};
	void  SetPlaying(int nLock)	{ nPlaying += nLock;UpdateMenuState();};
	int   GetPlaying(void)		{ return nPlaying;};

	HMENU GetViewMenu(void)		{ return hViewMenu; };

	// Menu commands
	 // File menu
	virtual void CMFileNew			(RTMessage) = [CM_FIRST + CM_FILENEW];
	virtual void CMFileOpen			(RTMessage) = [CM_FIRST + CM_FILEOPEN];
	virtual void CMFileSave			(RTMessage) = [CM_FIRST + CM_FILESAVE];
	virtual void CMFileSaveAs		(RTMessage) = [CM_FIRST + CM_FILESAVEAS];

    // Edit menu
	virtual void CMUndo				(RTMessage) = [CM_FIRST + CM_UNDO];
    virtual void CMRedo				(RTMessage) = [CM_FIRST + CM_REDO];
	virtual void CMSelectAll		(RTMessage) = [CM_FIRST + CM_SELECTALL];
	virtual void CMUndoBufferSize	(RTMessage) = [CM_FIRST + CM_UNDO_BUFFER_SIZE];
	virtual void CMRedoEnabled		(RTMessage) = [CM_FIRST + CM_REDO_ENABLED];

	// Frames menu
	virtual void CMStretchFrames			(RTMessage)		= [CM_FIRST + CM_STRETCH_FRAMES];
	virtual void CMKeepFramesTogether		(RTMessage)		= [CM_FIRST + CM_FRAMES_THE_SAME];
	virtual void CMSwapFrames1				(RTMessage)		= [CM_FIRST + CM_SWITCH_FRAMES_1];
	virtual void CMSwapFrames2				(RTMessage)		= [CM_FIRST + CM_SWITCH_FRAMES_2];
	virtual void CMSwapFrames3				(RTMessage)		= [CM_FIRST + CM_SWITCH_FRAMES_3];

    // Triangles menu
	virtual void CMTrianglesDeleteBetweenPoints(RTMessage)	= [CM_FIRST + CM_DELTRIBETWEEN];
	virtual void CMTrianglesDeleteWithPoints(RTMessage)		= [CM_FIRST + CM_DELTRIWITH];
	virtual void CMTriDuringEdit			(RTMessage)		= [CM_FIRST + CM_TRI_DURING_EDIT];
	virtual void CMTriBeforeMorph			(RTMessage)		= [CM_FIRST + CM_TRI_BEFORE_MORPH];
	virtual void CMTriOnCommand				(RTMessage)		= [CM_FIRST + CM_TRI_ON_COMMAND];

    // Points menu
	virtual void CMIntDuringEdit			(RTMessage)		= [CM_FIRST + CM_INT_DURING_EDIT];
	virtual void CMIntAfterEdit				(RTMessage)		= [CM_FIRST + CM_INT_AFTER_EDIT];
	virtual void CMCreateGrid  				(RTMessage)		= [CM_FIRST + CM_CREATE_GRID];
	virtual void CMAutoGrid    				(RTMessage)		= [CM_FIRST + CM_AUTO_GRID];
	virtual void CMSetGridSize 				(RTMessage)		= [CM_FIRST + CM_GRID_SIZE];
	virtual void CMComputeInterpolate		(RTMessage)		= [CM_FIRST + CM_COMPUTE_INTERPOLATE];
	virtual void CMComputeSphere			(RTMessage)		= [CM_FIRST + CM_COMPUTE_SPHERE];
	virtual void CMComputeEarth				(RTMessage)		= [CM_FIRST + CM_COMPUTE_EARTH];
	virtual void CMComputeWhirlpool			(RTMessage)		= [CM_FIRST + CM_COMPUTE_WHIRLPOOL];
	virtual void CMComputeYinYang			(RTMessage)		= [CM_FIRST + CM_COMPUTE_YINYANG];
	virtual void CMAutoInterpolateFrames	(RTMessage)		= [CM_FIRST + CM_AUTOINTERFRAMES];

	// Config menu
	virtual void CMSetPointSize				(RTMessage) = [CM_FIRST + CM_SETPOINTSIZE];
	virtual void CMSetPointDragSize			(RTMessage) = [CM_FIRST + CM_SETPOINTDRAG];
	virtual void CMSetKnotSize				(RTMessage) = [CM_FIRST + CM_SET_KNOT_SIZE];
	virtual void CMPointShapeRect			(RTMessage) = [CM_FIRST + CM_POINT_SHAPE_RECT];
	virtual void CMPointShapeCircle			(RTMessage) = [CM_FIRST + CM_POINT_SHAPE_CIRCLE];
	virtual void CMSetPointDragStart		(RTMessage) = [CM_FIRST + CM_POINT_DRAG_START];

	virtual void CMSetPointColor			(RTMessage) = [CM_FIRST + CM_POINT_COLOR];
	virtual void CMSetSelectedColor			(RTMessage) = [CM_FIRST + CM_SELECTED_COLOR];
	virtual void CMSetLineColor				(RTMessage) = [CM_FIRST + CM_LINE_COLOR];
	virtual void CMBeepWhenFinished			(RTMessage)	= [CM_FIRST + CM_BEEP];

	virtual void CMShowProjector			(RTMessage) = [CM_FIRST + CM_SHOW_PROJECTOR];
	virtual void CMMakeView				(RTMessage) = [CM_FIRST + CM_MAKE_VIEW];

	// Help menu
	virtual void CMAbout			(RTMessage) = [CM_FIRST + CM_HELP_ABOUT];
	virtual void CMHelpIndex		(RTMessage) = [CM_FIRST + CM_HELP_INDEX];
	virtual void CMHelpOnHelp		(RTMessage) = [CM_FIRST + CM_HELP_ON_HELP];

//////////////////////////////
// Morphmesh->MorphWindows
//////////////////////////////

	void InvalidateAll			(void);
	void FramesChanged			(void);
	void PointAdded				(void);
	void PointDeleted			(int nPoint);
	void PointInvalidate		(int nPoint);
	void InvalidateTriangle		(TTriangle &Tri);
	void ViewPointUpdate		(int nPoint);
	void ViewPointsUpdate		(int nNewPoints);
	void ConnectedRectAddPoints	(ItemList* SelectionList);
	void SelectionMoved			(int nBlock,ItemList* SelectionList);
	void AnimationChanged		(int nPicture,Animation *pNewAnim);
	void AnimationFrameChanged	(int nPicture,int nFrame);
	void InvalidateAnimFrame	(int nPicture,int nFrame);
};

#endif 
 
/* End of file MORPHWIN.H */ 
 

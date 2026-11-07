/********************************************************************** 
 * 
 *  NAME:         MORPHEDW.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          11. 8.95 MHM added headers 
 *  1.9471  11.12.95 MHM renamed morphstr.h to morphmsh.h
 *********************************************************************/ 
 
#ifndef MORPHEDIT_H
#define MORPHEDIT_H

#include "common.h"
#include "animwnd.h"
#include "morphmsh.h"

///////////////////////////////////////////////7
// TMorphEditWnd
//////////////////////////////////////////////////

class MorphPoint;
class TMorphWindowConfig;

class TMorphEditWnd : public TAnimationWnd
{
protected:
	BOOL	fAnimationFramesUsed;
	int 	MorphBlock;
    int		AnimBlock;
    int		MorphFrame;
	BOOL	fShowPoints;
    BOOL	fShowTriangles;

	virtual int GetNewFrames(void);

public:
	TMorphEditWnd(PTWindowsObject AParent,LPSTR Name,ACAnimContext *);
	~TMorphEditWnd();

	virtual void Paint(HDC, PAINTSTRUCT&);
	virtual void DefCommandProc(RTMessage Msg);

	virtual int   LoadCfgKeyword(wmfstream& file,LPSTR szKeyWord);
	virtual void  SaveCfgKeyword(wmfstream& file);

	virtual void  WMMenuInit	 	(RTMessage) = [WM_FIRST + WM_INITMENU];

	virtual void CMNew			(RTMessage) = [CM_FIRST + CM_ANIM_NEW];
	virtual void CMOpen			(RTMessage) = [CM_FIRST + CM_ANIM_OPEN];
	virtual void CMSave			(RTMessage) = [CM_FIRST + CM_ANIM_SAVE];
	virtual void CMSaveAs		(RTMessage) = [CM_FIRST + CM_ANIM_SAVEAS];

	virtual classType isA() const { return morphwndClass;};
	virtual	BOOL CanMorph		(void);
	virtual void SetupWindow	(void);
	virtual void SetViewFrame	(int Frame);
	virtual void SetViewRes		(POINT NewRes);
	virtual void UpdateTitle	(void);
	virtual void UpdateMenu		(BOOL fActivate);
			void AnimationChanged(int nPicture,Animation *pNewAnim);
	virtual void AnimationFrameChanged	(int nPicture,int nFrame);
	virtual void InvalidateAnimFrame	(int nPicture,int nFrame);
	virtual void SetMorphBlock	(int nBlock);
	virtual void SetAnimBlock	(int nBlock);

	virtual void CMCut			(RTMessage) = [CM_FIRST + CM_CUT];
	virtual void CMCopy			(RTMessage) = [CM_FIRST + CM_COPY];
	virtual void CMCopyFrame	(RTMessage) = [CM_FIRST + CM_COPY_FRAMES];
	virtual void CMPaste		(RTMessage) = [CM_FIRST + CM_PASTE];
	virtual void CMClear		(RTMessage) = [CM_FIRST + CM_CLEAR];
	virtual void CMTriangulate	(TMessage&) = [CM_FIRST + CM_TRIANGULATE];
	virtual void CMTriangulateAll(TMessage&)= [CM_FIRST + CM_TRIANGULATEALL];
	virtual void CMAttachPoints (TMessage&) = [CM_FIRST + CM_ATTACHPOINTS];
	virtual void CMDetachPointsOneFrame 	(TMessage&) = [CM_FIRST + CM_DETACHPOINTS_ONE_FRAME];
	virtual void CMDetachPointsSelectedFrames (TMessage&)=[CM_FIRST + CM_DETACHPOINTS_SEL_FRAMES];
	virtual void CMDetachPointsOneBlock		(TMessage&) = [CM_FIRST + CM_DETACHPOINTS_ONE_BLOCK];
	virtual void CMDetachPointsAllBlocks	(TMessage&) = [CM_FIRST + CM_DETACHPOINTS_ALL_BLOCKS];
	virtual void CMUseAnimFrames(RTMessage) = [CM_FIRST + CM_USE_ANIMFRAMES];
	virtual void CMShowPoints   (TMessage&) = [CM_FIRST + CM_SHOW_POINTS];
	virtual void CMShowTriangles(TMessage&) = [CM_FIRST + CM_SHOW_TRIANGLES];
	virtual void CMDeleteView	(TMessage&) = [CM_FIRST + CM_DELETE_VIEW];

	virtual void WMLButtonDown	(TMessage&) = [WM_FIRST + WM_LBUTTONDOWN];
	virtual void WMMouseMove	(TMessage&) = [WM_FIRST + WM_MOUSEMOVE];


//////////////////////////////////
// Viewpoints
/////////////////////////////////
protected:
	POINT 	GetLogical(POINT p);

	POINT*	ViewPoints;
	int		nViewPointsAllocated;
	int		nViewpoints;

	void	ViewPointsResize	(int nNewSize);
	void	ViewPointScale 		(int nPoint);
	void 	ViewPointsScale		(void);
public:
	void 	ViewPointsUpdate	(int nNewPoints);
	void 	ViewPointUpdate		(int nPoint);
	void	ViewPointNew   		(void);
	void	ViewPointDelete		(int nPoint);
	void	ViewPointInvalidate	(int nPoint);

	void 	ViewPointsScale(int nFrame,POINT pResolution);

//////////////////////////////////
// Selection
/////////////////////////////////
protected:
	BOOL	fTracking;
	BOOL	fDragStarted;

	POINT	pDragStart;
	Point2d pfDragStart;
	Point2d pfDragRelative;

	RECT	rConnectedRect;

	int		GetPointIn	(POINT pArea);
	BOOL	IsPointIn	(int nIndex,POINT pPoint);

	void	SelectedRectAddPoints(RECT& rInvalidate,ItemList *SelectionList);
	void	MoveRectToWindowCoordinates(RECT& rInvalidate);
	void	RectAddPoint(RECT& rRect,int nPoint);

public:
	void	ConnectedRectAddPoints(ItemList *SelectionList);

	void	SelectionMoved(int nBlock,ItemList *SelectionList);
	void	ViewPointScale(ItemList *SelectionList);
};


#endif 
 
/* End of file MORPHEDW.H */ 
 

/********************************************************************** 
 * 
 *  NAME:         MORPHMSH.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          11. 8.95 MHM added headers
 *  1.9470  11.12.95 MHM changed creation from (int) to (int,int,int)
 *                         (StartFrames, ResultFrames, EndFrames )
 *  1.9471  11.12.95 MHM renamed from morphstr.h
 *********************************************************************/ 
 
#ifndef MORPHSTR_H
#define MORPHSTR_H

#ifndef __WINDOWS_H
	#include <windows.h>
#endif
#include "morfdata.h"

/////////////////////////////////////////////////////////////////////
// TMorphmesh
/////////////////////////////////////////////////////////////////////

class TMorphWnd;
class ItemList;
class TPointMoveUndo;
class TClipBoard;

// MoveModes
#define NORMAL_MOVE	0
#define SCALING		1
#define ROTATION	2

class TMorphmesh : public TMorphmesh_Data
{
	friend class TPointAddUndo;
	friend class TPointDeleteUndo;
	friend class TPointMoveUndo;
	friend class TTriangulateUndo;
	friend class TDeleteTrianglesUndo;
	friend class TComputeResultUndo;
	friend class TDetachPointsUndo;
	friend class TPastePointUndo;
	friend class TPasteFrameUndo;
	friend class TCutUndo;
	friend class TClipBoardFrames;
	friend class TClipBoardPoints;
	friend class TStretchUndo;
	friend class TCreateGridUndo;
protected:
	TMorphWnd   *MorphWnd;

public:
	TMorphmesh();
	TMorphmesh(int nStartFrames,int nResultFrames,int nEndFrames);
	~TMorphmesh();

	static TMorphmesh* Create  (int nStartFrames,int nResultFrames,int nEndFrames);
	static TMorphmesh* Create  (LPSTR szFilename);
	virtual void Reset         (void);

	void AttachMorphWindow     (TMorphWnd*);
	void DetachMorphWindow     (void);
	virtual void SetAnimation   (int nPicture,Animation *pNewAnim);

	virtual void AnimationFrameChanged	(int nPicture,int nFrame);
	virtual void InvalidateAnimFrame	(int nPicture,int nFrame);

	virtual BOOL LoadDocument   (wmfstream& file);
	virtual void SaveDocument   (wmfstream& file);

	void Paint              	(HDC hDC,int nBlock,int nFrame,POINT ViewPoints[],BOOL fShowPoints,BOOL fShowTriangles);

	virtual void SwapPoints		(int nFirst,int nSecond);
	virtual void PointNew  		(Point2d pPoint);
	virtual void PointNew		(MorphPoint *pStart,int nStart,	MorphPoint *pEnd,int nEnd,
								MorphPoint *pResult,int nResult );
	virtual void PointNew  		(Point2d pPoint,BOOL Triangulate,int nBlock,int nFrame);
	virtual void PointNew		(MorphPoint *pStart,int nStart,	MorphPoint *pEnd,int nEnd,
								MorphPoint *pResult,int nResult,BOOL fTriangulate,int nBlock,int nFrame);
	virtual void PointDelete	(int nPoint);
	virtual void PointsDelete	(ItemList *pPointList,BOOL fTriangulate,int nBlock,int nFrame);
	virtual void PointFetch		(int nPoint,TMorphmeshPoints *pDestMesh);
	virtual void PointsFetch	(ItemList *PointList,TMorphmeshPoints *pDestMesh);
	virtual void PointsFetch	(ItemList *PointList,BOOL fTriangulate,int nBlock,int nFrame,TMorphmeshPoints *pDestMesh);

	virtual void PointsRestore	(TMorphmeshPoints *pSrcMesh,ItemList *pPointList);

	virtual void SwapBlocks		(int nBlock1,int nBlock2);

	void TriangleAdd       		(TTriangle&);
	void TriangleDelete    		(TTriangle&);

	virtual void Interpolate    (int nBlock);
	virtual void Interpolate    (int nBlock,int nPoint);
	virtual void Interpolate	(int nBlock,ItemList *pPointList);
	virtual void Triangulate	(int nBlock,int nFrame);
	virtual void TriangulateAll	(int nBlock,int nFrame);
	virtual void Compute		(TFunction2d *pFunction );
	virtual BOOL StretchFrames  (int nNewFrames[3]);
	virtual void DetachPoints	(ItemList *pPointList,int nBlock,int nStartFrame,int nEndFrame);
	virtual void CreateGrid		(int nXGrid,int nYGrid,BOOL fTriangulate);

	virtual void Cut			(BOOL fTriangulate,int nSelBlock,int nSelFrame);
	virtual void Copy			(int nBlock,int nStartFrame,int nEndFrame);
	virtual void Paste			(BOOL fTriangulate,int nSelBlock,int nSelFrame);
	virtual void Paste			(TClipBoard *,BOOL fTriangulate,int nSelBlock,int nSelFrame);

/////////////////////////////
// Stuff for selected point
////////////////////////////
protected:
	ItemList *SelectionList;
    int			nSelections;

	TPointMoveUndo *pMoveUndo;

	void ClearConnections();
	void UpdateConnections(TTriangle &tTri,int nDirection);
	void UpdateConnections(int nPoint,int nDirection);
	void SelectionToggle(int nSelection,BOOL fSelect);

public:
    BOOL SelectionsAre		(void){return nSelections > 0 ;};
	void SelectionClear		(void);
	void SelectionAdd		(int nNewSelection);
	void SelectionAddAll	(void);
	void SelectionRemove	(int nSelection);
	void SelectionToggle	(int nSelection);

	virtual void SelectionMoveStart	(int nMoveMode,int nBlock,int nStartFrame,int nCrrntFrame,int nEndFrame);
	virtual void SelectionMove		(Point2d pOrigin,Point2d pRelative);
	virtual void SelectionMoveEnd	(void);

	virtual void SelectionPointsDelete		(BOOL Triangulate,int nBlock,int nFrame);
	virtual void SelectionTriangulate		(int bBlock,int nFrame);
	virtual void SelectionDetachPoints		(int nBlock,int nStartFrame,int nEndFrame);
	virtual void TrianglesDeleteBetweenPoints(void);
	virtual void TrianglesDeleteWithPoints	(void);

	virtual void TrianglesDeleteWithPoints	(ItemList *pPoints, int nMinConnections,int nMaxConnections);
};

#endif
 

/* End of file MORPHMSH.H */ 
 

/**********************************************************************
 * 
 *  NAME:         MORPHUND.H
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  1.9471  25.02.96 MHM created
 *********************************************************************/
 
#ifndef MORPHUND_H
#define MORPHUND_H

#include "morphmsh.h"

class TMorphmeshUndo;
class ItemList;

class TUndo
{
	BOOL	fRedoEnabled;
public:
    TUndo *pNext,*pPrevious;
	TMorphmeshUndo *pMesh;

	TUndo	(TMorphmeshUndo *pNewMesh)
	{
		pMesh = pNewMesh;
		pNext = NULL;
		pPrevious = NULL;
        fRedoEnabled = TRUE;
	};
	virtual ~TUndo	()		{};

	virtual LPSTR GetName(void) = 0;
	virtual void  Undo	(void) = 0;
	virtual void  Do	(void);
	virtual void  DeleteRedo(void);
	virtual BOOL  CanRedo(void){ return fRedoEnabled; };
};

class TPointAddUndo : public TUndo
{
protected:
	Point2d Point;
	int					nBlock;
    int					nFrame;
	TMorphmeshTriangles *pUndoMesh;
public:
	TPointAddUndo	(TMorphmeshUndo *pMesh,Point2d Point,
		BOOL Triangulate,int nBlock,int nFrame); 
	~TPointAddUndo	();
	virtual LPSTR GetName(void){ return "Add point"; };
	virtual void  Undo	(void);
	virtual void  Do	(void);
}; 

class TPointDeleteUndo : public TUndo {
protected:
	ItemList 			*pPointList;
	int					nBlock;
    int					nFrame;
	TMorphmesh_Data		*pUndoMesh;
public:
	TPointDeleteUndo	(TMorphmeshUndo *pMesh,
		BOOL Triangulate,int nBlock,int nFrame);
	~TPointDeleteUndo	();
	virtual LPSTR GetName(void){ return "Delete points"; };
	virtual void  Undo	(void);
	virtual void  Do	(void);
}; 

class TPointMoveUndo : public TUndo {
protected:
    ItemList*	pPointList;
	int		nMoveMode;
	int		nStartFrame;
    int		nCrrntFrame;
	int		nEndFrame;

	MorphBlock *pBlock[3];

public:
	TPointMoveUndo(TMorphmeshUndo *pMesh,ItemList *pPointList,
		int nNewMoveMode,int nNewBlock,int nNewStartFrame,int nNewCrrntFrame,int nNewEndFrame);
	~TPointMoveUndo	();
	virtual LPSTR GetName	(void);
	virtual void  Undo		(void);
	virtual void  Do		(void);

	BOOL		fInteractive;
    BOOL 		fSelectionMoved;
	int			nBlock;
	Point2d		pOrigin;
	Point2d		pRelative;
};

class TCutUndo : public TUndo {
protected:
	BOOL				fClipped;
	ItemList 			*pPointList;
	int					nBlock;
    int					nFrame;
	TMorphmesh_Data		*pUndoMesh;
public:
	TCutUndo	(TMorphmeshUndo *pMesh,
		BOOL Triangulate,int nBlock,int nFrame);
	~TCutUndo	();
	virtual LPSTR GetName(void){ return "Cut"; };
	virtual void  Undo	(void);
	virtual void  Do	(void);
};


class TPastePointUndo : public TUndo {
protected:
	TClipBoardPoints *pClipBoard;

	ItemList 			*pPointList;
	int					nBlock;
    int					nFrame;
	TMorphmeshTriangles *pUndoMesh;
public:
	TPastePointUndo(TMorphmeshUndo *pMesh,BOOL fTriangulate,int nBlock,int nFrame);
	~TPastePointUndo	();
	virtual LPSTR GetName(void){ return "Paste"; };
	virtual void  Undo	(void);
	virtual void  Do	(void);
}; 

class TPasteFrameUndo : public TUndo {
protected:
	TClipBoardFrames *pClipBoard;

	ItemList*	pPointList;
	int			nBlock;
	int			nStartFrame;
	int			nEndFrame;
	MorphBlock *pBlock;
public:
	TPasteFrameUndo	(TMorphmeshUndo *pMesh,ItemList *pNewPointList,int nNewBlock,int nNewFrame);
	~TPasteFrameUndo	();
	virtual LPSTR GetName(void){ return "Paste"; };
	virtual void  Undo	(void);
	virtual void  Do	(void);
};

class TSwapBlocksUndo : public TUndo
{
protected:
	int		nBlock1;
	int		nBlock2;
public:
	TSwapBlocksUndo			(TMorphmeshUndo *pMesh,int nNewBlock1,int nNewBlock2);
	~TSwapBlocksUndo		();
	virtual LPSTR GetName	(void){ return "Swap blocks"; };
	virtual void  Undo		(void);
	virtual void  Do		(void);
};

class TDetachPointsUndo : public TUndo
{
protected:
	int 	nBlock;
	int		nStartFrame;
	int		nEndFrame;
    ItemList	*pPointList;
	MorphBlock 	*pBlock[3];
public:
	TDetachPointsUndo		(TMorphmeshUndo *pMesh,ItemList* pPointList,int nBlock,int nStartFrame,int nEndFrame);
	~TDetachPointsUndo		();
	virtual LPSTR GetName	(void){ return "Detach points"; };
	virtual void  Undo		(void);
	virtual void  Do		(void);
};

class TComputeResultUndo : public TUndo
{
protected:
	TFunction2d *pFunction;
	MorphBlock *pBlock;
public:
	TComputeResultUndo	(TFunction2d *pFunction,TMorphmeshUndo *pMesh);
	~TComputeResultUndo	();
	virtual LPSTR GetName	(void){ return pFunction->GetName(); };
	virtual void  Undo		(void);
	virtual void  Do		(void);
};

class TStretchUndo : public TUndo
{
	MorphBlock *pBlock[3];
    int nRedoFrames[3];

public:
	TStretchUndo	(TMorphmeshUndo *pMesh, int nNewFrames[3]);
	~TStretchUndo	();

	virtual LPSTR GetName(void){ return "Stretching"; };
	virtual void  Undo	(void);
	virtual void  Do	(void);
};

class TTriangulateUndo : public TUndo
{
	TTriangle			tTriangle;
	int					nBlock;
    int					nFrame;
	TMorphmeshTriangles *pUndoMesh;
public:
	TTriangulateUndo	(TMorphmeshUndo *pMesh,int nBlock,int nFrame);
	~TTriangulateUndo	();

	virtual LPSTR GetName(void){ return "Triangulating"; };
	virtual void  Undo	(void);
	virtual void  Do	(void);
};

class TDeleteTrianglesUndo : public TUndo
{
protected:
	TMorphmeshTriangles *pUndoMesh;

	ItemList *Selection;
	int nMinConnections;
    int nMaxConnections;
public:
	TDeleteTrianglesUndo		(TMorphmeshUndo *pMesh,ItemList *pSelection,int nMin,int nMax);
	~TDeleteTrianglesUndo		();
	virtual LPSTR GetName	(void){ return "Delete triangles"; };
	virtual void  Undo		(void);
	virtual void  Do		(void);
	virtual void  DeleteRedo(void);
};

class TCreateGridUndo : public TUndo
{
protected:
	TMorphmesh_Data		*pUndoMesh;

	int fTriangulate;
	int nXGrid;
    int nYGrid;
public:
	TCreateGridUndo		(TMorphmeshUndo *pMesh,int nXGrid,int nYGrid,BOOL fTriangulate);
	~TCreateGridUndo		();
	virtual LPSTR GetName	(void){ return "Create grid"; };
	virtual void  Undo		(void);
	virtual void  Do		(void);
};

////////////////////////////////7
// TMorphmeshUndo
///////////////////////////////77

class TMorphmeshUndo : public TMorphmesh
{
protected:
    int		nUndoItems;
	TUndo *FirstUndo;
	TUndo *CurrentUndo;

	void UndoRemoveFirst	(void);
	void UndoRemove			(TUndo *pFirstRemoved);
    void UndoAdd			(TUndo *pNewUndo);

public:
	TMorphmeshUndo(void);
	TMorphmeshUndo(int nStartFrames,int nResultFrames,int nEndFrames);
	~TMorphmeshUndo();

	static TMorphmeshUndo* Create  (int nStartFrames,int nResultFrames,int nEndFrames);
	static TMorphmeshUndo* Create  (LPSTR szFilename);

	virtual void AttachMorphWindow     (TMorphWnd*);
	virtual void DetachMorphWindow     (void);

	LPSTR GetUndoName(void);
	LPSTR GetRedoName(void);

	void Undo(void);
    void Redo(void);

	virtual void UndoRemoveLeftOvers			(void);
	virtual void DeleteRedo						(void);

	virtual void Cut							(BOOL fTriangulate,int nSelBlock,int nSelFrame);
	virtual void Paste							(BOOL fTriangulate,int nSelBlock,int nSelFrame);

	virtual void PointNew						(Point2d p,BOOL Triangulate,int nBlock,int nFrame);
	virtual void SelectionPointsDelete			(BOOL fTriangulate,int nBlock,int nFrame);
	virtual void SelectionMoveEnd				(void);
	virtual void Compute						(TFunction2d *pFunction );
	virtual void TriangulateAll					(int nBlock,int nFrame);
	virtual	void TrianglesDeleteBetweenPoints	(void);
	virtual void TrianglesDeleteWithPoints		(void);
	virtual BOOL StretchFrames					(int nNewFrames[3]);
	virtual void SwapBlocks						(int nBlock1,int nBlock2);
	virtual void SelectionDetachPoints			(int nBlock,int nStartFrame,int nEndFrame);
	virtual void CreateGrid						(int nXGrid,int nYGrid,BOOL fTriangulate);
};

#endif

/* End of file MORPHUND.H */
 

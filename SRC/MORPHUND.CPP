/**********************************************************************
 * 
 *  NAME:         MORPHUND.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what
 *  ------------------------------------------------------------------- 
 * 	1.9800  26.02.96 MHM created
 ********************************************************************/

#include "morphund.h"
#include "morphwin.h"
#include "itemlist.h"
#include "clipbrd.h"


TMorphmeshUndo::TMorphmeshUndo(void):
	TMorphmesh()
{
	nUndoItems = 0;
	FirstUndo = NULL;
    CurrentUndo = NULL;
};

TMorphmeshUndo::TMorphmeshUndo(int nStartFrames,int nResultFrames,int nEndFrames):
	TMorphmesh(nStartFrames,nResultFrames,nEndFrames)
{
	nUndoItems = 0;
	FirstUndo = NULL;
    CurrentUndo = NULL;
};

TMorphmeshUndo::~TMorphmeshUndo()
{
	UndoRemove(FirstUndo);
	FirstUndo = NULL;
    CurrentUndo = NULL;
};

TMorphmeshUndo* TMorphmeshUndo::Create(
		int nStartFrames,
		int nResultFrames,
		int nEndFrames
		)
{
	TMorphmeshUndo* Object = new TMorphmeshUndo(nStartFrames,nResultFrames,nEndFrames);
	if(Object)
		if(!Object->IsOK()){
			delete Object;
			Object = NULL;
			};
	return Object;
};

TMorphmeshUndo* TMorphmeshUndo::Create(LPSTR szFileName)
{
	if(szFileName == NULL)
		// Get file name from user
		szFileName = GetFileName(GetOpenFilter(),GetOpenDefExt(),Read);   

	TMorphmeshUndo* Object = NULL;
	 if(szFileName){
		  // Create morphmesh
		Object = new TMorphmeshUndo();
		if(Object){
			// Load mesh from file
			Object->TMorphmeshPoints::LoadFrom(szFileName);
				// If it didn't succeed
			if(!Object->IsOK()){
				// Delete the morphmesh
				delete Object;
				Object = NULL;
				};
			}
		}
	// Return result mesh
	return Object;
};

void TMorphmeshUndo::UndoRemoveLeftOvers(void)
{
	if(FirstUndo ==NULL)
		return;

    // First try to remove redo buffer
	while((nUndoItems > MorphConfig.nUndoBufferSize)&&
			(CurrentUndo->pNext!=NULL)){
		TUndo *pLast = CurrentUndo;
		while(pLast->pNext!=NULL)
			pLast = pLast->pNext;

		pLast->pPrevious->pNext = NULL;
		pLast->pPrevious = NULL;

		delete pLast;
		nUndoItems--;
    };

	// If there are too many undo items
	while(nUndoItems > MorphConfig.nUndoBufferSize)
		UndoRemoveFirst();

	if(MorphWnd)
    	MorphWnd->UpdateUndoMenu();
};

void TMorphmeshUndo::DeleteRedo(void)
{
	TUndo *pCrrnt = FirstUndo;

	while(pCrrnt!=NULL){
		pCrrnt->DeleteRedo();
        pCrrnt = pCrrnt->pNext;
    };
};

void TMorphmeshUndo::UndoRemoveFirst(void)
{
	if(FirstUndo){
        TUndo *pNextFirst = FirstUndo->pNext;
        if(pNextFirst)
			FirstUndo->pNext->pPrevious = NULL;
        FirstUndo->pNext = NULL;

		if(CurrentUndo == FirstUndo)
			CurrentUndo = NULL;

		delete FirstUndo;
        FirstUndo = pNextFirst; 
        nUndoItems--;
    	};
};

void TMorphmeshUndo::UndoRemove(TUndo *pFirstRemoved)
{
	if(pFirstRemoved==NULL)
		return;

	// Detach from previous item
	if(pFirstRemoved->pPrevious){
		pFirstRemoved->pPrevious->pNext = NULL;
		pFirstRemoved->pPrevious = NULL;
		};

	// Delete items
	while(pFirstRemoved!=NULL){
		TUndo *pNext = pFirstRemoved->pNext;
		delete pFirstRemoved;
		pFirstRemoved = pNext;
		nUndoItems--;
		};
};

void TMorphmeshUndo::UndoAdd(TUndo *pNewUndo)
{
    // Delete undo tail
	if(CurrentUndo==NULL){
        // Delete old undo's
		UndoRemove(FirstUndo);

		// Add new undo
		FirstUndo = pNewUndo;
		}
	   else {
		UndoRemove(CurrentUndo->pNext);

		// Add new undo
		CurrentUndo->pNext = pNewUndo;
		pNewUndo->pPrevious = CurrentUndo;
		}

	// Move undo-pointer to new undo
    CurrentUndo = pNewUndo;
	nUndoItems++;

	// Execute the command
	CurrentUndo->Do();

	// If there are too many undo items
	while(nUndoItems > MorphConfig.nUndoBufferSize)
		UndoRemoveFirst();

	if(MorphWnd)
    	MorphWnd->UpdateUndoMenu();
};

void TMorphmeshUndo::AttachMorphWindow(TMorphWnd *pNewMorphWnd)
{
	TMorphmesh::AttachMorphWindow(pNewMorphWnd);

	if(MorphWnd)
    	MorphWnd->UpdateUndoMenu();
};

void TMorphmeshUndo::DetachMorphWindow(void)
{
	if(MorphWnd)
    	MorphWnd->UpdateUndoMenu();

	TMorphmesh::DetachMorphWindow();
};

LPSTR TMorphmeshUndo::GetUndoName(void)
{
	return ((CurrentUndo!=NULL)? CurrentUndo->GetName() : NULL);
};

LPSTR TMorphmeshUndo::GetRedoName(void)
{
	TUndo *pNext;

	if(CurrentUndo)
		pNext = CurrentUndo->pNext;
	   else
		pNext = FirstUndo;

	if(pNext)
        if(pNext->CanRedo())
			return pNext->GetName();

	return NULL;
};

void TMorphmeshUndo::Undo(void)
{
    /* Undo action */
	CurrentUndo->Undo();
    /* Move back in undo-list */
	CurrentUndo = CurrentUndo->pPrevious;

    /* Update MorphWindow */
	if(MorphWnd)
		MorphWnd->UpdateUndoMenu();
};

void TMorphmeshUndo::Redo(void)
{
    /* Move forward in undo-list */
    if(CurrentUndo)
		CurrentUndo = CurrentUndo->pNext;
	   else
		CurrentUndo = FirstUndo;

    /* Redo action */
	CurrentUndo->Do();

    /* Update MorphWindow */
	if(MorphWnd)
    	MorphWnd->UpdateUndoMenu();
};

void TMorphmeshUndo::Cut(BOOL fTriangulate,int nSelBlock,int nSelFrame)
{
	UndoAdd(new TCutUndo(this,fTriangulate,nSelBlock,nSelFrame));
};

void TMorphmeshUndo::Paste(BOOL fTriangulate,int nSelBlock,int nSelFrame)
{
	TClipBoard *pClipBoard = MorphWnd->GetClipBoard();

    if(pClipBoard->NeedSelections())
    	// Copy frames
		UndoAdd(new TPasteFrameUndo(this,SelectionList,nSelBlock,nSelFrame));
	else
		// Copy points
		UndoAdd(new TPastePointUndo(this,fTriangulate,nSelBlock,nSelFrame));
};

void TMorphmeshUndo::PointNew(Point2d Point,BOOL Triangulate,int nBlock,int nFrame)
{
	UndoAdd(new TPointAddUndo(this,Point,Triangulate,nBlock,nFrame));
};

void TMorphmeshUndo::SelectionPointsDelete(BOOL fTriangulate,int nBlock,int nFrame)
{
	UndoAdd(new TPointDeleteUndo(this,fTriangulate,nBlock,nFrame));
};

void TMorphmeshUndo::SelectionMoveEnd(void)
{
	if(pMoveUndo->fSelectionMoved){
		// Not in interactive state any more
		pMoveUndo->fInteractive = FALSE;

		// Something moved, so save undo
		UndoAdd(pMoveUndo);

		// Interpolate points
		if(MorphConfig.nInterpolation==AFTER_MOVING){
			TMorphmesh_Data::Interpolate(pMoveUndo->nBlock,SelectionList);

			// Update Morphwindow
			if(MorphWnd)
				if(pMoveUndo->nBlock!=-1)
					MorphWnd->SelectionMoved(pMoveUndo->nBlock,SelectionList);
            	else
					for(int nBlock = 0; nBlock < 3 ; nBlock ++ )
						MorphWnd->SelectionMoved(nBlock,SelectionList);
        };

		// Set morphmesh as changed
		SetChanged(TRUE);

	} else
    	delete pMoveUndo;

	pMoveUndo = NULL;
};

BOOL TMorphmeshUndo::StretchFrames(int nNewFrames[3])
{
	UndoAdd(new TStretchUndo(this,nNewFrames));
    return TRUE;
};

void TMorphmeshUndo::SwapBlocks(int nBlock1,int nBlock2)
{
	UndoAdd(new TSwapBlocksUndo(this,nBlock1,nBlock2));
};

void TMorphmeshUndo::Compute(TFunction2d *pFunction)
{
	UndoAdd(new TComputeResultUndo(pFunction,this));
};

void TMorphmeshUndo::SelectionDetachPoints(int nBlock,int nStartFrame,int nEndFrame)
{
	UndoAdd(new TDetachPointsUndo(this,SelectionList,nBlock,nStartFrame,nEndFrame));
};

void TMorphmeshUndo::TriangulateAll(int nBlock,int nFrame)
{
	UndoAdd(new TTriangulateUndo(this,nBlock,nFrame));
};

void TMorphmeshUndo::TrianglesDeleteBetweenPoints(void)
{
	UndoAdd(new TDeleteTrianglesUndo(this,SelectionList,3,3));
};

void TMorphmeshUndo::TrianglesDeleteWithPoints(void)
{
	UndoAdd(new TDeleteTrianglesUndo(this,SelectionList,1,3));
};

void TMorphmeshUndo::CreateGrid(int nXGrid,int nYGrid,BOOL fTriangulate)
{
	UndoAdd(new TCreateGridUndo(this,nXGrid,nYGrid,fTriangulate));
};

/////////////////////////////////////////7
//class TUndo
/////////////////////////////////7

void TUndo::Do(void)
{
	if(!MorphConfig.fRedoEnabled)
    	DeleteRedo();
};

void TUndo::DeleteRedo(void){};

/////////////////////////////////////////7
//class TAddPointUndo : public TUndo
/////////////////////////////////7

TPointAddUndo::TPointAddUndo(
	TMorphmeshUndo *pMesh,Point2d pNewPoint,
		BOOL fTriangulate,int nNewBlock,int nNewFrame) :
	TUndo(pMesh)
{
    Point = pNewPoint;

	if(fTriangulate){
		// Save triangles for undo
		pUndoMesh = new TMorphmeshTriangles();
		pUndoMesh->TrianglesSet(pMesh->Triangles,pMesh->TrianglesAllocated,pMesh->nTriangles);

		// Save for redo
		nBlock = nNewBlock;
		nFrame = nNewFrame;
	} else {
		pUndoMesh = NULL;
		nBlock = -1;
        nFrame = -1;
    }
};

TPointAddUndo::~TPointAddUndo()
{
	if(pUndoMesh)
		delete pUndoMesh;
};

void TPointAddUndo::Undo(void)
{
    // Delete last point
	pMesh->TMorphmesh::PointDelete(pMesh->GetPoints()-1);

	if(pUndoMesh){
		// Restore UndoMesh
		pMesh->TrianglesSet(	pUndoMesh->Triangles,
			pUndoMesh->TrianglesAllocated,	pUndoMesh->nTriangles);

		// Update Morphwindow
		if(pMesh->MorphWnd)
			pMesh->MorphWnd->InvalidateAll();
    	};
};

void TPointAddUndo::Do(void)
{
	pMesh->TMorphmesh::PointNew(Point,(pUndoMesh!=NULL),nBlock,nFrame);

	TUndo::Do();
};

/////////////////////////////////////////7
//class TDeletePointUndo : public TUndo
/////////////////////////////////7

TPointDeleteUndo::TPointDeleteUndo(
	TMorphmeshUndo *pMesh,
		BOOL fTriangulate,int nNewBlock,int nNewFrame) :
	TUndo(pMesh)
{
    // Create undo mesh
	pUndoMesh = new TMorphmesh_Data(
			pMesh->GetFrames(MorphStart),pMesh->GetFrames(MorphResult),pMesh->GetFrames(MorphEnd));

	// Save triangles for undo
	pUndoMesh->TrianglesSet(pMesh->Triangles,pMesh->TrianglesAllocated,pMesh->nTriangles);

	if(fTriangulate){
		// Save for redo
		nBlock = nNewBlock;
		nFrame = nNewFrame;
	} else {
		nBlock = -1;
        nFrame = -1;
    }

	// Make copy of selections
	pPointList = new ItemList(pMesh->SelectionList);

    // Save Points
	pMesh->PointsCopy(pPointList,pUndoMesh);
};

TPointDeleteUndo::~TPointDeleteUndo()
{
	if(pUndoMesh)
		delete pUndoMesh;
	if(pPointList)
    	delete pPointList;
};

void TPointDeleteUndo::Undo(void)
{
	// Restore points
	pMesh->TMorphmesh::PointsRestore(pUndoMesh,pPointList);

	// Restore triangles
	pMesh->TrianglesSet(	pUndoMesh->Triangles,
		pUndoMesh->TrianglesAllocated,	pUndoMesh->nTriangles);

	// Update Morphwindow
	if(pMesh->MorphWnd)
		pMesh->MorphWnd->InvalidateAll();
};

void TPointDeleteUndo::Do(void)
{
    // Delete points
	pMesh->TMorphmesh::PointsDelete(pPointList,(nBlock!=-1),nBlock,nFrame);

	TUndo::Do();
};

/////////////////////////////////////////7
//class TMovePointUndo : public TUndo
/////////////////////////////////7

TPointMoveUndo::TPointMoveUndo(
	TMorphmeshUndo *pMesh,ItemList *pNewPointList,
		int nNewMoveMode,int nNewBlock,int nNewStartFrame,int nNewCrrntFrame,int nNewEndFrame) :
	TUndo(pMesh)
{
    fInteractive = TRUE;
	fSelectionMoved = FALSE;
	nMoveMode	= nNewMoveMode;
	nBlock 		= nNewBlock;
	nStartFrame = (nNewStartFrame != -1) ? nNewStartFrame : 0 ;
	nCrrntFrame = nNewCrrntFrame;
	nEndFrame   = (nNewEndFrame != -1) ? nNewEndFrame :
						((nBlock!=-1)? pMesh->GetFrames(nBlock)-1 : -1 );

	pBlock[MorphStart] = NULL;
	pBlock[MorphResult]= NULL;
	pBlock[MorphEnd]   = NULL;

    pOrigin.x = 0.0;
	pOrigin.y = 0.0;
	pRelative.x = 0.0;
	pRelative.y = 0.0;

	pPointList = new ItemList(pNewPointList);

	// Save original points
    // If all blocks are moved
    if(nBlock==-1){
        // Save all blocks(all frames)
		for(int nCrrntBlock = 0 ; nCrrntBlock < 3 ; nCrrntBlock ++)
			pBlock[nCrrntBlock] = pMesh->mBlock[nCrrntBlock]->CreateCopy(pPointList,
				0,pMesh->GetFrames(nCrrntBlock));

    } else {
        // Save only one block
		pBlock[nBlock] = pMesh->mBlock[nBlock]->CreateCopy(pPointList,
				nStartFrame,nEndFrame+1);
	};

	// Update morphwindows connected-rects
	if(pMesh->MorphWnd)
		pMesh->MorphWnd->ConnectedRectAddPoints(pPointList);
};

TPointMoveUndo::~TPointMoveUndo()
{
	if(pPointList)
		delete pPointList;
	for(int nBlock = 0 ; nBlock < 3 ; nBlock ++ )
		if(pBlock[nBlock])
        	delete pBlock[nBlock];
};

void TPointMoveUndo::Undo(void)
{
	// Update morphwindows connected-rects
	if(pMesh->MorphWnd)
		pMesh->MorphWnd->ConnectedRectAddPoints(pPointList);

	// Loop all block or only the selected one
	for(int nCrrntBlock = (nBlock != -1) ? nBlock : 0 ;
		nCrrntBlock < ((nBlock != -1) ? nBlock + 1 : 3) ; nCrrntBlock ++ ){
		int nSelPoint = 0;
		int nStartFr,nEndFr;

        // If all blocks used, use all frames, otherwise the selected frames
        nStartFr = (nBlock != -1) ? nStartFrame : 0 ;
		nEndFr   = (nBlock != -1) ? nEndFrame : pBlock[nCrrntBlock]->GetFrames() - 1;

	    for(int nPoint = 0 ; nPoint < pMesh->GetPoints() ; nPoint ++ )
			if(pPointList->GetFlag(nPoint,SL_SELECTED)){
				static Point2d pPoint;
				MorphPoint* pSelectionPoint = pMesh->Point(nCrrntBlock,nPoint);
				MorphPoint* pOriginPoint    = pBlock[nCrrntBlock]->Point(nSelPoint);

				for(int nFrame = nStartFr ; nFrame <= nEndFr ; nFrame ++ )
					pSelectionPoint[nFrame] = pOriginPoint[nFrame - nStartFr];

				// Interpolate point
				pMesh->TMorphmesh_Data::Interpolate(nCrrntBlock,nPoint);

				nSelPoint++;
				};

		// Update Morphwindow 
		if(pMesh->MorphWnd)
			pMesh->MorphWnd->SelectionMoved(nCrrntBlock,pPointList);
	};
};

LPSTR TPointMoveUndo::GetName(void)
{
    LPSTR pszMoveMode = NULL;
	switch(nMoveMode){
		case NORMAL_MOVE:	pszMoveMode = "Move point";		break;	
		case SCALING:		pszMoveMode = "Scale point";	break;
		case ROTATION:		pszMoveMode = "Rotate point";	break;
	};
    return pszMoveMode;
};

void TPointMoveUndo::Do(void)
{
	if(!fInteractive)
		// Update morphwindows connected-rects
		if(pMesh->MorphWnd)
			pMesh->MorphWnd->ConnectedRectAddPoints(pPointList);

	// Loop all block or only the selected one
	for(int nCrrntBlock = (nBlock != -1) ? nBlock : 0 ;
		nCrrntBlock < ((nBlock != -1) ? nBlock + 1 : 3) ; nCrrntBlock ++ ){
		int nSelPoint = 0;
		int nStartFr,nEndFr;

        // If all blocks used, use all frames, otherwise the selected frames
        nStartFr = (nBlock != -1) ? nStartFrame : 0 ;
		nEndFr   = (nBlock != -1) ? nEndFrame : pBlock[nCrrntBlock]->GetFrames() - 1;

		for(int nPoint = 0 ; nPoint < pMesh->GetPoints() ; nPoint ++ )
			if(pPointList->GetFlag(nPoint,SL_SELECTED)){
				static Point2d pPointNew;
                static Point2d pPointOld;
				MorphPoint* pSelectionPoint = pMesh->Point(nCrrntBlock,nPoint);
				MorphPoint* pOriginPoint    = pBlock[nCrrntBlock]->Point(nSelPoint);

    	        // If first time moved 
				if(!fSelectionMoved){
					// Set first frame as a keypoint
					pSelectionPoint[nStartFr].SetKeyPoint(TRUE);
					// Set last frame as a keypoint
					pSelectionPoint[nEndFr].SetKeyPoint(TRUE);
				};

				static Point2d pFunc;
				if(nMoveMode == ROTATION){
					pFunc.x = cos(pRelative.x * 2.0);
                    pFunc.y = sin(pRelative.x * 2.0);
                };
                	
				for(int nFrame = nStartFr ; nFrame <= nEndFr ; nFrame ++ ){
					if(pSelectionPoint[nFrame].GetKeyPoint()||
							MorphConfig.nInterpolation == AFTER_MOVING){
						pPointOld = pOriginPoint[nFrame - nStartFr].GetPoint();
                        switch(nMoveMode){
							case NORMAL_MOVE:
				pPointNew = pPointOld + pRelative;
								break;
							case SCALING:
				pPointNew.x = ( pPointOld.x - pOrigin.x ) * 4.0 * pRelative.x + pOrigin.x;
				pPointNew.y = ( pPointOld.y - pOrigin.y ) * 4.0 * pRelative.y + pOrigin.y;
								break;
							case ROTATION:
				static Point2d pDist;
				pDist.x = ( pPointOld.x - pOrigin.x );
				pDist.y = ( pPointOld.y - pOrigin.y );

				pPointNew.x = (pFunc.x * pDist.x + pFunc.y * pDist.y) + pOrigin.x;
				pPointNew.y = (pFunc.x * pDist.y - pFunc.y * pDist.x) + pOrigin.y;
								break;
						};

						// Check, if new value is out of boundaries
						pPointNew.x = inrange(pPointNew.x,0.0,1.0);
						pPointNew.y = inrange(pPointNew.y,0.0,1.0);

						// Set new point
						pSelectionPoint[nFrame].SetPoint(pPointNew);
					};				
				};

				// Interpolate point
				if(MorphConfig.nInterpolation==DURING_MOVING)
					pMesh->TMorphmesh_Data::Interpolate(nCrrntBlock,nPoint);

				nSelPoint++;
				};

		// Update Morphwindow 
		if(pMesh->MorphWnd)
			pMesh->MorphWnd->SelectionMoved(nCrrntBlock,pPointList);
	};

	fSelectionMoved = TRUE;

	TUndo::Do();
};

/////////////////////////////////////////7
//class TTriangulateUndo : public TUndo
/////////////////////////////////7

TTriangulateUndo::TTriangulateUndo(
	TMorphmeshUndo *pMesh,int nNewBlock,int nNewFrame) :
	TUndo(pMesh)
{
	// Save triangles for undo
	pUndoMesh = new TMorphmeshTriangles();
	pUndoMesh->TrianglesSet(pMesh->Triangles,pMesh->TrianglesAllocated,pMesh->nTriangles);

    // If only one triangle added
	if(pMesh->nSelections==3){
        int nPoint;

		for(nPoint=0;pMesh->SelectionList->GetFlag(nPoint,SL_SELECTED)==FALSE ; nPoint++);
		tTriangle.SetVertex(0,nPoint);
		for(nPoint++;pMesh->SelectionList->GetFlag(nPoint,SL_SELECTED)==FALSE ; nPoint++);
		tTriangle.SetVertex(1,nPoint);
		for(nPoint++;pMesh->SelectionList->GetFlag(nPoint,SL_SELECTED)==FALSE ; nPoint++);
		tTriangle.SetVertex(2,nPoint);
		// Save for redo
		nBlock = -1;
		nFrame = -1;
	} else {
	    // Save for redo
		nBlock = nNewBlock;
		nFrame = nNewFrame;
    }
};

TTriangulateUndo::~TTriangulateUndo()
{
	if(pUndoMesh)
		delete pUndoMesh;
};

void TTriangulateUndo::Undo(void)
{
    // Restore UndoMesh
	pMesh->TrianglesSet(	pUndoMesh->Triangles,
		pUndoMesh->TrianglesAllocated,	pUndoMesh->nTriangles);

	// Update Morphwindow
	if(pMesh->MorphWnd)
		pMesh->MorphWnd->InvalidateAll();
};

void TTriangulateUndo::Do()
{
    // If only one triangle addded
	if(nBlock==-1){
		pMesh->TriangleAdd(tTriangle);
	} else
		// Triangulate
		pMesh->TMorphmesh::Triangulate(nBlock,nFrame);

	TUndo::Do();
};

/////////////////////////////////////////
//class TDeleteTrianglesUndo : public TUndo
/////////////////////////////////7

TDeleteTrianglesUndo::TDeleteTrianglesUndo(
	TMorphmeshUndo *pMesh, ItemList *pPointList,int nMin,int nMax) :
	TUndo(pMesh)
{
	// Save triangles for undo
	pUndoMesh = new TMorphmeshTriangles();
	pUndoMesh->TrianglesSet(pMesh->Triangles,pMesh->TrianglesAllocated,pMesh->nTriangles);

    // Save Do-data
    Selection = new ItemList(pPointList);
	nMinConnections = nMin;
    nMaxConnections = nMax;
};

TDeleteTrianglesUndo::~TDeleteTrianglesUndo()
{
	if(pUndoMesh)
		delete pUndoMesh;

	if(Selection)
    	delete Selection;
};

void TDeleteTrianglesUndo::Undo(void)
{
	// Restore UndoMesh
	pMesh->TrianglesSet(	pUndoMesh->Triangles,
		pUndoMesh->TrianglesAllocated,	pUndoMesh->nTriangles);

	// Update Morphwindow
	if(pMesh->MorphWnd)
		pMesh->MorphWnd->InvalidateAll();
};

void TDeleteTrianglesUndo::Do(void)
{
	pMesh->TMorphmesh::TrianglesDeleteWithPoints(Selection,
    	nMinConnections,nMaxConnections);

	TUndo::Do();
};

void TDeleteTrianglesUndo::DeleteRedo(void)
{
	if(Selection)
    	delete Selection;
};

/////////////////////////////////////////7
//class TCutUndo : public TUndo
/////////////////////////////////7

TCutUndo::TCutUndo(
	TMorphmeshUndo *pMesh,
		BOOL fTriangulate,int nNewBlock,int nNewFrame) :
	TUndo(pMesh)
{
	// Create undo mesh
	pUndoMesh = new TMorphmesh_Data(
			pMesh->GetFrames(MorphStart),pMesh->GetFrames(MorphResult),pMesh->GetFrames(MorphEnd));

	// Save triangles for undo
	pUndoMesh->TrianglesSet(pMesh->Triangles,pMesh->TrianglesAllocated,pMesh->nTriangles);

	// Save for redo
	if(fTriangulate){
		nBlock = nNewBlock;
		nFrame = nNewFrame;
	} else {
		nBlock = -1;
        nFrame = -1;
    }

	fClipped = FALSE;

	// Make copy of selections
	pPointList = new ItemList(pMesh->SelectionList);

    // Save Points
	pMesh->PointsCopy(pPointList,pUndoMesh);
};

TCutUndo::~TCutUndo()
{
	if(pUndoMesh)
		delete pUndoMesh;
	if(pPointList)
    	delete pPointList;
};

void TCutUndo::Undo(void)
{
	// Restore points
	pMesh->TMorphmesh::PointsRestore(pUndoMesh,pPointList);

	// Restore triangles
	pMesh->TrianglesSet(	pUndoMesh->Triangles,
		pUndoMesh->TrianglesAllocated,	pUndoMesh->nTriangles);

	// Update Morphwindow
	if(pMesh->MorphWnd)
		pMesh->MorphWnd->InvalidateAll();
};

void TCutUndo::Do(void)
{
    if(!fClipped){
		pMesh->TMorphmesh::Cut((nBlock!=-1),nBlock,nFrame);
        fClipped = TRUE;
    } else
		pMesh->TMorphmesh::PointsDelete(pPointList,(nBlock!=-1),nBlock,nFrame);

	TUndo::Do();
};

/////////////////////////////////////////7
//class TPastePointUndo : public TUndo
/////////////////////////////////7

TPastePointUndo::TPastePointUndo(
	TMorphmeshUndo *pMesh,BOOL fTriangulate,int nNewBlock,int nNewFrame) :
	TUndo(pMesh)
{
// Redo
	pClipBoard = new TClipBoardPoints((TClipBoardPoints*)(pMesh->MorphWnd->GetClipBoard()));

// Undo
	if(fTriangulate){
		// Save triangles for undo
		pUndoMesh = new TMorphmeshTriangles();
		pUndoMesh->TrianglesSet(pMesh->Triangles,pMesh->TrianglesAllocated,pMesh->nTriangles);

		// Save for redo
		nBlock = nNewBlock;
		nFrame = nNewFrame;
	} else {
		pUndoMesh = NULL;
		nBlock = -1;
        nFrame = -1;
    }
};

TPastePointUndo::~TPastePointUndo()
{
	if(pClipBoard)
    	delete pClipBoard;
	if(pUndoMesh)
		delete pUndoMesh;
};

void TPastePointUndo::Undo(void)
{
	// Delete last points
    for(int nPoint = 0 ; nPoint < pClipBoard->pClipMesh->GetPoints() ; nPoint ++) 
		pMesh->TMorphmesh::PointDelete(pMesh->GetPoints()-1);

    // If triangulated
	if(nBlock!=-1){
		// Restore UndoMesh
		pMesh->TrianglesSet(	pUndoMesh->Triangles,
			pUndoMesh->TrianglesAllocated,	pUndoMesh->nTriangles);

		// Update Morphwindow
		if(pMesh->MorphWnd)
			pMesh->MorphWnd->InvalidateAll();
    	};
};

void TPastePointUndo::Do(void)
{
	pMesh->TMorphmesh::Paste(pClipBoard,(nBlock!=-1),nBlock,nFrame);

	TUndo::Do();
};

/////////////////////////////////////////7
//class TPasteFrameUndo : public TUndo
/////////////////////////////////7

TPasteFrameUndo::TPasteFrameUndo(
	TMorphmeshUndo *pMesh,ItemList *pNewPointList,int nNewBlock,int nNewFrame) :
	TUndo(pMesh)
{
	TClipBoardFrames *pRealClipBoard = (TClipBoardFrames*)(pMesh->MorphWnd->GetClipBoard());
// Redo
    // Create copy of clipboard
	pClipBoard  = new TClipBoardFrames(pRealClipBoard);

// Common
	nBlock 		= nNewBlock;
	nStartFrame = nNewFrame;
// Undo
	nEndFrame   = nStartFrame + pRealClipBoard->pBlock->GetFrames();

	pPointList = new ItemList(pNewPointList);

	// Save original points
	pBlock = pMesh->mBlock[nBlock]->CreateCopy(pPointList,
			nStartFrame,nEndFrame);
};

TPasteFrameUndo::~TPasteFrameUndo()
{
	if(pClipBoard)
		delete pClipBoard;
	if(pBlock)
    	delete pBlock;
};

void TPasteFrameUndo::Undo(void)
{
	int nSelPoint = 0;
    for(int nPoint = 0 ; nPoint < pMesh->GetPoints() ; nPoint ++ )
		if(pPointList->GetFlag(nPoint,SL_SELECTED)){
			MorphPoint* pMeshPoint = pMesh->Point(nBlock,nPoint);
			MorphPoint* pUndoPoint = pBlock->Point(nSelPoint);

			for(int nFrame = nStartFrame ; nFrame < nEndFrame ; nFrame ++ )
				pMeshPoint[nFrame] = pUndoPoint[nFrame - nStartFrame];

			// Interpolate point
			pMesh->TMorphmesh_Data::Interpolate(nBlock,nPoint);

			nSelPoint++;
			};

	// Update Morphwindow 
	if(pMesh->MorphWnd)
		pMesh->MorphWnd->ViewPointsUpdate(pMesh->GetPoints());
};

void TPasteFrameUndo::Do(void)
{
	pMesh->TMorphmesh::Paste(pClipBoard,FALSE,nBlock,nStartFrame);

	TUndo::Do();
};

/////////////////////////////////////////7
//class TStretchUndo : public TUndo
/////////////////////////////////7

TStretchUndo::TStretchUndo(
    TMorphmeshUndo *pMesh,
		int nNewFrames[3]):
    TUndo(pMesh)
{
	// Save original points
	for( int nBlock = 0 ; nBlock < 3 ; nBlock ++ )
		pBlock[nBlock] = pMesh->mBlock[nBlock]->CreateCopy(NULL,0,pMesh->GetFrames(nBlock));

    // For redo
	for(int nFrame = 0 ; nFrame < 3 ; nFrame ++)
		nRedoFrames[nFrame] = nNewFrames[nFrame];
};

void TStretchUndo::~TStretchUndo()
{
	for( int nBlock = 0 ; nBlock < 3 ; nBlock ++ )
		delete pBlock[nBlock];
};

void TStretchUndo::Undo(void)
{
	for(int nBlock = 0 ; nBlock < 3 ; nBlock ++)
		if(pBlock[nBlock]){
			MorphBlock *pNewBlock = new MorphBlock(pBlock[nBlock]);
			SWAP(MorphBlock*,pMesh->mBlock[nBlock],pNewBlock);
			if(pNewBlock)
				delete pNewBlock;			    		    	
	    };

	if(pMesh->MorphWnd)
		pMesh->MorphWnd->FramesChanged();
};

void TStretchUndo::Do()
{
	pMesh->TMorphmesh::StretchFrames(nRedoFrames);

	TUndo::Do();
};

/////////////////////////////////////////7
//class TSwapBlocksUndo : public TUndo
/////////////////////////////////7

TSwapBlocksUndo::TSwapBlocksUndo(
	TMorphmeshUndo *pMesh,int nNewBlock1,int nNewBlock2) :
	TUndo(pMesh)
{
	nBlock1 = nNewBlock1;
	nBlock2 = nNewBlock2;
};

TSwapBlocksUndo::~TSwapBlocksUndo()
{
};

void TSwapBlocksUndo::Undo(void)
{
	pMesh->TMorphmesh::SwapBlocks(nBlock2,nBlock1);
};

void TSwapBlocksUndo::Do(void)
{
	pMesh->TMorphmesh::SwapBlocks(nBlock1,nBlock2);

	TUndo::Do();
};

/////////////////////////////////////////7
//class TDetachPointsUndo : public TUndo
/////////////////////////////////7

TDetachPointsUndo::TDetachPointsUndo(
	TMorphmeshUndo *pMesh,ItemList *pNewPointList,int nNewBlock,int nNewStartFrame,int nNewEndFrame) :
	TUndo(pMesh)
{
	nBlock 		= nNewBlock;
	nStartFrame = (nNewStartFrame != -1) ? nNewStartFrame : 0 ;
	nEndFrame   = (nNewEndFrame != -1) ? nNewEndFrame :
						((nBlock!=-1)? pMesh->GetFrames(nBlock)-1 : -1 );

	pBlock[MorphStart] = NULL;
	pBlock[MorphResult]= NULL;
	pBlock[MorphEnd]   = NULL;

	pPointList = new ItemList(pNewPointList);

	// Save original points
    // If all blocks are moved
    if(nBlock==-1){
        // Save all blocks(all frames)
		for(int nCrrntBlock = 0 ; nCrrntBlock < 3 ; nCrrntBlock ++)
			pBlock[nCrrntBlock] = pMesh->mBlock[nCrrntBlock]->CreateCopy(pPointList,
				0,pMesh->GetFrames(nCrrntBlock));

    } else {
        // Save only one block
		pBlock[nBlock] = pMesh->mBlock[nBlock]->CreateCopy(pPointList,
				nStartFrame,nEndFrame+1);
	};
};

TDetachPointsUndo::~TDetachPointsUndo()
{
	if(pPointList)
		delete pPointList;
	for(int nBlock = 0 ; nBlock < 3 ; nBlock ++ )
		if(pBlock[nBlock])
        	delete pBlock[nBlock];
};

void TDetachPointsUndo::Undo(void)
{
	// Loop all block or only the selected one
	for(int nCrrntBlock = (nBlock != -1) ? nBlock : 0 ;
		nCrrntBlock < ((nBlock != -1) ? nBlock + 1 : 3) ; nCrrntBlock ++ ){
        int nSelPoint = 0;
		int nStartFr,nEndFr;

        // If all blocks used, use all frames, otherwise the selected frames
        nStartFr = (nBlock != -1) ? nStartFrame : 0 ;
		nEndFr   = (nBlock != -1) ? nEndFrame : pBlock[nCrrntBlock]->GetFrames() - 1;

	    for(int nPoint = 0 ; nPoint < pMesh->GetPoints() ; nPoint ++ )
			if(pPointList->GetFlag(nPoint,SL_SELECTED)){
				static Point2d pPoint;
				MorphPoint* pSelectionPoint = pMesh->Point(nCrrntBlock,nPoint);
				MorphPoint* pOriginPoint    = pBlock[nCrrntBlock]->Point(nSelPoint);

				for(int nFrame = nStartFr ; nFrame <= nEndFr ; nFrame ++ )
					pSelectionPoint[nFrame] = pOriginPoint[nFrame - nStartFr];

				nSelPoint++;
				};
	};

    // Interpolate and update viewpoints
	pMesh->TMorphmesh::Interpolate(nBlock,pPointList);
};

void TDetachPointsUndo::Do(void)
{
	pMesh->TMorphmesh::DetachPoints(pPointList,nBlock,nStartFrame,nEndFrame);

	TUndo::Do();
};

/////////////////////////////////////////7
//class TComputeResultUndo : public TUndo
/////////////////////////////////7

TComputeResultUndo::TComputeResultUndo(
	TFunction2d *pNewFunction,
	TMorphmeshUndo *pMesh) :
	TUndo(pMesh)
{
    pFunction = pNewFunction;

	// Save only one block
	pBlock = pMesh->mBlock[MorphResult]->CreateCopy(NULL,
			0,pMesh->GetFrames(MorphResult));
};

TComputeResultUndo::~TComputeResultUndo()
{
	if(pBlock)
    	delete pBlock;

	if(pFunction)
    	delete pFunction;
};

void TComputeResultUndo::Undo(void)
{
	// Copy selected points to origin block
	for(int nPoint = 0; nPoint < pMesh->GetPoints(); nPoint ++)
        // Copy frames for one point 
		for(int nFrame = 0 ; nFrame < pBlock->GetFrames() ; nFrame ++ )
			pMesh->Point(MorphResult,nPoint)[nFrame] = pBlock->Point(nPoint)[nFrame];

	if(pMesh->MorphWnd)
    	pMesh->MorphWnd->ViewPointsUpdate(pMesh->GetPoints());
};

void TComputeResultUndo::Do(void)
{
	pMesh->TMorphmesh::Compute(pFunction);

	TUndo::Do();
};

//////////////////////////////////////
//class TCreateGridUndo : public TUndo
//////////////////////////////////////

TCreateGridUndo::TCreateGridUndo(
	TMorphmeshUndo *pMesh,int nNewXGrid,int nNewYGrid,BOOL fNewTriangulate) :
	TUndo(pMesh)
{
    // Create undo mesh
	pUndoMesh = new TMorphmesh_Data(
			pMesh->GetFrames(MorphStart),pMesh->GetFrames(MorphResult),pMesh->GetFrames(MorphEnd));

	// Save triangles for undo
	pUndoMesh->TrianglesSet(pMesh->Triangles,pMesh->TrianglesAllocated,pMesh->nTriangles);

	nXGrid = nNewXGrid;
	nYGrid = nNewYGrid;

    fTriangulate = fNewTriangulate;

    // Save Points
	pMesh->PointsCopy(NULL,pUndoMesh);
};

TCreateGridUndo::~TCreateGridUndo()
{
	if(pUndoMesh)
		delete pUndoMesh;
};

void TCreateGridUndo::Undo(void)
{
    // Delete current points
	pMesh->TMorphmesh::PointsDelete(NULL,FALSE,-1,-1);

	// Restore old points
	pMesh->TMorphmesh::PointsRestore(pUndoMesh,NULL);

	// Restore triangles
	pMesh->TrianglesSet(	pUndoMesh->Triangles,
		pUndoMesh->TrianglesAllocated,	pUndoMesh->nTriangles);

	// Update Morphwindow
	if(pMesh->MorphWnd)
		pMesh->MorphWnd->InvalidateAll();
};

void TCreateGridUndo::Do(void)
{
    // Delete points
	pMesh->TMorphmesh::CreateGrid(nXGrid,nYGrid,fTriangulate);

	TUndo::Do();
};

/* End of file MORPHUND.CPP */

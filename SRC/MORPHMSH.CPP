/**********************************************************************
 * 
 *  NAME:         MORPHMSH.CPP 
 * 
 *  DESCRIPTION:  
 *
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          11. 8.95 MHM added headers 
 *  1.9470  11.12.95 MHM changed creation from (int) to (int,int,int)
 *                          (StartFrames,ResultFrames,EndFrames)
 *  1.9471  11.12.95 MHM renamed from morphstr.cpp
 *  1.9475  12.12.95 MHM Morpher has been completely changed, now it has a
 *                        separate function Morph(from,to), which is called
 *********************************************************************/ 
 
#include <assert.h>
#include "morphmsh.h"

#include "morphwin.h"
#include "morphcfg.h"      // class TMorphConfig
#include "itemlist.h"	   // class ItemList
#include "delaunay.h"	   // class Delaunay	
#include "morphund.h"	   // class TPointMoveUndo
#include "clipbrd.h"	   // class TClipBoard
#include "animatio.h"

TMorphmesh::TMorphmesh():
	TMorphmesh_Data()
{
	MorphWnd = NULL;

    nSelections    = 0;
    SelectionList  = new ItemList();
};

TMorphmesh::TMorphmesh(int nStartFrames,int nResultFrames,int nEndFrames):
	TMorphmesh_Data(nStartFrames,nResultFrames,nEndFrames)
{
	MorphWnd = NULL;

    nSelections    = 0;
	SelectionList  = new ItemList();
};

TMorphmesh::~TMorphmesh()
{
    if(SelectionList)
		delete SelectionList; 
};

TMorphmesh* TMorphmesh::Create(int nStartFrames,int nResultFrames,int nEndFrames)
{
	TMorphmesh* Object = new TMorphmesh(nStartFrames,nResultFrames,nEndFrames);
	if(Object)
		if(!Object->IsOK()){
			delete Object;
			Object = NULL;
			};
	return Object;
};

TMorphmesh* TMorphmesh::Create(LPSTR szFileName)
{
	if(szFileName == NULL)
		// Get file name from user
		szFileName = GetFileName(GetOpenFilter(),GetOpenDefExt(),Read);   

	TMorphmesh* Object = NULL;
	 if(szFileName){
		  // Create morphmesh
		Object = new TMorphmesh();
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

void TMorphmesh::AttachMorphWindow(TMorphWnd* Wnd)
{
	// Set new MorphWindow 
	assert(MorphWnd==NULL);
	MorphWnd = Wnd;

	// Inform MorphWindows that the number of frames has been changed
	MorphWnd->FramesChanged();

	// Inform MorphWindows that the number of points has been changed
	MorphWnd->ViewPointsUpdate(GetPoints());

	/* Update animations */
	for(int nPicture = 0 ; nPicture < 4 ; nPicture ++ )
    	MorphWnd->AnimationChanged(nPicture,GetAnimation(nPicture));
};

void TMorphmesh::DetachMorphWindow(void)
{
	// Inform MorphWindows that the number of points has been changed
	MorphWnd->ViewPointsUpdate(0);

    /* Update animations, set NULL animation */
	for(int nPicture = 0 ; nPicture < 4 ; nPicture ++ )
    	MorphWnd->AnimationChanged(nPicture,NULL);

	// Clear MorphWindow
	MorphWnd = NULL;
};

// Clears all data
void TMorphmesh::Reset()
{
	TMorphmesh_Data::Reset();

    SelectionList->Reset();
	nSelections = 0;

	// Update Morphwindow
	if(MorphWnd)
		MorphWnd->ViewPointsUpdate(0);
};

void TMorphmesh::SetAnimation(int nPicture,Animation *pNewAnim)
{
    // Save old animation
	Animation *pOldAnim = Picture[nPicture];

    // Set new animation
	Picture[nPicture] = pNewAnim;

	// Update Morphwindow
	if(MorphWnd)
		MorphWnd->AnimationChanged(nPicture,pNewAnim);

	// Delete earlier animation
	delete pOldAnim;
};

void TMorphmesh::AnimationFrameChanged(int nPicture,int nFrame)
{
	// Update Morphwindow
	if(MorphWnd)
		MorphWnd->AnimationFrameChanged(nPicture,nFrame);
};

void TMorphmesh::InvalidateAnimFrame(int nPicture,int nFrame)
{
	// Update Morphwindow
	if(MorphWnd)
		MorphWnd->InvalidateAnimFrame(nPicture,nFrame);
};

BOOL TMorphmesh::LoadDocument(wmfstream& file)
{
	BOOL bSuccess = TMorphmesh_Data::LoadDocument(file);
	if(bSuccess){
		// Update Morphwindow
		if(MorphWnd)
			MorphWnd->ViewPointsUpdate(GetPoints());

		// Remove duplicate lines from triangles
		for(int nNewTriangle = 0 ; nNewTriangle < nTriangles ; nNewTriangle++){
			Triangles[nNewTriangle].Set_Lines_Unique();

			for(int nOldTriangle = 0 ; nOldTriangle < nNewTriangle ; nOldTriangle++)
				Triangles[nNewTriangle].
					Remove_Duplicate_Lines(Triangles[nOldTriangle]);
			};
    	};
	return bSuccess;
};

void TMorphmesh::SaveDocument(wmfstream& file)
{
	// Save morphmesh
	TMorphmesh_Data::SaveDocument(file);
};

void TMorphmesh::Paint(HDC hDC,int nBlock,int nFrame,POINT ViewPoints[],
		BOOL fShowPoints,BOOL fShowTriangles)
{
	HPEN hPenTemp     =(HPEN)     SelectObject(hDC,MorphConfig.LineBorder);
	HBRUSH hBrushTemp =(HBRUSH)   SelectObject(hDC,MorphConfig.NormalInside);

	// Paint Triangles
	if(fShowTriangles)
		for(int nTri = 0 ; nTri < nTriangles ; nTri++)
			Triangles[nTri].Paint(ViewPoints,hDC);

    if(fShowPoints){
        
		// Paint Morphpoints ( but not selections )
		SelectObject(hDC,MorphConfig.NormalBorder);

		for(int nPoint = 0 ; nPoint < GetPoints() ; nPoint++ )
			if(!SelectionList->GetFlag(nPoint,SL_SELECTED))
                if(!(Point(nBlock,nPoint)[nFrame].GetKeyPoint()))
					MorphPoint::Paint(ViewPoints[nPoint],
						MorphConfig.nPointSize,MorphConfig.nPointShape,hDC);

		SelectObject(hDC,MorphConfig.NormalBorderKnot);

		for(    nPoint = 0 ; nPoint < GetPoints() ; nPoint++ )
			if(!SelectionList->GetFlag(nPoint,SL_SELECTED))
                if(Point(nBlock,nPoint)[nFrame].GetKeyPoint())
					MorphPoint::Paint(ViewPoints[nPoint],
						MorphConfig.nPointSize,MorphConfig.nPointShape,hDC);

		// Select selection colors
		SelectObject(hDC,MorphConfig.SelectedBorder);
		SelectObject(hDC,MorphConfig.SelectedInside);

    	// Paint Selections
		for(    nPoint = 0 ; nPoint < GetPoints(); nPoint ++ )
    		if(SelectionList->GetFlag(nPoint,SL_SELECTED))
				if(!(Point(nBlock,nPoint)[nFrame].GetKeyPoint()))
					MorphPoint::Paint(ViewPoints[nPoint],
						MorphConfig.nPointSize,MorphConfig.nPointShape,hDC);

		SelectObject(hDC,MorphConfig.SelectedBorderKnot);

		// Paint Selections
		for(    nPoint = 0 ; nPoint < GetPoints(); nPoint ++ )
    		if(SelectionList->GetFlag(nPoint,SL_SELECTED))
				if(Point(nBlock,nPoint)[nFrame].GetKeyPoint())
					MorphPoint::Paint(ViewPoints[nPoint],
						MorphConfig.nPointSize,MorphConfig.nPointShape,hDC);
	};

	SelectObject(hDC,hBrushTemp);
	SelectObject(hDC,hPenTemp);
};

void TMorphmesh::SwapPoints(int nFirst,int nSecond)
{
	TMorphmeshPoints::SwapPoints(nFirst,nSecond);
	if(MorphWnd){
		MorphWnd->ViewPointUpdate(nFirst);
		MorphWnd->ViewPointUpdate(nSecond);
    };
};

void TMorphmesh::PointNew(Point2d pPoint)
{
	SelectionList->AddItem(0);

	TMorphmesh_Data::PointNew(pPoint);
		
	// Update Morphwindow
	if(MorphWnd)
		MorphWnd->PointAdded();
};

void TMorphmesh::PointNew(Point2d pPoint,BOOL fTriangulate,int nBlock,int nFrame)
{
	PointNew(pPoint);

	// Triangulate if so configured
	if(fTriangulate)
		Triangulate(nBlock,nFrame);
};

void TMorphmesh::PointNew(
			MorphPoint *pStart,int nStart,
			MorphPoint *pEnd,int nEnd,
			MorphPoint *pResult,int nResult )
{
	SelectionList->AddItem(0);

	TMorphmesh_Data::PointNew(pStart,nStart,pEnd,nEnd,pResult,nResult);
		
	// Update Morphwindow
	if(MorphWnd)
		MorphWnd->PointAdded();
};

void TMorphmesh::PointNew(
			MorphPoint *pStart,int nStart,
			MorphPoint *pEnd,int nEnd,
			MorphPoint *pResult,int nResult,
			BOOL fTriangulate,int nBlock,int nFrame)
{
	PointNew(pStart,nStart,pEnd,nEnd,pResult,nResult);
		
	// Triangulate if so configured
	if(fTriangulate)
		Triangulate(nBlock,nFrame);
};

void TMorphmesh::PointDelete(int nPoint)
{
	 // Update Morphwindow
	if(MorphWnd)
		MorphWnd->PointDeleted(nPoint);

	TMorphmesh_Data::PointDelete(nPoint);

	SelectionList->DeleteItem(nPoint);
};

void TMorphmesh::PointFetch(int nPoint,TMorphmeshPoints *pDestMesh)
{
	 // Update Morphwindow
	if(MorphWnd)
		MorphWnd->PointDeleted(nPoint);

	TMorphmesh_Data::PointFetch(nPoint,pDestMesh);

	SelectionList->DeleteItem(nPoint);
};

void TMorphmesh::SwapBlocks(int nBlock1,int nBlock2)
{
    TMorphmesh_Data::SwapBlocks(nBlock1,nBlock2);

	// Update Morphwindow
	if(MorphWnd){
		MorphWnd->FramesChanged();
		MorphWnd->ViewPointsUpdate(GetPoints());
    };
};

void TMorphmesh::TriangleAdd(TTriangle& Tri)
{
	int nOldTriangles = nTriangles;

	TMorphmesh_Data::TriangleAdd(Tri);

    if(nOldTriangles!=nTriangles){
		// Check for duplicate lines
		Triangles[nTriangles-1].Set_Lines_Unique();

		// Remove lines from new triangle if they are already drawn
		for(int nIndex = 0 ; nIndex < nTriangles - 1 ; nIndex++)
			Triangles[nTriangles-1].Remove_Duplicate_Lines(Triangles[nIndex]);

	    // Update Selection connections
		UpdateConnections(Triangles[nTriangles-1],1);

		// Update Morphwindow
		if(MorphWnd)
			MorphWnd->InvalidateTriangle(Triangles[nTriangles-1]);
	};
};

void TMorphmesh::TriangleDelete(TTriangle &Tri)
{
	// Update Morphwindow
	if(MorphWnd)
		MorphWnd->InvalidateTriangle(Tri);

	 // Move drawn duplicate lines to other triangles
	for(int nIndex = 0 ; nIndex < nTriangles ; nIndex++)
		if(&Triangles[nIndex]!=&Tri)
			Tri.Add_Duplicate_Lines(Triangles[nIndex]);

    // Update Selection connections
    UpdateConnections(Tri,-1);

	TMorphmesh_Data::TriangleDelete(Tri);
};

void TMorphmesh::TrianglesDeleteWithPoints(ItemList * pPoints,int nMinConnections,int nMaxConnections)
{
	int *nTriangleConnections = new int [nTriangles];

	for( int nTri = 0 ; nTri < nTriangles ; nTri ++ )
		nTriangleConnections[nTri] = 0;

    if(pPoints){
	    // Count connections to triangles
		for(int nPoint = 0 ; nPoint < pPoints->GetItems() ; nPoint ++ )
			// If point was selected,
			if(pPoints->GetFlag(nPoint,SL_SELECTED))
				for(int nTri = 0 ; nTri < nTriangles ; nTri++ )
					// If triangle has a point
					if(Triangles[nTri].HasVertex(nPoint))
						nTriangleConnections[nTri]++;
	}

    // Delete triangles
	for(    nTri = 0; nTri < nTriangles ; nTri++)
		if((pPoints==NULL)||(
			(nTriangleConnections[nTri]>=nMinConnections)&&
			(nTriangleConnections[nTri]<=nMaxConnections))){
			// Delete triangle from connectionlist
			nTriangleConnections[nTri] = nTriangleConnections[nTriangles-1];
			// Delete triangle
            TriangleDelete(Triangles[nTri]);
			nTri--;
			};

	delete nTriangleConnections;
};
 
void TMorphmesh::Triangulate(int nBlock,int nFrame)
{
	// Normal triangulation
	TMorphmesh_Data::Triangulate(nBlock,nFrame);

	// Update Morphwindow
	if(MorphWnd)
		MorphWnd->InvalidateAll();
};

void TMorphmesh::Interpolate(int bBlock)
{
	TMorphmesh_Data::Interpolate(bBlock);

	// Update all points in Morphwindow
	if(MorphWnd)
		MorphWnd->ViewPointsUpdate(GetPoints());
};

void TMorphmesh::Interpolate(int bBlock,int nPoint)
{
	TMorphmesh_Data::Interpolate(bBlock,nPoint);

	// Update all points in Morphwindow
	if(MorphWnd)
		MorphWnd->ViewPointsUpdate(GetPoints());
};

void TMorphmesh::Interpolate(int nBlock,ItemList *pPointList)
{
	TMorphmesh_Data::Interpolate(nBlock,pPointList);

	// Update Morphwindow
	if(MorphWnd)
		MorphWnd->ViewPointsUpdate(GetPoints());
};

void TMorphmesh::Compute(TFunction2d *pFunction)
{
	TMorphmesh_Data::Compute(pFunction);

	// Update all points in Morphwindow
	if(MorphWnd)
		MorphWnd->ViewPointsUpdate(GetPoints());
};

void TMorphmesh::CreateGrid(int nXGrid,int nYGrid,BOOL fTriangulate)
{
	PointsDelete(NULL,FALSE,1,1);

	for(int nX = 0 ; nX <= nXGrid ; nX++ ){
		for ( int nY = 0 ; nY <= nYGrid ; nY++ ){
			PointNew(Point2d(
				(double)nX/(double)nXGrid,
                (double)nY/(double)nYGrid));
    	}
	}        

	// Triangulate if so configured
	if(fTriangulate)
		Triangulate(1,1);
};

BOOL TMorphmesh::StretchFrames(int nNewFrames[3])
{
	BOOL fFramesChanged = TMorphmesh_Data::StretchFrames(nNewFrames);

	// Stretch MorphBlock
	if(fFramesChanged)
		// Update Morphwindow
		if(MorphWnd)
			MorphWnd->FramesChanged();
	return fFramesChanged;
};

void TMorphmesh::Cut(BOOL fTriangulate,int nSelBlock,int nSelFrame)
{
	// Create clipboard
	TClipBoard *pClipBoard = new TClipBoardPoints(this,SelectionList,fTriangulate,nSelBlock,nSelFrame);

	// Set clipboard
	MorphWnd->SetClipBoard(pClipBoard);
};

void TMorphmesh::Copy(int nBlock,int nStartFrame,int nEndFrame)
{
	// Create clipboard
	TClipBoard *pClipBoard = new TClipBoardFrames(this,SelectionList,nBlock,nStartFrame,nEndFrame);

	// Set clipboard
	MorphWnd->SetClipBoard(pClipBoard);
};

void TMorphmesh::Paste(BOOL fTriangulate,int nSelBlock,int nSelFrame)
{
	Paste(MorphWnd->GetClipBoard(),fTriangulate,nSelBlock,nSelFrame);
};

void TMorphmesh::Paste(TClipBoard *pClipBoard,BOOL fTriangulate,int nSelBlock,int nSelFrame)
{
	pClipBoard->Paste(this,fTriangulate,nSelBlock,nSelFrame);

	if(MorphWnd)
    	MorphWnd->ViewPointsUpdate(GetPoints());
};

/////////////////////////
// Selection functions
 
void TMorphmesh::SelectionClear(void)
{
    // Remove all SELECTED and CONNECTED marks
	for(int nIndex = 0 ; nIndex < SelectionList->GetItems() ; nIndex ++ ){
		// If point was selected,
		if(SelectionList->GetFlag(nIndex,SL_SELECTED))
			// invalidate it
			if(MorphWnd)
				MorphWnd->PointInvalidate(nIndex);

        // Remove marks
		SelectionList->SetItem(nIndex,0);
		};

    nSelections = 0; 

    // Update menu
	if(MorphWnd)
		MorphWnd->UpdateSelectionMenu();
};

void TMorphmesh::SelectionAddAll(void)
{
    // Remove all SELECTED and CONNECTED marks
	for(int nIndex = 0 ; nIndex < SelectionList->GetItems() ; nIndex ++ ){
		// Remove marks
		SelectionList->SetItem(nIndex,0);
		};

	// Add all points
	for(   nIndex = 0 ; nIndex < GetPoints() ; nIndex ++ ){
        SelectionAdd(nIndex);
		};

	// Update menu
	if(MorphWnd)
		MorphWnd->UpdateSelectionMenu();
};

void TMorphmesh::SelectionAdd(int nNewSelection)
{
	SelectionToggle(nNewSelection,TRUE);
};

void TMorphmesh::SelectionRemove(int nSelection)
{
	SelectionToggle(nSelection,FALSE);
};

void TMorphmesh::SelectionToggle(int nSelection)
{
	SelectionToggle(nSelection,
		!SelectionList->GetFlag(nSelection,SL_SELECTED));

	// Update menu
	if(MorphWnd)
		MorphWnd->UpdateSelectionMenu();
};

void TMorphmesh::SelectionToggle(int nSelection,BOOL fSelect)
{
	assert(SelectionList->GetFlag(nSelection,SL_SELECTED)!= fSelect);

	if(fSelect)
		nSelections++;
	   else
		nSelections--;

	SelectionList->SetFlag(nSelection,SL_SELECTED,fSelect);

	if(MorphWnd)
			MorphWnd->PointInvalidate(nSelection);

	// Update Connection count
	UpdateConnections(nSelection,(fSelect ? +1 : -1 ));
};

void TMorphmesh::ClearConnections(void)
{
	for(int nPoint = 0 ; nPoint < SelectionList->GetItems() ; nPoint++)
		SelectionList->SetItem(nPoint,
        	SelectionList->GetItem(nPoint)&SL_SELECTED);
};

void TMorphmesh::UpdateConnections(TTriangle &tTri,int nDirection)
{
    // Loop all vertexes
	for(int nVertex =0 ; nVertex < 3 ; nVertex++){
		// If point was selected
		if(SelectionList->GetFlag(tTri.GetVertex(nVertex),SL_SELECTED))
            // Set other vertexes as connected
        	for(int nVert2 = 0 ; nVert2 < 3 ; nVert2++){
                if(nVert2!=nVertex){
                    int nVert2Point = tTri.GetVertex(nVert2);
					SelectionList->SetItem(nVert2Point,
						SelectionList->GetItem(nVert2Point)+ nDirection);
                    }
			}
    }
};

void TMorphmesh::UpdateConnections(int nPoint,int nDirection)
{
	for(int nTri = 0 ; nTri < nTriangles ; nTri++){
		if(Triangles[nTri].HasVertex(nPoint)){
			for(int nVertex = 0 ; nVertex < 3 ; nVertex++ ){
				int nVertPoint = Triangles[nTri].GetVertex(nVertex);
				 if(nVertPoint != nPoint)
					SelectionList->SetItem(nVertPoint,
                       	SelectionList->GetItem(nVertPoint)+ nDirection);
				}; /* for */
			}; /* if */
		}; /* for */
};

void TMorphmesh::SelectionMoveStart(int nMoveMode,int nBlock,int nStartFrame,int nCrrntFrame,int nEndFrame)
{
	pMoveUndo = new TPointMoveUndo((TMorphmeshUndo*)this,SelectionList,
			nMoveMode,nBlock,nStartFrame,nCrrntFrame,nEndFrame);
};

void TMorphmesh::SelectionMove(Point2d pOrigin,Point2d pRelative)
{
	pMoveUndo->pOrigin = pOrigin;
	pMoveUndo->pRelative = pRelative;

    pMoveUndo->Do();
};

void TMorphmesh::SelectionMoveEnd(void)
{
	if(pMoveUndo->fSelectionMoved){
		// Interpolate point
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
		};

    // Delete MoveStart structure ( could move it to undo buffer )
    delete pMoveUndo;
};

void TMorphmesh::PointsFetch(ItemList *pPointList,TMorphmeshPoints *pDestMesh)
{
	// Move points to dest mesh
	TMorphmeshPoints::PointsFetch(pPointList,pDestMesh);

	// Clear selectionlist
	SelectionClear();

	// Update menu
	if(MorphWnd)
		MorphWnd->UpdateSelectionMenu();
};

void TMorphmesh::PointsFetch(ItemList *pPointList,
		BOOL fTriangulate,int nBlock,int nFrame,TMorphmeshPoints *pDestMesh)
{
	if(!fTriangulate)
		// Delete triangles connected to points
		TrianglesDeleteWithPoints(pPointList,1,3);

    // Move points to dest mesh 
	PointsFetch(pPointList,pDestMesh);

	// Triangulate if so configured
	if(fTriangulate)
		Triangulate(nBlock,nFrame);
};

void TMorphmesh::PointsRestore(TMorphmeshPoints *pSrcMesh,ItemList *pPointList)
{
	// Clear selectionlist
	SelectionClear();

	// This function doesn't restore triangles !!!

    // Restore points
    TMorphmeshPoints::PointsRestore(pSrcMesh,pPointList);

	// Update menu
	if(MorphWnd)
		MorphWnd->UpdateSelectionMenu();
};

void TMorphmesh::PointsDelete(ItemList *pPointList,BOOL fTriangulate,int nBlock,int nFrame)
{
	TMorphmeshPoints *pMesh = new TMorphmeshPoints(
    		GetFrames(MorphStart),GetFrames(MorphResult),GetFrames(MorphEnd));

	PointsFetch(pPointList,fTriangulate,nBlock,nFrame,pMesh);

	delete pMesh;
};

void TMorphmesh::SelectionPointsDelete(BOOL fTriangulate,int nBlock,int nFrame)
{
	PointsDelete(SelectionList,fTriangulate,nBlock,nFrame);
};

void TMorphmesh::DetachPoints(ItemList *pPointList,int nBlock,int nStartFrame,int nEndFrame)
{
	TMorphmesh_Data::DetachPoints(pPointList,nBlock,nStartFrame,nEndFrame);
};

void TMorphmesh::SelectionDetachPoints(int nBlock,int nStartFrame,int nEndFrame)
{
	DetachPoints(SelectionList,nBlock,nStartFrame,nEndFrame);
};

void TMorphmesh::TriangulateAll(int nBlock,int nFrame)
{
	if(nSelections==3){
		TTriangle tNewTriangle;
		int nPoint;

		for(nPoint=0;SelectionList->GetFlag(nPoint,SL_SELECTED)==FALSE ; nPoint++);
		tNewTriangle.SetVertex(0,nPoint);

		for(nPoint++;SelectionList->GetFlag(nPoint,SL_SELECTED)==FALSE ; nPoint++);
		tNewTriangle.SetVertex(1,nPoint);

		for(nPoint++;SelectionList->GetFlag(nPoint,SL_SELECTED)==FALSE ; nPoint++);
		tNewTriangle.SetVertex(2,nPoint);
		TriangleAdd(tNewTriangle);
	} else
		Triangulate(nBlock,nFrame);
};

void TMorphmesh::TrianglesDeleteBetweenPoints(void)
{
	TrianglesDeleteWithPoints(SelectionList,3,3);
};

void TMorphmesh::TrianglesDeleteWithPoints(void)
{
	TrianglesDeleteWithPoints(SelectionList,1,3);
};

void TMorphmesh::SelectionTriangulate(int bBlock,int nFrame)
{
	// Clear the old triangles
	TrianglesDeleteWithPoints(SelectionList,3,3);

	// Count number of selected points
    int nSelectionPoints = 0;
	for(int nPoint = 0 ; nPoint < GetPoints() ; nPoint++ )
		if(SelectionList->GetFlag(nPoint,SL_SELECTED))
			nSelectionPoints++;

	// Map from selection points to real points
    int* nInverseMap = new int[nSelectionPoints];
	for(int nSelectionPoint = 0 ; nSelectionPoint < nSelectionPoints ; nSelectionPoint++ )
		nInverseMap[nSelectionPoint] = -1;

	// Create triangulator
	Delaunay *dDelaunay = new Delaunay();

	// Feed points to Delaunay triangulation algorithm
	dDelaunay->StartAddingPoints(nSelectionPoints);
    nSelectionPoint = 0;
	for(    nPoint = 0 ; nPoint < GetPoints() ; nPoint++ )
		if(SelectionList->GetFlag(nPoint,SL_SELECTED)){
			// Feed point to Delaunay
			Point2d pPoint = Point(bBlock,nPoint)->GetPoint(nFrame);
			dDelaunay->AddPoint(pPoint.x,pPoint.y);

            // Update inverse map
            nInverseMap[nSelectionPoint] = nPoint;
            nSelectionPoint++;
    		};
    assert(nSelectionPoint == nSelectionPoints);

	// Triangulate
	dDelaunay->Triangulate(FALSE);

	// Get triangles from triangulator and add them to morphmesh
	int *nTri = NULL;
	TTriangle tNewTriangle;
	dDelaunay->SetToFirstTriangle();
	while((nTri = dDelaunay->GetNextTriangle())!=NULL){
		tNewTriangle.SetVertex(0,nInverseMap[nTri[0]]);
		tNewTriangle.SetVertex(1,nInverseMap[nTri[1]]);
		tNewTriangle.SetVertex(2,nInverseMap[nTri[2]]);
		TriangleAdd(tNewTriangle);
		};

	 // Delete triangulator
	delete dDelaunay;

    // Delete inverse map
	delete nInverseMap;
};

/* End of file MORPHMSH.CPP */


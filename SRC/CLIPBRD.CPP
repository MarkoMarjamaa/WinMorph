/**********************************************************************
 * 
 *  NAME:         CLIPBRD.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when      who what
 *  ------------------------------------------------------------------- 
 *  1.9580  14.09.96  MHM created
 *
 *********************************************************************/ 
#include "clipbrd.h" 
#include "morphmsh.h"
#include "itemlist.h"

/////////////////////////////////////////////////////////
// TClipBoardFrames
////////////////////////////////////////////////////////

TClipBoardFrames::TClipBoardFrames(TMorphmesh *pMesh,
	ItemList* pPointList,int nNewBlock,int nStartFrame,int nEndFrame)
{
	nBlock 		= nNewBlock;
	pBlock		= pMesh->mBlock[nBlock]->CreateCopy(pPointList,nStartFrame,nEndFrame);
};

TClipBoardFrames::TClipBoardFrames(TClipBoardFrames *pSrc)
{
	nBlock 		= pSrc->nBlock;
	pBlock		= pSrc->pBlock->CreateCopy(NULL,0,pSrc->pBlock->GetFrames());
};

TClipBoardFrames::~TClipBoardFrames()
{
	if(pBlock)
		delete pBlock;
};

void TClipBoardFrames::Paste(TMorphmesh *pMesh,BOOL,int nSelBlock,int nSelFrame)
{
    int nSelPoint = 0 ;
	for(int nPoint = 0 ; nPoint < pMesh->GetPoints() ; nPoint ++ )
		if(pMesh->SelectionList->GetFlag(nPoint,SL_SELECTED))
            if(nSelPoint < pBlock->GetPoints()){
				MorphPoint* pMeshPoint = pMesh->Point(nSelBlock,nPoint);
				MorphPoint* pClipPoint = pBlock->Point(nSelPoint);
				int nFrames = min(pBlock->GetFrames(),pMesh->GetFrames(nSelBlock) - nSelFrame);

				for(int nFrame = 0 ; nFrame < nFrames ; nFrame ++ )
					pMeshPoint[nFrame + nSelFrame ] = pClipPoint[nFrame];

				// Set first and last copied frame as keypoints
				pMeshPoint[nSelFrame].SetKeyPoint(TRUE);
				pMeshPoint[nSelFrame + nFrames - 1].SetKeyPoint(TRUE);

				// Interpolate point
				pMesh->TMorphmesh_Data::Interpolate(nSelBlock,nPoint);

				nSelPoint++;
			};
};

/////////////////////////////////////////////////////////
// TClipBoardPoints
////////////////////////////////////////////////////////

TClipBoardPoints::TClipBoardPoints(TMorphmesh *pMesh,ItemList *pPointList,BOOL fTriangulate,int nSelBlock,int nSelFrame)
{
	// Create mesh
	pClipMesh = new TMorphmeshPoints(
			pMesh->GetFrames(MorphStart),
			pMesh->GetFrames(MorphResult),
			pMesh->GetFrames(MorphEnd));

	// Fetch points
	pMesh->TMorphmesh::PointsFetch(pPointList,fTriangulate,nSelBlock,nSelFrame,pClipMesh);
};

TClipBoardPoints::TClipBoardPoints(TClipBoardPoints *pSrc)
{
 
    // Create mesh
	pClipMesh = new TMorphmeshPoints(
			pSrc->pClipMesh->GetFrames(MorphStart),
			pSrc->pClipMesh->GetFrames(MorphResult),
			pSrc->pClipMesh->GetFrames(MorphEnd));

	// Copy points to clipmesh 
	pSrc->pClipMesh->PointsCopy(NULL,pClipMesh);
};

TClipBoardPoints::~TClipBoardPoints()
{
    if(pClipMesh)
	    delete pClipMesh;
};

void TClipBoardPoints::Paste(TMorphmesh *pMesh,BOOL fTriangulate,int nBlock,int nFrame)
{
	for(int nPoint = 0 ; nPoint < pClipMesh->GetPoints() ; nPoint ++)
		pClipMesh->PointCopy(nPoint,pMesh);

	if(fTriangulate)
		pMesh->TMorphmesh::Triangulate(nBlock,nFrame);
};

/* End of file CLIPBRD.CPP */
 

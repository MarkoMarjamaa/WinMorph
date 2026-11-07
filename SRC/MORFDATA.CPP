/**********************************************************************
 * 
 *  NAME:         MORFDATA.CPP 
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
 *********************************************************************/ 
 
#include <string.h>
#include <assert.h>
#include "filefunc.h"

#include "morfdata.h"
#include "delaunay.h"
#include "animatio.h"
#include "animndlg.h"
#include "itemlist.h"

/////////////////////////////////////////////////////////////////
// TMorphmesh_Document
////////////////////////////////////////////////////////////////

TMorphmesh_Document::TMorphmesh_Document()
{
};

LPSTR TMorphmesh_Document::GetSavePrompt(void)
{ return "Morphmesh has been changed. Save file before closing?";};

LPSTR TMorphmesh_Document::GetSaveFileName(void)
{  return Document::GetFileName(GetSaveFilter(),GetSaveDefExt(),Write);};

///////////////////////////////////////////////////////////////
// TMorphmeshPoints
/////////////////////////////////////////////////////////////////

LPSTR TMorphmeshPoints::MeshNames[4]={
	"Startmesh",	"Resultmesh",
	"Endmesh",  	NULL	};

TMorphmeshPoints::TMorphmeshPoints() :
	TMorphmesh_Document()
{
	Init();
};

TMorphmeshPoints::TMorphmeshPoints(int nStartFrames,int nResultFrames,int nEndFrames)
{
	// First initialize the structure
	Init();

	// Then create the morphblocks
	mBlock[MorphStart]   = new MorphBlock(nStartFrames);
	mBlock[MorphResult]  = new MorphBlock(nResultFrames);
	mBlock[MorphEnd]	 = new MorphBlock(nEndFrames);
};

TMorphmeshPoints::~TMorphmeshPoints()
{
	Reset();
};

void TMorphmeshPoints::Init(void)
{
	 // Init morphblocks
	mBlock[MorphStart]   = NULL;
	mBlock[MorphResult]  = NULL;
	mBlock[MorphEnd]  	 = NULL;

	nPoints     = 0;
};

// Should reset the morphblock to first state
void TMorphmeshPoints::Reset()
{
	// Delete morphblocks
	for(int nBlock = 0 ; nBlock < 3 ; nBlock++ )
		if(mBlock[nBlock]){
			delete mBlock[nBlock];
			mBlock[nBlock] = NULL;
				};

	// Mark morphmesh as changed
	SetChanged(TRUE);
};

void TMorphmeshPoints::SwapPoints(int nFirst,int nSecond)
{
	for(int nBlock = 0 ; nBlock < 3 ; nBlock++ )
		mBlock[nBlock]->SwapPoints(nFirst,nSecond);
};

void TMorphmeshPoints::PointNew(Point2d pPoint)
{
	// Add new point
	nPoints++;

	// Add new point to morphblocks
	for(int nBlock = 0 ; nBlock < 3 ; nBlock++ )
		mBlock[nBlock]->NewPoint(pPoint);

	// Mark morphmesh as changed
	SetChanged(TRUE);
};

void TMorphmeshPoints::PointNew(
			MorphPoint *pStart,int nStart,
			MorphPoint *pEnd,int nEnd,
			MorphPoint *pResult,int nResult )
{
	// Add new point
	nPoints++;

	// Add new point to morphblocks
	mBlock[MorphStart]->NewPoint(pStart,nStart);
	mBlock[MorphEnd]->NewPoint(pEnd,nEnd);
	mBlock[MorphResult]->NewPoint(pResult,nResult);

	// Mark morphmesh as changed
	SetChanged(TRUE);
};

void TMorphmeshPoints::PointDelete(int nIndex)
{
	assert(nIndex != NO_POINTS);
	assert(nIndex < GetPoints());

	// Delete points from morphblocks
	for(int nBlock = 0 ; nBlock < 3 ; nBlock++ )
		mBlock[nBlock]->DeletePoint(nIndex);

	/* Delete last point */
	nPoints--;

	// Mark morphmesh as changed
	SetChanged(TRUE);
};

void TMorphmeshPoints::PointFetch(int nPoint,TMorphmeshPoints *pDestMesh)
{
	MorphPoint *pStart,*pEnd,*pResult;

    // Fetch points from blocks
	pStart = mBlock[MorphStart] ->FetchPoint(nPoint);
	pEnd   = mBlock[MorphEnd]   ->FetchPoint(nPoint);
	pResult= mBlock[MorphResult]->FetchPoint(nPoint);

	/* Delete last point */
	nPoints--;

	// Add point to dest mesh
	pDestMesh->PointNew(
		pStart,	mBlock[MorphStart]->GetFrames(),
		pEnd,	mBlock[MorphEnd]->GetFrames(),
		pResult,mBlock[MorphResult]->GetFrames());

	// Mark morphmesh as changed
	SetChanged(TRUE);
};

void TMorphmeshPoints::PointsFetch(ItemList *pPointList,TMorphmeshPoints *pDestMesh)
{
	if(pPointList){
		ItemList *pTempList = new ItemList(pPointList);

		for(int nIndex = GetPoints() -1 ; nIndex >=0 ; nIndex -- )
			if(pTempList->GetFlag(nIndex,SL_SELECTED)){
        	    pTempList->DeleteItem(nIndex);
				PointFetch(nIndex,pDestMesh);
				};

		delete pTempList;
	} else {
		for(int nIndex = GetPoints() -1 ; nIndex >=0 ; nIndex -- )
			PointFetch(nIndex,pDestMesh);
	}

};

void TMorphmeshPoints::PointCopy(int nPoint,TMorphmeshPoints *pDestMesh)
{
	MorphPoint *pStart,*pEnd,*pResult;

    // Copy points from blocks
	pStart = mBlock[MorphStart] ->PointCopy(nPoint);
	pEnd   = mBlock[MorphEnd]   ->PointCopy(nPoint);
	pResult= mBlock[MorphResult]->PointCopy(nPoint);

	// Add point to dest mesh
	pDestMesh->PointNew(
		pStart,	mBlock[MorphStart]->GetFrames(),
		pEnd,	mBlock[MorphEnd]->GetFrames(),
		pResult,mBlock[MorphResult]->GetFrames());
};

void TMorphmeshPoints::PointsCopy(ItemList *pPointList,TMorphmeshPoints *pDestMesh)
{
	for(int nIndex = GetPoints()-1 ; nIndex >=0 ; nIndex -- )
		if((pPointList==NULL)||(pPointList->GetFlag(nIndex,SL_SELECTED)))
			PointCopy(nIndex,pDestMesh);
};

void TMorphmeshPoints::PointsRestore(TMorphmeshPoints *pSrcMesh,ItemList *pPointList)
{
	int nListPoint = -1;
	for(int nPoint = pSrcMesh->GetPoints()-1 ; nPoint >= 0 ; nPoint -- ){

		// Search next selected point from list
		do {
            nListPoint++;
		} while((pPointList!=NULL)&&
			(pPointList->GetFlag(nListPoint,SL_SELECTED)==FALSE));

		// Add point to dest mesh
		PointNew(
			pSrcMesh->mBlock[MorphStart] ->PointCopy(nPoint),
			pSrcMesh->mBlock[MorphStart]->GetFrames(),
			pSrcMesh->mBlock[MorphEnd]->PointCopy(nPoint),
			pSrcMesh->mBlock[MorphEnd]->GetFrames(),
			pSrcMesh->mBlock[MorphResult]->PointCopy(nPoint),
			pSrcMesh->mBlock[MorphResult]->GetFrames());

        // Swap points
		if(nListPoint != GetPoints()-1)
			SwapPoints(nListPoint,GetPoints()-1);
	};
};

void TMorphmeshPoints::DetachPoints(ItemList *pPointList,int nBlock,int nStartFrame,int nEndFrame)
{
	nStartFrame = (nStartFrame != -1) ? nStartFrame : 0 ;
	nEndFrame   = (nEndFrame != -1) ? nEndFrame :
						((nBlock!=-1)? GetFrames(nBlock)-1 : -1 );

	// Loop all block or only the selected one
	for(int nCrrntBlock = (nBlock != -1) ? nBlock : 0 ;
		nCrrntBlock < ((nBlock != -1) ? nBlock + 1 : 3) ; nCrrntBlock ++ ){
		int nStartFr,nEndFr;

        // If all blocks used, use all frames, otherwise the selected frames
        nStartFr = (nBlock != -1) ? nStartFrame : 0 ;
		nEndFr   = (nBlock != -1) ? nEndFrame : GetFrames(nCrrntBlock) - 1;
        // Check that first and last frames are not released
		nStartFr = max(nStartFr,1);
		nEndFr   = min(nEndFr,GetFrames(nCrrntBlock) - 1);

		// Loop all selected points
		for(int nPoint = 0 ; nPoint < GetPoints() ; nPoint++ )
			if(pPointList->GetFlag(nPoint,SL_SELECTED)){
				MorphPoint* SelectionPoint = Point(nCrrntBlock,nPoint);

                for(int nFrame = nStartFr ; nFrame < nEndFr ; nFrame ++ )
				// Set release point
					SelectionPoint[nFrame].SetKeyPoint(FALSE);
				};
		};

    Interpolate(nBlock,pPointList);
};

void TMorphmeshPoints::SwapBlocks(int nBlock1,int nBlock2)
{
	SWAP(MorphBlock *,mBlock[nBlock1],mBlock[nBlock2]);
};

// Interpolate all points in one morphblock
void TMorphmeshPoints::Interpolate(int nBlock)
{
	mBlock[nBlock]->Interpolate();
};

// Interpolate one point in one morphblock
void TMorphmeshPoints::Interpolate(int nBlock,int nPoint)
{
	 mBlock[nBlock]->Interpolate(nPoint);
};

void TMorphmeshPoints::Interpolate(int nBlock,ItemList *pPointList)
{
    int nStartBlock,nEndBlock;

	nStartBlock = (nBlock!=-1)? nBlock : 0;
	nEndBlock   = (nBlock!=-1)? nBlock + 1 : 3;

	for(int nPoint = 0 ; nPoint < GetPoints() ; nPoint ++ )
		if(pPointList->GetFlag(nPoint,SL_SELECTED))
			for(int nBlock = nStartBlock; nBlock < nEndBlock ; nBlock ++ )
				mBlock[nBlock]->Interpolate(nPoint);
};

void TMorphmeshPoints::Compute(	TFunction2d *pFunction)
{
	mBlock[MorphResult]->Compute(
		pFunction,
		mBlock[MorphStart],mBlock[MorphEnd]);
};

// Stretch morphblocks
BOOL TMorphmeshPoints::StretchFrames(int nNewFrames[3])
{
	//Stretch morphblock only if number of frames has been changed
	BOOL fFramesChanged = FALSE;

	// Stretch morphblocks
    for(int nBlock = 0 ; nBlock < 3 ; nBlock ++ ){
		if(nNewFrames[nBlock]!=GetFrames(nBlock)){
			mBlock[nBlock]->StretchFrames(nNewFrames[nBlock]);
			fFramesChanged = TRUE;
            };
		};

    if(fFramesChanged)
		// Mark morphmesh as changed
		SetChanged(TRUE);

	return fFramesChanged;
};

BOOL TMorphmeshPoints::LoadMorphBlock(wmfstream& file,int bBlock)
{
	// Check that we are reading the right morphblock
	if(!GetKeyword(file,MeshNames[bBlock])) return FALSE;

	return mBlock[bBlock]->Load(file);
};

BOOL TMorphmeshPoints::LoadDocument(wmfstream& file)
{
	BOOL bErrors = TRUE;

	do{
		// Read Number of points
		if(!GetKeyword(file,"Points"))  break;

		int nNewPoints;
		file >> nNewPoints;

		// Create MorphBlocks with 1 frame
		assert(mBlock[MorphStart]==NULL);
		mBlock[ MorphStart] = new MorphBlock(1);
		mBlock[MorphResult] = new MorphBlock(1);
		mBlock[   MorphEnd] = new MorphBlock(1);

		// Add points
		Point2d pPoint(0.0,0.0);
		for(int nPoint = 0 ; nPoint < nNewPoints ; nPoint++ )
			 PointNew(pPoint);

		// Read MorphBlocks
		if(!GetStartOfBlock(file))      break;

		if(!LoadMorphBlock(file,MorphStart) )  break;
		if(!LoadMorphBlock(file,MorphEnd)   )  break;
		if(!LoadMorphBlock(file,MorphResult))  break;

		if(!GetEndOfBlock(file))            break;

		//Interpolate all Morphpoints
		for(int bBlock = 0 ; bBlock < 3 ; bBlock++ )
			Interpolate(bBlock);

		// Morphmesh was previously marked as changed, so we have to unmark it
		SetChanged(FALSE);

		bErrors = FALSE;
		}
	while(FALSE);

	 // If there were errors
	if(bErrors)
		// Reset the morphmesh
		Reset();

    return !bErrors;
};

void TMorphmeshPoints::SaveMorphBlock(wmfstream& file,int bBlock)
{
	 // Save morphblock name
	file << "\t" << MeshNames[bBlock] << "\n";

	 // Save Morphblock
	 mBlock[bBlock]->Save(file);
};

void TMorphmeshPoints::SaveDocument(wmfstream& file)
{
	// Save Number of morphpoints
	file << "Points " << GetPoints() << " (\n";

	// Save Morphblocks
	SaveMorphBlock(file,MorphStart);
	SaveMorphBlock(file,MorphEnd);
	SaveMorphBlock(file,MorphResult);

	file << "\t)\n";
};

///////////////////////////////////////////////////////////////
// TMorphmesh_Triangle
/////////////////////////////////////////////////////////////////

TMorphmeshTriangles::TMorphmeshTriangles() :
	TMorphmesh_Document()
{
	Init();
};

TMorphmeshTriangles::~TMorphmeshTriangles()
{
	Reset();
};

void TMorphmeshTriangles::Init(void)
{
	 // Init triangles
	TrianglesAllocated   = 0;
	Triangles      = NULL;
	nTriangles     = 0;
};

void TMorphmeshTriangles::Reset()
{
	 // Clear triangle enumerator
	nCurrentTriangle = 0;

	// Delete all triangles
	nTriangles = 0;

	// Delete triangle array
	TrianglesResize(0);

	// Mark morphmesh as changed
	SetChanged(TRUE);
};

void TMorphmeshTriangles::TrianglesResize(int nNewSize)
{
	TrianglesSet(Triangles,nNewSize,nTriangles);
};

void TMorphmeshTriangles::TrianglesSet(
				TTriangle* pSrcTriangles,
                int nSrcAllocated,
				int nSrcTriangles)
{
	assert(nSrcAllocated >= nSrcTriangles);

	TTriangle *pNewTriangles = NULL;

	do {
	 	// Assure that nNewsize is a positive number
		nSrcAllocated = max(nSrcAllocated,0);

		// If nNewsize is greater than zero, create new triangle array
		if(nSrcAllocated > 0){
			// Create triangle array
			pNewTriangles = new TTriangle [nSrcAllocated];
			// Check for errors
			if(!pNewTriangles){
				assert(FALSE);
				break;
				}

			// Copy old triangle array to new one
			for(int nTriangle = 0 ; nTriangle < min(nSrcAllocated,nSrcTriangles) ;
					nTriangle++ )
				pNewTriangles[nTriangle] = pSrcTriangles[nTriangle];
			};
		// Assign new triangles
		SWAP(TTriangle *,Triangles,pNewTriangles);
		SWAP(int, TrianglesAllocated,nSrcAllocated);
		SWAP(int, nTriangles,nSrcTriangles);
		}
	 while(FALSE);

	// Delete triangle array not needed
	if(pNewTriangles)
		delete pNewTriangles;
};

void TMorphmeshTriangles::TriangleAdd(TTriangle& tTriangle)
{
	do {
		// Make room for new triangle
		if(nTriangles==TrianglesAllocated)
			TrianglesResize(TrianglesAllocated + 25);

		// Check if there was enough memory
		if(nTriangles==TrianglesAllocated)  break;
			
		// No duplicate triangles are allowed, so first check it
		if(TriangleFind(tTriangle)!=NULL)  break;

		// Add new triangle
		Triangles[nTriangles] = tTriangle;
		nTriangles++;

		// Mark morphmesh as changed
		SetChanged(TRUE);
		}
	while(FALSE);
};

void TMorphmeshTriangles::TriangleDelete(TTriangle& tTriangle)
{
	// Move the last triangle to tTriangles place
	tTriangle = Triangles[nTriangles-1];

	 // Delete last triangle
	nTriangles --;

	// Release memory
	if((TrianglesAllocated - nTriangles)>25)
		TrianglesResize(TrianglesAllocated-25);

	// Mark morphmesh as changed
	SetChanged(TRUE);
};

// Searches for triangle duplicates = same point indexes,
// not necessarily in the same order
TTriangle* TMorphmeshTriangles::TriangleFind(TTriangle& tTriangle)
{
	// Enumerate all triangles
	for(int nTriangle = 0 ; nTriangle < nTriangles ; nTriangle++ )
		  // If duplicate is found
		if(tTriangle.IsEqual(Triangles[nTriangle]))
				// return it's pointer
			return &Triangles[nTriangle];

	// Otherwise return NULL pointer
	return NULL;
};

/////////////////////////
// Triangle enumerator ( for the morpher )

void TMorphmeshTriangles::SetToFirstTriangle()
{
 // Initialize the enumerator
 nCurrentTriangle = 0;
};

TTriangle* TMorphmeshTriangles::GetNextTriangle()
{
	 // If no more triangles, return NULL pointer 
	return (WasLastTriangle() ? NULL : &(Triangles[nCurrentTriangle++]) );
};

BOOL TMorphmeshTriangles::WasLastTriangle()
{
	 // Check if no more triangles to enumerate
	return (nCurrentTriangle==nTriangles);
};

// Set Newvertex to all vertexes that had OldVertex
void TMorphmeshTriangles::SetVertex(int nOldVertex,int nNewVertex)
{
	for(int i = 0 ; i < nTriangles ; i++ ){
		TTriangle& Tri = Triangles[i];

		if(Tri.GetVertex(0)==nOldVertex) Tri.SetVertex(0,nNewVertex);
		if(Tri.GetVertex(1)==nOldVertex) Tri.SetVertex(1,nNewVertex);
		if(Tri.GetVertex(2)==nOldVertex) Tri.SetVertex(2,nNewVertex);
		};

	// Mark morphmesh as changed
	SetChanged(TRUE);
};

BOOL TMorphmeshTriangles::LoadDocument(wmfstream& file)
{
	 // Check that we are really reading the triangles
	if(!GetKeyword(file,"Triangles"))   return FALSE;

	if(!GetStartOfBlock(file))        return FALSE;

	 // Define keywords
	LPSTR szKeywords[]={"Triangle",NULL};
	#define TRIANGLE 0

	while(!file.eof()){
		//Read keyword
		int nKeyword = IdentifyKeyword(file,szKeywords);

		if(nKeyword == ENDOFBLOCK)    break;

		switch(nKeyword){
			case TRIANGLE:{
				TTriangle tTriangle;
				 // Read triangle
				file >> tTriangle;
				 // Add triangle to morphmesh
				 TriangleAdd(tTriangle);
				break;
					 };
			};
		};

	return TRUE;
};

void TMorphmeshTriangles::SaveDocument(wmfstream& file)
{
	// Save start of triangle block
	file << "Triangles " << " (\n";

	// Save triangles
	for(int nTriangle = 0 ; nTriangle < nTriangles ; nTriangle++ ){
		file << "\tTriangle ";
		file << Triangles[nTriangle] << "\n";
		};

	// Save end of triangle block 
	file << "\t)\n";
};

///////////////////////////////////////////////////////////////
// TMorphmeshPictures
/////////////////////////////////////////////////////////////////

LPSTR TMorphmeshPictures::szPictureNames[5]={
	"Start",	"Result",
	"End",  	"Background",
	NULL };

TMorphmeshPictures::TMorphmeshPictures() :
	TMorphmesh_Document()
{
	Init();
};

TMorphmeshPictures::~TMorphmeshPictures()
{
	Reset();
};

void TMorphmeshPictures::Init(void)
{
	// Init pictures
	for(int nIndex = 0 ; nIndex < 4 ; nIndex++ )
        Picture[nIndex] = NULL;
};

void TMorphmeshPictures::Reset(void)
{
	/* Delete pictures */
	for(int nIndex = 0 ; nIndex < 4 ; nIndex++ )
        SetAnimation(nIndex,NULL);
};

Animation *TMorphmeshPictures::GetAnimation(int nPicture)
{ return Picture[nPicture]; };

void TMorphmeshPictures::SetAnimation(int nPicture,Animation *pNewAnim)
{
	// Delete earlier animation
    delete GetAnimation(nPicture);

    // Set new animation
	Picture[nPicture] = pNewAnim;
};

void TMorphmeshPictures::AnimationFrameChanged(int ,int ){};
void TMorphmeshPictures::InvalidateAnimFrame(int,int){};

void TMorphmeshPictures::PictureNew(int nPicture,POINT pRes,int nFrames )
{
	// Check if the earlier animation can be closed 
	BOOL bCanClose = TRUE;
	if(GetAnimation(nPicture))
		bCanClose = GetAnimation(nPicture)->CanClose();

    if(bCanClose){
		// Should delete earlier animation here

        // Create new animation
		Animation *aNewAnimation = Animation::Create(pRes,nFrames);
        // If succeeded, set new animation
		if(aNewAnimation)
			SetAnimation(nPicture,aNewAnimation);
		};
};

void TMorphmeshPictures::PictureLoad(int nPicture,LPSTR szFilename)
{
	// Check if the earlier animation can be closed
	BOOL bCanClose = TRUE;
	if(GetAnimation(nPicture))
		bCanClose = GetAnimation(nPicture)->CanClose();

	if(bCanClose){
		// Should delete earlier animation here

		// Load new animation
		Animation* aNewAnim = Animation::Create(szFilename);

		// If succeeded, set new animation
		if(aNewAnim)				
			SetAnimation(nPicture,aNewAnim);
        };
};

void TMorphmeshPictures::PictureSave(int nPicture)
{
	if(GetAnimation(nPicture))
		GetAnimation(nPicture)->Save();
};

void TMorphmeshPictures::PictureSaveAs(int nPicture)
{
	if(GetAnimation(nPicture))
		GetAnimation(nPicture)->SaveAs();
};

// Load pictures
BOOL TMorphmeshPictures::LoadDocument(wmfstream& file)
{
	char szPicFileName[4][_MAX_PATH]={"","","",""};

	// Initialize pictures
    Init();

	// Check that we are reading the right block
	if(!GetKeyword(file,"Pictures")) return FALSE;

	// Get block start character
	if(!GetStartOfBlock(file))     return FALSE;

	while(!file.eof()){
		// Get keyword
		int nKeyword = IdentifyLabel(file,szPictureNames);

		// Check for end of block
		if(nKeyword == ENDOFBLOCK) break;

		// Check for right keyword
		if((nKeyword < 0) || (nKeyword > 3))   break;

		// Read picture name
		vReadWord(file,szPicFileName[nKeyword],_MAX_PATH);
		// Add base path to picture paths
		LPSTR szWholePath = AddPath(GetName(),szPicFileName[nKeyword]);
		_fstrcpy(szPicFileName[nKeyword],szWholePath);
		};

	// Load default pictures from Morphmesh
	for(int nPicture = 0 ; nPicture < 4 ; nPicture ++ )
		if(szPicFileName[nPicture][0]!='\0')
			PictureLoad(nPicture,szPicFileName[nPicture]);

	return TRUE;
};

void TMorphmeshPictures::SaveDocument(wmfstream& file)
{
	file << "Pictures (\n";

	for(int nIndex = 0 ; nIndex < 4 ; nIndex ++ ){
        
		if(GetAnimation(nIndex))
			if(GetAnimation(nIndex)->GetName()){
				file << "\t" << szPictureNames[nIndex] << " = ";
				// Get relative path for picture files, relative to morphmesh file
				file << szRelativePath(GetName(),GetAnimation(nIndex)->GetName()) << "\n";
			};
    	}
	file << "\t)\n";
};

///////////////////////////////////////////////////////////////
// TMorphmesh_Data
/////////////////////////////////////////////////////////////////

TMorphmesh_Data::TMorphmesh_Data() :
	TMorphmeshPoints(),
    TMorphmeshTriangles(),TMorphmeshPictures()
{
	Init();

};

TMorphmesh_Data::TMorphmesh_Data(int nStartFrames,int nResultFrames,int nEndFrames):
	TMorphmeshPoints(nStartFrames,nResultFrames,nEndFrames),
    TMorphmeshTriangles(),TMorphmeshPictures()
{
	// Create default animation
	POINT p = {320,200};
	SetAnimation(MorphResult,new Animation(p,GetFrames(MorphResult)));
};

TMorphmesh_Data::~TMorphmesh_Data()
{
	TMorphmesh_Data::Reset();
};

void TMorphmesh_Data::Init(void)
{
    TMorphmeshPoints::Init();
    TMorphmeshPictures::Init();
	TMorphmeshTriangles::Init();
};

// Should reset the morphblock to first state
void TMorphmesh_Data::Reset()
{
	TMorphmeshTriangles::Reset();
	TMorphmeshPoints::Reset();
	TMorphmeshPictures::Reset();

	// Mark morphmesh as changed
	SetChanged(TRUE);
};

BOOL  TMorphmesh_Data::CanClose(void)
{
	BOOL fCanClose = TMorphmeshPoints::CanClose();

    if(fCanClose)
		if(GetAnimation(MorphResult))
			fCanClose = GetAnimation(MorphResult)->CanClose();

	return fCanClose;
};

LPSTR TMorphmesh_Data::GetName(void)	{ return TMorphmeshPoints::GetName(); };
void  TMorphmesh_Data::Save(void)		{ TMorphmeshPoints::Save(); };
void  TMorphmesh_Data::SaveAs(void)		{ TMorphmeshPoints::SaveAs(); };
void TMorphmesh_Data::SetChanged(BOOL bNewValue){	TMorphmeshPoints::SetChanged(bNewValue); };
BOOL TMorphmesh_Data::HasChanged(void)	{ return TMorphmeshPoints::HasChanged(); };

BOOL TMorphmesh_Data::IsOK(void)
{
	return (mBlock[MorphResult]!=NULL);
};

void TMorphmesh_Data::PointDelete(int nIndex)
{
    TMorphmeshPoints::PointDelete(nIndex);

    TMorphmeshTriangles::SetVertex(nPoints,nIndex);
};

void TMorphmesh_Data::PointFetch(int nPoint,TMorphmeshPoints *pDestMesh)
{
	TMorphmeshPoints::PointFetch(nPoint,pDestMesh);

    TMorphmeshTriangles::SetVertex(nPoints,nPoint);
};

// Do Delaunay- triangulation
void TMorphmesh_Data::Triangulate(int bBlock,int nFrame)
{
	Delaunay *pDelaunay = NULL;

	if(nPoints>2){
		// Create triangulator
		pDelaunay = new Delaunay();

		// Feed points to Delaunay triangulation algorithm
		pDelaunay->StartAddingPoints(nPoints);

		for(int nPoint = 0 ; nPoint < nPoints ; nPoint++ ){
			Point2d pPoint = Point(bBlock,nPoint)->GetPoint(nFrame);
			pDelaunay->AddPoint(pPoint.x,pPoint.y);
			};

		// Triangulate
		pDelaunay->Triangulate(TRUE);
	};

	// Clear the old triangles
	TMorphmeshTriangles::Reset();

	if(nPoints>2){
		// Get triangles from triangulator and add them to morphmesh
		int *nTri = NULL;
		TTriangle tNewTriangle;
		pDelaunay->SetToFirstTriangle();

		while((nTri = pDelaunay->GetNextTriangle())!=NULL){
			tNewTriangle.SetVertex(0,nTri[0]);
			tNewTriangle.SetVertex(1,nTri[1]);
			tNewTriangle.SetVertex(2,nTri[2]);
			TriangleAdd(tNewTriangle);
			NormalizeTriangle(nTriangles-1, bBlock, nFrame);
		};

		 // Delete triangulator
		delete pDelaunay;
    };
};

void TMorphmesh_Data::NormalizeTriangle(int nTri, int nBlock, int nFrame)
{
	Point2d *fp = Triangles[nTri].GetPoints(mBlock[nBlock],nFrame);
	Point2d A,B;

	A.x = fp[1].x - fp[0].x;
	A.y = fp[1].y - fp[0].y;

	B.x = fp[2].x - fp[0].x;
	B.y = fp[2].y - fp[0].y;

	float area = (A.x * B.y - A.y * B.x);

    /* */
	if(area < 0 ){
	   /* flip two vertices */
		Triangles[nTri].FlipVertices();
    }
};


////////////////////////////
// File functions

BOOL TMorphmesh_Data::LoadDocument(wmfstream& file)
{
    BOOL fSuccess = FALSE;
	long lFilePos = file.tellg();

	switch(nCheckFileVersion(file)){
		case 1:{ // Load version 1.0 MorphMesh
			file.seekg(0L);
//			file.flags(ios::binary|ios::in);
			fSuccess = LoadVersion1(file);
			break;
			};
		case 2:{ // Load version 2.0 MorphMesh
			file.seekg(lFilePos);
//			file.flags(ios::in);
			LoadMesh(file);
            fSuccess = TRUE;
			break;
			};
		};

    if(fSuccess)
		// If no result animation is loaded,
		if(GetAnimation(MorphResult)==NULL){
			// Create default animation
			POINT p = {320,200};
			SetAnimation(MorphResult,
						new Animation(p,GetFrames(MorphResult)));
			};

    return fSuccess;
};

void TMorphmesh_Data::SaveDocument(wmfstream& file)
{
	// Save header
	file << "WinMorph\n";
	file << "Morphmesh\n";

	TMorphmeshPictures::SaveDocument(file);
	TMorphmeshPoints::SaveDocument(file);
	TMorphmeshTriangles::SaveDocument(file);
};

int TMorphmesh_Data::nCheckFileVersion(wmfstream& file)
{
	int nVersionNumber = 0;

	file.seekg(0L);
//	file.flags(ios::binary|ios::in);

	 /* Check if file is version 1.00 */
	do {
		  // Version 1.0 identification
		char ch;
		file >> ch;    if(ch!='M')    break;
		file >> ch;    if(ch!='R')    break;
		file >> ch;    if(ch!='F')    break;

		  /* File is version 1.00 */
		nVersionNumber = 1;
		}
	while(FALSE);

	 // Check if file is version 2.00
	if(nVersionNumber==0){
		  char szId[10];
		  /* Rewind to files start */
		file.seekg(0L);

		  // Read first 8 characters
		file.read(szId,8);

		szId[8]='\0';

		  // Version 2.0 identification
		if(_fstricmp(szId,"WinMorph")==0)
				// File is version 2.0
			nVersionNumber = 2;
		};

	// Return the version number
	return (nVersionNumber);
};

// Load version 1.0 morphmesh file
BOOL TMorphmesh_Data::LoadVersion1(wmfstream& file)
{
	char szId[3];

	wmfstream fNewFile(file.szFilename,ios::binary|ios::in);

	/* Read identification mark MRF*/
	fNewFile.read(szId,3);
	assert((szId[0]=='M')&&(szId[1]=='R')&&(szId[2]=='F'));

	 // Read Number of frames for Result MorphBlock
	int nFrames;
	fNewFile.read((BYTE*)&nFrames,sizeof(int));

	// Create MorphBlocks
	assert(mBlock[MorphStart]==NULL);
	mBlock[ MorphStart] = new MorphBlock(1);
	mBlock[MorphResult] = new MorphBlock(nFrames);
	mBlock[   MorphEnd] = new MorphBlock(1);

	 // Read Number of points
	int nNewpoints;
	fNewFile.read((BYTE*)&nNewpoints,sizeof(int));
	if(!fNewFile.good())
		return FALSE;

	// Read Morphpoints
	Point2d pNewPoint(0.0,0.0);
	for(int nPoint = 0 ; nPoint < nNewpoints ; nPoint++ ){   
		// Make new point
		PointNew(pNewPoint);

		//For all frames
		POINT pPoint;
		for(int nFrame = 0 ; nFrame < nFrames ; nFrame++ ){
			// Read one frame for the point
			fNewFile.read((BYTE*)&pPoint,sizeof(POINT));
			if(!fNewFile.good())
    			return FALSE;

			// Transfer to new coordinates
			pNewPoint.x = ((double)pPoint.x) / 16384;
			pNewPoint.y = ((double)pPoint.y) / 16384;

				// Set value for point
			Point(MorphResult,nPoint)->SetPoint(nFrame,pNewPoint);
				// Set all points as keypoints
			Point(MorphResult,nPoint)[nFrame].SetKeyPoint(TRUE);
			};
		};

	// Copy first frame from Resultblock to Startblock
	mBlock[MorphStart]->CopyFrame(0,mBlock[MorphResult],0);

	// Copy last frame from Resultblock to Endblock
	mBlock[MorphEnd]->CopyFrame(0,mBlock[MorphResult],GetFrames(MorphResult)-1);

	// Read Number of triangles
	int nNewtriangles;
	fNewFile.read((BYTE*)&nNewtriangles,sizeof(int));
	if(!fNewFile.good())
    	return FALSE;

	// Read Triangles
	int nPointIndex;                          
	TTriangle tTriangle;
	for(int nTriangle = 0 ; nTriangle < nNewtriangles ; nTriangle++ ){

		  // Read first vertex
		fNewFile.read((BYTE*)&nPointIndex,sizeof(int));
		tTriangle.SetVertex(0,nPointIndex);

		  // Read second vertex
		fNewFile.read((BYTE*)&nPointIndex,sizeof(int));
		tTriangle.SetVertex(1,nPointIndex);

		  // Read third vertex
		fNewFile.read((BYTE*)&nPointIndex,sizeof(int));
		tTriangle.SetVertex(2,nPointIndex);

		if(!fNewFile.good())
			return FALSE;

		// Add triangle to morphmesh
		TriangleAdd(tTriangle);
		};
	if(!fNewFile.good())
		return FALSE;

	return TRUE;
};

/* Saved for future needs , the old save function
void save_old(wmfstream& file)
{
	int File = _lcreat(Name, 0);

	_lwrite(File,"MRF",3);     // Write header

	_lwrite(File,&(Frames[MorphResult]),sizeof(int));  // Write Frames

	 ////////////////////
	 // Write points
	_lwrite(File,&nPoints,sizeof(int));

	for(int i=0;i<nPoints;i++)          // write points
		Point(MorphResult,i)->Save(File,Frames[MorphResult]);

	 ///////////////////7
	// Write nTriangles
	_lwrite(File,&nTriangles,sizeof(int));

	for(int j=0;j<nTriangles;j++){         // write triangles
		int index=Triangles[j].GetVertex(0);
		_lwrite(File,(void _huge*)&index,sizeof(int));
		index=Triangles[j].GetVertex(1);
		_lwrite(File,(void _huge*)&index,sizeof(int));
		index=Triangles[j].GetVertex(2);
		_lwrite(File,(void _huge*)&index,sizeof(int));
		};

	_lclose(File);
	 
};*/

void TMorphmesh_Data::LoadMesh(wmfstream& file)
{
	BOOL bErrors = TRUE;

	do{
		 // Check we are reading a right file type
		if(!GetKeyword(file,"WinMorph"))	break;
		if(!GetKeyword(file,"Morphmesh"))	break;

		if(!TMorphmeshPictures::LoadDocument(file))	break;
		if(!TMorphmeshPoints::LoadDocument(file))  	break;
		if(!TMorphmeshTriangles::LoadDocument(file))break;

		// Morphmesh was previously marked as changed, so we have to unmark it
		SetChanged(FALSE);

		bErrors = FALSE;
		}
	while(FALSE);

	 // If there were errors
	if(bErrors)
		  // Reset the morphmesh
		Reset();
};

/* End of file MORFDATA.CPP */
 

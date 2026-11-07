/**********************************************************************
 * 
 *  NAME:         MORFBLCK.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  20.11.95  MHM     created
 * 
 *********************************************************************/ 
 
#include <string.h>
#include "morfblck.h"
#include "filefunc.h"
#include "itemlist.h"

MorphBlock::MorphBlock(int nNewFrames = 0)
{
    nPoints		= 0;
	Points		= NULL;
	PointsAllocated = 0;

	nFrames		= nNewFrames;
};

MorphBlock::MorphBlock(MorphBlock *pSrcBlock)
{
	nPoints		= 0;
	Points		= NULL;
	PointsAllocated = 0;

	nFrames		= pSrcBlock->GetFrames();
	ResizePointArray(pSrcBlock->PointsAllocated);

	for(int nPoint = 0 ; nPoint < pSrcBlock->GetPoints() ; nPoint ++){
		MorphPoint *pDestPoint;
		MorphPoint *pSrcPoint = pSrcBlock->Points[nPoint];

		NewPoint();
		pDestPoint = Points[nPoint];
		for(int nFrame = 0 ; nFrame < nFrames ; nFrame ++ )
        	pDestPoint[nFrame] = pSrcPoint[nFrame];
    };
};

MorphBlock::~MorphBlock()
{
    Reset();
};

void MorphBlock::Reset(void)
{
	DeletePointArray(Points,PointsAllocated);
	Points = NULL;

    PointsAllocated = 0;
	nFrames = 0;
	nPoints = 0;
};

void MorphBlock::SwapPoints(int nFirst,int nSecond)
{
	SWAP(MorphPoint*,Points[nFirst],Points[nSecond]);
};

void MorphBlock::NewPoint(MorphPoint *pNewPoint,int nNewFrames)
{
	// Allocate more memory for MorphPoints if needed
	if(nPoints==PointsAllocated)
		ResizePointArray(PointsAllocated+25);

	// Check that there's enough MorphPoints
	if(nPoints==PointsAllocated)
    	return;

	// Assign new point
	nPoints++;

	assert(Points[nPoints-1]==NULL);

    // If new point has different number of frames
	if(nNewFrames!=nFrames){
		MorphPoint *pStretchPoint = MorphPoint::Create(nFrames);

		//Interpolate from pNewPoint
		MorphPoint::InterpolateKey
			(pNewPoint,nNewFrames,pStretchPoint,nFrames);

		delete pNewPoint;
        pNewPoint = pStretchPoint;
    };

	// Assign new point
	Points[nPoints-1] = pNewPoint;
};

void MorphBlock::NewPoint(void)
{
	// Add point
	NewPoint(MorphPoint::Create(nFrames),nFrames);
};

void MorphBlock::NewPoint(Point2d pPoint)
{
	MorphPoint *pNewPoint = MorphPoint::Create(nFrames);

	pNewPoint->InitPoint(nFrames,pPoint);

    // Add point
	NewPoint(pNewPoint,nFrames);
};

MorphPoint *MorphBlock::FetchPoint(int nIndex)
{
	assert(nIndex != NO_POINTS);
	assert(nIndex < nPoints);

    /* Swap point with last point */
	MorphPoint *pPoint = Points[nIndex];
    Points[nIndex] = Points[nPoints-1];
    Points[nPoints-1] = NULL;

	/* Delete last point */
	nPoints--;

	if((PointsAllocated-25)>nPoints)
		ResizePointArray(PointsAllocated-25);

    return pPoint; 
};

void MorphBlock::DeletePoint(int nIndex)
{
    delete FetchPoint(nIndex);
};

void MorphBlock::SetPoint(int nIndex,Point2d pPoint)
{
	Points[nIndex]->InitPoint(nFrames,pPoint);
};

// Interpolate all points
void MorphBlock::Interpolate(void)
{
	for(int nPoint = 0 ; nPoint < nPoints ; nPoint++)
		Points[nPoint]->Interpolate(nFrames);
};

void MorphBlock::Interpolate(int nPoint)
{
	Points[nPoint]->Interpolate(nFrames);
};

void MorphBlock::Compute(
				TFunction2d *pFunction,
				MorphBlock *pSrc1,MorphBlock *pSrc2)
{
	for(int nPoint = 0 ; nPoint < nPoints ; nPoint++)
		MorphPoint::Compute(
                pFunction,
				Points[nPoint],GetFrames(),
				pSrc1->Point(nPoint),pSrc1->GetFrames(),
				pSrc2->Point(nPoint),pSrc2->GetFrames());
};

void MorphBlock::StretchFrames(int nNewFrames)
{
	if((Points==NULL)||				// No points yet, so why worry
		(nFrames == nNewFrames)){	// Frame number doesn't change 
		nFrames = nNewFrames;
		return;
		};

	// Create new points
	MorphPoint **NewPoints = CreatePointArray(PointsAllocated);

    // Check for errors
	if(NewPoints==NULL){
		assert(FALSE);
		return;
		};

	int nOldFrames = nFrames;

    // Interpolate new points
	for(int nPoint = 0 ; nPoint < nPoints ; nPoint++ ){
        assert(NewPoints[nPoint]==NULL);
		NewPoints[nPoint] = MorphPoint::Create(nNewFrames);

		MorphPoint::InterpolateKey
			(Points[nPoint],nOldFrames,NewPoints[nPoint],nNewFrames);
		};

	// Set new points
	SWAP(MorphPoint**,Points,NewPoints);
    SWAP(int,nFrames,nNewFrames);

    // Delete old points
    DeletePointArray(NewPoints,PointsAllocated);
};

void MorphBlock::CopyFrame(int nDestFrame,MorphBlock *pSourceBlock,int nSourceFrame)
{                                                  
	assert(nDestFrame   < nFrames);
    assert(nSourceFrame < pSourceBlock->nFrames );
	for(int nPoint = 0 ; nPoint < nPoints ; nPoint++ ){
		Points[nPoint][nDestFrame] = pSourceBlock->Points[nPoint][nSourceFrame];
		};
};

MorphBlock *MorphBlock::CreateCopy(ItemList *pPointList,int nStartFrame,int nEndFrame)
{
	// Create block for point origins
	MorphBlock *pCopyBlock = new MorphBlock(nEndFrame - nStartFrame);

	PointCopy(pPointList,pCopyBlock,nStartFrame);

	return pCopyBlock;
}; 

MorphPoint *MorphBlock::PointCopy(int nPoint)
{
	return Points[nPoint]->Copy(GetFrames());
};

void MorphBlock::PointCopy(ItemList *pPointList,MorphBlock *pCopyBlock,int nStartFrame)
{
	int nSelPoint = 0;

	// Copy selected points to origin block
	for(int nPoint = 0; nPoint < GetPoints(); nPoint ++){
		if((pPointList==NULL)||(pPointList->GetFlag(nPoint,SL_SELECTED))){
            // Create new point
			pCopyBlock->NewPoint();

            // Copy frames for one point 
			for(int nFrame = 0 ; nFrame < pCopyBlock->GetFrames() ; nFrame ++ ){
				pCopyBlock->Points[nSelPoint][nFrame] = Points[nPoint][nFrame + nStartFrame];
				};

            nSelPoint ++;
            };
		};
};

MorphPoint **MorphBlock::CreatePointArray(int nPoints)
{
	MorphPoint **pNewPoints = NULL;

	// Create new MorphBlock
	if(nPoints>0)
		pNewPoints = new MorphPoint *[nPoints];

    // Init MorphBlock
	for(int nPoint = 0 ; nPoint < nPoints ; nPoint++ )
		pNewPoints[nPoint] = NULL;

	return pNewPoints;
};

void MorphBlock::DeletePointArray(MorphPoint **PointArray,int nPoints)
{
	if(PointArray){
        // Delete points
		for(int nPoint = 0 ; nPoint < nPoints ; nPoint++ )
			if(PointArray[nPoint])
				delete PointArray[nPoint];

		// Delete array
		delete PointArray;
		};
};

void MorphBlock::ResizePointArray(int nNewSize)
{
	assert(nNewSize>=nPoints);

    // Initialize new Pointblocks
	MorphPoint **NewPoints = NULL;

    /* Check if you have to copy old points */
	if(nNewSize>0){
		/* Create new pointblock and copy the points */

		// Create blocks
		NewPoints = new MorphPoint *[nNewSize];

		if(NewPoints==NULL){
            assert(FALSE);
			return;
			};

		// Init new points
		for(int nPoint = PointsAllocated ; nPoint < nNewSize ; nPoint++ )
			NewPoints[nPoint]  = NULL;

		// Move old MorphPoint points to new array
		for( nPoint = 0 ; nPoint < min(nNewSize,PointsAllocated) ; nPoint++ ){
			NewPoints[nPoint] = Points[nPoint];
            Points[nPoint] = NULL;
            };
		};

	// If succeeded, swap morphblocks
	SWAP(MorphPoint**,Points,NewPoints);
	SWAP(int,PointsAllocated,nNewSize);

    // Delete points not needed
    DeletePointArray(NewPoints,nNewSize);
};

BOOL MorphBlock::Load(wmfstream& file)
{
	BOOL bErrors = TRUE;

    do {
        // Get start of block
		if(GetStartOfBlock(file)==FALSE)	break;

		// Define keywords
		LPSTR szKeyWords[]={"Frames",NULL};
		#define FRAMES 0

		// Read Frames- keyword, other keywords are not allowed
		int nKeyword = IdentifyLabel(file,szKeyWords);
		if(nKeyword!=FRAMES)	break;

		// Read number of frames
		int nNewFrames = 0;
		file >> nNewFrames;

        // If number of frames is too small, stop loading and return FALSE
        if(nNewFrames<=0)	break; 

		// Create new morphblock
		MorphPoint **pNewPoints = CreatePointArray(PointsAllocated+20);
		if(pNewPoints==NULL){
			assert(FALSE);
			break;
            };

		// Create points
		for(int nPoint = 0 ; nPoint < nPoints ; nPoint ++  )
        	pNewPoints[nPoint] = MorphPoint::Create(nNewFrames); 	

		// Assign new morphblock
		SWAP(MorphPoint**,Points,pNewPoints);
	    SWAP(int,nFrames,nNewFrames);

	    // Delete old PointBlock
		DeletePointArray(pNewPoints,PointsAllocated);
		pNewPoints = NULL;

	    // Read frames
		while(!file.eof()){
			//Define keywords
			LPSTR szKeyWords[]={"Frame",NULL};
			#define FRAME 0
    	
			//Read keyword
			int nKeyword = IdentifyKeyword(file,szKeyWords);
        	
            // If end of block, stop reading the frames
			if(nKeyword == ENDOFBLOCK)		break;

			switch(nKeyword){
				case FRAME:
					// Load one frame
					LoadFrame(file);
					break;
				};
			};		
        bErrors = FALSE;
		}
	while(FALSE);

	return !bErrors;
};

BOOL MorphBlock::LoadFrame(wmfstream& file)
{
	// Read Frame number
	int nFrame;
	file >> nFrame;
    if(nFrame>=GetFrames()) return FALSE;

	if(GetStartOfBlock(file)==FALSE)	return FALSE;

    // Define keywords
	LPSTR szKeyWords[]={"Point",NULL};
	#define POINT 0

	while(!file.eof()){
        // Read keyword
		int nKeyword = IdentifyKeyword(file,szKeyWords);

        if(nKeyword==ENDOFBLOCK)		break;

		switch(nKeyword){
			case POINT:
				int nPoint;

                // Read morphpoints number
				file >> nPoint;
                // Check that morphpoint exists
				if(nPoint < 0)			{assert(FALSE);return FALSE;};
				if(nPoint>=GetPoints())		{assert(FALSE);return FALSE;};
                	
                // Read morphpoint
				file >> Points[nPoint][nFrame];
				break;
			};
		};
	return TRUE;
};

void MorphBlock::Save(wmfstream& file)
{
    // Save morphblock header
	file << "\t\t(\n";

    // Save Number of frames
	file << "\t\tFrames = " << GetFrames() << "\n";

    // Save frames
	for(int nFrame = 0 ; nFrame < GetFrames() ; nFrame++ )
		SaveFrame(file,nFrame);

	file << "\t\t)\n";
};

void MorphBlock::SaveFrame(wmfstream& file,int nFrame)
{
    BOOL FrameSaved = FALSE;

	for(int nPoint = 0 ; nPoint < GetPoints() ; nPoint++ ){
		MorphPoint& pPoint = Points[nPoint][nFrame];

		// Save only keypoints
		if(pPoint.GetKeyPoint()){
			if(FrameSaved==FALSE){
				FrameSaved = TRUE;
                // Save frame header
				file << "\t\tFrame " << nFrame << " (\n";
               	};
            // Save point
			file << "\t\t\tPoint " << nPoint << " ";
			file << pPoint << "\n";
            };
		};

    // Save frame footer
	if(FrameSaved)
		file << "\t\t\t)\n";
};

/* End of file MORFBLCK.CPP */

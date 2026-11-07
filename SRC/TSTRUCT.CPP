/**********************************************************************
 * 
 *  NAME:         TSTRUCT.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  -------------------------------------------------------------------
 *  11.8.95  MHM     added headers 
 *  20.9.95  MHM	  removed min(),max() and inrange() to common.h
 *********************************************************************/ 
 
#include "common.h"
#include "tstruct.h"
#include "filefunc.h"
#include "interpol.h"

/////////////////////////////////////////////7
// Point functions
////////////////////////////////////////

MorphPoint::MorphPoint(void)
{
	pPoint = Point2d(0.0,0.0);
    fKeyPoint = FALSE;
	fRatio = NO_RATIO;
    fKeyRatio = FALSE;
};

MorphPoint::MorphPoint(MorphPoint *mpPoint)
{
    *this = *mpPoint; 
};

wmfstream& operator>>(wmfstream& file,MorphPoint& mpPoint)
{
	do {
		float fValuesRead[3];
		// Read start of block
    	if(GetStartOfBlock(file)==FALSE) break;

        // Read values
		for(int nValuesRead = 0 ; nValuesRead < 3 ; nValuesRead ++){
			// Read first value
			file >> fValuesRead[nValuesRead];
			// Read comma, if not found start searching end of block
			if(!GetCharacterIf(file,',')) break;
            };

		// Read end of block
		if(GetEndOfBlock(file)==FALSE) break;

		// Set values to Morphpoint
		switch(nValuesRead){
            case 0:{ /* One frame ratio read */
				mpPoint.SetRatio(fValuesRead[0]);
                mpPoint.SetKeyRatio(TRUE);
				break;
                };
			case 1:{ /* point value read */
				mpPoint.pPoint.x = fValuesRead[0];
				mpPoint.pPoint.y = fValuesRead[1];
				mpPoint.SetKeyPoint(TRUE);
				break;
                };
			case 2:{ /* point values and frame ratio read */
				mpPoint.pPoint.x = fValuesRead[0];
				mpPoint.pPoint.y = fValuesRead[1];
				mpPoint.SetKeyPoint(TRUE);
				mpPoint.SetRatio(fValuesRead[2]);
				mpPoint.SetKeyRatio(TRUE);
				};
        	};

		}
	while(FALSE);

    return file;
};

wmfstream& operator<<(wmfstream& file,MorphPoint& mpPoint)
{
    // Save start of block
	file << " ( ";

	if(mpPoint.GetKeyPoint())
		// Save point value
		file << mpPoint.pPoint.x << " , " << mpPoint.pPoint.y;

	if(mpPoint.GetKeyRatio()&&mpPoint.GetKeyPoint())
		file << " , ";

	if(mpPoint.GetKeyRatio())
		// Save ratio
		file << mpPoint.fRatio;

	// Save end of block
	file << " ) ";
	return file;	
};

// Functions for array of MorphPoints

MorphPoint *MorphPoint::Copy(int nFrames)
{
	MorphPoint *mpPoint = MorphPoint::Create(nFrames);

	for(int nFrame = 0 ; nFrame < nFrames ; nFrame ++ )
		mpPoint[nFrame] = GetMorphPoint(nFrame); 

	return mpPoint;
};

void MorphPoint::InitPoint(int nFrames,Point2d pNewPoint)
{
	MorphPoint *mpPoint = NULL;
	// Init points
	for(int nFrame = 0 ; nFrame < nFrames ; nFrame++ ){
		mpPoint = &GetMorphPoint(nFrame);
		mpPoint->SetPoint(pNewPoint);
		mpPoint->SetKeyPoint(FALSE);
		mpPoint->SetRatio(NO_RATIO);
        };

	// Init first frame
	mpPoint = &GetMorphPoint(0);
	mpPoint->SetKeyPoint(TRUE);
    mpPoint->SetRatio(0.0);

	// Init last frame
	mpPoint = &GetMorphPoint(nFrames-1);
	mpPoint->SetKeyPoint(TRUE);
    mpPoint->SetRatio(1.0);
};

void MorphPoint::Interpolate(MorphPoint* mpSrc,int nSrcFrames,MorphPoint* mpDest,int nDestFrames)
{
	if(nSrcFrames==1){
        /* If there's only one source frame there's no need for interpolation */
		for(int nDestFrame = 0 ; nDestFrame < nDestFrames ; nDestFrame ++){
            // Set point
			mpDest->SetPoint(nDestFrame,mpSrc->GetPoint(0));

            // Set ratio
			float fFrameRatio;
			if((nDestFrames-1)==0)
				fFrameRatio = 0.0;
			   else
               	fFrameRatio = (float)nDestFrame / (float)(nDestFrames-1);

            mpDest->GetMorphPoint(nDestFrame).SetRatio(fFrameRatio);
			}
		return;
		}

    // Calculate how many keyframes there are
	int nKeyFrames = 0;
	for(int nFrame = 0 ; nFrame < nSrcFrames ; nFrame ++)
		if(mpSrc[nFrame].GetKeyPoint())
			nKeyFrames++ ;

    float* fFrame = new float[nKeyFrames+1];
	float* fX = new float[nKeyFrames+1];
	float* fY = new float[nKeyFrames+1];

	/* Copy MorphPoints for spline function */
    int nKeyFrame = 0;
	for( nFrame = 0 ; nFrame < nSrcFrames ; nFrame ++)
		if(mpSrc[nFrame].GetKeyPoint()){
			nKeyFrame ++;

			MorphPoint& mpPoint = mpSrc->GetMorphPoint(nFrame);
			fX[nKeyFrame] = mpPoint.pPoint.x;
			fY[nKeyFrame] = mpPoint.pPoint.y;

			fFrame[nKeyFrame] = nFrame;
			}

    /* Spline structure */
	Interpolation* iX = new Interpolation(fFrame,fX,nKeyFrames);
	Interpolation* iY = new Interpolation(fFrame,fY,nKeyFrames);

    // Interpolate with splines
	for(int nDestFrame = 0 ; nDestFrame < nDestFrames ; nDestFrame++ ){
		float nSrcFrame;

		if((nDestFrames-1)==0)
			nSrcFrame = 0.0;
		   else
			nSrcFrame = ( nSrcFrames - 1 ) * (float)nDestFrame /
										( nDestFrames -1);

		MorphPoint& mpPoint = mpDest->GetMorphPoint(nDestFrame);

		mpPoint.pPoint.x  = inrange(iX->GetPoint(nSrcFrame) , 0.0 , 1.0);
		mpPoint.pPoint.y  = inrange(iY->GetPoint(nSrcFrame) , 0.0 , 1.0);
		};

    // Delete splines
	delete iX;
    delete iY;

    // Delete temporary structures
	delete fX;
    delete fY;
	delete fFrame;
};

void MorphPoint::InterpolateKey(
			MorphPoint* mpSrc,int nSrcFrames,
			MorphPoint* mpDest,int nDestFrames)
{
	MorphPoint::Interpolate
		(mpSrc,nSrcFrames,mpDest,nDestFrames);

    // Clear old keypoints
	MorphPoint::SetKeyPoints(mpDest,nDestFrames,FALSE);

	// Set keypoints
	MorphPoint::MergeKeyPoints(mpSrc,nSrcFrames,mpDest,nDestFrames);
};

void MorphPoint::Compute(
            TFunction2d *pFunction,
			MorphPoint* mpDest,int nDestFrames,
			MorphPoint* mpSrc1,int nSrc1Frames,
			MorphPoint* mpSrc2,int nSrc2Frames )
{
	// Clear old keypoints
	MorphPoint::SetKeyPoints(mpDest,nDestFrames,TRUE);
	// Set keypoints 
//	MorphPoint::MergeKeyPoints(mpSrc1,nSrc1Frames,mpDest,nDestFrames);
//	MorphPoint::MergeKeyPoints(mpSrc2,nSrc2Frames,mpDest,nDestFrames);

    // Get interpolation from src1-->src2
	for(int nDestFrame = 0 ; nDestFrame < nDestFrames ; nDestFrame++ )
		if(mpDest[nDestFrame].GetKeyPoint()==TRUE){
			int nSrc1Frame = (int)(((long)nDestFrame * (long)(nSrc1Frames - 1))/(long)(nDestFrames-1));
			int nSrc2Frame = (int)(((long)nDestFrame * (long)(nSrc2Frames - 1))/(long)(nDestFrames-1));
			double fRatio = (double)nDestFrame / ((double)nDestFrames - 1.0 );

			pFunction->Compute(
				mpDest->GetMorphPoint(nDestFrame).pPoint,
				mpSrc1->GetMorphPoint(nSrc1Frame).pPoint,
				mpSrc2->GetMorphPoint(nSrc2Frame).pPoint,
				fRatio);
		};

	// and interpolate
//	MorphPoint::Interpolate
//		(mpDest,nDestFrames,mpDest,nDestFrames);
};

void MorphPoint::SetKeyPoints(
			MorphPoint* mpDest,int nDestFrames,
			BOOL fState)
{
	// First point is always keypoint
	mpDest[0].SetKeyPoint(TRUE);

    // Clear other keypoints
	for(int nDestFrame = 1 ; nDestFrame < nDestFrames - 1 ; nDestFrame++ )
		mpDest[nDestFrame].SetKeyPoint(fState);

    // Last point is always keypoint
	mpDest[nDestFrames - 1].SetKeyPoint(TRUE);
};

void MorphPoint::MergeKeyPoints(
			MorphPoint* mpSrc,int nSrcFrames,
			MorphPoint* mpDest,int nDestFrames)
{
	// Set keypoints
	for(int nSrcFrame = 1 ; nSrcFrame < nSrcFrames - 1 ; nSrcFrame++ ){
		int nDestFrame = (int)(((long)nSrcFrame * (long)(nDestFrames - 1))/(long)(nSrcFrames-1));
		if(mpSrc[nSrcFrame].GetKeyPoint()==TRUE)
			mpDest[nDestFrame].SetKeyPoint(TRUE);
		};
};

void MorphPoint::Interpolate(int nFrames)
{
	Interpolate(this,nFrames,this,nFrames);
};

//////////////////////////////////////////////////
//////////////////////////////////////////////////
POINT MorphPoint::Scale(int nFrame,POINT pRes)
{
	Point2d& fp = GetThePoint(nFrame);

	POINT pResultPoint;
	pResultPoint.x = (int)(fp.x * pRes.x);
	pResultPoint.y = (int)(fp.y * pRes.y);

	return pResultPoint;
};

///////////////////////////////////////////
// TTriangle
///////////////////////////////////////////

#include "morfblck.h"

Point2d *TTriangle::GetPoints(MorphBlock *mbBlock,int nFrame)
{
	static Point2d pPoint[3];

	pPoint[0] = mbBlock->Point(GetVertex(0))->GetPoint(nFrame);
	pPoint[1] = mbBlock->Point(GetVertex(1))->GetPoint(nFrame);
	pPoint[2] = mbBlock->Point(GetVertex(2))->GetPoint(nFrame);

	return pPoint;
};


int TTriangle::FindVertex(int nPoint)
{
	if(GetVertex(0)==nPoint) return 0;
	if(GetVertex(1)==nPoint) return 1;
	if(GetVertex(2)==nPoint) return 2;
    return -1;// Not Found
};

BOOL TTriangle::HasVertex(int nPoint)
{
	return (FindVertex(nPoint)!=-1);
};

BOOL TTriangle::IsEqual(TTriangle &Tri)
{
	return ( HasVertex(Tri.GetVertex(0))&&
			 HasVertex(Tri.GetVertex(1))&&
             HasVertex(Tri.GetVertex(2)));
};

void TTriangle::FlipVertices(void)
{
	SWAP(int,vertex[0],vertex[1]);
    SWAP(int,line[1],line[2]);
};

wmfstream& operator<<(wmfstream& file,TTriangle& tTri)
{
    // Save header
    file << " ( ";

    // Save vertexes
	file << tTri.GetVertex(0) << " , ";
	file << tTri.GetVertex(1) << " , ";
	file << tTri.GetVertex(2);

    // Save footer
	file << " ) ";

    return file;
};

wmfstream& operator>>(wmfstream& file,TTriangle& tTri)
{
	int nVertex[3];

	do {

    // Read vertexes
	if(!GetStartOfBlock(file)) break;
	file >> nVertex[0];
    if(!GetComma(file)) break;
	file >> nVertex[1];
    if(!GetComma(file)) break;
	file >> nVertex[2];
    if(!GetEndOfBlock(file)) break;

    // Move vertexes to triangle
	tTri.SetVertex(0,nVertex[0]);
	tTri.SetVertex(1,nVertex[1]);
	tTri.SetVertex(2,nVertex[2]);
		}
	while(FALSE);

    return file;
};

// These functions are to speed drawing of the triangles
// If two triangles share a line, the line should be drawn only once

void TTriangle::Set_Lines_Unique(void)
{
	line[0] = TRUE;
	line[1] = TRUE;
	line[2] = TRUE;
};

// When we are adding a new triangle we must remove all duplicate
// lines from this triangle
void TTriangle::Remove_Duplicate_Lines(TTriangle& tTri)
{
    // Check all lines
	for(int nVertex = 0 ; nVertex < 3 ; nVertex++ ){
		int nFirstVertex = tTri.FindVertex(GetVertex(nVertex));
		int nSecondVertex = tTri.FindVertex(GetVertex((nVertex+1)%3));

		// If the two triangles have common lines
		if((nFirstVertex!=-1)&&(nSecondVertex!=-1)){

			int nTriLine = nFirstVertex + nSecondVertex;
			if(nTriLine!=2)
				nTriLine = min(nFirstVertex,nSecondVertex);

            // If other triangle line is drawn 
			if(tTri.line[nTriLine])
				// Remove line from this triangles draw list
				line[nVertex] = FALSE;
			};	
		};
};

// When a triangle is deleted this should be called
// If this triangle has lines that are drawn it moves them to
// other triangles
void TTriangle::Add_Duplicate_Lines(TTriangle& tTri)
{
	for(int nVertex = 0 ; nVertex < 3 ; nVertex++ )
		if(line[nVertex]==TRUE){
	        // Checks if the other triangle has this line( nVertex --> nVertex+1)
			// Meaning: If the other triangle has vertexes nVertex and nVertex+1
        	// then they share a common line
			int nVertex1 = tTri.FindVertex(GetVertex(nVertex));
			int nVertex2 = tTri.FindVertex(GetVertex((nVertex+1)%3));

            if((nVertex1!=-1)&&(nVertex2!=-1)){
				//Common line found and this triangle has to draw it
				int nLine = nVertex1 + nVertex2;
				if(nLine!=2)
					nLine = min(nVertex1,nVertex2);

	            assert(tTri.line[nLine]==FALSE);
				// Give the line to Tri
				tTri.line[nLine] = TRUE;
				line[nVertex] = FALSE;
	            };
	};
};
 
void TInterpolate::Compute( Point2d& pDest, Point2d& pSrc1, Point2d& pSrc2, float fRatio)
{
	double fNegRatio = 1.0 - fRatio;

	pDest.x = pSrc1.x * fNegRatio +	pSrc2.x * fRatio;
	pDest.y = pSrc1.y * fNegRatio +	pSrc2.y * fRatio;
};

void TSphere::Compute( Point2d& pDest, Point2d& pSrc1, Point2d& pSrc2, float fRatio)
{
	double fX = ( sin(2 * M_PI * (pSrc1.x - 0.5 + fRatio ))/2.0 + 0.5 );

	pDest.x = (sin( M_PI * pSrc1.y  ) * (fX - 0.5) ) + 0.5;
	pDest.y = sin( M_PI * (pSrc1.y - 0.5 ))/2.0 + 0.5;
};

void TEarth::Compute( Point2d& pDest, Point2d& pSrc1, Point2d& pSrc2, float fRatio)
{
	double fX = ( sin(2 * M_PI * (pSrc1.x - 0.5 + fRatio ))/2.0 + 0.5 );

	pDest.x = (sin( M_PI * pSrc1.y  ) * (fX - 0.5) ) + 0.5;
	pDest.y = sin( M_PI * (pSrc1.y - 0.5 ))/2.0 + 0.5;
};

void TWhirlpool::Compute( Point2d& pDest, Point2d& pSrc1, Point2d& pSrc2, float fRatio)
{
	double fLength = sqrt( (pSrc1.x - 0.5) * (pSrc1.x - 0.5) + (pSrc1.y - 0.5) * (pSrc1.y - 0.5));

	if(fLength>0){
		double fAngle  = atan2((pSrc1.y - 0.5) / fLength,(pSrc1.x - 0.5) / fLength);

		if(fLength <= 0.5 )
			fAngle +=  (M_PI * fRatio) /fLength ;
		/*else
        	fAngle += -acos(2.0 - 2.0 * fLength) * fRatio;*/ 

		pDest.x = fLength * cos(fAngle) + 0.5;
		pDest.y = fLength * sin(fAngle) + 0.5;
	} else {
		pDest.x = pSrc1.x;
		pDest.y = pSrc1.y;
	}

};

void TYinYang::Compute( Point2d& pDest, Point2d& pSrc1, Point2d& pSrc2, float fRatio)
{
	double fLength = sqrt( (pSrc1.x - 0.5) * (pSrc1.x - 0.5) + (pSrc1.y - 0.5) * (pSrc1.y - 0.5));

	if(fLength>0){
		double fAngle  = atan2((pSrc1.y - 0.5) / fLength,(pSrc1.x - 0.5) / fLength);

		if(fLength <= 0.5 )
			fAngle +=  acos(2.0 * fLength) * fRatio;
		/*else
        	fAngle += -acos(2.0 - 2.0 * fLength) * fRatio;*/ 

		pDest.x = fLength * cos(fAngle) + 0.5;
		pDest.y = fLength * sin(fAngle) + 0.5;
	} else {
		pDest.x = pSrc1.x;
		pDest.y = pSrc1.y;
	}

};

/* End of file TSTRUCT.CPP */
 

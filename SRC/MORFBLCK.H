/**********************************************************************
 * 
 *  NAME:         MORFBLCK.H 
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
 
#ifndef MORFBLCK_H
#define MORFBLCK_H

#include "common.h"
#include "tstruct.h"

#define NO_POINTS -1

class ItemList;

class MorphBlock
{
protected:
	MorphPoint **	Points;
	int				nFrames;
    int				nPoints;
	int				PointsAllocated;

	void ResizePointArray(int newsize);

	MorphPoint **CreatePointArray(int nPoints);
	void 	DeletePointArray(MorphPoint **PointArray,int nPoints);

public:
	MorphBlock			(int nNewFrames);
	MorphBlock			(MorphBlock *pBlock);
	~MorphBlock			();

	MorphPoint *Point(int nPoint){ return Points[nPoint];};
	int GetFrames(void)		{return nFrames;};
    int GetPoints(void)		{return nPoints;};

	void SwapPoints		(int nFirst,int nSecond);

	void NewPoint		(void);
	void NewPoint		(Point2d pPoint);
	void NewPoint		(MorphPoint *pPoint,int nFrames);
	void SetPoint		(int nPoint,Point2d p);
	MorphPoint *FetchPoint(int nIndex);
	void DeletePoint	(int nIndex);
	void Reset			(void);

	MorphPoint *PointCopy	(int nPoint);
	void PointCopy		(ItemList *pPointList,MorphBlock *pCopyBlock,int nStartFrame);

	void CopyFrame		(int nDestFrame,MorphBlock *SourceBlock,int SourceFrame);
	MorphBlock *CreateCopy	(ItemList *pPointList,int nStartFrame,int nEndFrame);

	void StretchFrames	(int nNewFrames);
	void Interpolate	(void);
	void Interpolate	(int nPoint);
	void Compute(		TFunction2d *pFunction,
						MorphBlock *pSrc1,MorphBlock *pSrc2);

	BOOL Load			(wmfstream&);
	void Save			(wmfstream&);

	BOOL LoadFrame		(wmfstream&);
	void SaveFrame		(wmfstream&,int nFrames);
};

#endif
 
/* End of file MORFBLCK.H */ 

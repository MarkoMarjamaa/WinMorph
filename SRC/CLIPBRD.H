/********************************************************************** 
 * 
 *  NAME:         CLIPBRD.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  1.9580  12.09.96 MHM created
 *********************************************************************/ 
 
#ifndef CLIPBRD_H
#define CLIPBRD_H

#include "common.h"
#include "morphmsh.h"

///////////////////////////////////////////////7
// TClipBoard
//////////////////////////////////////////////////


class TClipBoard 
{
protected:
public:
	TClipBoard(){};
	virtual ~TClipBoard(){};
	virtual void Paste(TMorphmesh *pMesh,BOOL fTriangulate,int nSelBlock,int nSelFrame)=0;
	virtual BOOL NeedSelections(void) = 0;
};

// Copies selected points and frames
class TClipBoardFrames : public TClipBoard
{
protected:
public:
	TClipBoardFrames(TClipBoardFrames *pSrcMesh);
	TClipBoardFrames(TMorphmesh *pMesh,ItemList* pPointList,int nBlock,int nStartFrame,int nEndFrame);
	~TClipBoardFrames();

	virtual void Paste(TMorphmesh *pMesh,BOOL fTriangulate,int nSelBlock,int nSelFrame);
	virtual BOOL NeedSelections(void) { return TRUE; };

	int nBlock;
    MorphBlock *pBlock;
};

// Cuts selected points in whole
class TClipBoardPoints : public TClipBoard
{
protected:
public:
	TClipBoardPoints(TClipBoardPoints *pSrcMesh);
	TClipBoardPoints(TMorphmesh *pMesh,ItemList *pPointList,BOOL fTriangulate,int nSelBlock,int nSelFrame);
	~TClipBoardPoints();

	virtual void Paste(TMorphmesh *pMesh,BOOL fTriangulate,int nSelBlock,int nSelFrame);
	virtual BOOL NeedSelections(void) { return FALSE; };

	TMorphmeshPoints *pClipMesh;
};

#endif
 
/* End of file CLIPBRD.H */ 
 

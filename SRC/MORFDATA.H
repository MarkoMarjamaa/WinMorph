/**********************************************************************
 * 
 *  NAME:         MORFDATA.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          20.11.95 MHM created 
 *  1.9470  11.12.95 MHM TMorphmesh_Data(int) -> to (int S,int R,int E)
 *********************************************************************/ 
 
#ifndef MORFDATA_H
#define MORFDATA_H

#include "common.h"
#include "tstruct.h"
#include "morfblck.h"

#define RESOLUTION   16384    // Can't change (hardcoded somewhere else)

/////////////////////////////////////////////////////////////////////
// TMorphmesh_Document
/////////////////////////////////////////////////////////////////////

#include "document.h"

class TMorphmesh_Document : public Document
{
public:
	TMorphmesh_Document();

	LPSTR GetSavePrompt(void);
	LPSTR GetSaveFileName(void);

	static LPSTR GetSaveFilter(void){return "Morphmesh\0*.mrf\0\0";};
	 static LPSTR GetSaveDefExt(void){return "mrf";};

	static LPSTR GetOpenFilter(void){return "Morphmesh\0*.mrf\0\0";};
	static LPSTR GetOpenDefExt(void){return "mrf";};
};

/////////////////////////////////////////////////////////////////
// TMorphmeshPoints
///////////////////////////////////////////////////////////////

#define MorphStart   0
#define MorphResult  1
#define MorphEnd  2
#define MorphBack 3

class ItemList;

class TMorphmeshPoints : public TMorphmesh_Document
{
	friend class TStretchUndo;
protected:

	MorphBlock *mBlock[3];
	int         nPoints;
public:
	TMorphmeshPoints(void);
	TMorphmeshPoints(int nStartFrames,int nResultFrames,int nEndFrames);
	~TMorphmeshPoints();

	static LPSTR MeshNames[4];

	virtual void Init	(void);
	virtual void Reset  (void);

	int     GetFrames			(int nBlock)
										{return mBlock[nBlock]->GetFrames();};
	inline  int	GetPoints  (void)   { return nPoints;};
	inline  MorphBlock *GetMorphBlock(int nBlock)
										{return mBlock[nBlock];};

	virtual void SwapPoints			(int nFirst,int nSecond);

	virtual void PointNew			(Point2d pPoint);
	virtual void PointNew			(MorphPoint *pStart,int nStart,	MorphPoint *pEnd,int nEnd,
									MorphPoint *pResult,int nResult);
	virtual void PointDelete		(int nPoint);
	virtual void PointFetch			(int nPoint,TMorphmeshPoints *pDestMesh);
	virtual void PointsFetch		(ItemList *PointList,TMorphmeshPoints *pDestMesh);
	virtual void PointCopy			(int nPoint,TMorphmeshPoints *pDestMesh);
	virtual void PointsCopy			(ItemList *PointList,TMorphmeshPoints *pDestMesh);
	MorphPoint*  Point				(int nBlock,int nPoint)
									{ return mBlock[nBlock]->Point(nPoint);};
	virtual void PointsRestore		(TMorphmeshPoints *pSrcMesh,ItemList *pPointList);
	virtual void DetachPoints		(ItemList *pPointList,int nBlock,int nStartFrame,int nEndFrame);

	virtual void SwapBlocks			(int nBlock1,int nBlock2);

	virtual void Interpolate        (int nBlock);
	virtual void Interpolate        (int nBlock,int nPoint);
	virtual void Interpolate		(int nBlock,ItemList *pPointList);
	virtual void Compute			(TFunction2d *pFunction);
	virtual BOOL StretchFrames      (int nNewFrames[3]);

	BOOL LoadMorphBlock     (wmfstream&,int nBlock);
	void SaveMorphBlock     (wmfstream&,int nBlock);

	virtual BOOL LoadDocument   (wmfstream& file);
	virtual void SaveDocument   (wmfstream& file);
};

/////////////////////////////////////////////////////////////////
// TMorphmeshTriangles
///////////////////////////////////////////////////////////////

class TMorphmeshTriangles : public TMorphmesh_Document
{
	friend class TTriangulateUndo;
	friend class TDeleteTrianglesUndo;
	friend class TPointAddUndo;
	friend class TPointDeleteUndo;
	friend class TCutUndo;
	friend class TPastePointUndo;
	friend class TCreateGridUndo;

protected:
	TTriangle*  Triangles;
	int         TrianglesAllocated;
	int         nTriangles;
	void     	TrianglesResize(int nNewSize);

	int         nCurrentTriangle;
public:
	TMorphmeshTriangles(void);
	~TMorphmeshTriangles();

	virtual void Init          (void);
	virtual void Reset         (void);

	virtual void TriangleAdd		(TTriangle& tri);
	virtual void TriangleDelete		(TTriangle& Tri);
	TTriangle*   TriangleFind		(TTriangle& tri);
	virtual void SetVertex			(int nOldVertex,int nNewVertex);
	virtual void TrianglesSet(TTriangle* pSrcTriangles,int nSrcAllocated,int nSrcTriangles);

	void	SetToFirstTriangle		(void);
	TTriangle  *GetNextTriangle		(void);
	BOOL	WasLastTriangle			(void);

	virtual BOOL LoadDocument   (wmfstream& file);
	virtual void SaveDocument   (wmfstream& file);
};

/////////////////////////////////////////////////////////////////
// TMorphmeshPictures
///////////////////////////////////////////////////////////////

class Animation;

class TMorphmeshPictures : public TMorphmesh_Document
{
protected:
	Animation *Picture[4];
public:
	TMorphmeshPictures	(void);
	~TMorphmeshPictures	();

	static LPSTR szPictureNames[5];

	virtual void Init	(void);
	virtual void Reset	(void);

    Animation *GetAnimation		(int nPicture);
	virtual void SetAnimation	(int nPicture,Animation* pNewAnim);
	virtual void AnimationFrameChanged	(int nPicture,int nFrame);
	virtual void InvalidateAnimFrame	(int nPicture,int nFrame);

	virtual void PictureNew		(int nPicture,POINT pRes,int nFrames);
	virtual void PictureLoad	(int nPicture,LPSTR szFileName);
	virtual void PictureSave	(int nPicture);
	virtual void PictureSaveAs	(int nPicture);

	virtual BOOL LoadDocument   (wmfstream& file);
	virtual void SaveDocument   (wmfstream& file);
};

/////////////////////////////////////////////////////////////////
// TMorphmesh_Data
///////////////////////////////////////////////////////////////

class TMorphmesh_Data : public TMorphmeshPoints,
			public TMorphmeshTriangles,public TMorphmeshPictures
{
public:
	TMorphmesh_Data   (void);
	TMorphmesh_Data   (int nStartFrames,int nResultFrames,int nEndFrames);
	~TMorphmesh_Data  ();

	virtual void  Init  (void);
	virtual void Reset  (void);

	virtual	LPSTR	GetName		(void);
	virtual	BOOL	CanClose	(void);
	virtual	void	Save		(void);
	virtual	void	SaveAs		(void);

	BOOL  IsOK  (void);

	virtual void    SetChanged	(BOOL);
	virtual BOOL    HasChanged  (void);

	virtual void Triangulate(int nBlock,int nFrame);
	virtual void NormalizeTriangle(int nTri, int nBlock, int nFrame);
	virtual void PointDelete(int nIndex);
	virtual void PointFetch	(int nPoint,TMorphmeshPoints *pDestMesh);
protected:
	virtual BOOL LoadDocument   (wmfstream&);
	virtual void SaveDocument   (wmfstream&);

	int  nCheckFileVersion  (wmfstream&);
	BOOL LoadVersion1       (wmfstream&);
	void LoadMesh           (wmfstream&);
};

#endif
 
/* End of file MORFDATA.H */ 
 

/********************************************************************** 
 * 
 *  NAME:         TSTRUCT.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  11.8.95  MHM     added headers 
 * 
 *********************************************************************/ 
 
#ifndef TSTRUCT_H
#define TSTRUCT_H

#ifndef __WINDOWS_H
	#include <windows.h>
#endif

#include "filefunc.h"
#include "morphcfg.h"
#include "geom2d.h"


#define NO_RATIO -1.0

class MorphPoint
{
	Point2d pPoint;
	BOOL	fKeyPoint;
	float	fRatio;
    BOOL	fKeyRatio;
public:
    MorphPoint(void);

	MorphPoint(MorphPoint*);

	inline static void Paint	(POINT pPoint,int nPointSize,int nPointShape,HDC hDC);

	Point2d GetPoint			(void)			{ return pPoint;};
    void	SetPoint			(Point2d pNew)	{ pPoint = pNew;};	
	BOOL	GetKeyPoint			(void)			{ return fKeyPoint;};
	void	SetKeyPoint			(BOOL bValue)	{ fKeyPoint = bValue;};

	float	GetRatio			(void)			{ return fRatio;};
	void	SetRatio			(float fNew)	{ fRatio = fNew;};
	BOOL	GetKeyRatio			(void)			{ return fKeyRatio;};
	void	SetKeyRatio			(BOOL bValue)	{ fKeyRatio = bValue;};

	friend	wmfstream& operator>>	(wmfstream&,MorphPoint&);
	friend	wmfstream& operator<<	(wmfstream&,MorphPoint&);

// Functions for array of MorphPoints
protected:
	MorphPoint&	GetMorphPoint	(int nFrame)	{return ((MorphPoint*)this)[nFrame];};
	Point2d&	GetThePoint		(int nFrame)	{return GetMorphPoint(nFrame).pPoint;};
public:
	void	InitPoint			(int nFrames,Point2d pNewPoint);

	static MorphPoint *Create	(int nFrames)	{ return new MorphPoint[nFrames]; };
	MorphPoint *Copy			(int nFrames);

	Point2d GetPoint			(int nFrame)	{ return GetThePoint(nFrame);};
	void	SetPoint			(int nFrame,Point2d pNewPoint)
											{ GetThePoint(nFrame) = pNewPoint;};
	POINT	Scale				(int nFrame,POINT pRes);

	void	Interpolate			(int nFrames);
	static void Interpolate		(MorphPoint* mpSrc,int nSrcFrames,
								 MorphPoint* mpDest,int nDestFrames);
	static void Compute(
								class TFunction2d *pFunction,
								MorphPoint* mpDest,int nDestFrames,
								MorphPoint* mpSrc1,int nSrc1Frames,
								MorphPoint* mpSrc2,int nSrc2Frames );
	static void InterpolateKey	(MorphPoint* mpSrc,int nSrcFrames,
								 MorphPoint* mpDest,int nDestFrames);
	static  void SetKeyPoints	(MorphPoint* mpDest,int nDestFrames,BOOL fState);
	static  void MergeKeyPoints (MorphPoint* mpSrc,int nSrcFrames,
								MorphPoint* mpDest,int nDestFrames);
};

class MorphBlock;

class TTriangle
{
	int			vertex[3];
public:
	inline void Paint	(POINT pts[],HDC);

	Point2d*GetPoints	(MorphBlock *mbBlock,int frame);

	int 	GetVertex	(int nVertIndex)	{return vertex[nVertIndex];};
	void	SetVertex	(int nVertIndex,int nNewPoint)
											{vertex[nVertIndex] = nNewPoint;};

    int		FindVertex	(int nPoint);
	BOOL	HasVertex	(int nPoint);
	BOOL    IsEqual		(TTriangle&Tri);
	void	FlipVertices(void);

	// For optimized drawing
    // In a collection of triangles each line is draw only once
protected:
	BOOL		line[3];
public:
	void	Set_Lines_Unique		(void);
	void	Remove_Duplicate_Lines	(TTriangle& Tri);
	void	Add_Duplicate_Lines		(TTriangle& Tri);

	friend	wmfstream& operator>>		(wmfstream&,TTriangle&);
    friend	wmfstream& operator<<		(wmfstream&,TTriangle&);
};

inline void MorphPoint::Paint(POINT P,int nPointSize,int nPointShape, HDC hDC)
{
	if(nPointShape==POINT_SHAPE_RECT)
		Rectangle(hDC,
			P.x - nPointSize,			P.y - nPointSize,
			P.x + nPointSize,			P.y + nPointSize);
	else
		Ellipse(hDC,
			P.x - nPointSize,			P.y - nPointSize,
			P.x + nPointSize,			P.y + nPointSize);
};

inline void TTriangle::Paint(POINT pts[],HDC hDC)
{
	POINT	p1=pts[GetVertex(0)];MoveTo(hDC,p1.x,p1.y);
	POINT	p =pts[GetVertex(1)];
	if(line[0])
		LineTo(hDC, p.x, p.y);
	   else
		MoveTo(hDC, p.x, p.y);

	p =pts[GetVertex(2)];
	if(line[1])
		LineTo(hDC, p.x, p.y);
	   else
		MoveTo(hDC, p.x, p.y);

	p1=pts[GetVertex(0)];
	if(line[2])
		LineTo(hDC, p1.x, p1.y);
};

class TFunction2d
{
public:
	TFunction2d(){};
    virtual ~TFunction2d(){};

	virtual void Config(){};
    virtual LPSTR GetName()=0;
	virtual void Compute( Point2d& pDest, Point2d& pSrc1, Point2d& pSrc2, float fRatio)=0;
};

class TInterpolate : public TFunction2d
{
public:
    virtual LPSTR GetName(){ return "Compute : Interpolate";};
	virtual void Compute( Point2d& pDest, Point2d& pSrc1, Point2d& pSrc2, float fRatio);
};

class TSphere : public TFunction2d
{
public:
	virtual LPSTR GetName(){ return "Compute : Sphere";};
	virtual void Compute(Point2d& pDest, Point2d& pSrc1, Point2d& pSrc2, float fRatio);
};

class TEarth : public TFunction2d
{
public:
	virtual LPSTR GetName(){ return "Compute : Earth";};
	virtual void Compute(Point2d& pDest, Point2d& pSrc1, Point2d& pSrc2, float fRatio);
};

class TWhirlpool : public TFunction2d
{
public:
	virtual LPSTR GetName(){ return "Compute : Whirlpool";};
	virtual void Compute(Point2d& pDest, Point2d& pSrc1, Point2d& pSrc2, float fRatio);
};

class TYinYang : public TFunction2d
{
public:
	virtual LPSTR GetName(){ return "Compute : Yin&Yang";};
	virtual void Compute(Point2d& pDest, Point2d& pSrc1, Point2d& pSrc2, float fRatio);
};

#endif
 
/* End of file TSTRUCT.H */ 
 

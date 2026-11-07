/********************************************************************** 
 * 
 *  NAME:         MORPHER.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  11.8.95  MHM     added headers 
 *  1.9475  12.12.95 MHM Morpher has been completely changed, earlier it had
 *   a message loop which called the Morpher, now the Morph-function is called,
 *   which in turn calls the message loop when it seems it's necessary. Preparing 
 *   the morpher for the Animation Control
 *                   MHM and changed some function name to standards
 *********************************************************************/ 
 
#ifndef MORPHER_H
#define MORPHER_H

#include "process.h"
#include "tstruct.h"

//////////////////////////////////////////////////////
// TMorpher
///////////////////////////////////////////////////////
class TMorphWnd;
class Animation;
class TPicture;         // dib.h
class Scanline_Screen;
class MorphBlock;

class TriangleInfo {
public:
	long Ax,Ay;
	long Bx,By;
	long BaseX,BaseY;
	long px,py;
	long Delta1X,Delta1Y;
	long Delta2X,Delta2Y;
	long Delta4X,Delta4Y;

	MorphBlock *mbMorphBlock;
	int        MorphFrames;

	double      Frame;
	POINT    p[3];

	Animation   *Anim;
	TPicture   *Dib;
	RGBTRIPLE  Color;
	POINT    Res;
	float    W,H;

	void SetAnimation      (int nMorphFrames,MorphBlock *mbBlock,Animation*);

	void ChangeToFrame      (double fNewFrame);

	Point2d *GetPoints      (TTriangle& Tri);
	void CalculateVectors   (TTriangle& Tri);
	void SetToPoint         (long tx,long ty);
    void SetDelta			(long DeltaXX,long DeltaXY); 
	void AddDeltaX		    (void);
	inline   void GetColor1 (void);
	inline   void GetColor2 (void);
	inline   void GetColor4 (void);

	POINT FloatToPoint   (Point2d);
};

class ResTriangleInfo : public TriangleInfo
{
// For Result Animation
	BYTE huge   *Cur_Line;

public:
	BOOL NewFrame(double fNewFrame,int nFrame);
	void FrameFinished(int nFrame);

	void GetPictureTriangle(POINT v[3]);

	inline void SetColorY(int y);
	inline void SetColorX(int x);
};

#define MORPH		0
#define WARP_START	1
#define WARP_END	2	
#define AVERAGE		3
#define SUBSTRACT	4
#define MULTIPLY	5

class TMorphmesh_Data;


class TMorpher : public TProcess
{
protected:
	TMorphmesh_Data *MorphMesh;
	TriangleInfo   StartTri,EndTri;
	ResTriangleInfo ResultTri;
	Animation      *BackGround;
	int     nMorphType;
	int 	nAntiAliasing;

	Scanline_Screen   *Screen;

	BOOL  fCheckValidity (Animation* animation);

	virtual void CreateFrame   (double fNewFrame,int nFrame);
	virtual void MorphTriangle (double fFrame,TTriangle&);
	virtual void EndFrame      (int nFrame);
public:
	TMorpher(
		TMorphmesh_Data *mNewMorph,
		int nMorphType,int nAntiAliasing);
	~TMorpher();

	virtual void MorphFrame(int nFrame);

	virtual void Morph(int nFrom,int nTo);
};

#endif


/* End of file MORPHER.H */ 
 

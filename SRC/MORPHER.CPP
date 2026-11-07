/**********************************************************************
 * 
 *  NAME:         MORPHER.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what
 *  ------------------------------------------------------------------- 
 *          11. 8.95 MHM added headers 
 *  1.9471  11.12.95 MHM renamed morphstr.h to morphmsh.h
 *  1.9475  12.12.95 MHM Morpher has been completely changed, earlier it had
 *   a message loop which called the Morpher, now the Morph-function is called,
 *   which in turn calls the message loop when it seems it's necessary. Preparing
 *   the morpher for the Animation Control
 *                   MHM removed TMorpher::FloatToPoint(), no need
 *********************************************************************/ 
 
#include "morpher.h"
#include "morphwin.h"   // class TMorphWnd
#include "morphpic.h"   // class TMorphPictureWnd
#include "animation.h"  // class TAnimation
#include "tpicture.h"   // class TPicture
#include "morphmsh.h"   // class TMorphmesh
#include "scanline.h"
#include "morphcfg.h"	// class MorphConfig

BOOL TMorpher::fCheckValidity(Animation* animation)
{
	BOOL bValid;
	if(animation==NULL)
		bValid = FALSE;
	   else {
	   	bValid = TRUE;
        // Check that all the frames are present
		for(int i = 0 ; i < animation->GetFrames() ; i++ )
			if(animation->Get_Frame(i)==NULL){
				bValid = FALSE;
				break;
                }
		  };
	 return bValid;
};

TMorpher::TMorpher(
	TMorphmesh_Data *NewMesh,
	int nNewMorphType,
	int nNewAntiAliasing):
	TProcess()
{
	Screen = NULL;

	// Checking that all resources are available
	Canrun = FALSE;
	 do{
		  // Check that there's morphmesh
		if(!NewMesh) break;

		// Check first that all frames are available
        if(nNewMorphType!=WARP_END)
			if(!fCheckValidity(NewMesh->GetAnimation(MorphStart)))   break;
     	if(nNewMorphType!=WARP_START)
			if(!fCheckValidity(NewMesh->GetAnimation(MorphEnd)))     break;
	 
		  // Check for result animation
		if(!NewMesh->GetAnimation(MorphResult))   break;

		  // Create scanline screen
		Screen = new Scanline_Screen(NewMesh->GetAnimation(MorphResult)->GetResolution());
		if(!Screen)    break;

		// All resources available
		Canrun = TRUE;
		}
	while(FALSE);
	if(!Canrun) return;

	// Now set the resources
	MorphMesh = NewMesh;

	StartTri .SetAnimation(MorphMesh->GetFrames(MorphStart ),
		MorphMesh->GetMorphBlock(MorphStart) ,
		NewMesh->GetAnimation(MorphStart) );
	EndTri   .SetAnimation(MorphMesh->GetFrames(MorphEnd   ),
		MorphMesh->GetMorphBlock(MorphEnd)   ,
		NewMesh->GetAnimation(MorphEnd)   );
	ResultTri.SetAnimation(MorphMesh->GetFrames(MorphResult),
		MorphMesh->GetMorphBlock(MorphResult),
		NewMesh->GetAnimation(MorphResult));
	BackGround = NewMesh->GetAnimation(MorphBack);

	nMorphType    = nNewMorphType;
	nAntiAliasing = nNewAntiAliasing;

	if(MorphConfig.fAutoResultFrames){
		TInterpolate *pInterpolate = new TInterpolate();
		MorphMesh->Compute(pInterpolate);
        delete pInterpolate;
    }
};

TMorpher::~TMorpher()
{
	if(Screen!=NULL){
		delete Screen;
		Screen = NULL;
		  };
};

void TMorpher::Morph(int nFrom,int nTo)
{
	Running = TRUE;

	// Morph frames from...to
	for(int nFrame = nFrom ; (nFrame <= nTo)&&Running ; nFrame ++ ){
		MorphFrame(nFrame);

		// Let windows run for a while
		Yield();
		};

	Running = FALSE;
};


void TMorpher::MorphFrame(int nFrame)
{
	 // Start execution
	Running = TRUE;

    // Get frame number in floating point (0.0-1.0)
	double fFrame;

	if(nFrame == 0)
		fFrame = 0.0;
		else
		fFrame =(double)nFrame/
				(double)(ResultTri.Anim->GetFrames()-1);

	// Create bitmap and copy a background to it if needed
	CreateFrame(fFrame,nFrame);

	// Init Triangles to first frame
	StartTri.ChangeToFrame(fFrame);
	EndTri.  ChangeToFrame(fFrame);

	// Let windows run for a while
	Yield();

	// Morphing the triangles until 
	MorphMesh->SetToFirstTriangle();
	while((!MorphMesh->WasLastTriangle())&&Running){
		MorphTriangle(fFrame,*(MorphMesh->GetNextTriangle()));

		// Let windows run for a while
		Yield();
		}

	// Move frame to animation
	EndFrame(nFrame);

	// Let windows run for a while
	Yield();
};

void TMorpher::EndFrame(int nFrame)
{
	// Picture finished
	if(Running)
		ResultTri.FrameFinished(nFrame);
};

void TMorpher::CreateFrame(double fFrame,int nAnimFrame)
{
	// Create new frame to morph
	if(ResultTri.NewFrame(fFrame,nAnimFrame)==FALSE){
		MessageBox(NULL,
			"Can't create more frames because there's no more memory left."
			,NULL,MB_TASKMODAL | MB_OK);
		Running = FALSE;
		return;
		};

	// Copy background
	if(BackGround){
		int nBackFrame = (int)(fFrame*(BackGround->GetFrames()-1)+0.5);
		TPicture *pBackGroundFrame = BackGround->Get_Frame(nBackFrame);
		if(pBackGroundFrame)
			ResultTri.Dib->Copy(pBackGroundFrame,NULL);
		};
};

////////////////////////////////////
// TriangleInfo
////////////////////////////////////

inline void TriangleInfo::SetToPoint(long tx,long ty)
{
	px = (((BaseX<<16) + ty*Ax + tx*Bx )>>14)*Res.x;
	py = (((BaseY<<16) + ty*Ay + tx*By )>>14)*Res.y;

	py = (((long)Res.y-1L)<<16)-py;
};

inline void TriangleInfo::SetDelta(long addx,long addy)
{
	Delta1X =  ((addx*Bx + addy*Ax)>>14)*Res.x;
	Delta1Y = -((addx*By + addy*Ay)>>14)*Res.y;

	Delta2X = Delta1X>>1;
	Delta2Y = Delta1Y>>1;

	Delta4X = Delta1X>>2;
	Delta4Y = Delta1Y>>2;
};

inline void TriangleInfo::AddDeltaX()
{
	px += Delta1X;
	py += Delta1Y; 
};

#define PIC_BORDERSX(X) { if(X < 0) X = 0;else if(X >= Res.x) X = Res.x-1; }
#define PIC_BORDERSY(Y) { if(Y < 0) Y = 0;else if(Y >= Res.y) Y = Res.y-1; }

// range of x and y   16:16 only fractional part used
/*inline*/ void TriangleInfo::GetColor1()
{
	int x,y;

	x = HIWORD(px);PIC_BORDERSX(x);
	y = HIWORD(py);PIC_BORDERSY(y);

	Color = Dib->GetPixel(x,y);   // Get the color
};

/*inline*/ void TriangleInfo::GetColor2()
{
    int nRed,nGreen,nBlue;

	/*  The samples are taken from
		(px,py)			   (px + Delta2X,py)
		(px,py + Delta2Y)  (px + Delta2X,py + Delta2Y)
		=
        (X1,Y1)				(X2,Y1)
		(X1,Y2)				(X2,Y2)
	*/

	int X1,X2,Y1,Y2;

	X1 = HIWORD(px);				PIC_BORDERSX(X1);
	X2 = HIWORD(px + Delta2X);		PIC_BORDERSX(X2);

	Y1 = HIWORD(py);				PIC_BORDERSY(Y1);
	Y2 = HIWORD(py + Delta2Y);		PIC_BORDERSY(Y2);

	Color = Dib->GetPixel(X1,Y1);   // Get the color1
	nRed  = Color.rgbtRed;nGreen = Color.rgbtGreen;nBlue = Color.rgbtBlue;
	Color = Dib->GetPixel(X2,Y1);   // Get the color2
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X1,Y2);   // Get the color3
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X2,Y2);   // Get the color4
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;

	Color.rgbtRed  = (BYTE)(nRed   >> 2);
	Color.rgbtGreen= (BYTE)(nGreen >> 2);
	Color.rgbtBlue = (BYTE)(nBlue  >> 2);
};

/*inline*/ void TriangleInfo::GetColor4()
{
    int nRed,nGreen,nBlue;

	/*  The samples are taken from
		(px,py)			   (px + Delta2X,py)
		(px,py + Delta2Y)  (px + Delta2X,py + Delta2Y)
		=
        (X1,Y1)		(X2,Y1)		(X3,Y1)		(X4,Y1)
		(X1,Y2)		(X2,Y2)		(X3,Y2)		(X4,Y2)
		(X1,Y3)		(X2,Y3)		(X3,Y3)		(X4,Y3)
		(X1,Y4)		(X2,Y4)		(X3,Y4)		(X4,Y4)
	*/

	int X1,X2,X3,X4;
	int Y1,Y2,Y3,Y4;

	X1 = HIWORD(px);				PIC_BORDERSX(X1);
	X2 = HIWORD(px + Delta4X);		PIC_BORDERSX(X2);
	X3 = HIWORD(px + Delta2X);		PIC_BORDERSX(X3);
	X4 = HIWORD(px + Delta2X+Delta4X);PIC_BORDERSX(X4);

	Y1 = HIWORD(py);				PIC_BORDERSY(Y1);
	Y2 = HIWORD(py + Delta4Y);		PIC_BORDERSY(Y2);
	Y3 = HIWORD(py + Delta2Y);		PIC_BORDERSY(Y3);
	Y4 = HIWORD(py + Delta2Y+Delta4Y);PIC_BORDERSY(Y4);

	Color = Dib->GetPixel(X1,Y1);   // Get the color1
	nRed  = Color.rgbtRed;nGreen = Color.rgbtGreen;nBlue = Color.rgbtBlue;
	Color = Dib->GetPixel(X2,Y1);   // Get the color2
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X3,Y1);   // Get the color3
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X4,Y1);   // Get the color4
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X1,Y2);   // Get the color5
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X2,Y2);   // Get the color6
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X3,Y2);   // Get the color7
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X4,Y2);   // Get the color8
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X1,Y3);   // Get the color9
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X2,Y3);   // Get the color10
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X3,Y3);   // Get the color11
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X4,Y3);   // Get the color12
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X1,Y4);   // Get the color13
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X2,Y4);   // Get the color14
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X3,Y4);   // Get the color15
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;
	Color = Dib->GetPixel(X4,Y4);   // Get the color16
	nRed  += Color.rgbtRed;nGreen += Color.rgbtGreen;nBlue += Color.rgbtBlue;

	Color.rgbtRed  = (BYTE)(nRed   >> 4);
	Color.rgbtGreen= (BYTE)(nGreen >> 4);
	Color.rgbtBlue = (BYTE)(nBlue  >> 4);
};

void TriangleInfo::SetAnimation(int frames,MorphBlock *mbBlock,Animation* anim)
{
	 MorphFrames = frames;
	 mbMorphBlock = mbBlock;

	 Dib = NULL;
	Anim = anim;
	if(Anim!=NULL){
		Res = Anim->GetResolution();
		W = (float)RESOLUTION / (float)Res.x;
		H = (float)RESOLUTION / (float)Res.y;
		}
		else {
		Res.x = 0;
		Res.y = 0;
		W = 0;
		H = 0;
		}
};

POINT TriangleInfo::FloatToPoint(Point2d fp)
{
	POINT P= {(int)(fp.x*16384),(int)(fp.y*16384)};
	return P;
};

Point2d *TriangleInfo::GetPoints(TTriangle& Tri)
{
	return Tri.GetPoints(mbMorphBlock,(int)(Frame*(MorphFrames-1)+0.5));
};

void TriangleInfo::CalculateVectors(TTriangle& Tri)
{
	Point2d *fp = GetPoints(Tri);
	 p[0] = FloatToPoint(fp[0]);
	 p[1] = FloatToPoint(fp[1]);
	 p[2] = FloatToPoint(fp[2]);

	Ax = p[1].x - p[0].x;      Ay = p[1].y - p[0].y;
	Bx = p[2].x - p[0].x;      By = p[2].y - p[0].y;
	BaseX = p[0].x;            BaseY = p[0].y;
};

void TriangleInfo::ChangeToFrame(double fNewFrame)
{
	 assert(Anim!=NULL);

	 Frame = fNewFrame;
	 if(Anim){
		int nAnimFrame =(int)(Frame * (Anim->GetFrames()-1)+0.5);
		Dib = Anim->Get_Frame(nAnimFrame);
		  assert(Dib!=NULL);
		  };
};

////////////////////////////////
// For Result Triangle
/////////////////////////////////

inline void ResTriangleInfo::SetColorY(int y)
{
	Cur_Line = Dib->GetYLine(Res.y-y-1);
};

inline void ResTriangleInfo::SetColorX(int x)
{
	RGBTRIPLE *color = (RGBTRIPLE*)&Cur_Line[x*sizeof(RGBTRIPLE)];
	color->rgbtRed   = Color.rgbtRed;
	color->rgbtGreen = Color.rgbtGreen;
	 color->rgbtBlue  = Color.rgbtBlue;
};

BOOL ResTriangleInfo::NewFrame(double fNewFrame,int nAnimFrame)
{
	Frame = fNewFrame;
	Dib = TPicture::Create(Res.x,Res.y,24);
    if(Dib)
		Anim->Set_Frame(Dib,nAnimFrame);
	return (Dib!=NULL);
};

void ResTriangleInfo::FrameFinished(int)
{
	Dib = NULL;
};

void ResTriangleInfo::GetPictureTriangle(POINT v[3])
{
	 // Convert to device points
	 // point = (point*Result.Resolution) / 16384
	v[0].x = (int)((((long)p[0].x)*Res.x )>>14);
	v[0].y = (int)((((long)p[0].y)*Res.y )>>14);
	v[1].x = (int)((((long)p[1].x)*Res.x )>>14);
	v[1].y = (int)((((long)p[1].y)*Res.y )>>14);
	v[2].x = (int)((((long)p[2].x)*Res.x )>>14);
	v[2].y = (int)((((long)p[2].y)*Res.y )>>14);
};


///////////////////////////////7
// Morphing functions
//////////////////////////////////

#define GetColorBlend(color) \
((BYTE)(HIWORD(StartTri.Color.color  * sratio + EndTri.Color.color * eratio)))

#define MORPH_FUNC(GetColor)\
    {\
	StartTri.GetColor();		\
	EndTri  .GetColor();       \
    \
	ResultTri.Color.rgbtRed  = GetColorBlend(rgbtRed);\
	ResultTri.Color.rgbtGreen= GetColorBlend(rgbtGreen);\
	ResultTri.Color.rgbtBlue = GetColorBlend(rgbtBlue);\
    \
	ResultTri.SetColorX(x); \
    \
	StartTri.AddDeltaX();\
	EndTri  .AddDeltaX();\
    }

#define WARP_START_FUNC(GetColor)\
    {\
	StartTri.GetColor();\
    \
	ResultTri.Color.rgbtRed  = StartTri.Color.rgbtRed;\
	ResultTri.Color.rgbtGreen= StartTri.Color.rgbtGreen;\
	ResultTri.Color.rgbtBlue = StartTri.Color.rgbtBlue;\
    \
	ResultTri.SetColorX(x);\
    \
	StartTri.AddDeltaX();\
	}

#define WARP_END_FUNC(GetColor)\
    {\
	EndTri.GetColor1();\
	\
	ResultTri.Color.rgbtRed  = EndTri.Color.rgbtRed;\
	ResultTri.Color.rgbtGreen= EndTri.Color.rgbtGreen;\
	ResultTri.Color.rgbtBlue = EndTri.Color.rgbtBlue;\
    \
	ResultTri.SetColorX(x);\
    \
	EndTri.AddDeltaX();\
	}

#define AVERAGE_FUNC(GetColor)\
	{\
	StartTri.GetColor();		\
	EndTri  .GetColor();       \
	\
	ResultTri.Color.rgbtRed  = (BYTE)(((WORD)StartTri.Color.rgbtRed + (WORD)EndTri.Color.rgbtRed)>>1);\
	ResultTri.Color.rgbtGreen= (BYTE)(((WORD)StartTri.Color.rgbtGreen + (WORD)EndTri.Color.rgbtGreen)>>1);\
	ResultTri.Color.rgbtBlue = (BYTE)(((WORD)StartTri.Color.rgbtBlue + (WORD)EndTri.Color.rgbtBlue)>>1);\
    \
	ResultTri.SetColorX(x); \
    \
	StartTri.AddDeltaX();\
	EndTri  .AddDeltaX();\
    }

#define SUBSTRACT_FUNC(GetColor)\
	{\
	StartTri.GetColor();		\
	EndTri  .GetColor();       \
    \
	ResultTri.Color.rgbtRed  = StartTri.Color.rgbtRed - EndTri.Color.rgbtRed;\
	ResultTri.Color.rgbtGreen= StartTri.Color.rgbtGreen - EndTri.Color.rgbtGreen;\
	ResultTri.Color.rgbtBlue = StartTri.Color.rgbtBlue - EndTri.Color.rgbtBlue;\
	\
	ResultTri.SetColorX(x); \
    \
	StartTri.AddDeltaX();\
	EndTri  .AddDeltaX();\
	}

#define MULTIPLY_FUNC(GetColor)\
	{\
	StartTri.GetColor();		\
	EndTri  .GetColor();       \
	\
	ResultTri.Color.rgbtRed  = HIWORD((WORD)StartTri.Color.rgbtRed   * (WORD)EndTri.Color.rgbtRed);\
	ResultTri.Color.rgbtGreen= HIWORD((WORD)StartTri.Color.rgbtGreen * (WORD)EndTri.Color.rgbtGreen);\
	ResultTri.Color.rgbtBlue = HIWORD((WORD)StartTri.Color.rgbtBlue  * (WORD)EndTri.Color.rgbtBlue);\
	\
	ResultTri.SetColorX(x); \
    \
	StartTri.AddDeltaX();\
	EndTri  .AddDeltaX();\
	}	


#define GRAPH_FUNC(Function)\
	switch(nAntiAliasing){\
    case 1:                \
		for(int x = Scanline.x ; x < Scanline.y ; x++ )\
             Function(GetColor1);\
		break;\
	case 4:\
		for(x = Scanline.x ; x < Scanline.y ; x++ )\
			 Function(GetColor2);\
		break;\
	case 16:\
		for(x = Scanline.x ; x < Scanline.y ; x++ )\
			  Function(GetColor4);\
		break;\
	}


void TMorpher::MorphTriangle(double fFrame,TTriangle& Tri)
{
	StartTri .CalculateVectors(Tri);
	EndTri   .CalculateVectors(Tri);
	ResultTri.CalculateVectors(Tri);

	// Get trianglepoints for result animation
	POINT p[3];
	ResultTri.GetPictureTriangle(p);

	// Convert triangle to scanlines
	Screen->Clear();
	Screen->DrawTriangle(p[0],p[1],p[2]);

	// ... and lines to pixels

	// Calculate these in advance
	long Vectx,Vecty;
	long eratio = (long)(65536.0 * fFrame);
	long sratio =  65536L - eratio;

	// range 13:16
	long area = (ResultTri.Ay * ResultTri.Bx - ResultTri.Ax * ResultTri.By);
	if( area == 0 ) return;        // Can't divide with zero
    if( area > 0 ) return;
	// If divider is zero, it means that the points are all in same line
	float divider =   area / 65536.0;

	long addX = (ResultTri.Ax * ResultTri.BaseY - ResultTri.Ay * ResultTri.BaseX) / divider;
	long addY = (ResultTri.By * ResultTri.BaseX - ResultTri.Bx * ResultTri.BaseY) / divider;
	long axsc = (ResultTri.Ax * ResultTri.H) / divider;
	long aysc = (ResultTri.Ay * ResultTri.W) / divider;
	long bxsc = (ResultTri.Bx * ResultTri.H) / divider;
	long bysc = (ResultTri.By * ResultTri.W) / divider;

	RECT DestClip = Screen->GetExtents();

	StartTri.SetDelta(aysc,-bysc);
	EndTri.SetDelta	 (aysc,-bysc);

	for(int y = DestClip.top ; y < DestClip.bottom ; y++){
		// Get scanlines from screen
		  POINT Scanline = Screen->Get_Scanline(y);
		if(Scanline.y>-1){

			// Calculate first vector point
			Vectx = Scanline.x*aysc - y*axsc + addX;
			Vecty = y*bxsc - Scanline.x*bysc + addY;

			// Inform to Start and End triangles
			StartTri.SetToPoint(Vectx,Vecty);
			EndTri  .SetToPoint(Vectx,Vecty);

			ResultTri.SetColorY(y);

			switch(nMorphType){
            case MORPH:
                GRAPH_FUNC(MORPH_FUNC);
			break;
            case WARP_START:
                GRAPH_FUNC(WARP_START_FUNC);
			break;
			case WARP_END:
                GRAPH_FUNC(WARP_END_FUNC);
            break;
			case AVERAGE:
				GRAPH_FUNC(AVERAGE_FUNC);
            break;
			case SUBSTRACT:
            	GRAPH_FUNC(SUBSTRACT_FUNC);
			break;
			case MULTIPLY:
				GRAPH_FUNC(MULTIPLY_FUNC);
            break;
            }
			} // if(Scanline.y>-1)
		} // for y
};

/* End of file MORPHER.CPP */ 


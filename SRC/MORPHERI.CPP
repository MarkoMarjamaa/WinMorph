/**********************************************************************
 * 
 *  NAME:         MORPHERI.CPP 
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
 * 
 *********************************************************************/ 
 
#include "morpheri.h"
#include "animwnd.h"
#include "morfdata.h"

TMorpher_Interface::TMorpher_Interface(
		TMorphmesh_Data *mNewMorph,
		TAnimationWnd* wNewAnimWnd,
		int nNewMorphType,
		int nAntiAliasing ):
	TMorpher(mNewMorph,nNewMorphType,nAntiAliasing)
{
	if(!CanRun()) return;

	if(wNewAnimWnd==NULL){
		Canrun = FALSE;
		return;
		}

	AnimationWindow = wNewAnimWnd;
};

void TMorpher_Interface::~TMorpher_Interface()
{
};


void TMorpher_Interface::Morph(int nFrom,int nTo)
{
	TMorpher::Morph(nFrom,nTo);
};

void TMorpher_Interface::CreateFrame(double fFrame,int nFrame)
{
	TMorpher::CreateFrame(fFrame,nFrame);

    /* Inform other windows to update , not invalidate */
	MorphMesh->AnimationFrameChanged(MorphResult,nFrame);

    /* Update animation window */
	InvalidateRect(AnimationWindow->HWindow,NULL,FALSE);
};

void TMorpher_Interface::MorphTriangle(double Frame,TTriangle& tTri)
{
	TMorpher::MorphTriangle(Frame,tTri);

	 // Update result window
	AnimationWindow->InvalidateScanline(Screen);
};

void TMorpher_Interface::EndFrame(int nFrame)
{
	TMorpher::EndFrame(nFrame);

	/* Get viewmap */
	TViewmap * pView = AnimationWindow->FetchViewmap();

	/* Inform other windows to update , not invalidate */
	MorphMesh->AnimationFrameChanged(MorphResult,nFrame);

	/* Now set back animation windows viewmap */
    AnimationWindow->SetViewmap(pView);

	/* Invalidate all windows with frame */
	MorphMesh->InvalidateAnimFrame(MorphResult,nFrame);
};

/* End of file MORPHERI.CPP */ 


/**********************************************************************
 * 
 *  NAME:         ACMORPH.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          13.01.96 MHM created
 *
 *********************************************************************/

#include "acmorph.h"
#include "morpheri.h"	// class TMorpher_Interface
#include "morphwin.h"	// class TMorphWnd
#include "morphedw.h"	// class TMorphEditWnd
#include "acmorphc.h"	// class ACMorphContextCfg

ACMorphContext::ACMorphContext(void):
	ACAnimContext()
{
	Morpher		= NULL;

	nMorphing	= 0;
	nMorphMode	= MORPH;
	nAntiAliasing = 1;
};

ACMorphContext::~ACMorphContext()
{
	assert(nMorphing==FALSE);
};


BOOL ACMorphContext::CanMorph(void)
{
	if(Parent)
		return Parent->CanMorph();
    return FALSE;
};

void ACMorphContext::Config()
{
	ACMorphContextCfg NewCfg;

	NewCfg.nPlayMode  = nPlayMode;
	NewCfg.nPlayDelay = nPlayDelay;
    NewCfg.nMorphMode = nMorphMode;
	NewCfg.nAntiAliasing = nAntiAliasing;

	if(	((Parent->GetModule())->
		ExecDialog(
			new ACMorphConfigWnd(Parent,&NewCfg)))==IDOK){
		// Assign new values
		nPlayMode  = NewCfg.nPlayMode;
		nPlayDelay = NewCfg.nPlayDelay;
		nMorphMode = NewCfg.nMorphMode;
		nAntiAliasing = NewCfg.nAntiAliasing;
		};	
};

LPSTR ACMorphContext::GetStateName(void)
{
	LPSTR nStateName;

	if((nPlayState==ACPLAY_PLAYING)&&(nMorphing==TRUE))
		nStateName = "Morphing";
	   else
		nStateName = ACAnimContext::GetStateName();

	return nStateName;
};

void ACMorphContext::Morph(void)
{
	TMorphWnd *MorphWnd =
		(TMorphWnd*)(Parent->Parent);
	nMorphing = TRUE;

	ACContext::Play();

	Parent->UpdateTitle();

	// Create morpher
	Morpher = new TMorpher_Interface(
		MorphWnd->GetMorphmesh(),
		(TAnimationWnd *)Parent,
		nMorphMode,nAntiAliasing);

	// Inform parent
	MorphWnd->SetMorphing(LOCK_START);

	if(Morpher->CanRun()){
	    // Morph
		while(nPlayState!=ACPLAY_STOPPED)
			PlayFrame();
		};

	// Inform parent
	MorphWnd->SetMorphing(LOCK_END);

	// Delete morpher
	delete Morpher;
	Morpher = NULL;

	nMorphing = FALSE;

	Parent->UpdateTitle();

	if(MorphConfig.fBeepWhenFinished)
		MessageBeep(-1);
};

void ACMorphContext::SetPause(BOOL fNewPauseState)
{
	ACAnimContext::SetPause(fNewPauseState);

	if(nMorphing)
		Morpher->fPaused = fNewPauseState;
};

BOOL ACMorphContext::GetNextFrame(void)
{
	if(nMorphing){
		nNextFrame = nFrame + 1;
		return (nNextFrame <= nMarkOut );
		}
	   else
        return ACAnimContext::GetNextFrame();
};

void ACMorphContext::Stop(void)
{
	if(nPlayState!=ACPLAY_STOPPED){
		if(nMorphing){
	        // Stop morphing
			ACContext::Stop();

    	    Morpher->fPaused = FALSE;
			Morpher->Stop();
			}
		   else
			ACAnimContext::Stop();
		};
};

void ACMorphContext::SetFrame(void)
{
    // Default processing
	ACContext::SetFrame();

	// Update window's frame number
	Parent->SetViewFrame(nFrame);

	// Let windows run for a while
	Yield();

	if(nMorphing){
        // Morph one frame
		Morpher->MorphFrame(nFrame);
		}
};

/* End of file ACMORPH.CPP */

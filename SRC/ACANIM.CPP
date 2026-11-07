/**********************************************************************
 * 
 *  NAME:         ACANIM.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          13.12.95 MHM created 
 *
 *********************************************************************/

#include "acanim.h"
#include "morphpic.h"
#include "morphwin.h"
#include "acanimcf.h"
 
ACAnimContext::ACAnimContext()
{
	Parent = NULL;

	nPlayDelay = 50;
};

void ACAnimContext::Config(void)
{
	ACAnimContextCfg NewCfg;

	NewCfg.nPlayMode  = nPlayMode;
	NewCfg.nPlayDelay = nPlayDelay;

	if(	((Parent->GetModule())->
		ExecDialog(
			new ACAnimConfigWnd(Parent,&NewCfg)))==IDOK){
		// Assign new values
		nPlayMode  = NewCfg.nPlayMode;
		nPlayDelay = NewCfg.nPlayDelay;
		};	
};

void ACAnimContext::SetParent(TMorphPictureWnd *NewParent)
{
	Parent = NewParent;
};

void ACAnimContext::SetFrame(void)
{
	ACContext::SetFrame();

	Parent->SetViewFrame(nFrame);
};

void ACAnimContext::Stop(void)
{
    if(nPlayState!=ACPLAY_STOPPED){
		ACContext::Stop();

    	// Stop timer
		KillTimer(Parent->HWindow,ID_PLAYING);

    	// Inform parent
		((TMorphWnd*)(Parent->Parent))->SetPlaying(LOCK_END);

		Parent->UpdateTitle();
        };
};

void ACAnimContext::Play(void)
{
	ACContext::Play();

    // Start timer
	if(SetTimer(Parent->HWindow,ID_PLAYING,nPlayDelay,NULL)==0){
        assert(FALSE);
		};

	// Inform parent
	((TMorphWnd*)(Parent->Parent))->SetPlaying(LOCK_START);

	// Set first frame
	PlayFrame();
};

void ACAnimContext::SetPause(BOOL fNewPauseState)
{
	ACContext::SetPause(fNewPauseState);

    Parent->UpdateTitle();
};


/* End of file ACANIM.CPP */


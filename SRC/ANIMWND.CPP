/**********************************************************************
 * 
 *  NAME:         ANIMWND.CPP 
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
 
#include "common.h"
#include "animwnd.h"
#include "animndlg.h"
#include "animatio.h"
#include "tpicture.h"
#include "viewmap.h"

TAnimationWnd::TAnimationWnd(PTWindowsObject AParent,LPSTR Name,ACAnimContext *NewAC)
		:TMorphPictureWnd(AParent,Name,NewAC)
{
	animation = NULL;
	bOwnsAnimation = FALSE;
	Viewmaps = NULL;

    Anim_Frame = 0;
};

TAnimationWnd::~TAnimationWnd()
{
	Delete_Animation();
};

Animation* TAnimationWnd::GetAnimation(){ return animation;};

void TAnimationWnd::Delete_Animation()
{
    // Delete viewmaps
	if(Viewmaps){
        // First delete all viewmaps one by one
		for(int i = 0 ; i < animation->GetFrames() ; i++ )
			if(Viewmaps[i]!=NULL)
				delete Viewmaps[i];
        // Delete viewmap array
		delete Viewmaps;
		Viewmaps = NULL;
        };
    // Delete animation
	if((bOwnsAnimation)&&(animation)){
		delete animation;
		animation = NULL;
		};
};

void TAnimationWnd::CMNew(RTMessage)
{
    // Set default values
    TAnimationOpt Options={{320,200},1};

    // If there's an earlier animation, use it's dimensions
	if(GetAnimation()){
		Options.Res    = GetAnimation()->GetResolution();
		Options.Frames = GetAnimation()->GetFrames();
        };

    // Open New Animation window
	if(GetModule()->ExecDialog( new
			TAnimationNewDlg(this,&Options))==IDOK){

        // Check if the earlier animation can be closed 
		BOOL bCanClose = TRUE;
		if(GetAnimation())
			bCanClose = GetAnimation()->CanClose();

        if(bCanClose){
			// Should delete earlier animation here

            // Create new animation
			Animation *aNewAnimation = Animation::Create(Options.Res,Options.Frames);
            // If succeeded, set new animation
			if(aNewAnimation)
				SetAnimation(aNewAnimation,TRUE);
			};
        };
};

void TAnimationWnd::CMOpen(RTMessage){Load(NULL);};

void TAnimationWnd::CMAnimInfo(RTMessage)
{
	if(GetAnimation())
		GetAnimation()->Information(this);
};

void TAnimationWnd::Load(LPSTR szFilename)
{
	// Check if the earlier animation can be closed
	BOOL bCanClose = TRUE;
	if(GetAnimation())
		bCanClose = GetAnimation()->CanClose();

	if(bCanClose){
		// Should delete earlier animation here

		// Load new animation
		Animation* aNewAnim = Animation::Create(szFilename);

		// If succeeded, set new animation
		if(aNewAnim)				
			SetAnimation(aNewAnim,TRUE);
        };
};

void TAnimationWnd::CMSave(RTMessage)
{
    // Check that animation exists
	if(GetAnimation())
		GetAnimation()->Save();
};

void TAnimationWnd::CMSaveAs(RTMessage)
{
	// Check that animation exists
	if(GetAnimation())
		GetAnimation()->SaveAs();
};

POINT TAnimationWnd::GetResolution()
{
	POINT pRes = {0,0};

	if(GetAnimation())
		return GetAnimation()->GetResolution();
	else
		return pRes;
};

void TAnimationWnd::SetAnimation(Animation* NewAnim,BOOL owns_animation)
{
    // Delete earlier animation
    Delete_Animation();

    // Set new animation
	animation = NewAnim;
	bOwnsAnimation = owns_animation;

    // Create viewmaps
	if(animation){
		Viewmaps = new TViewmap *[animation->GetFrames()];
		for(int i = 0 ; i < animation->GetFrames() ; i++ )
			Viewmaps[i]=NULL;
		};

    // Inform that number of frames have been changed
	FramesChanged();

    if(HWindow){
		InvalidateRect(HWindow,NULL,FALSE);
		PostMessage(Parent->HWindow,WM_QUERYNEWPALETTE,0,0L);
	};
};

int  TAnimationWnd::GetNewFrames(void)
{
	// Set number of frames to same as animations frames
    if(animation)
		return animation->GetFrames();
       else
		return TMorphPictureWnd::GetNewFrames();
};

void TAnimationWnd::SetViewFrame(int nNewFrame)
{
	TMorphPictureWnd::SetViewFrame(nNewFrame);

    // Calculate animation frame
	if((GetFrames()==1)||(GetAnimation()==NULL))
		Anim_Frame = 0;
       else
		Anim_Frame = (int)(((long)ViewFrame*(animation->GetFrames()-1))
								/(GetFrames()-1));
};

TPicture* TAnimationWnd::GetBitmap(void)
{
    // Return picture if animation exists
	if(GetAnimation())
		return GetAnimation()->Get_Frame(Anim_Frame);
	   else
	   	return NULL;	
};

void TAnimationWnd::SetBitmap(TPicture* DibNew,BOOL owns_dib,TViewmap* viewmap,BOOL Clear)
{
    // Animation should own it's pictures
	if(owns_dib==FALSE){
		assert(FALSE);
		return;
        }

    // If there's no animation yet, create one with only one frame
	if(GetAnimation()==NULL){
		SetAnimation(new Animation(DibNew->GetResolution(),1),TRUE);
	   	};

	// Set picture to animation
    GetAnimation()->Set_Frame(DibNew,Anim_Frame);
    // Set frames viewmap
	SetViewmap(viewmap);

	bClear = Clear;
    // Draw window again
	InvalidateRect(HWindow, NULL, FALSE);
};


/////////////////////////
// Viewmaps
TViewmap* TAnimationWnd::FetchViewmap()
{
	return FetchViewmap(Anim_Frame);
};

TViewmap *TAnimationWnd::GetViewmap(void){ return Viewmaps[Anim_Frame];};

void TAnimationWnd::SetViewmap(TViewmap* NewMap)
{
    SetViewmap(Anim_Frame,NewMap);
};
 
// Viewmap functions for animation window
TViewmap* TAnimationWnd::FetchViewmap(int nFrame)
{
    // Get viewmap
	TViewmap* vmValue = Viewmaps[nFrame];

    // Erase it from animation window
	Viewmaps[nFrame] = NULL;

    // and return the viewmap
    return vmValue;
};

void TAnimationWnd::SetViewmap(int nFrame, TViewmap* NewMap)
{
    // Delete earlier viewmap
	DeleteViewmap();

    // Set new viewmap
    Viewmaps[nFrame] = NewMap;
};
 

/* End of file ANIMWND.CPP */ 
 

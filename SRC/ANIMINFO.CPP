/**********************************************************************
 * 
 *  NAME:         ANIMINFO.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  		18.10.96 MHM created
 * 
 *********************************************************************/ 
 
#include "animinfo.h"
#include "animation.h"

// Class for getting an integer number
TAnimInfoDlg::TAnimInfoDlg(PTWindowsObject Parent,LPSTR Caption,Animation* pNewAnim)
	: TDialog(Parent,"AnimInfoDlg")
{
    pAnim 	= pNewAnim;
	pFileName = new TStatic(this,ID_ANIM_FILENAME,80);
	pResX 	= new TStatic(this,ID_ANIM_RES_X,10);
	pResY 	= new TStatic(this,ID_ANIM_RES_Y,10);
	pFrames = new TStatic(this,ID_ANIM_FRAMES,10);
};

void TAnimInfoDlg::SetupWindow()
{
	TDialog::SetupWindow();
    // Initialize input window
    POINT pRes = pAnim->GetResolution();
	SetDlgItemText(HWindow,ID_ANIM_FILENAME,pAnim->GetName());
	SetDlgItemInt (HWindow,ID_ANIM_RES_X,	pRes.x,TRUE);
	SetDlgItemInt (HWindow,ID_ANIM_RES_Y,	pRes.y,TRUE);
	SetDlgItemInt (HWindow,ID_ANIM_FRAMES,	pAnim->GetFrames(),TRUE);
};

/* End of file ANIMINFO.CPP */ 
 

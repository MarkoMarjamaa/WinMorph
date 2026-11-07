/**********************************************************************
 * 
 *  NAME:         ANIMNDLG.CPP 
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
 
#include "animndlg.h"

TAnimationNewDlg::TAnimationNewDlg(PTWindowsObject Parent,TAnimationOpt* value)
	: TDialog(Parent,"AnimationNewDlg"),Opt(*value)
{
	X		= new TEdit(this,ID_ANIM_NEW_X,8);
	Y		= new TEdit(this,ID_ANIM_NEW_Y,8);
	Frames	= new TEdit(this,ID_ANIM_NEW_FRAMES,8);
};

TAnimationNewDlg::~TAnimationNewDlg()
{
	delete X;
	delete Y;
	delete Frames;
};

void TAnimationNewDlg::SetupWindow()
{
	TDialog::SetupWindow();

	SetDlgItemInt(HWindow,ID_ANIM_NEW_X		,Opt.Res.x,FALSE);
	SetDlgItemInt(HWindow,ID_ANIM_NEW_Y		,Opt.Res.y,FALSE);
	SetDlgItemInt(HWindow,ID_ANIM_NEW_FRAMES,Opt.Frames,FALSE);
};

int TAnimationNewDlg::nGetValue(int Id,int nValue)
{
	int nNewValue;
	BOOL bTranslated;

	nNewValue = GetDlgItemInt(HWindow,Id,(BOOL FAR*)&bTranslated,FALSE);
	if(bTranslated==TRUE)
		nValue = nNewValue;
    return nValue;
};

BOOL TAnimationNewDlg::CanClose()
{
	Opt.Res.x	= nGetValue(ID_ANIM_NEW_X		,Opt.Res.x);
	Opt.Res.y	= nGetValue(ID_ANIM_NEW_Y		,Opt.Res.y);
	Opt.Frames	= nGetValue(ID_ANIM_NEW_FRAMES	,Opt.Frames);

    return TRUE;
};
 
 
/* End of file ANIMNDLG.CPP */ 
 

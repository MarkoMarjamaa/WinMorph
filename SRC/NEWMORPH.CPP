/**********************************************************************
 * 
 *  NAME:         NEWMORPH.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  1.9470  10.12.95 MHM renamed from morfstrw.cpp
 * 
 *********************************************************************/ 
 
#include "newmorph.h"


TNewMorphWnd::TNewMorphWnd(PTWindowsObject Parent,TNewMorphOpt* value,LPSTR szNewTitle)
	: TDialog(Parent,"NewMorphWindow"),
		Opt(*value)
{
	 // Set title
	SetCaption(szNewTitle);

	 // Create frames edit windows 
	wFrames[0]  = new TEdit(this,ID_NEWMORPH_START,8);
	wFrames[1]  = new TEdit(this,ID_NEWMORPH_RESULT,8);
	wFrames[2]  = new TEdit(this,ID_NEWMORPH_END,8);

	 // Create Animation Frames windows
	wAnimFrames[0] = new TStatic(this,ID_NEWMORPH_A_START,8);
	wAnimFrames[1] = new TStatic(this,ID_NEWMORPH_A_RESULT,8);
	wAnimFrames[2] = new TStatic(this,ID_NEWMORPH_A_END,8);
};

TNewMorphWnd::~TNewMorphWnd()
{
	 // Delete Frames edit windows
	 for(int i = 0 ; i < 3 ; i++)
		delete wFrames[i];

	 // Delete Animation Frames windows
	for( i = 0 ; i < 3 ; i++)
		delete wAnimFrames[i];
};

void TNewMorphWnd::SetupWindow()
{
	TDialog::SetupWindow();

	 // Set default values
	SetDlgItemInt(HWindow,ID_NEWMORPH_START   ,Opt.nFrames[0],FALSE);
	SetDlgItemInt(HWindow,ID_NEWMORPH_RESULT  ,Opt.nFrames[1],FALSE);
	SetDlgItemInt(HWindow,ID_NEWMORPH_END     ,Opt.nFrames[2],FALSE);

	SetDlgItemInt(HWindow,ID_NEWMORPH_A_START ,Opt.nAnimFrames[0],FALSE);
	SetDlgItemInt(HWindow,ID_NEWMORPH_A_RESULT,Opt.nAnimFrames[1],FALSE);
	SetDlgItemInt(HWindow,ID_NEWMORPH_A_END   ,Opt.nAnimFrames[2],FALSE);
};

// Get integer value from edit window 
int TNewMorphWnd::nGetValue(int Id,int nValue)
{
	int nNewValue;
	BOOL bTranslated;

	 // Get value from control 
	nNewValue = GetDlgItemInt(HWindow,Id,(BOOL FAR*)&bTranslated,FALSE);
	if(bTranslated==TRUE)
		nValue = nNewValue;
	 return nValue;
};

BOOL TNewMorphWnd::CanClose()
{
	// Get values from edit windows
	Opt.nFrames[0] = nGetValue(ID_NEWMORPH_START ,Opt.nFrames[0]);
	Opt.nFrames[1] = nGetValue(ID_NEWMORPH_RESULT   ,Opt.nFrames[1]);
	Opt.nFrames[2] = nGetValue(ID_NEWMORPH_END      ,Opt.nFrames[2]);

	 return TRUE;
};

/* End of file NEWMORPH.CPP */ 
 

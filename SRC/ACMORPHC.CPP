/**********************************************************************
 * 
 *  NAME:         ACMORPHC.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          22.01.96 MHM created 
 *
 *********************************************************************/

#include "acmorphc.h"
#include "morpher.h"

ACMorphContextCfg::ACMorphContextCfg()
{
	nMorphMode = MORPH;
	nAntiAliasing = 1;
};

ACMorphConfigWnd::ACMorphConfigWnd(PTWindowsObject Parent,ACMorphContextCfg *NewCfg)
		: TDialog(Parent,"ACMorphContextCfgDlg")
{
	Cfg = NewCfg;

	cbPlayMode = new TComboBox(this,ID_PLAYMODE,8);
	ePlayDelay = new TEdit(this,ID_PLAYDELAY,8);
	cbMorphMode = new TComboBox(this,ID_MORPHMODE,8);
    cbAntiAliasing = new TComboBox(this,ID_ANTIALIASING,8);
};

ACMorphConfigWnd::~ACMorphConfigWnd()
{
	delete ePlayDelay;
	delete cbPlayMode;
	delete cbMorphMode;
	delete cbAntiAliasing;
};

void ACMorphConfigWnd::SetupWindow(void)
{
	TDialog::SetupWindow();

	cbPlayMode->AddString("Normal");
	cbPlayMode->AddString("Loop");
	cbPlayMode->AddString("Ping-pong");
	cbPlayMode->SetSelIndex(Cfg->nPlayMode);

	cbMorphMode->AddString("Morph");
	cbMorphMode->AddString("Warp Start");
	cbMorphMode->AddString("Warp End");
	cbMorphMode->AddString("Average");
	cbMorphMode->AddString("Delta");
	cbMorphMode->AddString("Multiply");
	cbMorphMode->SetSelIndex(Cfg->nMorphMode);

	cbAntiAliasing->AddString(" 1 sample");
	cbAntiAliasing->AddString(" 4 samples");
	cbAntiAliasing->AddString("16 samples");
	switch(Cfg->nAntiAliasing){
		case  1:cbAntiAliasing->SetSelIndex(0);break;
		case  4:cbAntiAliasing->SetSelIndex(1);break;
		case 16:cbAntiAliasing->SetSelIndex(2);break;
	};

	SetDlgItemInt(HWindow,ID_PLAYDELAY,Cfg->nPlayDelay,FALSE);
};

int ACMorphConfigWnd::GetDlgItemInt(int Id,int nDefault)
{
	int nReturnValue;
	BOOL fTranslated;

	nReturnValue = ::GetDlgItemInt(HWindow,Id,(BOOL FAR*)&fTranslated,FALSE);

	if(fTranslated==FALSE)
		nReturnValue = nDefault;

	return nReturnValue;
};

BOOL ACMorphConfigWnd::CanClose(void)
{
	Cfg->nPlayMode = cbPlayMode ->GetSelIndex();
	Cfg->nMorphMode= cbMorphMode->GetSelIndex();

	switch(cbAntiAliasing->GetSelIndex()){
		case 0:Cfg->nAntiAliasing =  1;break;
		case 1:Cfg->nAntiAliasing =  4;break;
		case 2:Cfg->nAntiAliasing = 16;break;
    };

	Cfg->nPlayDelay = GetDlgItemInt(ID_PLAYDELAY,Cfg->nPlayDelay);

	return TRUE;
};

/* End of file ACMORPHC.CPP */

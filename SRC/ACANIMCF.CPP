/**********************************************************************
 * 
 *  NAME:         ACANIMCF.CPP 
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

#include "acanimcf.h"

ACAnimContextCfg::ACAnimContextCfg()
{
	nPlayDelay = 200;
	nPlayMode = PINGPONG;
};

ACAnimConfigWnd::ACAnimConfigWnd(PTWindowsObject Parent,ACAnimContextCfg *NewCfg)
		: TDialog(Parent,"ACAnimContextCfgDlg")
{
	Cfg = NewCfg;

	cbPlayMode = new TComboBox(this,ID_PLAYMODE,8);

	ePlayDelay = new TEdit(this,ID_PLAYDELAY,8);
};

ACAnimConfigWnd::~ACAnimConfigWnd()
{
	delete ePlayDelay;

	delete cbPlayMode;
};

void ACAnimConfigWnd::SetupWindow(void)
{
	TDialog::SetupWindow();

	cbPlayMode->AddString("Normal");
	cbPlayMode->AddString("Loop");
	cbPlayMode->AddString("Ping-pong");
	cbPlayMode->SetSelIndex(Cfg->nPlayMode);

	SetDlgItemInt(HWindow,ID_PLAYDELAY,Cfg->nPlayDelay,FALSE);
};

int ACAnimConfigWnd::GetDlgItemInt(int Id,int nDefault)
{
	int nReturnValue;
	BOOL fTranslated;

	nReturnValue = ::GetDlgItemInt(HWindow,Id,(BOOL FAR*)&fTranslated,FALSE);

	if(fTranslated==FALSE)
		nReturnValue = nDefault;

	return nReturnValue;
};

BOOL ACAnimConfigWnd::CanClose(void)
{
	Cfg->nPlayMode = cbPlayMode->GetSelIndex();

	Cfg->nPlayDelay = GetDlgItemInt(ID_PLAYDELAY,Cfg->nPlayDelay);

	return TRUE;
};

/* End of file ACANIMCF.CPP */

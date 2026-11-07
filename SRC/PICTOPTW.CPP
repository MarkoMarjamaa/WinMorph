/**********************************************************************
 * 
 *  NAME:         PICTOPTW.CPP 
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
 
#include "pictoptw.h"

//
// Class for changing the attributes of picturewindow
//

TPictureOptionsWnd::TPictureOptionsWnd(PTWindowsObject Parent,TPic_Opt* NewOptions)
	: TDialog(Parent,"PictureOptionsDlg")
{
	Opt = NewOptions;

	X = new TEdit(this,ID_ZOOM_X,8);
	Y = new TEdit(this,ID_ZOOM_Y,8);

	Stretch		= new TCheckBox(this,ID_STRETCH,NULL);
	AutoUpdate	= new TCheckBox(this,ID_AUTOUPDATE,NULL);

    Viewmaps = new TComboBox(this,ID_VIEWMAPS,8);
};

TPictureOptionsWnd::~TPictureOptionsWnd()
{
	delete X;
	delete Y;
	delete Stretch;
	delete AutoUpdate;
    delete Viewmaps;
};

void TPictureOptionsWnd::SetupWindow()
{
	TDialog::SetupWindow();

	SetDlgItemInt(HWindow,ID_ZOOM_X,Opt->ZoomRes.x,FALSE);
	SetDlgItemInt(HWindow,ID_ZOOM_Y,Opt->ZoomRes.y,FALSE);

	if(Opt->bStretchDibToWindow==TRUE)
		Stretch->Check();
	   else
	    Stretch->Uncheck();

	if(Opt->bAutoUpdateViewmaps==TRUE)
		AutoUpdate->Check();
	   else
	    AutoUpdate->Uncheck();

	for(int i = 0 ; Opt->ViewmapNames[i] ; i++ )
		Viewmaps->AddString(Opt->ViewmapNames[i]);
	Viewmaps->SetSelIndex(Opt->ViewmapType);
};

void TPictureOptionsWnd::CMNoZooming(RTMessage)
{
	Opt->ZoomRes = Opt->Resolution;

	SetDlgItemInt(HWindow,ID_ZOOM_X,Opt->ZoomRes.x,FALSE);
	SetDlgItemInt(HWindow,ID_ZOOM_Y,Opt->ZoomRes.y,FALSE);
};

int TPictureOptionsWnd::GetDlgItemInt(int Id,int nDefault)
{
	int nReturnValue;
	BOOL fTranslated;

	nReturnValue = ::GetDlgItemInt(HWindow,Id,(BOOL FAR*)&fTranslated,FALSE);

	if(fTranslated==FALSE)
		nReturnValue = nDefault;

	return nReturnValue;
};

BOOL TPictureOptionsWnd::CanClose()
{ 
	Opt->bStretchDibToWindow = (Stretch->GetCheck()==BF_CHECKED);
	Opt->bAutoUpdateViewmaps = (AutoUpdate->GetCheck()==BF_CHECKED);

    Opt->ViewmapType = Viewmaps->GetSelIndex();

	Opt->ZoomRes.x = GetDlgItemInt(ID_ZOOM_X,Opt->ZoomRes.x);
	Opt->ZoomRes.y = GetDlgItemInt(ID_ZOOM_Y,Opt->ZoomRes.y);

	return TRUE;
};
 
 
/* End of file PICTOPTW.CPP */ 
 

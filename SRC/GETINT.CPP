/**********************************************************************
 * 
 *  NAME:         GETINT.CPP 
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
 
#include "getint.h"

// Class for getting an integer number
GetInteger::GetInteger(PTWindowsObject Parent,LPSTR Caption,int* nValue)
	: TDialog(Parent,"GetInteger")
{
    SetCaption(Caption);
	Ed = new TEdit(this,ID_GETINTEGER,8);
	returnvalue = nValue;
};

void GetInteger::SetupWindow()
{
	TDialog::SetupWindow();
    // Initialize input window
	SetDlgItemInt(HWindow,ID_GETINTEGER,(*returnvalue),FALSE);
};

BOOL GetInteger::CanClose()
{ 
	int nNewValue;
	BOOL bTranslated;

	nNewValue = GetDlgItemInt(HWindow,ID_GETINTEGER,(BOOL FAR*)&bTranslated,FALSE);
	if(bTranslated==TRUE)
		(*returnvalue) = nNewValue;
    return TRUE;
};

// Class for getting a point
GetPoint::GetPoint(PTWindowsObject Parent,LPSTR Caption,POINT* value)
	: TDialog(Parent,"GetPoint")
{
    SetCaption(Caption);
	X = new TEdit(this,ID_GETPOINT_X,8);
    Y = new TEdit(this,ID_GETPOINT_Y,8);
	returnvalue = value;
};

void GetPoint::SetupWindow()
{
	TDialog::SetupWindow();

    SetDlgItemInt(HWindow,ID_GETPOINT_X,(*returnvalue).x,FALSE);
    SetDlgItemInt(HWindow,ID_GETPOINT_Y,(*returnvalue).y,FALSE);
};

BOOL GetPoint::CanClose()
{ 
	int nNewXValue,nNewYValue;
	BOOL bXTrans,bYTrans;

	nNewXValue = GetDlgItemInt(HWindow,ID_GETPOINT_X,(BOOL FAR*)&bXTrans,FALSE);
	nNewYValue = GetDlgItemInt(HWindow,ID_GETPOINT_Y,(BOOL FAR*)&bYTrans,FALSE);

	if((bXTrans==TRUE)&&(bYTrans==TRUE)){
		(*returnvalue).x =nNewXValue;
		(*returnvalue).y =nNewYValue;
		}
	return TRUE;
};

/* End of file GETINT.CPP */ 
 

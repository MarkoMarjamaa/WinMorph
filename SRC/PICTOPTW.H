/********************************************************************** 
 * 
 *  NAME:         PIC_OPT.H 
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
 
#ifndef PICOPT_H
#define PICOPT_H

#ifndef __OWL_H
	#include <owl.h>
#endif
#include <edit.h>
#include <checkbox.h>
#include <combobox.h>

#include "winmorph.rch"

class TPic_Opt
{
public:
	POINT ZoomRes;
    POINT Resolution;
	BOOL bStretchDibToWindow;
	BOOL bAutoUpdateViewmaps;
	LPSTR *ViewmapNames;
	int ViewmapType;
};

class TPictureOptionsWnd : public TDialog
{
	TPic_Opt* Opt;
	TEdit* X;
	TEdit* Y;
	TCheckBox* Stretch;
    TCheckBox* AutoUpdate;
	TComboBox* Viewmaps;
public :
	TPictureOptionsWnd(PTWindowsObject AParent,TPic_Opt*);
    ~TPictureOptionsWnd();
	virtual void SetupWindow(void);
	virtual void CMNoZooming(RTMessage)=[ID_FIRST + CM_NO_ZOOMING];
	BOOL CanClose(void);

	int GetDlgItemInt(int Id,int nDefault);
};

#endif
 
 
/* End of file PIC_OPT.H */ 
 

/********************************************************************** 
 * 
 *  NAME:         ANIMNDLG.H 
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
 
#ifndef ANIMNDLG_H
#define ANIMNDLG_H

#ifndef __OWL_H
	#include <owl.h>
#endif
#include <edit.h>

#include "winmorph.rch"

class TAnimationOpt
{
public:
	POINT	Res;
    int		Frames;
};

class TAnimationNewDlg : public TDialog
{
	TAnimationOpt& Opt;

	TEdit* X;
	TEdit* Y;
	TEdit* Frames;

	int nGetValue(int Id,int value);
public:
	TAnimationNewDlg(PTWindowsObject Parent,TAnimationOpt* value);
	~TAnimationNewDlg();

	virtual void SetupWindow(void);
	BOOL CanClose(void);
};


#endif

/* End of file ANIMNDLG.H */
 

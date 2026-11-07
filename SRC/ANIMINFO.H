/********************************************************************** 
 * 
 *  NAME:         ANIMINFO.H 
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

#ifndef ANIMINFO_H
#define ANIMINFO_H

#ifndef __OWL_H
	#include <owl.h>
#endif
#include <static.h>

#include "winmorph.rch"	// ID-numbers for controls

class Animation;


class TAnimInfoDlg : public TDialog
{
	Animation *pAnim;

	TStatic *pFileName;
	TStatic *pResX;
	TStatic *pResY;
	TStatic *pFrames;
public:
	TAnimInfoDlg(PTWindowsObject Parent,LPSTR Caption,Animation *pNewAnim);
	virtual void SetupWindow();
};

#endif
 
 
/* End of file ANIMINFO.H */ 
 

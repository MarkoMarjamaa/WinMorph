/********************************************************************** 
 * 
 *  NAME:         NEWMORPH.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  1.9470  10.12.95 MHM renamed from morfstrw.h
 * 
 *********************************************************************/ 
 
#ifndef NEWMORPH_H
#define NEWMORPH_H

#ifndef __OWL_H
	#include <owl.h>
#endif
#include <edit.h>
#include <static.h>

#include "winmorph.rch"

class TNewMorphOpt
{
public:
	int nFrames[3];
	int nAnimFrames[3];
};

class TNewMorphWnd : public TDialog
{
	TNewMorphOpt& Opt;

	TEdit *wFrames[3];
	TStatic *wAnimFrames[3];

	int nGetValue(int Id,int value);
public:

	TNewMorphWnd(PTWindowsObject Parent,TNewMorphOpt* value,LPSTR szNewTitle);
	~TNewMorphWnd();

	virtual void SetupWindow(void);
	BOOL CanClose(void);
};

#endif
 
 
/* End of file NEWMORPH.H */
 

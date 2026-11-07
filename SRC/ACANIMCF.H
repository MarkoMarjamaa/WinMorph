/**********************************************************************
 * 
 *  NAME:         ACANIMCF.H 
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

#ifndef ACANIMCF_H
#define ACANIMCF_H

#ifndef __OWL_H
	#include <owl.h>
#endif
#include <edit.h>
#include <combobox.h>

#include "winmorph.rch"

#include "accontcf.h"

class ACAnimContextCfg : public ACContextCfg
{
public:
	int 	nPlayDelay;

	ACAnimContextCfg();
};

class ACAnimConfigWnd : public TDialog
{
	ACAnimContextCfg *Cfg;

	TComboBox* cbPlayMode;
	TEdit* ePlayDelay;

	int GetDlgItemInt(int Id,int nDefault);
public:

	ACAnimConfigWnd(PTWindowsObject Parent,ACAnimContextCfg *);
	~ACAnimConfigWnd();

	virtual void SetupWindow(void);
	BOOL CanClose(void);
};

#endif

/* End of file ACANIMCF.H */

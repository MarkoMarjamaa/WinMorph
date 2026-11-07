/**********************************************************************
 * 
 *  NAME:         ACMORPHC.H 
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

#ifndef ACMORPHC_H
#define ACMORPHC_H

#ifndef __OWL_H
	#include <owl.h>
#endif
#include <edit.h>
#include <combobox.h>

#include "winmorph.rch"

#include "acanimcf.h"

class ACMorphContextCfg : public ACAnimContextCfg
{
public:
	int nMorphMode;
	int nAntiAliasing;

	ACMorphContextCfg();
};

class ACMorphConfigWnd : public TDialog
{
	ACMorphContextCfg *Cfg;

	TComboBox* cbPlayMode;
	TEdit* ePlayDelay;
	TComboBox* cbMorphMode;
	TComboBox* cbAntiAliasing;

	int GetDlgItemInt(int Id,int nDefault);
public:

	ACMorphConfigWnd(PTWindowsObject Parent,ACMorphContextCfg *);
	~ACMorphConfigWnd();

	virtual void SetupWindow(void);
	BOOL CanClose(void);
};

#endif

/* End of file ACMORPHC.H */

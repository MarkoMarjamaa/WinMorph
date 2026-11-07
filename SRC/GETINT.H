/********************************************************************** 
 * 
 *  NAME:         GETINT.H 
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

#ifndef GETINT_H
#define GETINT_H

#ifndef __OWL_H
	#include <owl.h>
#endif
#include <edit.h> // class TEdit

#include "winmorph.rch"	// ID-numbers for controls

class GetInteger : public TDialog
{
	int *returnvalue;
    TEdit *Ed;
public:
	GetInteger(PTWindowsObject Parent,LPSTR Caption,int* value);
	virtual void SetupWindow();
	BOOL CanClose(void);
};

class GetPoint : public TDialog
{
	POINT *returnvalue;
	TEdit *X;
	TEdit *Y;
public:
	GetPoint(PTWindowsObject Parent,LPSTR Caption,POINT *value); 
	virtual void SetupWindow();
	BOOL CanClose(void);
};

#endif
 
 
/* End of file GETINT.H */ 
 

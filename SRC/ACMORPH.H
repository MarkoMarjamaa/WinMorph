/**********************************************************************
 * 
 *  NAME:         ACMORPH.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          13.01.96 MHM created
 * 
 *********************************************************************/ 
 
#ifndef ACMORPH_H
#define ACMORPH_H

#include "acanim.h"
#include "acanimcf.h"

class TMorphWnd;		// morphwnd.h
class ACAnimContext;
class TMorpher_Interface;

class ACMorphContext : public ACAnimContext
{
	TMorpher_Interface *Morpher;

	int nMorphMode;
    int nAntiAliasing;
public:
	ACMorphContext(void);
	~ACMorphContext();

	int nMorphing;

	virtual void Config(void);

	virtual void Morph(void);
	virtual void SetPause(BOOL fNewPauseState);
	virtual void Stop(void);
	virtual LPSTR GetStateName(void);

	virtual BOOL GetNextFrame(void);
	virtual void SetFrame(void);
	virtual BOOL CanMorph(void);
};

#endif
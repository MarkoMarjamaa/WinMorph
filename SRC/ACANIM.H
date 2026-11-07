/**********************************************************************
 * 
 *  NAME:         ACANIM.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          14.12.95 MHM added headers 
 * 
 *********************************************************************/ 
 
#ifndef ACANIM_H
#define ACANIM_H

#include "accontex.h"


class ACAnimContext : public ACContext
{
protected:
    int nPlayDelay;

	class TMorphPictureWnd *Parent;

	virtual void Config(void);

	virtual void SetFrame		(void);
public:

	ACAnimContext();

	void SetParent(TMorphPictureWnd *NewParent);
	TMorphPictureWnd *GetParent(void)	{return Parent;};

	virtual void Play	(void);
	virtual void Stop	(void);
	virtual void SetPause(BOOL fNewPauseState);
};

#endif

/* End of file ACANIM.H */

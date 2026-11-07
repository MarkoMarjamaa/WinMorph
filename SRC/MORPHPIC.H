/********************************************************************** 
 * 
 *  NAME:         MORPHPIC.H 
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
 
#ifndef MORPHPIC_H
#define MORPHPIC_H

#include "picturew.h"

class Scanline_Screen;
class ACAnimContext;
class ACContext;

///////////////////////////////////////////////////////////////////////
//	TMorphPictureWnd
///////////////////////////////////////////////////////////////////////

#define LAST_FRAME 16384

class TMorphPictureWnd : public TPictureWindow
{
protected:
			int		ViewFrame;
            int		Frames;
	ACAnimContext  *AC;

	virtual int GetNewFrames	(void);

public:
	TMorphPictureWnd(PTWindowsObject AParent,LPSTR szName,ACAnimContext*);
	~TMorphPictureWnd();

	virtual void  SetupWindow	(void);

	virtual	BOOL CanMorph(void);
			int	  GetFrames		(void);
	virtual void  ActivationResponse(WORD Activated,  BOOL IsIconified);
	virtual void UpdateTitle	(void);
	virtual void  FramesChanged(void);
    virtual ACContext *GetACContext(void);
	virtual void  SetViewFrame	(int nFrame);
			int   GetViewFrame	(void){return ViewFrame;};

    virtual void  TimerTick		(RTMessage) = [WM_FIRST + WM_TIMER];
	void	InvalidateScanline	(Scanline_Screen* Area);
};

#endif 
 
/* End of file MORPHPIC.H */ 


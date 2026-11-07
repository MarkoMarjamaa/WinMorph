/********************************************************************** 
 * 
 *  NAME:         ACWINDOW.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          16.12.95 MHM renamed Toolbox to ACWindow 
 *********************************************************************/
 
#ifndef ACWINDOW_H
#define ACWINDOW_H

#ifndef __OWL_H
	#include <owl.h>
#endif
#include <scrollba.h>

#include "winmorph.rch"

class ACContext;
class ACMorphContext;

_CLASSDEF(TStatic)
_CLASSDEF(TODButton)

class TACScrollBar : public TScrollBar
{
public:
	TACScrollBar(PTWindowsObject AParent, int AnId,
		int X, int Y, int W, int H, BOOL IsHScrollBar, PTModule AModule = NULL);

	virtual void SBLineUp(RTMessage Msg);
	virtual void SBLineDown(RTMessage Msg);
	virtual void SBPageUp(RTMessage Msg);
	virtual void SBPageDown(RTMessage Msg);
	virtual void SBThumbPosition(RTMessage Msg);
	virtual void SBThumbTrack(RTMessage Msg);
	virtual void SBTop(RTMessage Msg);
	virtual void SBBottom(RTMessage Msg);
};

class TACWindow : public TWindow
{
	ACMorphContext *AC;

    HMENU	   hACMenu;

	TScrollBar	*sbFrames;
	TODButton   *btnPlay;

	RECT		rScrollBar;
	RECT		rMarkBar;
	int			nScrollButtonWidth;
	int			nScrollThumbWidth;

	void EnableControl(int nControl,BOOL fState);

public :
	TACWindow(PTWindowsObject AParent,LPSTR szTitle,HMENU hNewMenu);

	virtual LPSTR GetClassName();
	virtual void GetWindowClass( WNDCLASS& WndClass );
	virtual void DefCommandProc(RTMessage);
	virtual void SetupWindow(void);

	virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR &ps);

	virtual void SetACContext(ACMorphContext *NewAC);
	virtual void UpdateContext(void);
	virtual void SetPlayPause(void);
	virtual void UpdateFrame(void);
	virtual void UpdateFrameRatio(void);
	virtual void UpdatePlayState(void);
	virtual void UpdateFrameChangers(void);

	virtual void Cm_ACPlay		(RTMessage) = [ID_FIRST + CM_ACPLAY];
	virtual void Cm_ACStop		(RTMessage) = [ID_FIRST + CM_ACSTOP];
    virtual void CM_Config		(RTMessage) = [ID_FIRST + CM_ACCONFIG];

	virtual void Cm_ACPrevMark	(RTMessage) = [ID_FIRST + CM_ACPREVMARK];
	virtual void Cm_ACNextMark	(RTMessage) = [ID_FIRST + CM_ACNEXTMARK];

	virtual void Cm_ACMarkIn	(RTMessage) = [ID_FIRST + CM_ACMARKIN];
	virtual void Cm_ACMarkOut	(RTMessage) = [ID_FIRST + CM_ACMARKOUT];

	virtual void Cm_ACFirstFrame(RTMessage) = [CM_FIRST + CM_ACFIRSTFRAME];
	virtual void Cm_ACLastFrame	(RTMessage) = [CM_FIRST + CM_ACLASTFRAME];

	virtual void Cm_ACMorph		(RTMessage) = [ID_FIRST + CM_ACMORPH];

	virtual void SetFrame(int nNewFrame);
};

#endif
 
 
/* End of file ACWINDOW.H */
 

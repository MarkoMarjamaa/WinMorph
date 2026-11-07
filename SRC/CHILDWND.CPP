/**********************************************************************
 * 
 *  NAME:         CHILDWND.H
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *
 *********************************************************************/ 
 
#ifndef CHILDWND_H
#define CHILDWND_H

#include <owl.h>


/////////////////////////////////////////////////////
// TChildWindow
////////////////////////////////////////////////////////


class TChildWindow : public TWindow
{
protected:
    int 	nWindowType;
	virtual HMENU GetMenu(void);
	void 		UpdateMenuList(BOOL fActivate, int nSubMenuPos, int nItemPos);

// Methods
public:
	TChildWindow(PTWindowsObject AParent,LPSTR Name);
	~TChildWindow();

	virtual void  GetWindowClass(WNDCLASS& wc);
	virtual LPSTR GetClassName();
	virtual void  ActivationResponse(WORD Activated,  BOOL IsIconified);
	virtual void  DefCommandProc(RTMessage Msg);
			void  SetMenuItem(int nMenuItem,BOOL bState);
	virtual void  UpdateMenu(BOOL fActivate);
};

#endif
 
/* End of file CHILDWND.H */
 

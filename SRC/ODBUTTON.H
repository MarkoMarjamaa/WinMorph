/**********************************************************************
 * 
 *  NAME:         ODBUTTON.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          29.02.96 MHM created
 *********************************************************************/

#ifndef ODBUTTON_H
#define ODBUTTON_H

#include <button.h>

#define ODBUTTON_NORMAL   0
#define ODBUTTON_PRESSED  1
#define ODBUTTON_FOCUSED  2
#define ODBUTTON_DISABLED 3

class TODButton : public TButton {
protected:
	HBITMAP bmBitmap[4];

	virtual void ODADrawEntire	(DRAWITEMSTRUCT far &DrawInfo);
	virtual void ODASelect		(DRAWITEMSTRUCT far &DrawInfo);
	virtual void ODAFocus		(DRAWITEMSTRUCT far &DrawInfo);

	void	DrawButton	(DRAWITEMSTRUCT far &DrawInfo);
	int		GetState	(DRAWITEMSTRUCT far &DrawInfo);

public:
	TODButton::TODButton(PTWindowsObject pParent, int nResourceId,
		LPSTR szTitle, int x, int y, int w, int h,BOOL IsDefault,
		LPSTR ResId1, LPSTR ResId2, LPSTR ResId3,LPSTR ResId4);
	~TODButton();

	void SetButton(int nButton,LPSTR ResId);
};


#endif

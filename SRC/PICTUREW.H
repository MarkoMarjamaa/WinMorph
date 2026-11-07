/********************************************************************** 
 * 
 *  NAME:         PICTUREW.H 
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
 
#ifndef PICTUREW_H
#define PICTUREW_H

#ifndef __OWL_H
	#include <owl.h>
#endif

#include "winmorph.rch" // menu codes

class TPicture;            // Tdib.h
class TViewmap;
class wmfstream;

/////////////////////////////////////////////////////
// TPictureWindow
////////////////////////////////////////////////////////


class TPictureWindow : public TWindow
{
protected:
	LPSTR		WindowTitle;

	BOOL		Owns_DIB;
	TPicture*   Dib;

	BOOL		bStretchDibToWindow;
	BOOL		bAutoUpdateViewmaps;

	TViewmap	*ViewMap;
	int			ViewMapType;
	POINT		ViewRes;
	float		fZoomFactor;
	BOOL		bClear;
	virtual HMENU GetMenu(void);
	void 		UpdateMenuList(BOOL fActivate, int nSubMenuPos, int nItemPos);

// Methods
public:
	TPictureWindow(PTWindowsObject AParent,LPSTR Name);
	~TPictureWindow();

	virtual void  GetWindowClass(WNDCLASS& wc);
	virtual LPSTR GetClassName();
	virtual void  ActivationResponse(WORD Activated,  BOOL IsIconified);
	virtual void  DefCommandProc(RTMessage Msg);
			void  SetMenuItem(int nMenuItem,BOOL bState);
	virtual void  UpdateMenu(BOOL fActivate);

	virtual void  LoadCfg(wmfstream& file);
	virtual int   LoadCfgKeyword(wmfstream& file,LPSTR szKeyWord);
	virtual void  SaveCfg(wmfstream& file);
	virtual void  SaveCfgKeyword(wmfstream& file);

	virtual void  Paint(HDC PaintDC, PAINTSTRUCT _FAR &ps);

	virtual void  WMGetMinMax	 	(RTMessage) = [WM_FIRST + WM_GETMINMAXINFO];
	virtual void  WMPaletteChanged	(RTMessage) = [WM_FIRST + WM_PALETTECHANGED];
	virtual void  WMQueryNewPalette	(RTMessage) = [WM_FIRST + WM_QUERYNEWPALETTE];
	virtual void  WMSize         	(RTMessage) = [WM_FIRST + WM_SIZE];
	virtual void  CMOptions		 	(RTMessage) = [CM_FIRST + CM_OPTIONS];
	virtual void  CMStretchToWindow	(RTMessage) = [CM_FIRST + CM_STRETCH_DIB];

	virtual POINT	GetResolution();
	virtual void	SetBitmap(TPicture* Dib,BOOL Owns_DIB,TViewmap* View=NULL,BOOL ClearViewmap=FALSE);
	virtual TPicture*	GetBitmap();
	virtual void 	SetStretchToWindow(BOOL State);

			POINT	GetViewRes();
	virtual void	SetViewmap(TViewmap*);
	virtual TViewmap*	GetViewmap();
	virtual TViewmap*	FetchViewmap();

			void	Invalidate(RECT*);
protected:
			void	DeleteBitmap();

	virtual void	CreateViewmap();
	virtual void	DeleteViewmap();

			void	AdjustScroller();
			POINT	GetViewSize();
	virtual void	SetViewRes(POINT NewRes);
	virtual POINT	CalculateFrameSize(void);
			void	UpdateViewmap();

			void	ChangeViewMapType(int nNewType);
			void	ChangeViewRes(POINT pNewRes,BOOL bNewStretch);
};


#endif
 
 
/* End of file PICTUREW.H */ 
 

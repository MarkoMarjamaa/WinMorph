/********************************************************************** 
 * 
 *  NAME:         ANIMWND.H 
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
 
#ifndef ANIMWND_H
#define ANIMWND_H

#include "morphpic.h"

class Animation;

class TAnimationWnd: public TMorphPictureWnd
{
protected:
	int			Anim_Frame;

	Animation	*animation;
	BOOL		bOwnsAnimation;

    TViewmap**	Viewmaps;

	virtual int	GetNewFrames	(void);

public:
	TAnimationWnd(PTWindowsObject AParent,LPSTR Name,ACAnimContext *);
	~TAnimationWnd();

	virtual void CMNew			(RTMessage) = [CM_FIRST + CM_ANIM_NEW];
	virtual void CMOpen			(RTMessage) = [CM_FIRST + CM_ANIM_OPEN];
	virtual void CMSave			(RTMessage) = [CM_FIRST + CM_ANIM_SAVE];
	virtual void CMSaveAs		(RTMessage) = [CM_FIRST + CM_ANIM_SAVEAS];
	virtual void CMAnimInfo		(RTMessage) = [CM_FIRST + CM_ANIM_INFO];

			void 	Load			(LPSTR szFilename);

	virtual void	SetViewFrame	(int nNewFrame);

	virtual POINT	GetResolution();
			Animation	*GetAnimation	(void);
	virtual	void		SetAnimation	(Animation* NewAnim,BOOL Owns_Animation);
	virtual void		SetBitmap		(TPicture* NewDib,BOOL Owns_Dib,TViewmap* viewmap=NULL,BOOL Clear=FALSE);
	virtual TPicture*	GetBitmap	(void);
	virtual TViewmap*	GetViewmap	(void);

	virtual TViewmap*	FetchViewmap(void);
	virtual void		SetViewmap	(TViewmap*);

	virtual TViewmap*	FetchViewmap(int nFrame);
	virtual void		SetViewmap	(int nFrame, TViewmap*);

protected:
			void		Delete_Animation();
};

#endif 
 
/* End of file ANIMWND.H */ 
 

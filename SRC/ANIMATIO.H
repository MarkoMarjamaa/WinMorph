/********************************************************************** 
 * 
 *  NAME:         ANIMATIO.H 
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
 
#ifndef ANIMATION_H
#define ANIMATION_H

#include "common.h"
#ifndef __WINDOWS_H
	#include <windows.h>
#endif

#include "document.h"
#include "filefunc.h"

/////////////////////////////////////////////////////////////////////
// Animation_Document
/////////////////////////////////////////////////////////////////////

class Animation_Document : public Document
{
public:
	Animation_Document(){};

	virtual LPSTR GetSavePrompt(void);
	virtual LPSTR GetSaveFileName(void);

	static LPSTR GetSaveFilter(void)
{return "Targa animation\0*.anm\0BMP animation\0*.anm\0\0";};
	static LPSTR GetSaveDefExt(void){return "tga";};

	static LPSTR GetOpenFilter(void)
{return "Animation files\0*.anm;*.bmp;*.tga\0Animation file\0*.anm\0BMP picture\0*.bmp\0TGA picture\0*.tga\0\0";};
	static LPSTR GetOpenDefExt(void){return "anm";};
};

////////////////////////////////////////////////////////////7
// Animation 
////////////////////////////////////////////////////////////7
class TPicture;
_CLASSDEF(TWindow)

class Animation : public Animation_Document
{
    BOOL	AllOK;
	int		Frames;
	POINT	Resolution;
	TPicture**	Frame;

	BOOL SaveAsSingleFile;
	int  Filetype;
public:
	Animation(POINT res,int frames);
    ~Animation();

	virtual LPSTR GetSaveFileName(void);

	static Animation *Create	(POINT res,int frames);
    static Animation *Create	(LPSTR filename);
	BOOL	IsOK				(void){ return AllOK; };
	int		GetFrames			(void);
    POINT	GetResolution		(void);

	void	Set_Frame			(TPicture* picture,int frame);
	TPicture*	Get_Frame			(int frame);

	virtual void Reset(void);

	virtual BOOL LoadDocument(wmfstream& file);
	virtual void SaveDocument(wmfstream& file);

	BOOL Identify(wmfstream& file);
	void Information(TWindow *pWnd);

protected:
	void Load_Frames(LPSTR szFramename);
	BOOL Load_AnimFile(wmfstream& file,LPSTR szFramepath);
	void SaveAnimationFile(wmfstream& file,LPSTR szFramePath);
};

#endif
 
 
/* End of file ANIMATIO.H */ 
 

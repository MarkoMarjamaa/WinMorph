/********************************************************************** 
 * 
 *  NAME:         PICFILE.H 
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
 
/* picfile.h
      Header file for interpreter between bitmaps and bitmapfiles
*/

#ifndef PICFILE_H
#define PICFILE_H

#ifndef __WINDOWS_H
	#include <windows.h>
#endif
#include "tpicture.h"

class TPicture;

class picfile{
protected:
	BITMAPINFO* GetDIBInfo(TPicture* Dib){ return Dib->Info;};
	BYTE huge*  GetDIBPicture(TPicture* Dib){return Dib->pPicture;};

	static  void Error(LPSTR szErrorMessage);
public:
	picfile()		{};
	virtual BOOL		save(LPSTR,TPicture*)=0;
	virtual TPicture*	load(LPSTR)=0;

	static	BOOL ErrorMessages;
	static	int nIdentifyPictureFile(LPSTR szFileName);
	static  TPicture* pLoadPictureFile(LPSTR szFilename,int Filetype);
	static	LPSTR		GetExtension(int nFiletype);
};

class BMPfile:public picfile{
	HGLOBAL			hLoadDib(int File);
public:
	BMPfile()	: picfile()		{};
	static  BOOL		Identify(int hFile);
	virtual BOOL		save(LPSTR,TPicture*);
	virtual TPicture*	load(LPSTR);
};

class TGAfile:public picfile{
	TPicture *pLoadType(int hFile);
	TPicture *pLoadTypeRGB(int hFile,struct TGAHEADER *pHeader);
public:
	TGAfile()	: picfile()		{};
	static  BOOL		Identify(int hFile);
	virtual BOOL		save(LPSTR,TPicture*);
	virtual TPicture*	load(LPSTR);
	static  LPSTR		GetExtension(){return "tga";};
};

// Supported filetypes
#define UNKNOWN 0
#define BMP 1
#define TGA 2

#endif
 
 
/* End of file PICFILE.H */ 


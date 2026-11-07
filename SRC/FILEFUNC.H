/********************************************************************** 
 * 
 *  NAME:         FILEFUNC.H 
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
 
#ifndef FILEFUNC_H
#define FILEFUNC_H

#include "common.h"
#ifndef __WINDOWS_H
	#include <windows.h>
#endif
#include <fstream.h>

#define UNDEFINED -1
#define ENDOFBLOCK -2
#define EMPTYLINE -3

// Stream class with error messages
class wmfstream: public fstream
{
public:
	char szFilename[_MAX_PATH];
	 BOOL bShowErrors;

	_Cdecl wmfstream(const signed char _FAR *szNewFilename, int nMode);

	void vErrorMessage(const signed char _FAR *szErrorMessage);
};


BOOL GetKeyword(wmfstream& file,LPSTR);
BOOL ReadKeyword(wmfstream& file,LPSTR dest,int destlength);

int IdentifyKeyword(LPSTR szKeywords[],LPSTR szWord);
int IdentifyKeyword(wmfstream& file,LPSTR szKeywords[]);
int IdentifyLabel(wmfstream& file,LPSTR szKeywords[]);

BOOL fIsWhiteSpace(BYTE bChar);
void vEatWhiteSpace(wmfstream& file);
BOOL GetCharacterIf(wmfstream& file,BYTE searched);
BOOL GetCharacter(wmfstream& file,BYTE searched);

BOOL GetStartOfBlock(wmfstream& file);
BOOL GetEndOfBlock(wmfstream& file);
BOOL GetComma(wmfstream& file);

BOOL vReadWord(wmfstream& file,LPSTR dest,int destlength);
BOOL vReadLine(wmfstream& file,LPSTR szLine,int nLineLength);

wmfstream& operator>>(wmfstream& file,POINT& p);
wmfstream& operator<<(wmfstream& file,POINT& p);

LPSTR szRelativePath(LPSTR szBasePath, LPSTR szNewPath);
LPSTR AddPath(LPSTR szBase,LPSTR szNewPath);

#endif
 
 
/* End of file FILEFUNC.H */ 
 

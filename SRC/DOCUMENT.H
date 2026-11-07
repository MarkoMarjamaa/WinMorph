/********************************************************************** 
 * 
 *  NAME:         DOCUMENT.H 
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
 
#ifndef DOCUMENT_H
#define DOCUMENT_H

///////////////////////////////////////////////////
// Document class
///////////////////////////////////////////////////
#ifndef __WINDOWS_H
	#include <windows.h>
#endif
#include <dir.h>
#include <fstream.h>
#include "filefunc.h"

enum FileMode { Read,Write };

class Document
{
protected:
	LPSTR	szFilename;
	int		SaveIndex;
	BOOL	bChanged;

public:
	Document(LPSTR szFilename);
	Document(void);
	~Document();

	virtual	LPSTR	GetName		(void){return szFilename;};

	virtual	BOOL	CanClose	(void);
	virtual	void	Rename		(LPSTR szNewFilename);
    virtual	void	Save		(void);
	virtual	void	SaveAs		(void);

	virtual void Reset (void) = 0;

	virtual void LoadFrom(LPSTR szFileName);
	virtual void SaveTo  (LPSTR szFilename);

	virtual BOOL LoadDocument	(wmfstream& file) = 0;
	virtual void SaveDocument   (wmfstream& file) = 0;

	virtual LPSTR	GetSaveFileName	(void);
	virtual LPSTR	GetSavePrompt	(void);

	virtual void    SetChanged	(BOOL);
	virtual BOOL    HasChanged  (void);

	static LPSTR GetFileName(LPSTR Filter,LPSTR DefExt,FileMode mode);
	static LPSTR GetSaveFilter(void){return "All files\0*.*\0\0";};
    static LPSTR GetSaveDefExt(void){return "*";};

	static LPSTR GetOpenFilter(void){return "All files\0*.*\0\0";};
    static LPSTR GetOpenDefExt(void){return "*";};
};

#endif
 
/* End of file DOCUMENT.H */
 

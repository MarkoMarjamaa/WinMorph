/**********************************************************************
 * 
 *  NAME:         DOCUMENT.CPP 
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
 
#include <commdlg.h>	// GetOpenFileName()
#include <ctype.h>		// tolower
#include <string.h>
#include "document.h"

Document::Document(LPSTR szNewFilename)
{
	szFilename = NULL;
	SetChanged(FALSE);
	SaveIndex = 0;

	Rename(szNewFilename);
};

Document::Document()
{
    szFilename = NULL;
	SetChanged(FALSE);
	SaveIndex = 0;
};

Document::~Document()
{
	if(szFilename)
    	delete szFilename;
};

BOOL Document::CanClose(void)
{
	if(!HasChanged())
		return TRUE;
	int UserReply = MessageBox(NULL,GetSavePrompt(),"",MB_YESNOCANCEL| MB_TASKMODAL);
	switch(UserReply){
		case IDYES:			Save();
							return !HasChanged();
		case IDNO:          SetChanged(FALSE);
							return TRUE;
		case IDCANCEL:		return FALSE;
		};
	return  TRUE;
};

void Document::Rename(LPSTR szNewFilename)
{
	if(szFilename==NULL)
    	szFilename = new char[MAXPATH];

	for(int nChar = 0 ; nChar < _fstrlen(szNewFilename)+1 ; nChar ++ )
    	szFilename[nChar] = tolower(szNewFilename[nChar]);
};

int SaveIndex = 0;

void Document::Save(void)
{
	if(szFilename==NULL)
		SaveAs();
       else{
		SaveTo(szFilename);
		SetChanged(FALSE);
		};
};

void Document::SaveAs(void)
{
	LPSTR szNewFilename = GetSaveFileName();
	if(szNewFilename){
	    Rename(szNewFilename);
        SaveIndex = ::SaveIndex;
		SaveTo(szFilename);
		SetChanged(FALSE);
		};
};

void Document::LoadFrom(LPSTR szFileName)
{
	// Set hourglass cursor
	HCURSOR hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));

	// Clear the object
	Reset();

	// Rename object
	Rename(szFileName);

	wmfstream file(szFileName,ios::in);
	if(file.good())
		LoadDocument(file);
	file.close();

	// Set normal cursor
	SetCursor(hcurSave);
};

void Document::SaveTo(LPSTR szFileName)
{
	// Set hourglass cursor
	HCURSOR hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));

	// Open file
	wmfstream file(szFileName,ios::out);
	if(file.good())
		SaveDocument(file);
	file.close();

	// Set normal cursor
	SetCursor(hcurSave);
};

void Document::Reset (void)
{};

void Document::SetChanged(BOOL bNewValue)
{
	bChanged = bNewValue;
};

BOOL Document::HasChanged(void)
{
	return bChanged;
};

LPSTR Document::GetSaveFileName(void)
{	return Document::GetFileName(GetSaveFilter(),GetSaveDefExt(),Write);};

LPSTR Document::GetSavePrompt(void)
{
	static char SavePrompt[MAXPATH +46];
    wsprintf(SavePrompt,"%s has been changed. Save file before closing?",szFilename);
	return SavePrompt;
};

LPSTR Document::GetFileName(LPSTR Filter,LPSTR DefExt,FileMode mode)
{
	static char szDirectory[MAXPATH];
	static char szFile[MAXPATH];

	char szFileTitle[MAXPATH];

    szDirectory[0]='\0';
	szFile[0] = '\0';				// Clear filename
  
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	 ofn.lStructSize     = sizeof(OPENFILENAME);
	 ofn.hwndOwner       = NULL;
	 ofn.lpstrFilter	 = Filter;
	 ofn.nFilterIndex    = 1;
	 ofn.lpstrDefExt	 = DefExt;
     ofn.lpstrFile       = szFile;
     ofn.nMaxFile        = sizeof(szFile);
     ofn.lpstrFileTitle  = szFileTitle;
	 ofn.nMaxFileTitle   = sizeof(szFileTitle);
     ofn.lpstrInitialDir = szDirectory;
	if(mode == Read){
		ofn.Flags = OFN_SHOWHELP | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		if(::GetOpenFileName(&ofn)==FALSE) return NULL;}
	   else {
		ofn.Flags = OFN_SHOWHELP | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT ;
		if(::GetSaveFileName(&ofn)==FALSE)
			return NULL;
		   else
           	::SaveIndex = (int)ofn.nFilterIndex;
		};
    return szFile;
};

/* End of file DOCUMENT.CPP */
 

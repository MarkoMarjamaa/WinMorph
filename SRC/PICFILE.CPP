/**********************************************************************
 * 
 *  NAME:         PICFILE.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  28.9.95  MHM     created 
 * 
 *********************************************************************/ 
 

#include "common.h"
#include "picfile.h"

BOOL picfile::ErrorMessages = TRUE;
 
void picfile::Error(LPSTR szErrorMessage)
{
	if(picfile::ErrorMessages)
		MessageBox(NULL,szErrorMessage,"File Error",MB_OK | MB_TASKMODAL | MB_ICONSTOP);
};

TPicture* picfile::pLoadPictureFile(LPSTR szFilename,int Filetype)
{
	picfile *File = NULL;
	switch(Filetype){
		case BMP: File = new BMPfile; break;
		case TGA: File = new TGAfile; break;
		}
        
	TPicture* pNewPicture = NULL;
	if(File){
		pNewPicture = File->load(szFilename);

		delete File;
		File = NULL;
		}
    return pNewPicture;
};

int picfile::nIdentifyPictureFile(LPSTR szFilename)
{
	int nFiletype = UNKNOWN;

	HFILE hFile = _lopen(szFilename, READ);
	if ( hFile == -1 ) {
        char szErrorMessage[200];
		wsprintf(szErrorMessage,"Can't open the picture file %s",szFilename);
		Error(szErrorMessage);
		return NULL;
		};

	if(BMPfile::Identify(hFile))
		nFiletype = BMP;

	if(TGAfile::Identify(hFile)){
		assert(nFiletype==UNKNOWN);
		nFiletype = TGA;
		}

	_lclose(hFile);

    return nFiletype;
};

LPSTR picfile::GetExtension(int nFiletype)
{
    LPSTR szExtension = NULL;
	switch(nFiletype){
		case BMP: szExtension = "bmp";break;
        case TGA: szExtension = "tga";break;
		};

    return szExtension;
};

/* End of file PICFILE.CPP */

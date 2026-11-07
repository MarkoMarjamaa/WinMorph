/**********************************************************************
 * 
 *  NAME:         BMPFILE.CPP 
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
 
#include "picfile.h"
#include "tpicture.h"
#include <io.h>


/* Function implementations */

TPicture* BMPfile::load(LPSTR Name)
{
	HFILE File = _lopen(Name, READ);
	if ( File == -1 ) {
		Error("Can't open the picture file");
		return NULL;
		};

	HGLOBAL hDib = hLoadDib(File);
	_lclose(File);
	if ( hDib == NULL ){
		Error("Can't read the picture file");
		return NULL;
		};

    TPicture* Picture = TPicture::Create(hDib);
	if(Picture==NULL)
		Error("Can't create the picture");

	return Picture;
}

BOOL BMPfile::save(LPSTR Name,TPicture* Dib)
{
    // only truecolor BMPs allowed
	if(Dib->GetBitsPerPixel()!=24) return FALSE;

	int File = _lcreat(Name, 0);

	if ( File == -1 ) {
		Error("Can't write a picture file");
		return FALSE;
		};

    POINT Res = Dib->GetResolution();
    DWORD BitsSize = (long)Res.y * (long)(((Res.x * 24)+ 31)/32)* 4; 
	DWORD BitsOffset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Write FileHeader
	BITMAPFILEHEADER FileHeader;
		FileHeader.bfType = ((UINT)'B') + ((UINT)'M')*256;
        FileHeader.bfSize = BitsOffset + BitsSize;
		FileHeader.bfReserved1 = 0;
		FileHeader.bfReserved2 = 0;
		FileHeader.bfOffBits   = BitsOffset;

    do{
		if(_hwrite(File,(void _huge*)&FileHeader,
				sizeof(BITMAPFILEHEADER))==-1L) break;
        
	    // Write BitmapInfo ...
		if(_hwrite(File,(void _huge*)GetDIBInfo(Dib),
				sizeof(BITMAPINFOHEADER))==-1L) break;

	    // Write Bitmap ...
		if(_hwrite(File,(void _huge*)GetDIBPicture(Dib),
			BitsSize)==-1L) break;

		// Close file and get out
		_lclose(File);
		return TRUE;
		}
    while(FALSE);

    // Errors occurred
	Error("Out of disk space");
	_close(File);
	return FALSE;	
};

BOOL BMPfile::Identify(int hFile)
{
	BITMAPFILEHEADER	bfhFileHeader;

	BOOL Identified = FALSE;
	do {
		_llseek(hFile, 0 , 0);

		/* Read fileheader */
		if(_lread(hFile,(LPSTR)&bfhFileHeader,sizeof(bfhFileHeader))!=sizeof(bfhFileHeader))
			break;			// Couldn't read the fileheader from file

		/* Check that file is a BMP-file */
		if (bfhFileHeader.bfType != ((UINT)'B') + ((UINT)'M')*256)
			break;

		Identified = TRUE;
        }
	while(FALSE);

    return Identified;
};

HGLOBAL BMPfile::hLoadDib(int hfFile)
{
	BITMAPFILEHEADER	bfhFileHeader;
	HGLOBAL				hDib = NULL;			/* Handle of DIB */
	LPSTR				pDib = NULL;			/* Pointer of DIB */
	DWORD				dwBitmapInfoSize,dwDataSize;

    BOOL Errors = TRUE;
	do {
		/* Read fileheader */
		if(_lread(hfFile,(LPSTR)&bfhFileHeader,sizeof(bfhFileHeader))!=sizeof(bfhFileHeader))
			break;			// Couldn't read the fileheader from file

		/* Check that file is a BMP-file */
		if (bfhFileHeader.bfType != ((UINT)'B') + ((UINT)'M')*256)
			break;

	    /* Calculate size of Dib data */
		dwDataSize = filelength(hfFile) - bfhFileHeader.bfOffBits;

		{	/*Check that the file is a Windows BMP, not OS/2 BMP */
		DWORD dwSizeofInfoHeader;
		_lread(hfFile,(LPSTR)&dwSizeofInfoHeader,sizeof(dwSizeofInfoHeader));
		if ( dwSizeofInfoHeader != sizeof(BITMAPINFOHEADER))
			break;
		}
    	
		{
		BITMAPINFOHEADER	bihInfoHeader;

		/* Read BitmapInfoHeader */
		_llseek(hfFile, sizeof(BITMAPFILEHEADER), 0);
		_lread(hfFile,(LPSTR)&bihInfoHeader,sizeof(BITMAPINFOHEADER));

		/* Check validity of BitmapInfoHeader */
		if(bihInfoHeader.biPlanes != 1)
			break;	
		if(		(bihInfoHeader.biBitCount!=1)&&(bihInfoHeader.biBitCount!= 4) &&
				(bihInfoHeader.biBitCount!=8)&&(bihInfoHeader.biBitCount!=24))
			break;
		if(bihInfoHeader.biCompression != BI_RGB)
			break;

		/* Get the size of files BitmapInfo */
		dwBitmapInfoSize = dwGetBitmapInfoSize(&bihInfoHeader);

		/* Allocate memory for DIB */
		hDib = TPicture::AllocateDib(
					(int)bihInfoHeader.biWidth,(int)bihInfoHeader.biHeight,bihInfoHeader.biBitCount);
		if (hDib == NULL)
			break;
	
		/* Get the pointer of Dib */
		pDib = (LPSTR)GlobalLock (hDib);
		}

		/* Read the BitmapInfoHeader again and now with palette */
		_llseek(hfFile, sizeof(BITMAPFILEHEADER), 0);
		_lread(hfFile,(LPSTR)pDib,(int)dwBitmapInfoSize);

		BITMAPINFO *biInfo = (BITMAPINFO*)pDib;

		/* If file is a truecolor Dib,
			then it should have a 256 color palette */
		if(biInfo->bmiHeader.biBitCount>8){
        	/* Fill in the remaining colors with default black */
			int nIndex;
			RGBQUAD rgbDefaultBlack={0,0,0,0};
			for(nIndex = (int)biInfo->bmiHeader.biClrUsed ; nIndex < 256 ; nIndex++){
				biInfo->bmiColors[nIndex] = rgbDefaultBlack;
            	};
			biInfo->bmiHeader.biClrUsed = 256;
            };

		/* Seek the data */
		_llseek(hfFile,bfhFileHeader.bfOffBits,0);

		{	/* Read the data */
		BYTE huge* bDIBits = pGetDibDataPointer((BYTE*)pDib);
		if(_hread(hfFile,(void _huge*)bDIBits,dwDataSize) < dwDataSize)
			break;
		  }

		  /* Loading of DIB succeeded */
		  Errors = FALSE;
		}
	while(FALSE);

	/* Unlock Dib */
	if(hDib)
		GlobalUnlock(hDib);

    /* Delete Dib, if there were errors */ 
	if(Errors)
		if(hDib){
			GlobalFree(hDib);
			hDib = NULL;
			};

	/* Return Dib */
	return hDib;
};


/* End of file BMPFILE.CPP */ 
 

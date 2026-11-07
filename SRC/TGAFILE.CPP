/**********************************************************************
 * 
 *  NAME:         TGAFILE.CPP 
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
 
#include <mem.h>		// memset()
#include "picfile.h"
#include "tpicture.h"

struct TGAHEADER {
	BYTE	bIdFieldLength;
	BYTE	bColorMapType;
    BYTE	bImageType;
	int		nColorMapOrigin;
	int 	nColorMapLength;
    BYTE	bColorMapEntrySize;	
	int		nXOrigin;
	int		nYOrigin;
	int		nWidth;
	int		nHeight;
	BYTE	bBitsPerPixel;
	BYTE	bImageDescriptor;
};

/* Color map types */
#define COLOR_MAP_NO	0
#define	COLOR_MAP_YES	1

/* Image types */
#define IMAGE_TYPE_NO_IMAGE		0
#define IMAGE_TYPE_PALETTED		1
#define IMAGE_TYPE_RGB			2
#define IMAGE_TYPE_BW			3
#define IMAGE_TYPE_PALETTE_RLE	9
#define IMAGE_TYPE_RGB_RLE		10
#define IMAGE_TYPE_BW_RLE		11
#define IMAGE_TYPE_PALETTE_HD_RLE	32
#define IMAGE_TYPE_PALETTE_HD_RLE_4	33

/* Image descriptor fields */

#define SCREEN_ORIGIN_LOWER	0
#define SCREEN_ORIGIN_UPPER	32

#define INTERLEAVED_NONE		0
#define INTERLEAVED_TWO_WAY		64
#define INTERLEAVED_FOUR_WAY    128
#define INTERLEAVED_RESERVED    192


TPicture* TGAfile::load(LPSTR pszFileName)
{
	HFILE hFile = _lopen(pszFileName, READ);
	if ( hFile == -1 ) {
		Error("Can't open the picture file");
		return NULL;
		};

	TPicture* pPicture = pLoadType(hFile);
	_lclose(hFile);
	if ( pPicture == NULL ){
		Error("Can't read the picture file");
		};

	return pPicture;
};

BOOL TGAfile::Identify(int hFile) 
{
	TGAHEADER 	FileHeader;

	BOOL Identified = FALSE;
	do {
		_llseek(hFile, 0 , 0);

		/* Read fileheader */
		if(_lread(hFile,(LPSTR)&FileHeader,sizeof(FileHeader))!=sizeof(FileHeader))
			break;			// Couldn't read the fileheader from file

		/* Check that color map type is supported */
		if(FileHeader.bColorMapType!=COLOR_MAP_NO)
			break;

		/* Check that image type is supported */
		if(FileHeader.bImageType!=IMAGE_TYPE_RGB)
			break;

		/* Check that width is reasonable */
		if((FileHeader.nWidth>4096)||(FileHeader.nWidth<0))
			break;

        /* Check that height is reasonable */
		if((FileHeader.nHeight>4096)||(FileHeader.nHeight<0))
			break;

		/* Check bits per pixel */
		if(FileHeader.bBitsPerPixel!=24)
        	break;

        /* Normal image descriptions */
    	if(FileHeader.bImageDescriptor!=0)
			break;

		Identified = TRUE;
        }
	while(FALSE);

    return Identified;
};

BOOL TGAfile::save(LPSTR Name,TPicture* Dib)
{
    // Saves only truecolor TGAs
	if(Dib->GetBitsPerPixel()!=24) return FALSE;

	int File = _lcreat(Name, 0);

	if ( File == -1 ) {
		Error("Can't write the picture file");
		return FALSE;
		};

    POINT Res = Dib->GetResolution();

    // Write FileHeader
	TGAHEADER FileHeader;
	memset(&FileHeader,0,sizeof(TGAHEADER));
		FileHeader.bColorMapType = COLOR_MAP_NO;
		FileHeader.bImageType	= IMAGE_TYPE_RGB;
		FileHeader.nWidth		= Res.x;
		FileHeader.nHeight		= Res.y;
		FileHeader.bBitsPerPixel	= 24;
	_hwrite(File,&FileHeader,sizeof(TGAHEADER));

	int colorwidth = FileHeader.nWidth * 3;

	// Write Bitmap ...
	for( int y = 0 ; y < Res.y ; y++ )
		if(_hwrite(File,Dib->GetYLine(y),colorwidth)==-1){
			Error("Out of disk space");
			_lclose(File);
			return FALSE;
			};


    // Close file and get out
	_lclose(File);
    return TRUE;
};
 
// Korjaa !!!!!!!
#define LOHICHECK(nInteger)

TPicture *TGAfile::pLoadType(int hFile)
{
	TGAHEADER	FileHeader;
    TPicture 	*pPicture = NULL;	
	BOOL Errors = TRUE;
	do {
		_llseek(hFile, 0 , 0);

		/* Read fileheader */
		if(_lread(hFile,(LPSTR)&FileHeader,sizeof(FileHeader))!=sizeof(FileHeader))
			break;			// Couldn't read the fileheader from file

		switch(FileHeader.bImageType){
			case IMAGE_TYPE_RGB:pPicture = pLoadTypeRGB(hFile,&FileHeader);
        };
	} while(FALSE);

	return pPicture;
};

TPicture *TGAfile::pLoadTypeRGB(int hFile,TGAHEADER *pHeader)
{
	long lRGBData;
	// Seek to start of picture data
	lRGBData = sizeof(TGAHEADER) + pHeader->bIdFieldLength;
	_llseek(hFile, lRGBData, 0);

	// If image contains color map
	if(pHeader->bColorMapType==COLOR_MAP_YES){
		long 	lColorMapSize;
		int 	nColorMapOrigin; 
		int 	nColorMapLength;
    	BYTE	bColorMapEntrySize;	
		return NULL;
    };

	TPicture* pPicture = TPicture::Create(pHeader->nWidth,pHeader->nHeight,24);

	int colorwidth = pHeader->nWidth * 3;

	// Write Bitmap ...
	for( int y = 0 ; y < pHeader->nHeight ; y++ )
		if(_hread(hFile,pPicture->GetYLine(y),colorwidth)==-1){
			Error("Out of disk space");
			_lclose(hFile);
			return FALSE;
			};

	return pPicture;
};

/* End of file TGAFILE.CPP */ 
 

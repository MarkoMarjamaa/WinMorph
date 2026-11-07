/**********************************************************************
 * 
 *  NAME:         ANIMATIO.CPP 
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

#include <stdlib.h>        // _makepath(),_splitpath()
#include <string.h>
#include <owl.h>
#include "common.h"
#include "animatio.h"
#include "tpicture.h"
#include "picfile.h"
#include "point.h"
#include "animinfo.h"

/////////////////////////////////////////////////////////////////
// Animation_Document
////////////////////////////////////////////////////////////////

LPSTR Animation_Document::GetSavePrompt()
{ return "Animation has been changed. Save file before closing?";};

LPSTR Animation_Document::GetSaveFileName(void)
{  return Document::GetFileName(GetSaveFilter(),GetSaveDefExt(),Write);};

//////////////////////////////////////////////////////////////
// Animation
//////////////////////////////////////////////////////////////

Animation::Animation(POINT pRes,int nFrames)
{
	Frames = nFrames;
	Resolution = pRes;
	AllOK = FALSE;

	Frame = NULL;

	SaveAsSingleFile = FALSE;
	Filetype = BMP;
	SaveIndex = 2;

    // Create frame array if needed
	if(Frames!=0){
		Frame = new TPicture *[Frames];

        // Init frames
		for(int nFrame = 0 ; nFrame < Frames ; nFrame++ )
			Frame[nFrame] = NULL;
		  AllOK = TRUE;
		};
};

Animation::~Animation()
{
    Reset();
};

void Animation::Reset(void)
{
    if(Frame){
		// Delete frames
		for(int nFrame = 0 ; nFrame < Frames ; nFrame++ )
			if(Frame[nFrame])
				delete Frame[nFrame];

	    // Delete frame array
		delete Frame;
		Frame = NULL;
        };
};

Animation *Animation::Create(POINT pRes,int nFrames)
{
	Animation *aAnim = new Animation(pRes,nFrames);

	if(aAnim)
		if(!aAnim->IsOK()){
			delete aAnim;
			aAnim = NULL;
			}
	return aAnim;
};

Animation* Animation::Create(LPSTR szFilename)
{
	Animation* aAnim = NULL;
	if(szFilename == NULL){
		// Get file name from user
		szFilename = GetFileName(GetOpenFilter(),GetOpenDefExt(),Read);   
		  };

	 if(szFilename !=NULL){
        // Create null animation
		POINT pNullRes = {0,0};
		aAnim = new Animation(pNullRes,0);
		if(aAnim!=NULL){
            // Load animation
			aAnim->LoadFrom(szFilename);
            // If it didn't succeed
			if(!aAnim->IsOK()){
                // delete animation
				delete aAnim;
				aAnim = NULL;
				}
			}
		}
	 return aAnim;
};

int         Animation::GetFrames()        { return Frames;};
POINT    Animation::GetResolution()    { return Resolution;};
TPicture*   Animation::Get_Frame(int nFrame)  { return Frame[nFrame];};

void Animation::Set_Frame(TPicture* pPicture,int nFrame)
{
	assert(nFrame<GetFrames());
    assert(nFrame>-1);

	do {
		// If the new pictures resolution is different than others, don't
        // accept it
		if(pPicture)
			if(!(Resolution == pPicture->GetResolution()))
					break;

        // Assign new picture
		SWAP(TPicture*,Frame[nFrame],pPicture);

        // Set animation as changed
		SetChanged(TRUE);
		  }
	 while(FALSE);

    // Delete the leftover picture
	if(pPicture)
		delete pPicture;     
};

LPSTR Animation::GetSaveFileName(void)
{
	 LPSTR szFileName = NULL;

	 switch(Filetype){
		case TGA: SaveIndex = 1; break;
		case BMP: SaveIndex = 2; break;
		default: assert(FALSE);
		};
	 if(SaveAsSingleFile)
		SaveIndex +=2;

	 if(GetFrames()>1)
		szFileName = Document::GetFileName(GetSaveFilter(),GetSaveDefExt(),Write);
		else
		szFileName = Document::GetFileName(
"Targa animation\0*.anm\0BMP animation\0*.anm\0Targa picture\0*.tga\0BMP picture\0*.bmp\0\0",
			"anm",Write);

	return szFileName;
};

// File functions for Animation

void Animation::SaveDocument(wmfstream& file)
{
	switch(SaveIndex){
		case 0: assert(FALSE) ; break;
		case 1: SaveAsSingleFile = FALSE ; Filetype = TGA ; break;
		case 2: SaveAsSingleFile = FALSE ; Filetype = BMP ; break;
		case 3: SaveAsSingleFile = TRUE  ; Filetype = TGA ; break;
		case 4: SaveAsSingleFile = TRUE  ; Filetype = BMP ; break;
		  default:assert(FALSE); break;
		};

	// Create picture file filter
	picfile *pPicFile = NULL;
	switch(Filetype){
		case BMP:   pPicFile = new BMPfile; break;
		case TGA:   pPicFile = new TGAfile; break;
		};
	assert(pPicFile!=NULL);

	if(SaveAsSingleFile == TRUE){
		// Save as usual picture file(BMP,TGA)(only first frame)
		if(Get_Frame(0))
			pPicFile->save(szFilename,Get_Frame(0));
		}
		 else {        // Save as WinMorph Animation
		// Make frame file names
		char szName    [2*_MAX_FNAME];
		char szFramePath[_MAX_PATH ];
		  // Get animation file name
		_splitpath(szFilename,NULL,NULL,szName,NULL);

		  // Add trailing char '_' 
		_fstrcat(szName,"________");
		// Truncate to 5 chars and add frame number and extension
		wsprintf(szFramePath,"%.5s%%.3d.%s",szName,
			picfile::GetExtension(Filetype));

        // Save animation file
		SaveAnimationFile(file,szFramePath);

		// Save frames
		char szPicPath[_MAX_PATH];
		for(int nFrame = 0 ; nFrame < GetFrames() ; nFrame++ )
			if(Get_Frame(nFrame)){
                // Create picture file name
				wsprintf(szPicPath,szFramePath,nFrame);
				if(pPicFile->save(szPicPath,Get_Frame(nFrame))==FALSE)
					break;
				};
		};

    // Delete picture file filter
	delete pPicFile;
};

void Animation::SaveAnimationFile(wmfstream& file,LPSTR szFramePath)
{
	file << "WinMorph\n";
	file << "Animation\n";

	file << "Frames = "     << GetFrames()  << "\n";
	file << "Resolution = ";
	file << Resolution      << "\n";
	file << "Pictures = "   << szFramePath     << "\n";
};

BOOL Animation::LoadDocument(wmfstream& file)
{
	assert(GetFrames()==0);

    // Identify file type
 	SaveAsSingleFile = ! Identify(file);

	if(!SaveAsSingleFile){  // File was an ANM file
		char szFrameFilename[_MAX_PATH];
		  
		// Load animation file
		if(Load_AnimFile(file,szFrameFilename)){
			// Create frame array and init it
			Frame = new TPicture *[GetFrames()];
			for(int fr = 0 ; fr < GetFrames() ; fr++ )
				Frame[fr] = NULL;

			// Make frame file name
			char szDrive   [_MAX_DRIVE];
			char szDir     [_MAX_DIR  ];
			char szFramePath[_MAX_PATH ]="";

			_splitpath(szFilename,szDrive,szDir,NULL,NULL);
			_makepath(szFramePath,szDrive,szDir,szFrameFilename,NULL);

			// Load frames
			Load_Frames(szFrameFilename);
			}
		}
	   else { // File was an individual picture file
		// Identify picture file type
		Filetype = picfile::nIdentifyPictureFile(szFilename);
		// Load picture frame
		TPicture *NewFrame = picfile::pLoadPictureFile(szFilename,Filetype);

        // If loading succeeded
		if(NewFrame){
            // Create frame array and init it
			Frames = 1;
			Frame = new TPicture *[Frames];

			Frame[0] = NewFrame;
			Resolution = NewFrame->GetResolution();
			};
		};

	AllOK = (Frames!=0);

	return AllOK;
};

// Identify file type
BOOL Animation::Identify(wmfstream& file)
{
	BOOL bAnimation = FALSE;
	do{
		file.bShowErrors = FALSE;
		if(!GetKeyword(file,"WinMorph"))  break;
		if(!GetKeyword(file,"Animation")) break;
		file.bShowErrors = TRUE;
		bAnimation = TRUE;
		}
	while(FALSE);

	file.seekg(0L);

    // Return results
	return bAnimation;
};

BOOL Animation::Load_AnimFile(wmfstream& file,LPSTR szFramePath)
{
	BOOL GotFrames		= FALSE;
	BOOL GotResolution	= FALSE;
	BOOL GotPictures	= FALSE;

	do{
        // Read identifying words
		if(!GetKeyword(file,"WinMorph"))  break;
		if(!GetKeyword(file,"Animation")) break;

        // Define keywords
		#define FRAMES    0
		#define RESOLUTION   1
		#define PICTURES  2
		LPSTR szKeywords[]={"Frames","Resolution","Pictures",NULL};

		while(!file.eof()){
			int nKeyword = IdentifyLabel(file,szKeywords);

			switch(nKeyword){
				case FRAMES:   {
					int nNewFrames = 0;
                    // Read number of animation frames
					file >> nNewFrames;
                    // If succeeded ...
					if(nNewFrames>0){
                        // Set new number of frames
						Frames = nNewFrames;
						GotFrames = TRUE;
						}
					break;
					};
				case RESOLUTION:{
					POINT pNewRes;
					// Read animation resolution
					file >> pNewRes;
                    // If succeeded ...
					if(pNewRes.x>0){
                        // Set new resolution
						Resolution = pNewRes;
						GotResolution = TRUE;
						}
					break;
					};
				case PICTURES: {
					// Read frame picture's names
					vReadWord(file,szFramePath,_MAX_PATH);
                    
					if(_fstrlen(szFramePath)>0)
							GotPictures = TRUE;
					break;
					};
				};
			};
		  }
	while(FALSE);

	return (GotFrames&&GotResolution&&GotPictures);
};

void Animation::Load_Frames(LPSTR szFramePath)
{
	char szPicPath[_MAX_PATH];

    // Do not allow error messages
	picfile::ErrorMessages = FALSE;

	for(int nFrame = 0 ; nFrame < GetFrames() ; nFrame++ ){
		// Create filename
		wsprintf(szPicPath,szFramePath,nFrame);      
		// Identify file type
		Filetype = picfile::nIdentifyPictureFile(szPicPath);
		// Load frame
		TPicture *NewFrame = picfile::pLoadPictureFile(szPicPath,Filetype);
		// Set frame
		Set_Frame(NewFrame,nFrame);
		}

	// Error messages are again allowed
	picfile::ErrorMessages = TRUE;

	//
	SetChanged(FALSE);
};
 
void Animation::Information(class TWindow *pWnd)
{
	// Open Animation Information window
	pWnd->GetModule()->ExecDialog( new TAnimInfoDlg(pWnd,
		"AnimInfoDlg",this));
};

/* End of file ANIMATIO.CPP */ 
 

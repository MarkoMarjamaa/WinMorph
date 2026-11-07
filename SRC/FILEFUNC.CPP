/**********************************************************************
 * 
 *  NAME:         FILEFUNC.CPP 
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

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "filefunc.h"

#define ENDOFBLOCKCHAR ')'

////////////////////////
// WMfstream , our stream with errormessages

_Cdecl wmfstream::wmfstream(const signed char _FAR *szNewFilename, int nMode):
	fstream(szNewFilename,nMode)
{
	 bShowErrors = TRUE;
	_fstrcpy(szFilename,szNewFilename);
};

void wmfstream::vErrorMessage(const signed char _FAR *szErrorMessage)
{
	if(!bShowErrors) return;

	char szWholeMessage[400];
	int nErrorLine = 0;

	/* Count the errorline */
	int i;
	long nErrorPosition = tellp();
	seekp(0L);
	for( i = 0 ; i < nErrorPosition - 1 ; i++ ){
		BYTE ch = get();
		if(ch==10)  nErrorLine++;
		}

	 /* Load error message strings */
	wsprintf(szWholeMessage,"%s : in file %s, line %d\n",szErrorMessage,szFilename,nErrorLine);

	 /* Show error message box*/
	MessageBox(NULL,szWholeMessage,"File Error",MB_OK | MB_TASKMODAL | MB_ICONSTOP);
};

////
// Normal file functions

BOOL ReadKeyword(wmfstream& file,LPSTR szDest,int nDestLength)
{
	int nDestOff = 0;

	do {
		char bChar;

		// Erase white space
		vEatWhiteSpace(file);

		  // If end of file, return 
		  if(file.eof())  break;

		// Read first character
		file.get(bChar);

		// End of Block- character is a keyword also
		// If EOB is read, stop reading and return EOB
		if(bChar==ENDOFBLOCKCHAR)
			if(!file.eof()){
				szDest[nDestOff] = bChar;
				nDestOff++;
					 break;
					};

		// Read string
		for(nDestOff = 0 ; nDestOff < nDestLength - 1 ; nDestOff++ ){
				// if EOF, stop reading
			if(file.eof())          break;

			// If a word separator is read, return separator to stream
				// and stop reading
			if((bChar==ENDOFBLOCKCHAR)||fIsWhiteSpace(bChar)){
				file.putback(bChar);
				break;
				};

				// Add character to word
			szDest[nDestOff] = bChar;

				// Read next character
			file.get(bChar);
			};
		}
	 while(FALSE);

	// Add terminating null to word
	szDest[nDestOff] = '\0';

	return !file.eof();
};

// Check for certain keyword
BOOL GetKeyword(wmfstream& file,LPSTR szKeyword)
{
	char szWord[80];
	 // Read keyword
	ReadKeyword(file,szWord,sizeof(szWord));
	 // Check if it's the right one
	BOOL fFound =(_fstricmp(szKeyword,szWord)==0);

	 // Show an error message if it's not
	if(!fFound){
		char szErrorMessage[200];
		wsprintf(szErrorMessage,"Wrong keyword, \"%s\" expected",szKeyword);
		  file.vErrorMessage(szErrorMessage);
		};

	 // Return results
	 return fFound;
};

// Read keyword and identify it
int IdentifyKeyword(LPSTR szKeywords[],LPSTR szWord)
{
	int nKeyword = UNDEFINED;

	 // Check entire keyword list for a match
	for(int nIndex = 0 ; szKeywords[nIndex]!=NULL ; nIndex++ ){
		if(_fstricmp(szKeywords[nIndex],szWord)==0){
			assert(nKeyword==UNDEFINED);
			nKeyword = nIndex;
				break;
			  };
		};

	 // Check for end of block character
	if(_fstricmp(szWord,")")==0){
		assert(nKeyword==UNDEFINED);
		nKeyword = ENDOFBLOCK;
		};

	// Check for empty line
	if(_fstricmp(szWord,"")==0){
		assert(nKeyword==UNDEFINED);
		nKeyword = EMPTYLINE;
		};

	return nKeyword;
};

// Read keyword and identify it
int IdentifyKeyword(wmfstream& file,LPSTR szKeywords[])
{
	char szWord[80];
	 // First read a keyword
	ReadKeyword(file,szWord,sizeof(szWord));
	 // and then identify it
	int nKeyword = IdentifyKeyword(szKeywords,szWord);

	 // If keyword is not identified
	if(nKeyword == UNDEFINED){
		  // show an error message
		file.vErrorMessage("Can't identify keyword");
		};

	 // Return results
	 return nKeyword;
};

// Read label and identify it
int IdentifyLabel(wmfstream& file,LPSTR szKeywords[])
{
	int nKeyword = IdentifyKeyword(file,szKeywords);

	// If keyword was end of block, we don't have the
	 // label separator
	 if((nKeyword>=0))
		if(GetCharacter(file,'=')==FALSE){
			file.vErrorMessage("Can't find label separator");
			// Couldn't find '=', the label separator
			nKeyword = UNDEFINED;
			 };

	return nKeyword;
};

// Check if character is considered white space
BOOL fIsWhiteSpace(BYTE bChar)
{
	return (bChar==' ')||(bChar==10)||(bChar==13)||(bChar==9);
};

// Delete any unnecessary white space
void vEatWhiteSpace(wmfstream& file)
{
	do {
		// Read character
		BYTE bChar;
		file.get(bChar);

		// If end of file stop reading
		if(file.eof()) break;

		// Check if character is not white space
		if(!fIsWhiteSpace(bChar)){
			file.putback(bChar);
				break;
			}
		}
	 while (TRUE);
};

BOOL GetCharacterIf(wmfstream& file,BYTE bSearched)
{
	// Erase white space
	vEatWhiteSpace(file);

	char bChar;
	file.get(bChar);
	if(bChar != bSearched)
		file.putback(bChar);
	return (bChar == bSearched);
};

// Get certain character from stream
BOOL GetCharacter(wmfstream& file,BYTE bSearched)
{
	// Erase white space
	vEatWhiteSpace(file);

	char bChar;
	file.get(bChar);
	return (bChar==bSearched);
};

BOOL GetStartOfBlock(wmfstream& file)
{
	BOOL fFound = GetCharacter(file,'(');
	if(!fFound)
		file.vErrorMessage("Can't find Start Of Block- character");
	return fFound;
};

BOOL GetEndOfBlock(wmfstream& file)
{
	BOOL fFound = GetCharacter(file,')');
	if(!fFound)
		file.vErrorMessage("Can't find End Of Block- character");
	return fFound;
};

BOOL GetComma(wmfstream& file)
{
	BOOL fFound = GetCharacter(file,',');
	if(!fFound)
		file.vErrorMessage("Can't find integer separator");
	return fFound;
};

// Read a point from a stream
wmfstream& operator>>(wmfstream& file,POINT& p)
{
	 do {
		  // Read start of block
		if(!GetStartOfBlock(file))   break;
		file >> p.x;                  // Read x
		  // Read separator 
		if(!GetCharacter(file,','))   break;
		file >> p.y;                  // Read y
		  // Read end of block
		if(!GetEndOfBlock(file))   break;
		}
	 while(FALSE);

	return file;
};

// Save a point into a stream
wmfstream& operator<<(wmfstream& file,POINT& p)
{
	  file << " ( " << p.x << " , " << p.y << " )";
	  return file;  
};

// Read a word ending in a white space
BOOL vReadWord(wmfstream& file,LPSTR szWord,int nWordLength)
{
	char bChar;
	int nDestIndex;

	// Erase white space
	vEatWhiteSpace(file);

	// Read word
	for(nDestIndex = 0 ; nDestIndex < nWordLength-1 ; nDestIndex++){
		  // Read a character
		file.get(bChar);

		  // Check for EOF
		if(file.eof())           break;

		  // Check for a white space
		if(fIsWhiteSpace(bChar)){
			file.putback(bChar);
			break;
			}
			  
		  // Add character to word
		szWord[nDestIndex] = bChar;
		};

	 // Put terminating null
	szWord[nDestIndex] = '\0';

	return !file.eof();
};

// Read a line ending in a return code 
BOOL vReadLine(wmfstream& file,LPSTR szLine,int nLineLength)
{
	char bChar;
	int nDestIndex;

	 // Erase white space
	vEatWhiteSpace(file);

	 // Read word
	for(nDestIndex = 0 ; nDestIndex < nLineLength-1 ; nDestIndex++){
		  // Read character
		file.get(bChar);

		if(file.eof())     break;
		if(bChar=='\n')    break;

		  // Add character to word
		szLine[nDestIndex] = bChar;
		};

	// Put terminating null
	szLine[nDestIndex] = '\0';

	return !file.eof();
};

void RemoveSameDirectories(LPSTR szStart,LPSTR szEnd)
{
    BOOL fSameRoot = TRUE;

	do {
		// If no more same directories
		if(szStart[1]=='\0') break;
		if(szEnd  [1]=='\0') break;

		// Check if same root directories
		for(int i = 1; i < _fstrlen(szStart) ; i ++){
			if(szStart[i]!=szEnd[i]){
				//Roots are not the same
				fSameRoot = FALSE;
				break;
				};

            // One directory compared, and it checked clean
			if(szStart[i]=='\\'){
				char szTemp[2*_MAX_PATH];
    	
            	// Eliminate same root directories
				_fstrcpy(szTemp,&szStart[i]);
				_fstrcpy(szStart,szTemp);

				_fstrcpy(szTemp,&szEnd[i]);
				_fstrcpy(szEnd,szTemp);
				break;
				};
			};
		}
    while(fSameRoot);
};

LPSTR szRelativePath(LPSTR szStartPath, LPSTR szEndPath)
{
	static char szStart[_MAX_PATH];
	static char szEnd  [2*_MAX_PATH];
			 char szTemp [2*_MAX_PATH];
			 char szFilename[_MAX_PATH];

	char szDrive[_MAX_DRIVE];
	char szDir  [_MAX_DIR  ];
	char szFname[_MAX_FNAME];
    char szExt  [_MAX_PATH ];

    // Create start path
	_splitpath(szStartPath,szDrive,szDir,szFname,szExt);
	_fstrcpy(szStart,szDrive);
	_fstrcat(szStart,szDir);

    // Create end path
	_splitpath(szEndPath,szDrive,szDir,szFname,szExt);
	_fstrcpy(szEnd,szDrive);
	_fstrcat(szEnd,szDir);

    // Create file name
	_fstrcpy(szFilename,szFname);
	_fstrcat(szFilename,szExt);

	// If the paths are in a same drive
	if(szStart[0] == szEnd[0]){
		// Remove Drive part from directories
		_fstrcpy(szTemp,&szStart[2]);
		_fstrcpy(szStart,szTemp);

		_fstrcpy(szTemp,&szEnd[2]);
		_fstrcpy(szEnd,szTemp);

	    RemoveSameDirectories(szStart,szEnd);

        // Remove last slash from szStart path
		szStart[_fstrlen(szStart)-1]='\0';

		// Substitute szStart's directories with '\..' and move them to szEnd
		int nSlashes = 0;
        // Count slashes
		for(int i = 0 ; i < _fstrlen(szStart) ; i ++ )
			if(szStart[i]=='\\')
            	nSlashes ++;

		// add '\..' -directories to end path
		char szSlashes[2*_MAX_PATH]="";
		for( ;nSlashes > 0 ; nSlashes -- )
			// Add "\\.."- directories to end path
			_fstrcat(szSlashes,"\\..");
        _fstrcat(szSlashes,szEnd);

        // Copy path to result path and remove first slash
		_fstrcpy(szEnd,&(szSlashes[1]));
		};

    // Add filename to endpath
	_fstrcat(szEnd,szFilename);

	// Return result path & file name
	return szEnd;
};

LPSTR AddPath(LPSTR szBase,LPSTR szNewPath)
{
	static char szPath [_MAX_PATH ];

	char szDrive[_MAX_DRIVE];
	char szDir  [_MAX_DIR  ];
	char szFname[_MAX_FNAME];
    char szExt  [_MAX_PATH ];

	_splitpath(szNewPath,szDrive,szDir,szFname,szExt);

	// Check if the new directory has a drive
	if(_fstrlen(szDrive)==0){
        // Get new directory
		char szBaseDir[_MAX_PATH];
		_splitpath(szBase,szDrive,szBaseDir,NULL,NULL);

		int nNewIndex = 0;
		// Start eliminating '..'- directories
		while(_fstrlen(&(szDir[nNewIndex]))>0){
			if((szDir[nNewIndex]!='.')||(szDir[nNewIndex+1]!='.'))
                // Next directory is not '..', so stop 
            	break;

            // Remove '..'-directory from szDir
			nNewIndex += 3;

            // Remove last directory from basedir
			for(int i = _fstrlen(szBaseDir)-2; i >=0 ; i -- )
				if(szBaseDir[i]=='\\'){
					szBaseDir[i+1]='\0';
					break;
                    };
        	};

        // Add new directory to base directory
		_fstrcat(szBaseDir,&(szDir[nNewIndex]));

        // Copy base directory to new directory 
        _fstrcpy(szDir,szBaseDir);
		};

    // Put path together again
	_makepath(szPath,szDrive,szDir,szFname,szExt);

    // return result path
    return szPath; 
};

/* End of file FILEFUNC.CPP */
 

/**********************************************************************
 * 
 *  NAME:         MORPHCFG.CPP 
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
 
#include <commdlg.h> // ChooseColor()
#include <string.h>     // _fstricmp()
#include "morphcfg.h"
#include "getint.h"     // class GetInteger
#include "filefunc.h"
#include "morphwin.h"
#include "picturew.h"
#include "morphedw.h"


LPSTR szOnOffValues[]={"On","Off",NULL};

/////////////////////////////////////////7
// TMorphConfig
///////////////////////////////////////////

TMorphConfig MorphConfig;// Only one instance of this class, accessible to all

TMorphConfig::TMorphConfig()
{
	NormalBorder   = NULL;
    NormalBorderKnot = NULL;
	NormalInside   = NULL;
	LineBorder     = NULL;
	SelectedBorder = NULL;
    SelectedBorderKnot = NULL;
	SelectedInside = NULL;

	Init();

	// Init file list
	for( int nFiles = 0 ; nFiles < MAX_PREVIOUS_FILES ; nFiles ++ )
		szFileList[nFiles] = NULL;
};

void TMorphConfig::Init(void)
{
	nPointSize = 3;
    nPointDragSize = 3; 
	nPointShape = POINT_SHAPE_RECT;
    nKnotSize = 2;
    nPointDragStart = 2;

	nInterpolation = AFTER_MOVING;
	nTriangulation = ON_MORPHING;
	fAutoGrid = TRUE;
    nGridSize = 1;
	fAutoResultFrames = TRUE;
	fKeepFramesTogether = TRUE;
	fBeepWhenFinished = FALSE;

	nUndoBufferSize = 20;
	fRedoEnabled = TRUE;

	Point	= RGB(255,  0,  0);
	Line  	= RGB(  0,255,  0);
	Selected= RGB(  0,  0,255);

	if(NormalBorder    	== NULL) NormalBorder   	= CreatePen(PS_SOLID,1,Point);
	if(NormalBorderKnot	== NULL) NormalBorderKnot	= CreatePen(PS_SOLID,nKnotSize,Point);
	if(NormalInside    	== NULL) NormalInside   	= (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	if(LineBorder   	== NULL) LineBorder     	= CreatePen(PS_SOLID,1,Line);
	if(SelectedBorder	== NULL) SelectedBorder 	= CreatePen(PS_SOLID,1,Selected);
	if(SelectedBorderKnot==NULL) SelectedBorderKnot = CreatePen(PS_SOLID,nKnotSize,Selected);
	if(SelectedInside	== NULL) SelectedInside 	= (HBRUSH)GetStockObject(HOLLOW_BRUSH);
};

TMorphConfig::~TMorphConfig()
{
	Destroy();
};

void TMorphConfig::Destroy(void)
{
	DeleteObject(SelectedBorder); 		SelectedBorder = NULL;
	DeleteObject(SelectedBorderKnot); 	SelectedBorderKnot = NULL;
	DeleteObject(SelectedInside);		SelectedInside = NULL;
	DeleteObject(NormalBorder);      	NormalBorder = NULL;
	DeleteObject(NormalBorderKnot);    	NormalBorderKnot = NULL;
	DeleteObject(NormalInside);      	NormalInside = NULL;
	DeleteObject(LineBorder);    		LineBorder = NULL;

	// Delete file list
	for( int nFiles = 0 ; nFiles < MAX_PREVIOUS_FILES ; nFiles ++ )
		if(szFileList[nFiles])
			delete szFileList[nFiles];
};

BOOL TMorphConfig::SetNewValue(TWindow *Wnd,int *pValue,LPSTR pszWindowTitle)
{
	int nOldValue = *pValue;
	Wnd->GetModule()->ExecDialog( new
		GetInteger(Wnd,pszWindowTitle,pValue));

	return (nOldValue!=*pValue);
};

BOOL TMorphConfig::SetPointSize(TWindow* Wnd)
{
	return SetNewValue(Wnd,&nPointSize,"Set new point size");
};

BOOL TMorphConfig::SetPointDragSize(TWindow* Wnd)
{
	return SetNewValue(Wnd,&nPointDragSize,"Set new point drag size");
};

BOOL TMorphConfig::SetGridSize(TWindow* Wnd)
{
	BOOL fChanged = SetNewValue(Wnd,&nGridSize,"Set new automatic grid size");

	nGridSize = max(nGridSize,1);

    return fChanged;
};

BOOL TMorphConfig::SetKnotSize(TWindow* Wnd)
{
    BOOL fChanged = SetNewValue(Wnd,&nKnotSize,"Set new knot width");

	if(fChanged){
		DeleteObject(NormalBorderKnot);
		NormalBorderKnot = CreatePen(PS_SOLID,nKnotSize,Point);
		DeleteObject(SelectedBorderKnot);
		SelectedBorderKnot = CreatePen(PS_SOLID,nKnotSize,Selected);
    };

	return fChanged;
};

BOOL TMorphConfig::SetUndoBufferSize(TWindow* Wnd)
{
	return SetNewValue(Wnd,&nUndoBufferSize,"Set new undo buffer size");
};

BOOL TMorphConfig::SetPointDragStart(TWindow *Wnd)
{
	return SetNewValue(Wnd,&nPointDragStart,"Set new point drag start value");
};

BOOL TMorphConfig::SetPointColor(TWindow* Wnd)
{
	if(GetColorFromUser(Wnd,Point)){
		DeleteObject(NormalBorder);
		NormalBorder = CreatePen(PS_SOLID,1,Point);
		DeleteObject(NormalBorderKnot);
		NormalBorderKnot = CreatePen(PS_SOLID,nKnotSize,Point);
		return TRUE;
	} else
		return FALSE;
};

BOOL TMorphConfig::SetSelectedColor(TWindow *Wnd)
{
	if(GetColorFromUser(Wnd,Selected)){
		DeleteObject(SelectedBorder);
		SelectedBorder = CreatePen(PS_SOLID,1,Selected);
		DeleteObject(SelectedBorderKnot);
		SelectedBorderKnot = CreatePen(PS_SOLID,nKnotSize,Selected);
		  return TRUE;
	} else
		 return FALSE;
};

BOOL TMorphConfig::SetLineColor(TWindow* Wnd)
{
	if(GetColorFromUser(Wnd,Line)){
		DeleteObject(LineBorder);
		LineBorder = CreatePen(PS_SOLID,1,Line);
		return TRUE;
		}
		else
		  return FALSE;
};

BOOL TMorphConfig::GetColorFromUser(TWindow* Wnd,COLORREF& color)
{
	COLORREF aclrCust[16];

	/* Set the custom color controls to white. */
	for (int i = 0; i < 16; i++)
		aclrCust[i] = RGB(255, 255, 255);

	CHOOSECOLOR cc;
		memset(&cc, 0, sizeof(CHOOSECOLOR));
		cc.lStructSize = sizeof(CHOOSECOLOR);

		cc.hwndOwner = Wnd->HWindow;
		cc.rgbResult = color;
		cc.lpCustColors = aclrCust;
		cc.Flags = CC_RGBINIT;

	if(ChooseColor(&cc)){
		color = cc.rgbResult;
		return TRUE;
		}
		else
		  return FALSE;
};

void TMorphConfig::FileListAdd(LPSTR szNewFileName)
{
	if(szNewFileName==NULL)
		return;
	if(szNewFileName[0]=='\0')
    	return;

	LPSTR szPrevFileName = _fstrdup(szNewFileName);

	for(int nFiles = 0 ; nFiles < MAX_PREVIOUS_FILES ; nFiles ++ ){
		SWAP(LPSTR,szFileList[nFiles],szPrevFileName);
		if(szPrevFileName==NULL)
        	break;
		if(_fstricmp(szPrevFileName,szNewFileName)==0)
			break;
    	};

	if(szPrevFileName)
		delete szPrevFileName;
};

// File functions

#define AUTOGRID 	 0
#define GRIDSIZE	 1
#define TRIANGULATE  2
#define INTERPOLATE  3
#define MORPHMESH    4 
#define WINDOW		 5
#define MAKERESULT	 6
#define POINT_SIZE	 7
#define POINT_DRAG	 8
#define POINT_SHAPE	 9
#define KNOT_SIZE	 10
#define DRAG_START	 11
#define FRAMES_SAME  12
#define UNDO_BUFFER	 13
#define BEEP		 14

 
LPSTR szConfigItemNames[]={	"AutoGrid",				"GridSize",
							"Triangulate",
							"Interpolate",			"MorphMesh",
							"Window",				"MakeResult",
							"PointSize",			"PointDragSize",
							"PointShape",			"KnotWidth",
                            "DragStart",			"FramesSame",
							"UndoBuffer",			"Beep",
							NULL };

LPSTR szTriangulationValues[]={"Edition","Morphing","Command",NULL};

LPSTR szInterpolationValues[]={"During","After",NULL};

LPSTR szPointShapeValues[]={"Rectangle","Circle",NULL};

void TMorphConfig::Load(LPSTR szFilename,TMorphWnd *ParentWnd)
{
	// Open file
	wmfstream file(szFilename,ios::in);

	do{
	 // Check file type
	if(GetKeyword(file,"MorphConfig")==FALSE)          break;

	while(!file.eof()){
		int nKeyword = IdentifyLabel(file,szConfigItemNames);

		switch(nKeyword){
	case AUTOGRID:  	fAutoGrid = (IdentifyKeyword(file,szOnOffValues)==ON);
    					break;
	case GRIDSIZE:		file >> nGridSize;
						break;
	case INTERPOLATE:	nInterpolation = IdentifyKeyword(file,szInterpolationValues);
						break;
	case TRIANGULATE:   nTriangulation = IdentifyKeyword(file,szTriangulationValues);
						break;
	case MORPHMESH:   {
					LPSTR szFileName = new char[_MAX_PATH];

					vReadWord(file,szFileName,_MAX_PATH - 1);

					// Search next free slot in list
					for(int nFiles = 0 ; nFiles < MAX_PREVIOUS_FILES ; nFiles ++)
						if(szFileList[nFiles]==NULL)
							SWAP(LPSTR,szFileList[nFiles],szFileName);

                    delete szFileName;
					break;
						  }
	case WINDOW:{
					// Create new window ;
    				// Window->LoadCfg();
					TMorphEditWnd *pWnd = ParentWnd->MakeMorphWindow();
					pWnd->LoadCfg(file);
					break;
    			};
	case MAKERESULT:	fAutoResultFrames =	(IdentifyKeyword(file,szOnOffValues)==ON);
						break;
	case POINT_SIZE:	file >> nPointSize;
						break;
	case POINT_DRAG:	file >> nPointDragSize;
						break;
	case POINT_SHAPE:	nPointShape = IdentifyKeyword(file,szPointShapeValues);
						break;
	case KNOT_SIZE:		file >> nKnotSize;
						break;
	case FRAMES_SAME:	fKeepFramesTogether = (IdentifyKeyword(file,szOnOffValues)==ON);
						break;
	case UNDO_BUFFER:	file >> nUndoBufferSize;
    					break;
	case DRAG_START:	file >> nPointDragStart;
    					break;
	case BEEP:			fBeepWhenFinished = (IdentifyKeyword(file,szOnOffValues)==ON);
						break;
			};
		};
	} while(FALSE);

    // Close the file
	file.close();
};

void TMorphConfig::Save(LPSTR szFilename,TMorphWnd *Parent)
{
	wmfstream file(szFilename,ios::out);

	file << "MorphConfig\n\n";
 
    // Save auto grid
	file << szConfigItemNames[AUTOGRID]      << " = ";
	file << szOnOffValues[fAutoGrid ? ON : OFF];
	file << "\n";

	file << szConfigItemNames[GRIDSIZE]    << " = ";
	file << nGridSize << "\n";

	// Save triangulation
	file << szConfigItemNames[TRIANGULATE]    << " = ";
	file << szTriangulationValues[nTriangulation] << "\n";

    // Save interpolation
	file << szConfigItemNames[INTERPOLATE]    << " = ";
	file << szInterpolationValues[nInterpolation] << "\n";

	// Save Point size
	file << szConfigItemNames[POINT_SIZE]    << " = ";
	file << nPointSize << "\n";

	// Save Point drag size
	file << szConfigItemNames[POINT_DRAG]    << " = ";
	file << nPointDragSize << "\n";

	// Save point shape
	file << szConfigItemNames[POINT_SHAPE]    << " = ";
	file << szPointShapeValues[nPointShape] << "\n";

	// Save Knot width
	file << szConfigItemNames[KNOT_SIZE]    << " = ";
	file << nKnotSize << "\n";

	// Save Point drag start
	file << szConfigItemNames[DRAG_START]    << " = ";
	file << nPointDragStart << "\n";

	// Save MakeResult
	file << szConfigItemNames[MAKERESULT]      << " = ";
	file << szOnOffValues[fAutoResultFrames ? ON : OFF];
	file << "\n";

	// Save KeepFramesTogether
	file << szConfigItemNames[FRAMES_SAME]      << " = ";
	file << szOnOffValues[fKeepFramesTogether ? ON : OFF];
	file << "\n";

	// Save BeepWhenFinished
	file << szConfigItemNames[BEEP]      << " = ";
	file << szOnOffValues[fBeepWhenFinished ? ON : OFF];
	file << "\n";

	// Save morphmesh file name
    for(int nFiles = 0 ; nFiles < MAX_PREVIOUS_FILES ; nFiles ++ ){
		if(szFileList[nFiles])
			if(szFileList[nFiles][0]!='\0')
				file << szConfigItemNames[MORPHMESH]  << " = " << szFileList[nFiles] << "\n";
		};

	// Save Undo buffer size
	file << szConfigItemNames[UNDO_BUFFER]    << " = ";
	file << nUndoBufferSize << "\n";

    // Save Redo Enabled
/*	file << szConfigItemNames[REDO_ENABLED]      << " = ";
	file << szOnOffValues[fRedoEnabled ? ON : OFF];
	file << "\n";*/

	// Save windows
	TWindow *ActiveChild = Parent->ActiveChild;

	IterateMorphWnds(Index){
		file << szConfigItemNames[WINDOW]<< " = ";
		((TMorphEditWnd*)Index)->SaveCfg(file);
	};
		
	// Close the file
	file.close();
};

/* End of file MORPHCFG.CPP */ 
 

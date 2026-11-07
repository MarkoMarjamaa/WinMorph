/**********************************************************************
 * 
 *  NAME:         MORPHWIN.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when      who what
 *  ------------------------------------------------------------------- 
 *          11. 8.95  MHM added headers 
 *           6. 9.95  MHM removed Interpolation button
 *  1.9466  10.12.95  MHM remove ModulePath etc. new CreateModulePath
 *  1.9470  11.12.95  MHM changed MorphStretchWnd to NewMorphWnd
 *                           now NewMorph & MorphStretch use the same dialog
 *                    MHM Morphmesh is now created with (int,int,int) 
 *  1.9471  11.12.95  MHM MAKEWINDOW-macro, corrected the bug with NewMorph-
 *                   dialogs title
 *  1.9475  12.12.95  MHM CanClose() returns FALSE, if morphing is in progress
 *
 *********************************************************************/ 
 
#include <string.h>     // strlen,_fstrncpy

#include "morphwin.h"   // main class
#include "morphmsh.h"	// class TMorphmesh
#include "acwindow.h"   // class TACWindow
#include "accontex.h"	// class ACContext
#include "acmorph.h"	// class ACMorphContext
#include "morphedw.h"   // class TMorphEditWnd
#include "animwnd.h"    // class TAnimationWnd  
#include "animatio.h"   // class Animation
#include "tpicture.h"   // class TPicture
#include "getint.h"     // class GetInteger
#include "newmorph.h"   // class TNewMorphWnd + TNewMorphOpt
#include "morphcfg.h"   // class tMorphConfig
#include "morphund.h"
#include "clipbrd.h" 
#include "process.h"

TMorphWnd::TMorphWnd(LPSTR ATitle) :
	TMDIFrame(ATitle, "COMMANDS")
{
    szProgName = _fstrdup(ATitle);

	// Init 
	nPlaying = 0;
	nMorphing= 0;

	ResultWnd      = NULL;
	ACWindow       = NULL;
    pClipBoard	   = NULL;

    // Create default null AC context
	ACDefault	   = new ACMorphContext();

	// Create default morphmesh
	Morphmesh = TMorphmeshUndo::Create(1,30,1);
	// Attach new Morphmesh to MorphWindow
	if(Morphmesh)
		Morphmesh->AttachMorphWindow(this);

	hViewMenu = NULL;
};

TMorphWnd::~TMorphWnd()
{
	// Clear ACWindow
	ACWindow->SetACContext(NULL);

	// Delete default ACContext
    if(ACDefault)
	    delete ACDefault;

	// Delete morphmesh
	if(Morphmesh){
		Morphmesh->DetachMorphWindow();
		delete Morphmesh;
		Morphmesh = NULL;
		};
	if(pClipBoard)
    	delete pClipBoard;
};

void TMorphWnd::InitClientWindow()
{
	TMDIFrame::InitClientWindow();

	// Allow Toolbox- window 
	ClientWnd->Attr.Style |= MDIS_ALLCHILDSTYLES | WS_CLIPSIBLINGS;
};

LPSTR   TMorphWnd::GetClassName(){return "MorphWindow";};

void TMorphWnd::GetWindowClass(WNDCLASS& wc)
{
	TMDIFrame::GetWindowClass(wc);

	// Define WinMorph icon
	wc.hIcon = LoadIcon(GetApplication()->hInstance,"Icon");
}

TMorphEditWnd *TMorphWnd::MakeMorphWindow(void)
{
	TMorphEditWnd *pWnd = (TMorphEditWnd *)(GetApplication()->MakeWindow(
		new TMorphEditWnd (this,"Window",new ACMorphContext())));
	return pWnd;
};

void TMorphWnd::SetupWindow()
{
	TMDIFrame::SetupWindow();

    hViewMenu = GetSubMenu(GetMenu(HWindow),6);

	#define MAKEWINDOW(class,parameters) \
		((class *)GetApplication()->MakeWindow( new class parameters));

	// Create Animation Control Window
	ACWindow  = MAKEWINDOW(TACWindow,(this,"Projector",GetMenu(HWindow)));

	// Load initialization file
	MorphConfig.Load(CreateModulePath("winmorph.ini"),this);

	// Tile Windows
	TileChildren();

	// Update previous file names
	FileMenuUpdate();

	// Update window title
	UpdateTitle();

	// Add Cornerpoints
	if(MorphConfig.fAutoGrid){
		Morphmesh->CreateGrid(MorphConfig.nGridSize,MorphConfig.nGridSize,
			(MorphConfig.nTriangulation==ON_EDITION));
		Morphmesh->SetChanged(FALSE);
    }

	// If no old animation is loaded,
	if(Morphmesh->GetAnimation(MorphResult)==NULL){
		// Create default animation
		POINT p = {320,200};
		Morphmesh->SetAnimation(MorphResult,
					new Animation(p,Morphmesh->GetFrames(MorphResult)));
		};

    if(ActiveChild)
	    SetFocus(ActiveChild->HWindow);
};

BOOL TMorphWnd::CanClose()
{
	static BOOL Closing = FALSE;

	if((nMorphing>0)||(nPlaying<0)){
		//Stop ACContexts
		IterateMorphWnds(Index)
			((TMorphEditWnd*)Index)->GetACContext()->Stop();
		PostMessage(HWindow,WM_CLOSE,0,0L);
        return FALSE;
	}

	do{
		// Check if we can close the morphmesh
		if (!Morphmesh->CanClose())
			break;
		// Can close
		Closing = TRUE;
		}
	 while(FALSE);

	if(Closing){
		// Close WinHelp
		WinHelp(HWindow,CreateModulePath("winmorph.hlp"),HELP_QUIT,0L);
		WinHelp(HWindow,"winhelp.hlp",HELP_QUIT,0L);

		MorphConfig.FileListAdd(Morphmesh->GetName());
		FileMenuUpdate();

		// Save MorphConfig to file
		MorphConfig.Save(CreateModulePath("winmorph.ini"),this);
		};

	 return Closing;
};

// Checks if currently active window is a Morphwindow
BOOL TMorphWnd::IsMorphWindowActive(void)
{
    if(ActiveChild)
		return (ActiveChild->isA() == morphwndClass);
    return FALSE;
};

// Sends message to all child windows
void TMorphWnd::SendMessageToAllChildren(UINT message,WPARAM wParam,LPARAM lParam)
{
	IterateMorphWnds(Index)
		SendMessage(Index->HWindow,message,wParam,lParam);
}

void TMorphWnd::DefCommandProc(RTMessage Msg)
{
	switch(Msg.Message){
		case WM_COMMAND:{
			if(	(Msg.WParam >= CM_PREVIOUS_FILES)&&
				(Msg.WParam <  CM_PREVIOUS_FILES + MAX_PREVIOUS_FILES)){
				LoadMorphmesh(MorphConfig.szFileList[Msg.WParam-CM_PREVIOUS_FILES]);
            	};
			if((Msg.WParam >= CM_ACFIRSTCOMMAND)&&
				(Msg.WParam<= CM_ACLASTCOMMAND))
	    	    // Send BN_CLICKED message to ACWindow
				SendMessage(ACWindow->HWindow,WM_COMMAND,Msg.WParam,
					MAKELPARAM(GetDlgItem(ACWindow->HWindow,Msg.WParam),BN_CLICKED));
			};
	    };

	TMDIFrame::DefCommandProc(Msg);
};

// This function is executed always when another child window is activated
// So it's perfect for updating menu states, toolbox etc.
void TMorphWnd::WMQueryNewPalette(TMessage &Msg)
{
	// Update menu
	UpdateMenuState();

	if(ActiveChild){
		if(ActiveChild->HWindow){
			// Bring AC Window to top
			SetWindowPos(ACWindow->HWindow,HWND_TOP,
				0, 0, 0, 0,SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

			// Update Palettes
			SendMessage(ActiveChild->HWindow,WM_FIRST + WM_QUERYNEWPALETTE,0,0L);

			Msg.Result = TRUE;
			return;
			}
		}
	Msg.Result = FALSE;
	return;
};

void TMorphWnd::UpdateACContext(void)
{
    if(ACWindow)
		if(IsMorphWindowActive())
			// Use windows AC, if available
			ACWindow->SetACContext(
				(ACMorphContext*)((TMorphEditWnd*)ActiveChild)->GetACContext());
		   else
			// Otherwise use default null context
			ACWindow->SetACContext(ACDefault);
};

void TMorphWnd::UpdateTitle(void)
{
	char szNewTitle[50 + _MAX_PATH];
	char *szFileName = "Untitled";

	if(Morphmesh)
        if(Morphmesh->GetName())
			szFileName = Morphmesh->GetName();

	wsprintf(szNewTitle,"%s-[%s]",szProgName,szFileName);
	SetCaption(szNewTitle);
};

void TMorphWnd::WMPaletteChanged(TMessage& Msg)
{
	SendMessageToAllChildren(WM_FIRST + WM_PALETTECHANGED,(WPARAM)Msg.WParam,0L);
};

void TMorphWnd::SetMorphmesh(TMorphmeshUndo* NewMorph)
{
	assert(Morphmesh);

	if(NewMorph==NULL){
		assert(FALSE);
		return;// Do nothing
		  };

	// Check can we close the morphmesh
	BOOL fCanClose = TRUE;
	if(Morphmesh){
		fCanClose = Morphmesh->CanClose();
		};

	 // It's OK to assign the new morphmesh
	 if(fCanClose){
		// Detach MorphWindow from old Morphmesh
		if(Morphmesh){
			Morphmesh->DetachMorphWindow();

            // Save old file name to list 
			MorphConfig.FileListAdd(Morphmesh->GetName());
			FileMenuUpdate();
			};

		// Set new Morphmesh
		SWAP(TMorphmeshUndo*,Morphmesh,NewMorph);

		// Attach new Morphmesh to MorphWindow
		if(Morphmesh)
			Morphmesh->AttachMorphWindow(this);
		};

    // Update Title
	UpdateTitle();

	// Delete morphmesh not needed
	if(NewMorph)
		delete NewMorph;
};

void TMorphWnd::SetClipBoard(TClipBoard *pNewClipBoard)
{
    // delete old clipboard
	if(pClipBoard)
		delete pClipBoard;

    // Set new clipboard 
	pClipBoard = pNewClipBoard;

	// update menu
	UpdateSelectionMenu();
};

//////////////////////
// Menu commands
/////////////////////
// File menu

void TMorphWnd::CMFileNew(RTMessage)
{
	TNewMorphOpt Options;

	// Initialize window data with MorphBlocks data
	for( int nWindow = 0 ; nWindow < 3 ; nWindow++){
		Options.nFrames[nWindow] = Morphmesh->GetFrames(nWindow);
		if(Morphmesh->GetAnimation(nWindow))
			Options.nAnimFrames[nWindow] =
				Morphmesh->GetAnimation(nWindow)->GetFrames();
			else
			Options.nAnimFrames[nWindow] = 0;
		};

	 // Open window
	if(GetModule()->ExecDialog( new
		TNewMorphWnd(this,&Options,"New morphmesh"))==IDOK){

		// Create new morphmesh
		TMorphmeshUndo *NewMorph = TMorphmeshUndo::Create(
			Options.nFrames[MorphStart],
			Options.nFrames[MorphResult],
			Options.nFrames[MorphEnd]);

		  // If succeeded
		if(NewMorph){
            // Add Cornerpoints
			if(MorphConfig.fAutoGrid){
				NewMorph->CreateGrid(MorphConfig.nGridSize,MorphConfig.nGridSize,
					(MorphConfig.nTriangulation==ON_EDITION));
				NewMorph->SetChanged(FALSE);
            }

			// assign new morphmesh
			SetMorphmesh(NewMorph);
			};
		};
};

void TMorphWnd::CMFileOpen(TMessage&)
{
	// Load morphmesh
	LoadMorphmesh((LPSTR)NULL);
};

void TMorphWnd::LoadMorphmesh(LPSTR szFileName)
{
	 // Load new Morphmesh
	TMorphmeshUndo* NewMorph = TMorphmeshUndo::Create(szFileName);

	// If succeeded
	if(NewMorph){
		// Assign new Morphmesh
		SetMorphmesh(NewMorph);
		};
};

// Save morphmesh with old file name
void TMorphWnd::CMFileSave(RTMessage)  { Morphmesh->Save();};

// Save morphmesh with new file name
void TMorphWnd::CMFileSaveAs(RTMessage)
{
	Morphmesh->SaveAs();
    UpdateTitle();
};

#define SETMENUSTATE(bState) ((bState ? (MF_ENABLED) : (MF_DISABLED | MF_GRAYED)) | MF_BYCOMMAND)
#define CHECKMENUSTATE(nMenuItem,nMenuState) \
	CheckMenuItem(GetMenu(HWindow),(nMenuItem), MF_BYCOMMAND | \
	  ((nMenuState) ? MF_CHECKED : MF_UNCHECKED ));

// Enable/Disable one menu item
void TMorphWnd::SetMenuItem(int nMenuItem,BOOL bState)
{
	EnableMenuItem(GetMenu(HWindow),nMenuItem,SETMENUSTATE(bState));
};

// Update menu, when it's state changes
void TMorphWnd::UpdateMenuState(void)
{
	if(HWindow==NULL)
		return;

	BOOL bMorphLocked = (nMorphing==0);

	BOOL bMenuState = bMorphLocked;
	SetMenuItem(CM_COMPUTE_INTERPOLATE,		bMenuState);
	SetMenuItem(CM_COMPUTE_SPHERE,			bMenuState);
	SetMenuItem(CM_COMPUTE_EARTH,			bMenuState);
	SetMenuItem(CM_COMPUTE_WHIRLPOOL,		bMenuState);
	SetMenuItem(CM_COMPUTE_YINYANG,			bMenuState);

	bMenuState = bMorphLocked && (nPlaying==0);
	SetMenuItem(CM_FILENEW,				bMenuState);
	SetMenuItem(CM_FILEOPEN,			bMenuState);
	SetMenuItem(CM_STRETCH_FRAMES,		bMenuState);
	/* Swap blocks */
	EnableMenuItem(GetSubMenu(GetMenu(HWindow),2),1,
		(bMenuState ? (MF_ENABLED) : (MF_DISABLED | MF_GRAYED) )|MF_BYPOSITION);

    // Update menu items connected to selections
    UpdateSelectionMenu();

	bMenuState = bMorphLocked && IsMorphWindowActive();
	SetMenuItem(CM_TRIANGULATEALL,		bMenuState);

	bMenuState = bMorphLocked && (nPlaying==0) && IsMorphWindowActive();
	HMENU hAnimSubMenu  = GetSubMenu(GetSubMenu(GetMenu(HWindow),0),5);

	EnableMenuItem(hAnimSubMenu,CM_ANIM_NEW,	SETMENUSTATE(bMenuState));
	EnableMenuItem(hAnimSubMenu,CM_ANIM_OPEN,	SETMENUSTATE(bMenuState));

	if(bMenuState){
		bMenuState = (((TMorphEditWnd*)ActiveChild)->GetAnimation()!=NULL);
    };

	EnableMenuItem(hAnimSubMenu,CM_ANIM_SAVE,	SETMENUSTATE(bMenuState));
	EnableMenuItem(hAnimSubMenu,CM_ANIM_SAVEAS,	SETMENUSTATE(bMenuState));
	EnableMenuItem(hAnimSubMenu,CM_ANIM_INFO,	SETMENUSTATE(bMenuState));

	// Update triangulation menu
	CHECKMENUSTATE(CM_TRI_DURING_EDIT,		(MorphConfig.nTriangulation==ON_EDITION ));
	CHECKMENUSTATE(CM_TRI_BEFORE_MORPH,		(MorphConfig.nTriangulation==ON_MORPHING));
	CHECKMENUSTATE(CM_TRI_ON_COMMAND,		(MorphConfig.nTriangulation==ON_COMMAND ));

    // Update interpolation menu
	CHECKMENUSTATE(CM_INT_DURING_EDIT,		(MorphConfig.nInterpolation==DURING_MOVING ));
	CHECKMENUSTATE(CM_INT_AFTER_EDIT,		(MorphConfig.nInterpolation==AFTER_MOVING  ));

	// Update point shape menu
	CHECKMENUSTATE(CM_POINT_SHAPE_RECT,		(MorphConfig.nPointShape==POINT_SHAPE_RECT ));
	CHECKMENUSTATE(CM_POINT_SHAPE_CIRCLE,	(MorphConfig.nPointShape==POINT_SHAPE_CIRCLE ));

	// Update KeepFramesTogether menu
	CHECKMENUSTATE(CM_FRAMES_THE_SAME,		MorphConfig.fKeepFramesTogether);

	// Update BeepWhenFinished menu
	CHECKMENUSTATE(CM_BEEP,					MorphConfig.fBeepWhenFinished);

	// Update Cornerpoints menu
	CHECKMENUSTATE(CM_AUTO_GRID,			MorphConfig.fAutoGrid);

	// Make result points
	CHECKMENUSTATE(CM_AUTOINTERFRAMES,		MorphConfig.fAutoResultFrames);

	// Redo enabled
	CHECKMENUSTATE(CM_REDO_ENABLED,			MorphConfig.fRedoEnabled);

	UpdateUndoMenu();
};

void TMorphWnd::UpdateSelectionMenu(void)
{
	BOOL bMorphLocked = (nMorphing==0);

	BOOL bMenuState = bMorphLocked && Morphmesh->SelectionsAre();
	SetMenuItem(CM_CUT,				bMenuState);
	SetMenuItem(CM_CLEAR,			bMenuState);
	SetMenuItem(CM_DELTRIBETWEEN,	bMenuState);
	SetMenuItem(CM_DELTRIWITH,		bMenuState);

	bMenuState = bMorphLocked &&
		Morphmesh->SelectionsAre() && IsMorphWindowActive();
	SetMenuItem(CM_COPY,			bMenuState);
	SetMenuItem(CM_COPY_FRAMES,		bMenuState);
	/*SetMenuItem(CM_TRIANGULATE,		bMenuState);*/
    /* Points | Detach points */
	EnableMenuItem(GetSubMenu(GetMenu(HWindow),4),0,
		(bMenuState ? (MF_ENABLED) : (MF_DISABLED | MF_GRAYED) )|MF_BYPOSITION);

	bMenuState = bMorphLocked && IsMorphWindowActive() && pClipBoard;
	SetMenuItem(CM_PASTE,			bMenuState);
};

void TMorphWnd::UpdateUndoMenuItem(int nMenuItem,LPSTR szMenuName,BOOL fEnabled,LPSTR szUndoName,LPSTR szKeyName)
{
	char szMenuText[80];

	if(szUndoName != NULL)
		wsprintf(szMenuText,"%s %s%s",szMenuName,szUndoName,szKeyName);
	   else 
		wsprintf(szMenuText,"%s%s",szMenuName,szKeyName);
		
	UINT fuFlags = ((fEnabled&& (szUndoName != NULL))? MF_ENABLED : (MF_DISABLED | MF_GRAYED))
			| MF_BYCOMMAND | MF_STRING;

	//Update menu text
	ModifyMenu(GetMenu(HWindow),nMenuItem, fuFlags,nMenuItem,szMenuText);
};

void TMorphWnd::UpdateUndoMenu(void)
{
    if(HWindow){
		BOOL bMorphLocked = (nMorphing==0);

		UpdateUndoMenuItem(CM_UNDO,"&Undo",bMorphLocked,
			Morphmesh->GetUndoName(),"\bAlt+BkSp");
		UpdateUndoMenuItem(CM_REDO,"&Redo",bMorphLocked,
			Morphmesh->GetRedoName(),"\bAlt+Shift+BkSp");
    };
};

void TMorphWnd::FileMenuUpdate(void)
{
	HMENU hFileMenu = GetSubMenu(GetMenu(HWindow),0);

	// Delete file names
	for(int nFiles = 0 ; nFiles < MAX_PREVIOUS_FILES ; nFiles ++ )
		DeleteMenu(hFileMenu,CM_PREVIOUS_FILES + nFiles , MF_BYCOMMAND);

	// Set file menu separator
	DeleteMenu(hFileMenu,8,MF_BYPOSITION);
	if(MorphConfig.szFileList[0])
		AppendMenu(hFileMenu,MF_BYCOMMAND | MF_SEPARATOR | MF_ENABLED | MF_UNCHECKED,0,0L);

	// Add new file names
	for(nFiles = 0 ; nFiles < MAX_PREVIOUS_FILES ; nFiles ++ ){
		char szTempFileName[_MAX_PATH + 10 ];
		if(MorphConfig.szFileList[nFiles]){
			wsprintf(szTempFileName,"&%d %s",nFiles+1,MorphConfig.szFileList[nFiles]);
			AppendMenu(hFileMenu,MF_BYCOMMAND | MF_STRING | MF_ENABLED | MF_UNCHECKED,
				CM_PREVIOUS_FILES + nFiles,szTempFileName);
		}
	};
};

void TMorphWnd::CMUndo(RTMessage)
{
	Morphmesh->Undo();
};

void TMorphWnd::CMRedo(RTMessage)
{
	Morphmesh->Redo();
};

// Select all points
void TMorphWnd::CMSelectAll(RTMessage)
{
	Morphmesh->SelectionAddAll();
};

void TMorphWnd::CMUndoBufferSize(RTMessage)
{
	if(MorphConfig.SetUndoBufferSize(this))
		Morphmesh->UndoRemoveLeftOvers();
};

void TMorphWnd::CMRedoEnabled(RTMessage)
{
	MorphConfig.fRedoEnabled = !MorphConfig.fRedoEnabled;
	Morphmesh->DeleteRedo();
	UpdateMenuState();
};

// Stretch number of morph frames
void TMorphWnd::CMStretchFrames(RTMessage)
{
	TNewMorphOpt Options;

	// Initialize window data with MorphBlocks data
	for( int nWindow = 0 ; nWindow < 3 ; nWindow++){
		Options.nFrames[nWindow] = Morphmesh->GetFrames(nWindow);
		if(Morphmesh->GetAnimation(nWindow))
			Options.nAnimFrames[nWindow] =
				Morphmesh->GetAnimation(nWindow)->GetFrames();
			else
			Options.nAnimFrames[nWindow] = 0;
		};

	// Open window
	if(GetModule()->ExecDialog( new
		TNewMorphWnd(this,&Options,"Stretch frames"))==IDOK){

		// Set hourglass cursor
		HCURSOR hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));

		// Stretch Morphblocks
		Morphmesh->StretchFrames(Options.nFrames);

		// Set normal cursor
		SetCursor(hcurSave);
		};
};

void TMorphWnd::CMKeepFramesTogether(RTMessage)
{
	MorphConfig.fKeepFramesTogether = !MorphConfig.fKeepFramesTogether;
	UpdateMenuState();
};

void TMorphWnd::CMSwapFrames1(RTMessage)
{
	Morphmesh->SwapBlocks(MorphStart,MorphEnd);
};

void TMorphWnd::CMSwapFrames2(RTMessage)
{
	Morphmesh->SwapBlocks(MorphStart,MorphResult);
};

void TMorphWnd::CMSwapFrames3(RTMessage)
{
	Morphmesh->SwapBlocks(MorphEnd,MorphResult);
};

void TMorphWnd::CMTrianglesDeleteBetweenPoints(RTMessage)
{
	Morphmesh->TrianglesDeleteBetweenPoints();
};

void TMorphWnd::CMTrianglesDeleteWithPoints(RTMessage)
{
	Morphmesh->TrianglesDeleteWithPoints();
};

void TMorphWnd::CMComputeInterpolate(RTMessage)
{
	Morphmesh->Compute(new TInterpolate());
};

void TMorphWnd::CMComputeSphere(RTMessage)
{
	Morphmesh->Compute(new TSphere());
};

void TMorphWnd::CMComputeEarth(RTMessage)
{
	Morphmesh->Compute(new TEarth());
};

void TMorphWnd::CMComputeWhirlpool(RTMessage)
{
	Morphmesh->Compute(new TWhirlpool());
};

void TMorphWnd::CMComputeYinYang(RTMessage)
{
	Morphmesh->Compute(new TYinYang());
};

void TMorphWnd::CMAutoInterpolateFrames(RTMessage)
{
	MorphConfig.fAutoResultFrames = !MorphConfig.fAutoResultFrames;
    UpdateMenuState();
};

void TMorphWnd::CMTriDuringEdit(RTMessage)
{
	MorphConfig.nTriangulation = ON_EDITION;
    UpdateMenuState();
};

void TMorphWnd::CMTriBeforeMorph(RTMessage)
{
	MorphConfig.nTriangulation = ON_MORPHING;
    UpdateMenuState();
};

void TMorphWnd::CMTriOnCommand(RTMessage)
{
	MorphConfig.nTriangulation = ON_COMMAND;
    UpdateMenuState();
};

void TMorphWnd::CMIntDuringEdit(RTMessage)
{
	MorphConfig.nInterpolation = DURING_MOVING;
    UpdateMenuState();
};

void TMorphWnd::CMIntAfterEdit(RTMessage)
{
	MorphConfig.nInterpolation = AFTER_MOVING;
    UpdateMenuState();
};

void TMorphWnd::CMCreateGrid(RTMessage)
{
	int nGridSize = 1;

	GetApplication()->ExecDialog( new
		GetInteger(this,"Set grid size",&nGridSize));

	Morphmesh->CreateGrid(nGridSize,nGridSize,
					(MorphConfig.nTriangulation==ON_EDITION));

};

void TMorphWnd::CMAutoGrid(RTMessage)
{
	MorphConfig.fAutoGrid = !MorphConfig.fAutoGrid;
    UpdateMenuState();
};

void TMorphWnd::CMSetGridSize(RTMessage)
{
	MorphConfig.SetGridSize(this);
};

void TMorphWnd::CMBeepWhenFinished(RTMessage)
{
	MorphConfig.fBeepWhenFinished = !MorphConfig.fBeepWhenFinished;
	UpdateMenuState();
};

void TMorphWnd::CMShowProjector(RTMessage)
{
	BOOL fShow = (IsWindowVisible(ACWindow->HWindow)==FALSE);
	// TRUE, if window has to be shown

	ShowWindow(ACWindow->HWindow,fShow ? SW_SHOW : SW_HIDE );

	CheckMenuItem(GetMenu(HWindow),CM_SHOW_PROJECTOR,
		(fShow ? MF_CHECKED : MF_UNCHECKED ) | MF_BYCOMMAND );
};

void TMorphWnd::CMMakeView(RTMessage)
{
	MakeMorphWindow();
};

/////////////////////
// Config menu

void TMorphWnd::CMSetPointSize(RTMessage)
{  if(MorphConfig.SetPointSize(this))      InvalidateAll();};

void TMorphWnd::CMSetPointDragSize(RTMessage)
{  if(MorphConfig.SetPointDragSize(this))	InvalidateAll();};

void TMorphWnd::CMSetPointDragStart(RTMessage)
{  MorphConfig.SetPointDragStart(this);};

void TMorphWnd::CMPointShapeRect(RTMessage)
{
	MorphConfig.nPointShape = POINT_SHAPE_RECT;
    InvalidateAll();
	UpdateMenuState();
};

void TMorphWnd::CMPointShapeCircle(RTMessage)
{
	MorphConfig.nPointShape = POINT_SHAPE_CIRCLE;
	InvalidateAll();
	UpdateMenuState();
};

void TMorphWnd::CMSetKnotSize(RTMessage)
{  if(MorphConfig.SetKnotSize(this))	InvalidateAll();};

void TMorphWnd::CMSetPointColor(RTMessage)
{  if(MorphConfig.SetPointColor(this))    InvalidateAll();};

void TMorphWnd::CMSetSelectedColor(RTMessage)
{  if(MorphConfig.SetSelectedColor(this)) InvalidateAll();};

void TMorphWnd::CMSetLineColor(RTMessage)
{  if(MorphConfig.SetLineColor(this))     InvalidateAll();};

//////////////////
// Help functions

void TMorphWnd::CMAbout(RTMessage)
{   GetApplication()->ExecDialog(new TDialog(this, "About"));};

void TMorphWnd::CMHelpIndex(RTMessage)
{  WinHelp(HWindow,CreateModulePath("winmorph.hlp"),HELP_CONTENTS,0L);};

void TMorphWnd::CMHelpOnHelp(RTMessage)
{  WinHelp(HWindow,"winhelp.hlp",HELP_HELPONHELP,0L);};

// Gets WinMorphs path and adds the filename to it
// Can be used to create INI&HLP- filenames 
LPSTR TMorphWnd::CreateModulePath(LPSTR szFilename)
{
	 static char szModulePath[_MAX_PATH];

	 // Get WinMorphs path
	GetModuleFileName(GetModule()->hInstance,(LPSTR)szModulePath,_MAX_PATH);

	char szDrive[_MAX_DRIVE];
	char szDir  [_MAX_DIR  ];
	char szFname[_MAX_FNAME];
	char szExt  [_MAX_EXT  ];
	 // Separate path from filename 'winmorph.exe'
	_splitpath(szModulePath,szDrive,szDir,szFname,szExt);

	 // Make path again the whole
	_makepath(szModulePath,szDrive,szDir,NULL,NULL);

	 // Add filename to path
	_fstrcat(szModulePath,szFilename);

	 // and return the resulting file path 
	return szModulePath;
};

////////////////////////////////////////
// Morphstruct -> EditWindows commands
//////////////////////////////////////////

#define  CALL_ALL(command)     							\
		IterateMorphWnds(Index)                         \
			((TMorphEditWnd *)Index)->command;          \

void TMorphWnd::InvalidateAll(void)
{
	IterateMorphWnds(Index)
		InvalidateRect(Index->HWindow,NULL,FALSE);
};

void TMorphWnd::FramesChanged(void)
{   
	CALL_ALL(FramesChanged());
};

void TMorphWnd::PointAdded(void)
{
	CALL_ALL(ViewPointNew(););
};

void TMorphWnd::PointDeleted(int nPoint)
{
	CALL_ALL(ViewPointDelete(nPoint));
};

void TMorphWnd::PointInvalidate(int nPoint)
{
	CALL_ALL(ViewPointInvalidate(nPoint););
};

void TMorphWnd::InvalidateTriangle(TTriangle&)
{
	InvalidateAll();// A little shortcut here 
};

void TMorphWnd::ViewPointsUpdate(int nNewPoints)
{
	CALL_ALL(ViewPointsUpdate(nNewPoints););
};

void TMorphWnd::ViewPointUpdate(int nPoint)
{
	CALL_ALL(ViewPointUpdate(nPoint););
};

void TMorphWnd::ConnectedRectAddPoints(ItemList *SelectionList)
{
	CALL_ALL(ConnectedRectAddPoints(SelectionList););
};

void TMorphWnd::SelectionMoved(int nBlock,ItemList *SelectionList)
{
	CALL_ALL(SelectionMoved(nBlock,SelectionList););
};

void TMorphWnd::AnimationChanged(int nPicture,Animation *pNewAnim)
{
	CALL_ALL(AnimationChanged(nPicture,pNewAnim););
};

void TMorphWnd::AnimationFrameChanged(int nPicture,int nFrame)
{
	CALL_ALL(AnimationFrameChanged(nPicture,nFrame););
};

void TMorphWnd::InvalidateAnimFrame(int nPicture,int nFrame)
{
	CALL_ALL(InvalidateAnimFrame(nPicture,nFrame););
};

/* End of file MORPHWIN.CPP */
 

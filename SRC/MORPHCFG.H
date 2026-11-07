/********************************************************************** 
 * 
 *  NAME:         MORPHCFG.H 
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
 
#ifndef MORPHCFG_H
#define MORPHCFG_H

#ifndef __OWL_H
	#include <owl.h>
#endif
#include <stdlib.h>

#include "filefunc.h"

class TWindow;
class TMorphWnd;

// Triangulation types
#define ON_EDITION		0
#define ON_MORPHING		1
#define ON_COMMAND		2 

// Interpolation types
#define DURING_MOVING	0
#define AFTER_MOVING    1

// Point Shape types
#define POINT_SHAPE_RECT	0
#define POINT_SHAPE_CIRCLE	1

#define MAX_PREVIOUS_FILES 5
#define MAX_MORPH_VIEWS    32

#define ON   0
#define OFF  1
extern LPSTR szOnOffValues[];

class TMorphConfig {
public:
	int 	nInterpolation;
	int 	nTriangulation;
	BOOL 	fAutoResultFrames;
	BOOL	fKeepFramesTogether;
	BOOL	fBeepWhenFinished;

	LPSTR szFileList[MAX_PREVIOUS_FILES];

	int		nUndoBufferSize;
	BOOL	fRedoEnabled;

	int		nPointSize;
    int		nPointDragSize;
	int		nPointShape;
	int		nKnotSize;
    int		nPointDragStart;

	BOOL    fAutoGrid;
	int     nGridSize;

	HBRUSH	NormalInside;
	HBRUSH	SelectedInside;

	HPEN	NormalBorder;
	HPEN	NormalBorderKnot;
	HPEN	SelectedBorder;
	HPEN	SelectedBorderKnot;
	HPEN	LineBorder;

	TMorphConfig	();
    ~TMorphConfig	();

	void Init		(void);
	void Destroy	(void);

	void Load		(LPSTR szFilename,TMorphWnd *Parent);
	void Save		(LPSTR szFilename,TMorphWnd *Parent);

	BOOL SetPointSize		(TWindow*);
	BOOL SetPointDragSize	(TWindow*);
	BOOL SetKnotSize		(TWindow*);
	BOOL SetGridSize		(TWindow*);
	BOOL SetPointDragStart  (TWindow*);
	BOOL SetPointColor		(TWindow*);
	BOOL SetSelectedColor	(TWindow*);
	BOOL SetLineColor		(TWindow*);
	void FileListAdd		(LPSTR szNewFileName);
	BOOL SetUndoBufferSize	(TWindow*);

protected:
	BOOL SetNewValue(TWindow *Wnd,int *pValue,LPSTR pszWindowTitle);
	BOOL GetColorFromUser	(TWindow*,COLORREF& color);
    COLORREF Point;
	COLORREF Selected;
	COLORREF Line;
};

extern TMorphConfig MorphConfig;// Only one instance

#endif

/* End of file MORPHCFG.H */ 
 

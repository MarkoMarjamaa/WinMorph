/********************************************************************** 
 * 
 *  NAME:         WINMORPH.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  11.8.95  MHM     added headers 
 *  16.9.95  MHM     corrected the MDI child list place
 *********************************************************************/
 
#include "winmorph.h"
#include "morphcfg.h"

HINSTANCE hInstance;

void TMorphApp::InitMainWindow()
{
  MainWindow = new TMorphWnd(Name);
  ((TMDIFrame*)MainWindow)->ChildMenuPos = 8;
};

void TMorphApp::InitInstance()
{
	TApplication::InitInstance();
	HAccTable = LoadAccelerators(hInstance, "Accelerators");
    ::hInstance=hInstance;
}

LPSTR ATitle="WinMorph v2.00";

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
  LPSTR lpCmdLine, int nCmdShow)
{
  TMorphApp MorphApp(ATitle, hInstance, hPrevInstance,
    lpCmdLine, nCmdShow);
  MorphApp.Run();
  return MorphApp.Status;
}; 
 
/* End of file WINMORPH.CPP */ 
 

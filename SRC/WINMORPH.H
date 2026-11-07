/********************************************************************** 
 * 
 *  NAME:         WINMORPH.H 
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
 
#ifndef __OWL_H
	#include <owl.h>
#endif
#include "morphwin.h"

class TMorphApp : public TApplication
{
public:
  TMorphApp(LPSTR AName, HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
    : TApplication(AName, hInstance, hPrevInstance, lpCmdLine, nCmdShow) {};
  virtual void InitMainWindow();
  virtual void InitInstance();
};
 
 
/* End of file WINMORPH.H */ 
 

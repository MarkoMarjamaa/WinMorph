/**********************************************************************
 * 
 *  NAME:         PROCESS.CPP 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  25.10.95  MHM     created 
 *  1.9475  12.12.95 MHM Process has been completely changed, earlier it had
 *   a message loop which called the appropriate function, now the function is 
 *   called, which in turn yields the control to message loop. (Preparing 
 *   the morpher for the Animation Control). The WM_QUIT- message is handled
 *   by not letting the application close when the morpher is running
 * 
 *********************************************************************/ 

#include <windows.h>
#include "process.h"

TProcess::TProcess(void)
{
	Canrun = FALSE;
	Running = FALSE;
	fPaused = FALSE;
};

void TProcess::Yield(void)
{
	MSG msg;

	// Peek the message queue
	do {
		while(PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE)){
			if(msg.message == WM_CLOSE){
					// If WM_CLOSE message is found
					//assert(!Running);
					// Stop everything
					Stop();
				break;
				}
				  else {
				// Normal message handler
				GetMessage(&msg ,NULL ,0 ,0);
				TranslateMessage (&msg) ;
				DispatchMessage (&msg) ;
				};
			};
		}
	while(fPaused);// Is process is paused, continue messageloop
};

void TProcess::Stop(void)
{
	Running = FALSE;
};

/* End of file PROCESS.H */

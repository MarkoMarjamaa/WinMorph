/********************************************************************** 
 * 
 *  NAME:         PROCESS.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what
 *  ------------------------------------------------------------------- 
 *          11. 8.95 MHM added headers 
 *  1.9475  12.12.95 MHM Process has been completely changed, earlier it had
 *   a message loop which called the appropriate function, now the function is 
 *   called, which in turn yields the control to message loop. (Preparing 
 *   the morpher for the Animation Control)
 * 
 *********************************************************************/ 

#ifndef PROCESS_H
#define PROCESS_H

#ifndef __WINDOWS_H
	#include <windows.h>
#endif
#include <assert.h>

//////////////////////////////////////////////////////
// TProcess
//////////////////////////////////////////////////////

class TProcess {
protected:
	BOOL Canrun;
	BOOL Running;

public:
	TProcess(void);
	~TProcess(){};

	BOOL CanRun(void)    { return Canrun; };

	virtual void Yield  (void);
	virtual void Stop (void);

	BOOL fPaused;
};

#endif
 
 
/* End of file PROCESS.H */ 

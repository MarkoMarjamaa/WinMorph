/********************************************************************** 
 * 
 *  NAME:         ACCONTEX.H
 * 
 *  DESCRIPTION:
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  18.9.95  MHM     created
 * 
 *********************************************************************/ 
 
#ifndef ACCONTEX_H
#define ACCONTEX_H

#include "common.h"
#ifndef __WINDOWS_H
	#include <windows.h>
#endif

// define Play states
#define ACPLAY_STOPPED		0
#define ACPLAY_PLAYING		1
#define ACPLAY_PAUSED		2

class ACWindow;
class ACContextCfg;

class ACContext
{
	friend TACWindow;
protected:
	class  TACWindow* ACW;// Toolbox window

	int 	nFrame;
	int		nNextFrame;

	int		nPlayMode;
	int		nDirection;

	virtual BOOL GetNextFrame	(void);
	virtual void SetFrame	(void);
	virtual void SetFrameN	(int nNewFrame);
public:
	ACContext(void);
	~ACContext();

	int	nPlayState;

	int		nFrames;
	int 	nMarkIn;
	int 	nMarkOut;

	virtual void Config		(void);

	virtual void Play		(void);
	virtual void PlayFrame	(void);
	virtual void Stop		(void);

	virtual LPSTR GetStateName(void);

	virtual void SetFrames	(int nNewFrames);
	virtual void SetPause(BOOL fNewPauseState);

			void Attach		(TACWindow* NewACW);
			void Detach		(void);
};

#endif

/* End of file ACCONTEXT.H */ 
 

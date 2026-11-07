/**********************************************************************
 * 
 *  NAME:         ACCONTEX.CPP
 * 
 *  DESCRIPTION:
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  1.9480  13.12.95 MHM created 
 * 
 *********************************************************************/ 
 
#include "accontex.h"
#include "acwindow.h"
#include "accontcf.h"

ACContext::ACContext(void)
{
	nPlayState	= ACPLAY_STOPPED;
	ACW			= NULL;
	nDirection 	= 0;

	SetFrames(0);

	nPlayMode = PINGPONG;
};

ACContext::~ACContext()
{
	assert(nPlayState == ACPLAY_STOPPED);
    assert(ACW == NULL);
};

void ACContext::Config(void){};

LPSTR ACContext::GetStateName(void)
{
	LPSTR szStateName;

	switch(nPlayState){
		case ACPLAY_STOPPED:
			szStateName = "Stopped";
			break;
		case ACPLAY_PAUSED:
			szStateName = "Paused";
			break;
		case ACPLAY_PLAYING:
			szStateName = "Playing";
			break;
		};

	return szStateName;
};

void ACContext::Play(void)
{
    // Set first frame
	nNextFrame = nMarkIn;

    assert(nDirection==0);
    // Play direction is forward
    nDirection = 1;

    nPlayState = ACPLAY_PLAYING;

	// Update play buttons in ACW
	if(ACW){
		ACW->UpdateContext();
		};
};

void ACContext::SetPause(BOOL fNewPauseState)
{
	switch(nPlayState){
		case ACPLAY_STOPPED:
            // You can't pause a stopped animation
			assert(FALSE);
            break;
		case ACPLAY_PLAYING:
			assert(fNewPauseState==TRUE);
            nPlayState = ACPLAY_PAUSED;
			break;
		case ACPLAY_PAUSED:
			assert(fNewPauseState==FALSE);
			nPlayState = ACPLAY_PLAYING;
			break;
        };
};

void ACContext::Stop(void)
{
    if(nPlayState != ACPLAY_STOPPED){
		nDirection = 0;

		nPlayState = ACPLAY_STOPPED;

		// Update play buttons in ACW
		if(ACW){
			ACW->UpdateContext();
			};
		};
};

void ACContext::PlayFrame(void)
{
	if(nPlayState!=ACPLAY_PAUSED){
		// Set new frame
		SetFrame();

		// Calculate next frame
		if(!GetNextFrame())
	        // If this was the last frame, stop the playing
			if(nPlayState!=ACPLAY_STOPPED)
				Stop();

		// Update frame in ACW scrollbar
		if(ACW)
			ACW->SetFrame(nFrame);
		};
};

BOOL ACContext::GetNextFrame(void)
{
	int nMovement = nDirection;

	do{
		// Add movement
		nNextFrame += nMovement;
		nMovement = 0;

		// Only way to stop this thing
		if(((nNextFrame == nMarkIn - 1 )||(nNextFrame == nMarkOut + 1))
				&&(nPlayMode==NORMAL))
           	return FALSE;

		// Cut off the edges
		if(nNextFrame > nMarkOut){
			nMovement = nNextFrame - nMarkOut;
			nNextFrame = nMarkOut;
            };
		if(nNextFrame < nMarkIn){
			nMovement = nNextFrame - nMarkIn;
			nNextFrame = nMarkIn;
            };

		if(nMovement != 0)// Still going on, still going strong
			switch(nPlayMode){
				case NORMAL:
					nMovement = 0;
	                break;
				case LOOP:
					nNextFrame = nMarkIn - 1;
					break;
				case PINGPONG:
					if(nMarkIn == nMarkOut) /* To prevent eternal loop */
                    	nMovement = 0;
					nMovement  *= -1;
					nDirection *= -1;
					break;
				}
		}
	while(nMovement !=0);

    return TRUE;
};

void ACContext::SetFrame(void)
{
	// Set new frame
	nFrame = nNextFrame;
};

void ACContext::SetFrameN(int nNewFrame)
{
	// Set new frame
	nNextFrame = nNewFrame;

	// Set new frame to window, get next frame if playing and update ACWindow
	PlayFrame();
};

void ACContext::SetFrames(int nNewFrames)
{
	assert(nPlayState==ACPLAY_STOPPED);

	nFrames = nNewFrames;

	nFrame		= 0;
	nNextFrame	= 0;

	nMarkIn		= 0;
	nMarkOut	= nNewFrames - 1;

    // If Context is attached to ACW, update ACWs number of frames
	if(ACW){
		ACW->UpdateContext();
    	};
};

// Attach AC Window to Context (this context's window now has focus)
void ACContext::Attach(TACWindow* NewACW)
{
	assert(ACW==NULL);

    ACW = NewACW;
};

// Detach AC Window from Context (some other window got focus) 
void ACContext::Detach(void)
{
	assert(ACW!=NULL);

    ACW = NULL;
};

/* End of file ACCONTEX.CPP */


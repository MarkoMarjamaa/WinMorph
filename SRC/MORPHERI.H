/********************************************************************** 
 * 
 *  NAME:         MORPHERI.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          11. 8.95 MHM added headers 
 *  1.9475  12.12.95 MHM Morpher has been completely changed, earlier it had
 *   a message loop which called the Morpher, now the Morph-function is called,
 *   which in turn calls the message loop when it seems it's necessary. Preparing 
 *   the morpher for the Animation Control
 * 
 *********************************************************************/ 
 
#ifndef MORPHERI_H
#define MORPHERI_H

#include "morpher.h"

/////////////////////////////////////////////////////////////
// TMorpher_Interface
/////////////////////////////////////////////////////////////
class TMorphPictureWnd;
class TAnimationWnd;

class TMorpher_Interface : public TMorpher
{
protected:
	TAnimationWnd     *AnimationWindow;

	virtual void CreateFrame(double fNewFrame,int nFrame);
	virtual void MorphTriangle(double fFrame,TTriangle& tTri);
	virtual void EndFrame(int nFrame);

public:
	TMorpher_Interface(
		TMorphmesh_Data *mNewMorphmesh,
		TAnimationWnd	*wNewAnimWnd,
		int nMorphType,int nAntiAliasing);

	~TMorpher_Interface();

	virtual void Morph(int nFrom,int nTo);
};

#endif
 
/* End of file MORPHERI.H */ 
 

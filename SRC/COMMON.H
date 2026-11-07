/**********************************************************************
 * 
 *  NAME:         COMMON.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  20.9.95  MHM     created 
 *********************************************************************/
 
#ifndef COMMON_H
#define COMMON_H

#include <assert.h>


#define NOSOUND
#define NOCOMM
#define NODRIVERS
#define NOLOGERROR
#define NOPROFILER
#define NOOPENFILE
#define NORESOURCE
#define NOATOM
#define NOLANGUAGE
#define NODBCS
#define NOKEYBOARDINFO
#define NOGDICAPMASKS
#define NODRAWTEXT
#define NOTEXTMETRIC
#define NOSCALABLEFONT
#define NOMETAFILE
#define NOSYSTEMPARAMSINFO
#define NOKEYSTATES
#define NOWH
#define NOCLIPBOARD
#define NOICONS

#define morphwndClass	(dialogClass +1)

#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))

#define inrange(a,lower,higher)(	\
	(a) < (lower) ? (lower) :       \
		((a) > (higher) ? (higher) : (a)))

#define ShowLocalMemory(szTitle) \
	{	\
    char szMessage[60];	\
	UINT nFreeLocalMem = LocalCompact(0);	\
	wsprintf(szMessage,"Free local memory : %d",nFreeLocalMem);	\
    MessageBox(NULL,szMessage,szTitle,MB_OK | MB_SYSTEMMODAL);	\
    }

#ifndef _MAX_PATH
	#define _MAX_PATH 80
#endif

#define SWAP(type,a,b) \
	{     				\
	type Temp = (a); \
	(a) = (b);              \
	(b) = Temp;           \
    }

#endif
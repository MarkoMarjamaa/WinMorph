/********************************************************************** 
 * 
 *  NAME:         STORAGE.H 
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
 
// Storage.h

#ifndef STORAGE_H
#define STORAGE_H

#include "dcel.h"

#define STOR_ALLOC(type,items)		(farmalloc(sizeof(type)*(long)items))
#define STOR_FREE(block)			{farfree(block);}

//#define STOR_ALLOC(type,items)	(new type [items])
//#define STOR_FREE(block)			{delete block;} 

//#include <windows.h>
//#define STOR_ALLOC(type,items)	(GlobalLock(GlobalAlloc(GPTR,sizeof(type)*(long)items)))
//#define STOR_FREE(block)         {GlobalUnlock((HGLOBAL)SELECTOROF(block));		\
//								  GlobalFree((HGLOBAL)SELECTOROF(block));}

/*
 USE_STORAGE controls the storage allocation strategy:
      0 -> use malloc/free
           small, but slow
      1 -> use private freelists for Points, Edges, and Triangles
           faster, but a memory hog
  */

#define USE_STORAGE 0

#include <alloc.h>

#if (USE_STORAGE == 0)
  /* Allocation routines - using malloc/free */
	#define NewPoint() 		(PointSetPointer)new(Point)
	#define DisposePoint(p) delete(p)
	#define NewEdge() 		(EdgePointer)new(Edge)
	#define DisposeEdge(e)  delete(e)
	#define NewTriangle()   (TrianglePointer)new(Triangle)
	#define DisposeTriangle(t) delete(t)
	#define StorageInit()
    #define StorageFree()
#endif

#if (USE_STORAGE == 1)
/* Allocation routines  - using the "Storage" package

   Profiles reveal that "malloc" was a heavy hitter.  Here, we
   maintain separate free lists for Points, Edges, and Triangles.

   In the current implementation, we never return any of this storage
   to the system, so beware...
 */

PointSetElement* NewPoint();
void	DisposePoint(PointSetElement*);
Edge*	NewEdge();
void	DisposeEdge(Edge*);
Triangle *NewTriangle();
void	DisposeTriangle(Triangle*);
void	StorageInit();
void	StorageFree();

#define POINT_GLOB_SIZE 1024
extern PointSetPointer PointFreeList;

#define EDGE_GLOB_SIZE 1024
extern EdgePointer EdgeFreeList;

#define TRIANGLE_GLOB_SIZE 1024
extern TrianglePointer TriangleFreeList;

#endif

#endif
 
 
/* End of file STORAGE.H */ 
 

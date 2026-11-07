/**********************************************************************
 * 
 *  NAME:         STORAGE.CPP 
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
 
/*
 File: Storage.c
 Authors: J. Painter,
			 K.R. Sloan
 Last Modified: 24 August 1990
 Purpose: storage allocation in globs.
			 This module provides dynamic memory storage allocation functions
			 which minimizes the cost of allocation by allocating elements in
			 large "globs".  The cost of a malloc is amortized over many 
			 allocations.

			 For each size allocated, a free list must be maintained.
			 It should be initialized to NULL.  All further maintenance of the
			 free list is handle in this module.

			 One disadvantage: once allocated to a particular freelist, memory
			 is dedicated to that freelist.  We never give anything back.

			 So, beware....

 */

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "storage.h"

#if (USE_STORAGE == 1)


void Init_FreeList(ListElement* Block,unsigned itemsize,unsigned items)
{
	ListElement* freelist=Block;

	for(int i=1;i<items;i++){
        unsigned char* nextitem = &(((unsigned char*)freelist)[itemsize]);
        freelist->Next =(ListElement*) nextitem;
        freelist = freelist->Next;
		};
    freelist->Next=NULL;
}; 


ListElement *NewElement(ListElement* &freelist)
{
  // Get one from the free list 
  ListElement *new_element = freelist;

  if(freelist!=NULL) freelist = freelist->Next;

  return new_element;
};
	

void DisposeElement(ListElement* &freelist,ListElement* item)
{
  item->Next = freelist;
  freelist = item;
};


#define POINT_BLOCK_SIZE 1024
PointSetElement huge *PointBlock=NULL;
PointSetElement *PointFreeList=NULL;
PointSetElement NullPoint={NULL,NULL,-1};

PointSetElement* NewPoint()
{
	PointSetElement* newpoint =
		(PointSetElement*)NewElement((ListElement*)PointFreeList);
	if(newpoint!=NULL){
		*newpoint=NullPoint;// Init point(Not necessary)
		}
	   else
       	assert(FALSE);
	return newpoint;
};

void DisposePoint(PointSetElement* P)
{    DisposeElement((ListElement*)PointFreeList,(ListElement*)P);};

#define EDGE_BLOCK_SIZE 1024
Edge huge *EdgeBlock=NULL;
Edge *EdgeFreeList=NULL;
Edge NullEdge={NULL,NULL, {-1,NULL,NULL,NULL},{-1,NULL,NULL,NULL}};

Edge* NewEdge()
{
	Edge *newedge = (Edge*)NewElement((ListElement*)EdgeFreeList);
	if(newedge!=NULL){
   		*newedge = NullEdge;
		}
	   else
       	assert(FALSE);
    return newedge;
};

void DisposeEdge(Edge* E)
{DisposeElement((ListElement*)EdgeFreeList,(ListElement*)E);};

#define TRIANGLE_BLOCK_SIZE 1024
Triangle huge *TriangleBlock=NULL;
Triangle *TriangleFreeList=NULL;

Triangle NullTriangle={NULL,NULL,0.0,-1,-1,-1,NULL,NULL,NULL};

Triangle* NewTriangle()
{
	Triangle *newtriangle=
		(Triangle*)NewElement((ListElement*)TriangleFreeList);
	if(newtriangle!=NULL){
		*newtriangle = NullTriangle;
		}
	   else
       	assert(FALSE);
	return newtriangle;
};

void DisposeTriangle(Triangle* T)
{DisposeElement((ListElement*)TriangleFreeList,(ListElement*)T);};

void StorageFree()
{
	STOR_FREE(PointBlock);
	PointFreeList=NULL;PointBlock=NULL;

	STOR_FREE(EdgeBlock);
    EdgeFreeList=NULL;EdgeBlock=NULL;

	STOR_FREE(TriangleBlock);
	TriangleFreeList=NULL;TriangleBlock=NULL;
};

void StorageInit()
{
    if(PointBlock==NULL)
	PointBlock = (PointSetElement *)
		STOR_ALLOC(PointSetElement,POINT_BLOCK_SIZE);
    assert(PointBlock!=NULL);
	Init_FreeList((ListElement*) PointBlock,
			sizeof(PointSetElement),POINT_BLOCK_SIZE);
	PointFreeList=(PointSetElement*)PointBlock;

    if(EdgeBlock==NULL)
	EdgeBlock = (Edge *)
		STOR_ALLOC(Edge,EDGE_BLOCK_SIZE);
	assert(EdgeBlock!=NULL);
	Init_FreeList((ListElement*) EdgeBlock,
			sizeof(Edge),EDGE_BLOCK_SIZE);
    EdgeFreeList=(Edge*)EdgeBlock;

    if(TriangleBlock==NULL)
	TriangleBlock = (Triangle *)
		STOR_ALLOC(Triangle,TRIANGLE_BLOCK_SIZE);
	assert(TriangleBlock!=NULL);
	Init_FreeList((ListElement*) TriangleBlock,
			sizeof(Triangle),TRIANGLE_BLOCK_SIZE);
    TriangleFreeList=(Triangle*)TriangleBlock;
};

#endif 
 
/* End of file STORAGE.CPP */ 
 

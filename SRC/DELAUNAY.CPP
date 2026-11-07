/**********************************************************************
 * 
 *  NAME:         DELAUNAY.CPP 
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
 File: Delaunay.c
 Authors: K.R. Sloan
 Last Modified: 28 August 1992
 Purpose: triangulates a set of points

 */
#include "delaunay.h"
#include <assert.h>
#include "DCEL.h"
#include "GrahamHull.h"
#include "storage.h"

 /*
  InsertNonHullPoints expects that the convex hull has been found, and
  triangulated. 

  The GLOBAL variable Hull (from DCEL.h) points at the 
  set of points in the hull.

  The GLOBAL variable ProbeRoot (from DCEL.h) is a hint to InsertPoint
  about where to start searching for the (existing) triangle in which 
  the new point falls.
 */  

void InsertNonHullPoints(int Npoints,PointSetElement* Hull,int fOptimize)
{
  ProbeRoot = Hull->P; /* just to get started */

  for (int i = 0 ; i < Npoints ; i++ ){
    // If point is not in hull
  	if (!InPointSet(i, Hull))
     // Insert point
     if (InsertPoint(i)){ /* success */
       ProbeRoot = i; 
       if(fOptimize)
		   Optimize();
	   };
	}; 
};

void DeletePoints(PointSetElement* Set)
{
    if(Set==NULL) return;
    PointSetElement* Current = Set;
    do{
		PointSetElement *Next = Current->Next;
		Current->Next     = NULL;
        Current->Previous = NULL;

		DisposePoint(Current);
        Current = Next;
		}
    while(Current!=Set);
};

void Triangulate(int fOptimize)
{
  // Create hull for the points
  PointSetElement *Hull =  GrahamHull(Points, Npoints);

  // Triangulate the hull
  ConvexTriangles(Hull); 
  if(fOptimize)
	  Optimize();

  InsertNonHullPoints(Npoints,Hull,fOptimize);

  // Delete Hull
  DeletePoints(Hull);
  Hull = NULL;	
};

/////////////////////////////
// Delaunay
///////////////////////////7

// Pointadding functions
void Delaunay::StartAddingPoints(int nNewPoints)
{
	StorageInit();

    assert(Points==NULL);
	Points = (Point*)STOR_ALLOC(Point,nNewPoints);
	assert(Points!=NULL);

	Npoints = 0;
};

void Delaunay::AddPoint(double x,double y)
{
	Points[Npoints].x = x;
	Points[Npoints].y = y;
    Points[Npoints].Edges = NULL;

	Npoints++;
};

Delaunay::~Delaunay()
{
    
    // Delete triangles
	while(Triangles!=NULL)
		RemoveTriangle(Triangles);

	StorageFree();

	assert(Points!=NULL);
	STOR_FREE(Points);
	Points = NULL;

	Npoints = -1;
};

// Triangle enumerating functions
void Delaunay::SetToFirstTriangle(void)
{
	CurrentTriangle = Triangles;
};

int *Delaunay::GetNextTriangle(void)
{
	static int Tri[3];

	if(CurrentTriangle==NULL) return NULL;

	Tri[0]=CurrentTriangle->P1;
	Tri[1]=CurrentTriangle->P2;
	Tri[2]=CurrentTriangle->P3;

	CurrentTriangle = CurrentTriangle->Next;
	if(CurrentTriangle==Triangles)
		CurrentTriangle=NULL;
    return Tri;
};

/* At last...the main line (are we in Philadelphia, yet?) */
void Delaunay::Triangulate(int fOptimize)
{
	assert(Triangles==NULL);
	assert(Suspects==NULL);

    if(Npoints>2)
		::Triangulate(fOptimize);
};
 
 
/* End of file DELAUNAY.CPP */ 
 

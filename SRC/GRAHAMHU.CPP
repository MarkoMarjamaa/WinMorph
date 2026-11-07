/********************************************************************** 
 * 
 *  NAME:         GRAHAMHU.CPP 
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
 File: GrahamHull.c
 Authors: K.R. Sloan
 Last Modified: 24 August 1990
 Purpose: calculates the convex hull of a set of <x,y> points,
          using the Graham Scan method.
*/
#include <assert.h>
#include "common.h"		// SWAP,min,max
#include "DCEL.h"
#include "GrahamHull.h"
#include "storage.h"

struct HeapKey {
	double Angle;
	int P;
	};

static void Heapify(HeapKey V[],int Root, int Last)
{
  HeapKey t = V[Root]; 
  int r = Root;
  for (int s = r;r==s;){
    s = r + r;
	if (s <  Last)
		if (V[s].Angle < V[s+1].Angle)
			s = s + 1;
	if (s <= Last)
		if (t.Angle < V[s].Angle ){
			V[r] = V[s];
			r = s;
			}
	}

  V[r] = t;
};

void HeapSort(HeapKey V[],int LastElement)
{
	for (int i = LastElement / 2; i > 1; i--) 
		Heapify(V,i,LastElement);

	for (i = LastElement; i > 1; i--){ /* 2..i form a Heap */
    	Heapify(V, 1,i);
		SWAP(HeapKey, V[1],V[i]);
		}
};

// Create a point that's inside the others
Point CreatePointInsidePoints(Point *Points,int nPoints)
{
	// First create a triangle
	double Ax = Points[0].x; double Ay = Points[0].y;
	double Bx = Points[1].x; double By = Points[1].y;
	double Cx;
	double Cy;

	// Find third point for the triangle (area!=0.0)
	for( int i = 2 ; i < nPoints ; i++ ){
		Cx  =  Points[i].x;		Cy  =  Points[i].y;
		if (((Ax-Bx)*(Ay-Cy) - (Ay-By)*(Ax-Cx)) != 0.0)
			break;
		}/* not colinear */
	assert(i < Npoints);

	// Calculate a point that's inside the first triangle
	Point out;
	out.x  =  (Ax+Bx+Cx) / 3.0; 
	out.y  =  (Ay+By+Cy) / 3.0;   /* an internal point */

	return out;
};

PointSetElement *CreateHullPoints(Point *Points,int nPoints)
{
  // Creates a point that's surely inside the other points
  Point InsidePoint = CreatePointInsidePoints(Points,nPoints);

  // Create sort heap
  HeapKey *Heap = new HeapKey[nPoints+1];

  // Insert all points to Heap
  for (int nPoint = 0; nPoint < nPoints ; nPoint++ ){
    Heap[nPoint+1].P = nPoint; /* off by one to help HeapSort */
	Heap[nPoint+1].Angle = FowlerAngle(InsidePoint, Points[nPoint]);
	};

  // Sort points according to angle to InsidePoint
  HeapSort(Heap, nPoints);

  // Copy Heap to list
  // Create first hull point
  PointSetPointer Hull = NewPoint();
  Hull->P  			=  Heap[1].P;
  Hull->Previous  	=  Hull;
  Hull->Next  		=  Hull; /* circular list */

  for(nPoint = 2 ; nPoint <= nPoints ; nPoint++ ){
    // Create new hull point
	PointSetElement *H	  = NewPoint();
    // Set point index
	H->P 		 		  = Heap[nPoint].P;
    // Add point to hull
	H->Previous  		  = Hull->Previous;
	H->Next  	 		  = Hull;
	Hull->Previous->Next  = H;
	Hull->Previous  	  = H;
	};  

  // delete sort heap 
  delete Heap;
  Heap = NULL;

  // return hull
  return Hull;
};

PointSetElement *FindFirstHullPoint(PointSetElement* Hull)
{
  // Searches the point that's closest to top-left corner
  // Not really, the point with lowest x and lowest y
  // So it's actually finding the first hull point
  
  PointSetElement *FirstHullPoint = Hull;
  for(PointSetElement *H = Hull ; ; ){
    H  =  H->Next;
	Point &IndexPoint = Points[H->P];
	Point &FirstPoint = Points[FirstHullPoint->P];

	if (IndexPoint.x < FirstPoint.x)
		FirstHullPoint = H;
	if (IndexPoint.x==FirstPoint.x)
		if(IndexPoint.y<FirstPoint.y)
			FirstHullPoint  =  H;
	if (H == Hull) break;
	}
  return FirstHullPoint;
};

PointSetElement *GrahamHull(Point *Points,int Npoints)
{
  PointSetElement *Hull = CreateHullPoints(Points,Npoints);
  /*   All points are now in a doubly-linked, circular list, sorted
		by angle about O	*/

  // Searches the point that's closest to top-left corner
  // Not really, the point with lowest x and lowest y
  // So it's actually finding the first hull point
  PointSetElement *FirstHullPoint = FindFirstHullPoint(Hull);

	/*  FirstHullPoint is on the hull - now find the others */  
  for(PointSetElement* H = FirstHullPoint; H->Next != FirstHullPoint; ){
    assert(H->Next->P<Npoints);
    if (LeftTurn(H->P, H->Next->P, H->Next->Next->P))
      H = H->Next;
	else { /* next point is internal */
	  PointSetElement *pInternal  =  H->Next;

	  // Remove Point from Hull
	  pInternal->Previous->Next  =  pInternal->Next;
      pInternal->Next->Previous  =  pInternal->Previous;
	  pInternal->Next     = NULL;
      pInternal->Previous = NULL;

      // delete point
	  DisposePoint(pInternal);
      pInternal = NULL;

      if (H != FirstHullPoint)
		H  =  H->Previous; /* back up, maybe */
     }  /* next point is internal */
   }

  return (FirstHullPoint);
};

 
 
/* End of file GRAHAMHU.CPP */ 
 

/********************************************************************** 
 * 
 *  NAME:         DCEL.H 
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
 File: DCEL.h
 Authors: K.R. Sloan,
          J. Painter
 Last Modified: 29 November 1990
 Purpose: Doubly-Connected Edge List (improved),
          along with a few utilities for triangulating planar
          point sets
 */

#ifndef DCEL_H
#define DCEL_H

/* 
  ArraySize determines (static) allocation of the Points array.
  Static allocation is ugly - but we have more important things to
  worry about.  Change this number to suit your environment, and recompile.

  Also, static allocation make it easier for user programs to use
  parallel arrays to keep more information about points than we are
  interested in.
 */


#include <stdio.h>
#ifndef __WINDOWS_H
	#include <windows.h>
#endif

#define EPSILON (1.0e-16)

  /*
    Points are stored in an array, so that an application can pun
    on the array index and keep auxiliary information. 
    A typical application program will have parallel arrays containing
    other information about each point.  We only care about the
    x,y coordinates, and the Edges emanating from this vertex
   */ 
struct Point
{
   double x, y;
   struct Edge *Edges;
};  


/*
	All the PointSetElements,Edges and Triangles can be cast to 
	ListElement when we are only dealing with a list of items
	For instance, storage functions can use this
*/

struct ListElement
{
	struct ListElement* Next;
    struct ListElement* Previous;
};


  /*
    On the other hand, we occasionally need a "set of points".  For that,
    we use PointSetElements, arranged in a circular, doubly-linked list.
  */
struct PointSetElement
{
   struct PointSetElement *Next;
   struct PointSetElement *Previous;
   int P;
};
typedef PointSetElement*	PointSetPointer;

  /*
    Edges have two ends.  Each end has a vertex, links for a circular,
	doubly-linked list, and an associated Triangle.
    In addition, we will need sets of Edges - yet another circular, 
    doubly-linked list.
  */
struct EdgeEnd {
   int 		 V;       /* Vertex at this End */
   struct 	 Triangle *T; /* Triangle on the LEFT, seen from this End */ 
   Edge 	*Next;
   Edge		*Previous;
   };

struct Edge {
   Edge 	*Next;
   Edge		*Previous;
   EdgeEnd	 Head;
   EdgeEnd	 Tail;
   };
typedef Edge*		EdgePointer;

  /*
    Triangles have three vertices, three edges, and a figure-of-merit.
    Like everything else, they can be collected on circular, doubly-connected
    linked lists.

    For the truly ambitious: generalize the Triangle to Facet.  Give up
	the efficiency of immediate access to the vertices and edges, in favor
    of arbitrary k-gons (yup, more circular, doubly connected linked lists.

    Me, I like triangles... 
  */
struct Triangle {
   Triangle	*Next;
   Triangle	*Previous;
   double	 Merit;             /* See Rho, and Shrink  */
   int		 P1, P2, P3;           /* turning to the RIGHT */
   Edge		*E1,*E2,*E3;       /* Ei = <Pi,P(i+1)>     */
   };
typedef Triangle*		TrianglePointer;

  /*
   Thus, by convention:
 
         LOOKING FROM THE TAIL               LOOKING FROM THE HEAD
       ------------------------              ---------------------
               Head.V                               Tail.V
                 ^                                    |
                 |                                    |
       Tail.T    |   Head.T                  Head.T   |   Tail.T
                 |                                    |
                 |                                    \/ 
               Tail.V                               Head.V  
  */

/* data */
extern Point *Points;
extern int Npoints;

  /*
   Triangulation information -  a Hull, a set of Triangles, a set of
   "Suspect" edges, and a "ProbeRoot" (a vertex used as a hint to the
   Search procedure.

   Only one region here.  You want more?  Looks easy, go do it.

  */

extern Triangle        *Triangles;
extern Edge            *Suspects;
extern int ProbeRoot;

void	AddSuspect		(Edge *E);
void	RemoveSuspect	(Edge *E);
Edge*	FindEdge 		(int P1,int P2);
int		LeftTurn		(int A,int B,int C);
double	FowlerAngle		(Point& A, Point& B);
void	FindNeighbors	(int O,int P,Edge **LeftNeighbor,Edge **RightNeighbor);
Edge*	AddEdge 		(int P1,int  P2, Triangle *T);
void	RemoveEdge		(Edge *E,Triangle *T);
void	AddTriangle		(int A, int B, int C, double R);
void	RemoveTriangle	(Triangle *T);
void	InsideTriangle	(int A,int B,int C,int P,
						int *Inside,int *OnEdge,   int *V1,int *V2);
Triangle *Search (		int P,
						Edge **LeftNeighbor, Edge **RightNeighbor,
					   int *Inside,               int *OnEdge,
					   int *V1,                   int *V2);
double	Rho				(int A,int B,int C);

void	Shrink			(Edge *SharedEdge);
void	Optimize		(void);
BOOL	InsertPoint		(int P);
void	ConvexTriangles	(PointSetElement *Hull);
int		InPointSet		(int A,PointSetElement *P);
BOOL	InSideHull		(int A,PointSetElement*H);
void	VoronoiPt		( double x1, double y1, double x2, double y2, 
						  double x3, double y3, double *x, double *y);


 /*
  Does the path A -> B -> C turn to the left?

  Watch out for colinear points - you may get a random answer.
  Since we are asking for "LeftTurns" - make sure it REALLY turns
  left...  Note that we may miss a VERY SLIGHT, legitimate, LeftTurn.
 */
inline int LeftTurn (int A,int B,int C)
{
   return((((Points[A].x-Points[B].x)*(Points[C].y-Points[B].y))
          -((Points[A].y-Points[B].y)*(Points[C].x-Points[B].x)))<-EPSILON);
};

#endif 
 
/* End of file DCEL.H */ 
 

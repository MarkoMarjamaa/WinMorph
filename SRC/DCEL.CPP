/********************************************************************** 
 * 
 *  NAME:         DCEL.CPP 
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
 File: DCEL.c
 Authors: K.R. Sloan,
          J. Painter
 Last Modified: 2 November 1992
 Purpose: Doubly-Connected Edge List (improved),
          along with a few utilities for triangulating planar
          point sets

          This version is limited to ONE, CONVEX, TRIANGULATED region,
          with up to ArraySize points (see DCEL.h).

          Obvious things to do next are to allow MULTIPLE, CONCAVE,
          POLYGONALIZED regions, with dynamic allocation of the point set.
          Go ahead, make my day. 

          USE_STORAGE allows you to choose between:
                * malloc/free (#define USE_STORAGE  0)
                * private storage management (#define USE_STORAGE 1)

          malloc/free is a performance hot spot - but it works

          private storage management is fast - but it's a memory hog

*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "DCEL.h"
#include "storage.h"

Point *Points = NULL;
int Npoints = -1;

Triangle		*Triangles  = NULL;
Edge			*Suspects   = NULL;
int ProbeRoot=-1;

/*
  Add and Delete Suspect handle a queue of edges.

  AddSuspect allows the same edge to be added multiple times. 
  The first addition puts the edge at the end of the queue.
  Subsequent additions do nothing.

  RemoveSuspect may be called even when the edge is not in the queue.
 */ 
static void AddSuspect(Edge *E)
 { 
 	if (E->Previous == NULL){		/* a new suspect */
    	if (Suspects == NULL){		/* only one */
			Suspects = E;  
			E->Previous = E; 
			E->Next = E;
			} 
		   else {						/* at the end */   
			E->Previous = Suspects->Previous; 
			E->Next = Suspects;
			Suspects->Previous->Next = E; 
			Suspects->Previous = E;
			}
		};
};

void RemoveSuspect(Edge *E)
{
   if (E->Previous != NULL){ /* once a suspect, now cleared */
      E->Previous->Next = E->Next;
      E->Next->Previous = E->Previous;
      if (Suspects == E){
         Suspects = E->Next;
         if (Suspects == E) 
			Suspects = NULL;
			} /* all gone */
      E->Next = NULL; 
	  E->Previous = NULL;
	  } 
};

 /* FindEdge tries to find an existing edge (P1,P2) in the edge list for P1. */
EdgePointer FindEdge(int P1,int P2)
{
	Edge *E = Points[P1].Edges; 
	Edge *E_first = E;

    if(E!=NULL)
		for (;;){ /* is this the edge? */
			if (E->Tail.V == P1){
				if(E->Head.V == P2)
					break;
				E = E->Tail.Next;
				}
			   else {
				if(E->Tail.V == P2)
					break;
				E = E->Head.Next;
				}
			if (E == E_first) {
				E = NULL; 
				break;
				};
			};
	return (E);
}

 /*
  This little gem is due to Rob Fowler.  Given two points A, and B
  we calculate a number in [0.0, 8.0) which is a monotonic function of
  the direction from A to B. 

  (0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0) correspond to
  (  0,  45,  90, 135, 180, 225, 270, 315, 360) degrees, measured
  counter-clockwise from the positive x axis.
 */
double FowlerAngle(Point& A, Point& B)
{
  double adx, ady;

  double dx = B.x-A.x; if (dx<0.0) adx = -dx; else adx = dx;
  double dy = B.y-A.y; if (dy<0.0) ady = -dy; else ady = dy;
  
  int code=-1;
  if (adx<ady) code = 1; else code = 0;
  if (dx<0.0)  code += 2;
  if (dy<0.0)  code += 4;

  double angle;
  switch (code){
    case 0: if (dx == 0.0) 
             angle = 0.0;                 /* A = B */
            else 
			 angle = (ady/adx);        /*   0 <= a <=  45 */
			 break; 
	case 1: angle = (2.0 - (adx/ady));break; /*  45 <  a <=  90 */
	case 2: angle = (4.0 - (ady/adx));break; /* 135 <= a <= 180 */
	case 3: angle = (2.0 + (adx/ady));break; /*  90 <  a <  135 */
	case 4: angle = (8.0 - (ady/adx));break; /* 315 <= a <  360 */
	case 5: angle = (6.0 + (adx/ady));break; /* 270 <= a <  315 */
	case 6: angle = (4.0 + (ady/adx));break; /* 180 <  a <= 225 */
	case 7: angle = (6.0 - (adx/ady));break; /* 225 <  a <  270 */
   };
   return angle;
};

  /*
    FindNeighbors locates the edges which would be the left
    and right neighbors of the edge O-P, in the list of edges
    emanating from O.

    ASSUMPTION: there must be at least 1 Edge emanating from O; check
                before you call...

    Really special case: if O-P is colinear with L and R, make sure
    that P is on the same side of O as R.  This choice ensures that 
    a triangle will be found (by Search) if it is at all possible.

    The above doesn't work for all cases!  But - we've pushed off
    the last (!) special case to Search - go and read Search.

  */

// Uses Points[]
void FindNeighbors (int O,int P,Edge **LeftNeighbor,Edge **RightNeighbor)
{
  double Ox = Points[O].x;
  double Oy = Points[O].y;
  double Px = Points[P].x - Ox; 
  double Py = Points[P].y - Oy;
  Edge   *L = Points[O].Edges;
  if (L == NULL){
    *LeftNeighbor  = NULL; 
    *RightNeighbor = NULL;
	return;
	};
  
  int A;
  if (L->Tail.V == O)  
      A = L->Head.V; 
     else 
      A = L->Tail.V;
  
  double Ax = Points[A].x - Ox; 
  double Ay = Points[A].y - Oy;  
  double AOP = (Ax * Py) - (Ay * Px);
  EdgePointer R;
  if (L->Tail.V == O)  
      R = L->Tail.Next; 
     else 
      R = L->Head.Next;
  int found = (L==R); /* if only one edge, we are done */

  for (;!found;)
   {
	R = L;
	double Bx = Ax;
	double By = Ay;
	double BOP = AOP;
    if (R->Tail.V == O) L = R->Tail.Next; else L = R->Head.Next;
    if (L->Tail.V == O) A = L->Head.V;    else A = L->Tail.V;
    Ax = Points[A].x - Ox; Ay = Points[A].y - Oy;  

    AOP = (Ax * Py) - (Ay * Px);
    double BOA = (Bx * Ay) - (By * Ax);      

    /* 
      L and R are currently neighbors, L is to the left of R
      <Ax,Ay> and <Bx, By> are the other ends of L and R
      AOP is the signed area of the trapezoid defined by OA and OP
      BOP is the signed area of the trapezoid defined by OB and OP
      BOA is the signed area of the trapezoid defined by OB and OA
      a positive sign means that the angle (at O) is less than pi
      a negative sign means that the angle (at O) is greater than pi
      a zero area means that the points are colinear
        in this case, we resort to the dot product, to see if
        the angle is 0 or pi

      The determination that P would fit between A and B in the Edge list
      requires a small amount of case analysis, viz.
     */

    if (BOA > 0.0) /* less than pi from B to A */
     found = ((BOP >= 0.0) && (AOP <= 0.0));
    else if (BOA < 0.0) /* more than pi from B to A */
     found = ((BOP >= 0.0) || (AOP <= 0.0));
    else if (((Ax*Bx) + (Ay*By)) < 0.0) 
     { /* pi from B to A */
      if      (BOP > 0.0)  found = TRUE;
      else if (BOP < 0.0)  found = FALSE; 
      else found = (((Bx*Px)+(By*Py)) >= 0.0);
      /*
       when A,O,P, and B are colinear, we prefer to return
       with B and P on the same side of O.  This helps find
       triangles that otherwise might be missed.
       When B and P are on the same side of O,  the
       RightNeighbor points towards P.  If RightNeighbor is
       part of ONLY ONE triangle,  that triangle is on its
       left.
     
       [N.B. - the above claim is incorrect - see Search ]
       [it may be worse than that - worry about this later... ]

       If we return with A and P on the same side of O
       (i.e., with LeftNeighbor pointing at P)  Search
       may conclude that P cannot be part of any triangle.

       Making this arbitrary choice here makes Search's job
       easier.  

       But not trivial - see the comments in Search for the last(!)
       special case.

       [Search should still work, even if we are confused about
         where the triangles are.  FindNeighbors doesn't really
         know about triangles, and should not need to know
          about them - that's Search's problem]

      */ 
     } /* pi from B to A */
    else 
     { /* 0 from B to A */
	   Triangle *T;
	   if (R->Tail.V == O) T = R->Tail.T; else T = R->Head.T;
       found =   (T==NULL) 
               || ((BOP == 0.0) && (((Bx*Px)+(By*Py)) >= 0.0));
     }   /* 0 from B to A */
   }
  *LeftNeighbor = L; 
  *RightNeighbor = R;
};

void Add_EdgeEnd(Edge* E,EdgeEnd& End,int P1,int P2)
{
	if (Points[P1].Edges == NULL){ /* empty list */
      Points[P1].Edges = E;
	  End.Previous = E;
	  End.Next = E;} /* empty list */
    else{ /* non-empty list */
	  Edge *L, *R;
	  FindNeighbors(P1, P2, &L, &R);
	  End.Previous = R;
	  End.Next = L;
      if (R->Tail.V == P1) R->Tail.Next = E;
      else                 R->Head.Next = E;
      if (L->Tail.V == P1) L->Tail.Previous = E;
      else                 L->Head.Previous = E;
     }   /* non-empty list */
};

 /*
  AddEdge inserts the edge P1-P2, and the information that this edge
  has the Triangle T on its left.
 */
Edge *AddEdge (int P1,int  P2, Triangle *T)
{
  Edge *E = FindEdge(P1, P2);
  if (E != NULL)
   if (E->Tail.V == P1)  E->Tail.T = T; else E->Head.T = T;
  else 
   { /* new edge - put <P1, T> at the Tail, <P2, 0> at the Head */
    E = NewEdge();
    if ( E == NULL) {assert(FALSE);}
    E->Tail.V = P1;     E->Tail.T = T;
    E->Head.V = P2;     E->Head.T = NULL;
    E->Previous = NULL; E->Next = NULL;

    Add_EdgeEnd(E,E->Tail,P1,P2);
	Add_EdgeEnd(E,E->Head,P2,P1);
   } /* new edge */
  return (E);
};

void RemoveEdgeEnd(EdgeEnd& End)
{
	if (End.Next == End.Previous)	// This was the last EdgeEnd
		Points[End.V].Edges = NULL;
	   else { /* one of many */
		if (End.Next->Tail.V == End.V){
			End.Next->Tail.Previous  = End.Previous;
            }
		   else {
            assert(End.Next->Head.V == End.V);
			End.Next->Head.Previous  = End.Previous;
            }
		if (End.Previous->Tail.V == End.V){
			End.Previous->Tail.Next  = End.Next;
            }
		   else {
            assert(End.Previous->Head.V == End.V);
			End.Previous->Head.Next  = End.Next;
            };
		Points[End.V].Edges = End.Next;
		}  
};

 /*
  RemoveEdge removes the Triangle T as support for Edge E.
  If there is no triangle on the other side of E,  E is destroyed.
 */
void RemoveEdge(Edge *E,Triangle *T)
{
  // Same triangle can't be on both sides of the edge
  if (E->Tail.T == T)
	E->Tail.T = NULL;
  else
	E->Head.T = NULL;

  // If there are no triangles in either side, destroy the edge
  if (( E->Tail.T == NULL)&&( E->Head.T == NULL) )
   {  
	RemoveEdgeEnd(E->Tail);
    RemoveEdgeEnd(E->Head);
    // delete edge from suspectlist
    RemoveSuspect(E);
    // finally delete edge
	DisposeEdge(E);
	}
};

 /*
  AddTriangle creates the triangle A,B,C - with figure of merit R.
  (See Rho and Shrink for the definition and use of R)
 */
void AddTriangle (int A, int B, int C, double R)
{
  Triangle *T = NewTriangle();
  if ( T == NULL)		{assert(FALSE);return;};

  if (Triangles == NULL){ /* only one */
	Triangles = T;
	T->Previous = T;
	T->Next = T;
	}
  else { /* at the end */
	T->Previous = Triangles->Previous;
	T->Next = Triangles;
	Triangles->Previous->Next = T;
	Triangles->Previous = T;
	};

  T->P1 = A;
  T->P2 = B;
  T->P3 = C;
  T->E1 = AddEdge(A, B, T);
  T->E2 = AddEdge(B, C, T);
  T->E3 = AddEdge(C, A, T);
  T->Merit = R;
};

 /*
  RemoveTriangle destroys T
 */
void RemoveTriangle (Triangle *T)
 {
  // detach triangle
  T->Previous->Next = T->Next;
  T->Next->Previous = T->Previous;
  if (Triangles == T){
    Triangles = T->Next;
	if (Triangles == T)
		Triangles = NULL;} /* all gone */

  RemoveEdge(T->E1, T);
  RemoveEdge(T->E2, T);
  RemoveEdge(T->E3, T);

  T->Next     = NULL;
  T->Previous = NULL;
  DisposeTriangle(T);	//delete Triangle
};


 /*
   InsideTriangle decides if a point P is Inside, or OnEdge
   of the triangle defined by A, B, C.

   If P is on an edge, V1 and V2 identify that edge
 */
void InsideTriangle (
   int A,int B,int C,int P,
   int *Inside, int* OnEdge,
   int *V1, int*V2)
 {
  double DOT;

  *Inside = FALSE; *OnEdge = FALSE;
  int OnEdgeAB = FALSE; 
  int OnEdgeBC = FALSE; 
  int OnEdgeCA = FALSE;

  Point& PointA = Points[A];
  Point& PointB = Points[B];
  Point& PointC = Points[C];
  Point& PointP = Points[P];

  double ax = PointC.x - PointB.x;  
  double ay = PointC.y - PointB.y;
  double bx = PointA.x - PointC.x;  
  double by = PointA.y - PointC.y;
  double cx = PointB.x - PointA.x;  
  double cy = PointB.y - PointA.y;
  double apx= PointP.x - PointA.x;  
  double apy= PointP.y - PointA.y;
  double bpx= PointP.x - PointB.x;  
  double bpy= PointP.y - PointB.y;
  double cpx= PointP.x - PointC.x;  
  double cpy= PointP.y - PointC.y;

  double aCROSSbp = ax*bpy - ay*bpx;
  double cCROSSap = cx*apy - cy*apx;
  double bCROSScp = bx*cpy - by*cpx;

  *Inside = ((aCROSSbp > 0.0) && (bCROSScp > 0.0) && (cCROSSap > 0.0));

  if (!(*Inside)){
    if ((EPSILON > aCROSSbp) && (aCROSSbp > -EPSILON)){
      DOT = (ax*bpx)+(ay*bpy);
      OnEdgeBC = ((0.0 <= DOT) && (DOT <= (ax*ax)+(ay*ay)));}

    if ((EPSILON > bCROSScp) && (bCROSScp > -EPSILON)){
      DOT  = (bx*cpx)+(by*cpy);
      OnEdgeCA = ((0.0 <= DOT) && (DOT <= (bx*bx)+(by*by)));}

    if ((EPSILON > cCROSSap) && (cCROSSap > -EPSILON)){
      DOT = (cx*apx)+(cy*apy);
      OnEdgeAB = ((0.0 <= DOT) && (DOT <= (cx*cx)+(cy*cy)));}
    if      (OnEdgeAB) { *V1 = A; *V2 = B; *OnEdge = TRUE; }
    else if (OnEdgeBC) { *V1 = B; *V2 = C; *OnEdge = TRUE; }
    else if (OnEdgeCA) { *V1 = C; *V2 = A; *OnEdge = TRUE; }
   }
};

  /*
    Given (the GLOBAL variable) ProbeRoot - a vertex in a 
    CONVEX, TRIANGULATED region
    of the plane, Search attempts to find the Triangle containing P.
    This is accomplished by traversing the triangulation from ProbeRoot
    towards P until an enclosing triangle is found, or P is shown to be
    outside the convex hull. 

    If an enclosing triangle is found, Search also determines if P is
    strictly Inside, or OnEdge.  If OnEdge,  V1 and V2 identify
    the edge.  

    LeftNeighbor and RightNeighbor are two edges emanating from a vertex
    in the triangulated region which bracket the point P.  If P is Inside
    or OnEdge,  these are edges of the enclosing triangle.  If no 
    enclosing triangle is found,  these are edges from the convex hull
    of the triangulated region.  In the latter case, V1 identifies the
    shared vertex.

    Thus, if P is known to be inside the convex hull,  Search will
    always return the enclosing triangle.

    If P is OUTSIDE the convex hull,  V1 is visible from P, and the
    Neighboring edges are the hull edges emanating from V1.
  */
Triangle* Search(
   int P,
   Edge **LeftNeighbor, Edge **RightNeighbor,
   int *Inside,               int *OnEdge,
   int *V1,                   int *V2)
{
  Edge *L, *R;
  Triangle *T;

  int O = ProbeRoot; 
  int done = FALSE; 
  for (;!done;)
   {
    FindNeighbors(O, P, &L, &R);
    assert(L!=NULL);
	assert(R!=NULL);

	if (R->Tail.V == O)
		T = R->Tail.T;
	   else
	    T = R->Head.T;
    if ( T == NULL){ 
      /* 
        We should have a triangle - but we don't.
        It may be that we are ON the hull, and there is a triangle
        on the other side (to the right) of R.

        Look for the other triangle.
        If we find one, see if we are OnEdge (or, perhaps even Inside?)

        If we are not Inside or OnEdge, there is still on last hope.
        If the points are arranged O->B->P  
        (where B is the vertex at the other end of edge R)
        then we should move to B and try again.

        Otherwise, we are outside the hull,
        and should return the original L and R, and a null Triangle.

        But...if we do turn out to be Inside or OnEdge, adjust L,R,T
        and heave a sigh of relief.
       */
      if (R->Tail.V == O) T = R->Head.T; else T = R->Tail.T;
      if (T==NULL){ 
         *V1 = O; done = TRUE; }   /* this should never happen, but... */
      else {
		*Inside = FALSE;
		*OnEdge = FALSE;
		InsideTriangle(T->P1, T->P2, T->P3, P, Inside, OnEdge, V1, V2);
        if ((*OnEdge) || (*Inside)) {
          /* Huzzah!  We dodged a bullet there! */
          L = R;  
          if (L->Tail.V == O)  R = L->Tail.Next; else R = L->Head.Next;
		  done = TRUE;
		  }
        else {
          /* last chance */
          int B;
          double dx, dy, dOP, dOB; 
          if (R->Tail.V == O)  B = R->Head.V; else B = R->Tail.V;          
     
          Point& PointO=Points[O];
          dx = Points[P].x - PointO.x;
          dy = Points[P].y - PointO.y;
          dOP = (dx*dx) + (dy*dy);

          dx = Points[B].x - PointO.x;
          dy = Points[B].y - PointO.y;
          dOB = (dx*dx) + (dy*dy);

          if (dOP > dOB) {/* getting closer... */
			O = B;
			}
          else{ /* Sigh.... this P is truly outside the hull */
            T = NULL;
            *V1 = O;
			done = TRUE;
			}
         }
       }
     }
    else {
	  *Inside = FALSE;
	  *OnEdge = FALSE;
	  InsideTriangle(T->P1, T->P2, T->P3, P, Inside, OnEdge, V1, V2);
      if ((*Inside) || (*OnEdge))   
       done = TRUE;
      else 
       if (L->Tail.V == O)  O = L->Head.V; else O = L->Tail.V;
     } 
   }
  *LeftNeighbor = L; 
  *RightNeighbor = R;
  return (T);
};

 /*
  Rho calculates 4r^2, where r is the radius of the circle passing 
  through A, B, C

  The calculation of Rho depends on the observations that:

                            2
                (   abc    )     2 2 2             2
   Rho = 4*r*r= (-------   )  = a b c / (a cross c)
                (a cross c )

             2   2   2
   and that a , b , c , and (a cross c) are all easy to calculate.

    2
   a  is the square of the length of the side opposite vertex A

    2
   b  is the square of the length of the side opposite vertex B

    2
   c  is the square of the length of the side opposite vertex C
   
 */

double Rho (int A,int B,int C)
{
  Point& vA=Points[A];
  Point& vB=Points[B];
  Point& vC=Points[C];

  double ax = vC.x-vB.x;      double ay = vC.y-vB.y;
  double bx = vA.x-vC.x;      double by = vA.y-vC.y;
  double aCROSSb = ax*by - ay*bx;
  if(aCROSSb > 0.0){ /* correctly oriented triangle */

    if (aCROSSb < EPSILON) aCROSSb = EPSILON; /* fault protection */

    double ax2 = ax*ax; double ay2 = ay*ay;    
    double bx2 = bx*bx; double by2 = by*by;
    
    double cx = vB.x-vA.x;double cy = vB.y-vA.y;       
    double cx2 = cx*cx; double cy2 = cy*cy;
    return ( (ax2+ay2)*(bx2+by2)*(cx2+cy2)/(aCROSSb*aCROSSb) );
   }   /* correctly oriented triangle */
  else
   return ( -1.0 ); /* backwards | colinear triangles don"t count */
};

 /*
   Shrink is the absolute HOT SPOT - so we've optimized as much as
   is seemly.  Given a pair of triangles ABC and BAD, we want to know if
   the pair ADC, BCD would be better.  If it is,  we make the switch.
   
   To determine which local configuration is best, we calculate

    Rabc = the square of the radius of the circle through ABC
    Rbad = the square of the radius of the circle through BAD

    Radc = the square of the radius of the circle through ADC
    Rbcd = the square of the radius of the circle through BCD
    
    Rabc and Rbad are associated with the current configuration;
    Radc and Rbcd are associated with the proposed configuration.

    The calculation of the  R"s depends on the observations that:

                            2
               (   abc    )
        4*r*r= (-------   )
               (a cross c )

    so, we get:
               2 2 2              2
     Rabc =   a c e  / (a cross c)

               2 2 2              2
     Racd =   a d f  / (a cross d)

               2 2 2              2 
     Rbad =   b d e  / (b cross d)

               2 2 2              2
     Rbcd =   b c f  / (b cross c)

   where
        a is the edge connecting A and C
        b is the edge connecting B and D
        c is the edge connecting C and B
        d is the edge connecting D and A  
        e is the existing diagonal  connecting A and B
        f is the candidate diagonal connecting C and D

  Of the four circles, the smallest one is guaranteed to EXCLUDE the fourth 
  point - that is the configuration we want.  So, we first pick the minimum
  from each pair ([Rabc, Rabd] and [Racd, Rbcd]) and  compare the minima.
  if min(Rabc,Rabd) > min(Racd, Rbcd)  SWAP else DO NOT SWAP.

  BE CAREFUL: Rho is negative for colinear or "backwards" triangles;
              negative Rhos should compare GREATER than positive
              This should only happen when one of the already created
              triangles is linear...so we add an extra check.

  Finally, this criterion is not valid when the quadrilateral ADBC is not
  convex. So, we first check the angles DAC and CBD: 
  If (a cross d) and (b cross c) are both positive
  all is well.  If either (a cross d) of (b cross c) are negative,
  all of the above is moot, and no swap is possible.  
  Here, it is important that:
        
         a is the DIRECTED edge from A to C
         b is the DIRECTED edge from B to D
         c is the DIRECTED edge from C to B
         d is the DIRECTED edge from D to A

  Rabc and Rbad have already been calculated.
  Radc and Rbcd need to be calculated - we do it ourself, for speed.

 */
void Shrink(Edge *SharedEdge)
{
  double R1, R2;

  int A = SharedEdge->Tail.V;       
  int B = SharedEdge->Head.V;

  int C = SharedEdge->Tail.Next->Tail.V;
  if (C == A)  C = SharedEdge->Tail.Next->Head.V;

  int D = SharedEdge->Tail.Previous->Tail.V;
  if (D == A)  D = SharedEdge->Tail.Previous->Head.V;

  Point& vA=Points[A];
  Point& vB=Points[B];
  Point& vC=Points[C];
  Point& vD=Points[D];

  double ax = vC.x-vA.x; double ay = vC.y-vA.y;      
  double bx = vD.x-vB.x; double by = vD.y-vB.y;
  double cx = vB.x-vC.x; double cy = vB.y-vC.y;
  double dx = vA.x-vD.x; double dy = vA.y-vD.y;

  double aCROSSd = ax*dy - ay*dx;
  double bCROSSc = bx*cy - by*cx;   
  if ((aCROSSd > 0.0) && (bCROSSc > 0.0)){ /* ADBC is convex */
    double ax2 = ax*ax; double ay2 = ay*ay;
    double bx2 = bx*bx; double by2 = by*by;
    double cx2 = cx*cx; double cy2 = cy*cy;
    double dx2 = dx*dx; double dy2 = dy*dy;
    double fx = vC.x-vD.x;double fy = vC.y-vD.y; 
    double fx2 = fx*fx; double fy2 = fy*fy;

    if (aCROSSd < EPSILON) aCROSSd = EPSILON; /* fault protection */
    if (bCROSSc < EPSILON) bCROSSc = EPSILON; /* fault protection */

    double Radc = (ax2+ay2)*(dx2+dy2)*(fx2+fy2)/(aCROSSd*aCROSSd);
    double Rbcd = (bx2+by2)*(cx2+cy2)*(fx2+fy2)/(bCROSSc*bCROSSc);
    if (Radc < Rbcd)  R2 = Radc; else R2 = Rbcd;

    double Rabc = SharedEdge->Tail.T->Merit;
    double Rbad = SharedEdge->Head.T->Merit;
    if (Rabc < Rbad)
     if (Rabc >= 0.0)  R1 = Rabc; else R1 = Rbad;
    else if (Rbad >= 0.0)  R1 = Rbad; else R1 = Rabc;

    if (R1 > R2){ /* swap */
      RemoveTriangle(SharedEdge->Tail.T);
      RemoveTriangle(SharedEdge->Head.T);
      AddTriangle(A, D, C, Radc);
      AddTriangle(B, C, D, Rbcd);
      AddSuspect(FindEdge(A,D));
      AddSuspect(FindEdge(A,C));
      AddSuspect(FindEdge(B,D));
      AddSuspect(FindEdge(B,C));}}
};

 /*
   Optimize is called to clean up a triangulation.

   On entry: the set pointed at by Suspects contains "suspect" edges.  

   Optimize considers each suspect edge, in turn.  For each such edge,
   the local quadrilateral is examined.  If the suspect edge is OK,
   it is simply removed from the suspect queue.  If not,  the
   two triangles which share this edge are removed, and two other triangles
   are inserted.  The four outer edges of these two triangles  become 
   suspect, and are added to the end of the queue.

   Parameters?  WE DON"T GOT TO SHOW YOU NO STINKING PARAMETERS!
 */
void Optimize(void)
{
  for (;Suspects != NULL;){ /* cross-examine */
	Edge *E = Suspects;
	RemoveSuspect(E);
	if ((E->Tail.T != NULL)&&(E->Head.T!=NULL))
      Shrink(E);}
};

 /*
   Duplicate checks for (within EPSILON) duplicate points
  */

inline int Duplicate(int A,int B)
{
  return (  (EPSILON > fabs(Points[A].x-Points[B].x))
          &&(EPSILON > fabs(Points[A].y-Points[B].y))
         );
};
 /*
   InsertPoint adds a new point.

   The current version only understands how to do that when P is
   inside an already triangulated convex region.

   In case of error, return FALSE.

   Coming soon, a version that will do on-line insertions
 */ 
BOOL InsertPoint (int P)
{
  int Inside, OnEdge;
  Triangle *T2;
  Edge *L, *R, *E;
  int A, B, C, D;

  Triangle *T = Search(P, &L, &R, &Inside, &OnEdge, &A, &B);
  
  if (Inside){ 
    A = T->P1; B = T->P2; C = T->P3;
    RemoveTriangle (T);
    AddTriangle(A, B, P, Rho(A,B,P));
    AddTriangle(B, C, P, Rho(B,C,P));
    AddTriangle(C, A, P, Rho(C,A,P));
    AddSuspect(FindEdge(A, B));
    AddSuspect(FindEdge(B, C));
    AddSuspect(FindEdge(C, A));}
  else 
    if (OnEdge){
      if (Duplicate(A,P))
         return FALSE;
       else 
         if (Duplicate(B,P))
         return FALSE;
    E = FindEdge(A, B);
    A = E->Tail.V; B = E->Head.V;
    C = E->Tail.Next->Tail.V;
    if (C == A)  C = E->Tail.Next->Head.V;
    D = E->Tail.Previous->Tail.V;
    if (D == A)  D = E->Tail.Previous->Head.V;
    T = E->Tail.T; T2 = E->Head.T;
    if (T  != NULL)  RemoveTriangle(T);
    if (T2 != NULL)  RemoveTriangle(T2);
    if (T  != NULL) { /* replace T */
      AddTriangle(A, P, C,Rho(A,P,C));
      AddTriangle(C, P, B,Rho(C,P,B));
      AddSuspect(FindEdge(A, C));
      AddSuspect(FindEdge(C, B));
      AddSuspect(FindEdge(P, A));
      AddSuspect(FindEdge(P, B));}  /* replace T */
    if (T2 != NULL) { /* replace T2 */
      AddTriangle(B, P, D,Rho(B,P,D));
      AddTriangle(D, P, A,Rho(D,P,A));
      AddSuspect(FindEdge(B, D)); 
      AddSuspect(FindEdge(D, A));
      AddSuspect(FindEdge(P, A));
      AddSuspect(FindEdge(P, B));}   /* replace T2 */
   }
  else
   return FALSE;
 return TRUE;
};

 /*
  ConvexTriangles will triangulate a CONVEX polygon.

  ASSUMPTION: the polygon is "strictly convex" - there are no
              configurations of the form ....A B C.... where B
              is on the line segment A-C.

  This assumption is only important at the very beginning,
  and the very end of the triangulation.  If it is violated,
  the triangulation will include zero-area triangles.
 */
void ConvexTriangles (PointSetElement *Hull)
{
  PointSetElement *Trailer = Hull;              /* first Hull point */
  PointSetElement *Top     = Trailer->Next;     /* second Hull point */
  PointSetElement *Bottom  = Trailer->Previous; /* last Hull point */
  int incrementTop = FALSE;
  for (;Top != Bottom;) {
    AddTriangle(Trailer->P,Top->P,Bottom->P,Rho(Trailer->P,Top->P,Bottom->P));
    AddSuspect(FindEdge(Top->P,Bottom->P));
    incrementTop = !incrementTop;
    if (incrementTop){ 
       Trailer = Top; 
       Top     = Top->Next; }
     else { 
       Trailer = Bottom; 
       Bottom = Bottom->Previous; }}
};

 /*  Is A in PointSet P ? */
int InPointSet(int A,PointSetElement *P)
{
  PointSetElement *ThisP;
  for (ThisP=P;
       ((ThisP->Next) != P) && ((ThisP->P) != A);
       ThisP = ThisP->Next)
   { /* just looking, thanks */ }
  return (ThisP->P == A);
};

 /*    Is A in Convex Polygon H?  */
BOOL InsideH(int A,PointSetElement*H)
{
  assert(A<Npoints);
  PointSetElement *H2;
  PointSetElement *H1 = H;
  for (;;){
    H2=H1->Next;
    if (LeftTurn(H1->P,A,H2->P)) return FALSE;
    if (H2 == H) return TRUE;
    H1 = H2;}
};

 /*
     given three points, calculates the center of the circle thru
     those points.

     WARNING: if the points are colinear, we print a message, and 
              return something reasonble.  Best not to call this routine
              with colinear points, eh? 
  */

void VoronoiPt(
 double x1, double y1,
 double x2, double y2, 
 double x3, double y3,
 double *x, double *y)
{
  /* see Newman & Sproull, p 494-496 */

  /* transform points to homogeneous coords */
  double a1 = x1; double a2 = y1; double a3 = 1.0;
  double b1 = x2; double b2 = y2; double b3 = 1.0;
  double c1 = x3; double c2 = y3; double c3 = 1.0;

  /* find the lines from b-c (call it A) and a-c (call it B) */
  double A1 = (b2*c3)-(b3*c2); 
  double A2 = (b3*c1)-(b1*c3);
  //double A3 = (b1*c2)-(b2*c1);
  double B1 = (a2*c3)-(a3*c2); 
  double B2 = (a3*c1)-(a1*c3); 
  //double B3 = (a1*c2)-(a2*c1);

  /* find the midpoints on these lines */
  double bc1 = b1+c1; 
  double bc2 = b2+c2; 
  double bc3 = 2.0;    /* we're being clever here... */
  double ac1 = a1+c1; 
  double ac2 = a2+c2; 
  double ac3 = 2.0;

  /* find lines perpendicular to A and B, through bc and ac */
  double Ap1 = A2*bc3; 
  double Ap2 = -A1*bc3; 
  double Ap3 = (A1*bc2)-(A2*bc1);
  double Bp1 = B2*ac3; 
  double Bp2 = -B1*ac3; 
  double Bp3 = (B1*ac2)-(B2*ac1);

  /* intersect the perpendicular bisectors */
  double v1=(Bp3*Ap2)-(Bp2*Ap3); 
  double v2=(Bp1*Ap3)-(Bp3*Ap1); 
  double v3=(Bp2*Ap1)-(Bp1*Ap2);

  if ((-EPSILON < v3) && (v3 < EPSILON)){
    if (v3 < 0.0) 
      v3 = -EPSILON; 
     else 
      v3 = EPSILON;}
  
  /* back to 2D */
  *x = v1/v3; 
  *y = v2/v3;
};
 
 
/* End of file DCEL.CPP */ 
 

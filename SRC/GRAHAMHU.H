/********************************************************************** 
 * 
 *  NAME:         GRAHAMHU.H 
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
 File: GrahamHull.h
 Authors: K.R. Sloan
 Last Modified: 5 January 1987
 Purpose: calculates the convex hull of a set of <x,y> points
 Note: requires DCEL.h, .c
 */

PointSetElement* GrahamHull(Point *Points,int nPoints);
 
 
/* End of file GRAHAMHU.H */ 
 

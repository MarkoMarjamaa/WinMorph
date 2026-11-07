/**********************************************************************
 * 
 *  NAME:         POINT.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *  28.9.95  MHM     added headers 
 * 
 *********************************************************************/ 

#ifndef POINT_H
#define POINT_H

inline int operator ==(POINT a,POINT b)
{
	return ((a.x==b.x)&&(a.y==b.y));
};

#endif
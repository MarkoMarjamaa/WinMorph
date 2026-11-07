/**********************************************************************
 * 
 *  NAME:         ACCONTCF.H 
 * 
 *  DESCRIPTION:  
 * 
 *  M O D I F I C A T I O N   H I S T O R Y 
 * 
 *  version when     who what 
 *  ------------------------------------------------------------------- 
 *          22.01.96 MHM created
 * 
 *********************************************************************/

#ifndef ACCONTCF_H
#define ACCONTCF_H

// define Play modes
#define NORMAL		0
#define LOOP		1
#define PINGPONG	2
 
class ACContextCfg
{
public:
	int		nPlayMode;

	ACContextCfg() {nPlayMode = NORMAL;};
};

#endif


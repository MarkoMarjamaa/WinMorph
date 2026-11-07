/********************************************************************** 
 * 
 *  NAME:         DELAUNAY.H 
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
 
class Delaunay {

public:
    ~Delaunay();
	void	StartAddingPoints	(int nNewPoints);
	void	AddPoint			(double x,double y);

	void	SetToFirstTriangle	(void);
	int*	GetNextTriangle		(void);

	void 	Triangulate			(int fOptimize);

protected:
	struct Triangle* CurrentTriangle;
};
 

/* End of file DELAUNAY.H */ 
 

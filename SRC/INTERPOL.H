/********************************************************************** 
 * 
 *  NAME:         INTERPOL.H 
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


class Interpolation {
protected:
	float *x;
	float *y;
    float *p;

	static void	 MakeSpline(float *x,float *y,int N,float *p);
public:
	Interpolation(float *x,float *y,int N);
	~Interpolation();

	float GetPoint(float v);
}; 
 
/* End of file INTERPOL.H */ 
 

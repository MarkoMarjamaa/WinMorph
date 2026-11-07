/**********************************************************************
 * 
 *  NAME:         INTERPOL.CPP 
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

#include "interpol.h"

void Interpolation::MakeSpline(float *x,float *y,int N,float *p)
{
	float *d = new float[N];
	float *u = new float[N];
	float *w = new float[N];

	for(int i = 2 ; i < N ; i++ )
		d[i] = 2 * (x[i+1] - x[i-1]);

	for(    i = 1 ; i < N ; i++ )
		u[i] = x[i+1] - x[i];

	for(    i = 2 ; i < N ; i++ )
		w[i] = 6.0 *( (y[i+1] - y[i]  ) / u[i]
					 -(y[i]    -y[i-1]) / u[i-1]);

	for(i = 2 ; i < N-1 ; i++){
		w[i+1] = w[i+1] - w[i] * u[i] / d[i];
		d[i+1] = d[i+1] - u[i] * u[i] / d[i];
		}

	p[1] = 0.0;
	p[N] = 0.0;
	for( i = N-1 ; i > 1 ; i-- )
		p[i] = ( w[i] - u[i]*p[i+1] ) / d[i];

    delete w;
    delete u;
	delete d;
};

Interpolation::Interpolation(float *X,float *Y,int N)
{
	x = X;
    y = Y;
	p = new float [N+2];
    MakeSpline(x,y,N,p);
};

float Interpolation::GetPoint(float v)
{
	int i = 1;
	while(v > x[i+1]) i++;

    float u = x[i+1] - x[i];
	float t = ( v - x[i] ) / u;
    float tr = 1 - t;

	return t * y[i+1] +  tr * y[i] +
		 	u * u * (
			(t  * t  * t  - t ) * p[i+1] +
			(tr * tr * tr - tr) * p[i]			) / 6.0;
};

Interpolation::~Interpolation()
{
	delete p;
};

 
 
/* End of file INTERPOL.CPP */ 
 

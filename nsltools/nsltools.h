#ifndef _libnsl_
#define _libnsl_


#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h>
#include <math.h>

#define L 26
#define M 129

// structure for  wav2aud parameters 

typedef struct nsltools_wav2aud_param{
		
        float fac;
        float shft;
        float alpha;
        float beta;
        float L_frm;
	
	
}wav2aud_param_t;



// funtions

double COCHBA(char , int , int ); 

void nsltools_get_param(wav2aud_param_t *, int , float *);

void nsltools_normalize_coef(double *, double *, int , double );

void nsltools_filter(double *, double *, double *, double *,  int , int );

void nsltools_halfregu(double *, int );

void nsltools_sigmoid(double *, float , int );

void nsltools_wav2aud(float *, double *, int , int, wav2aud_param_t);


#ifdef __cplusplus
}
#endif


#endif

#include <math.h>
#include<stdlib.h>
#define PI 3.141592653589793

double STEP_INC=0.5;

double calc_amplitude(int* buffer,int size,int freq){
  double psum1=0;
  double psum2=0;
  int i;
  double precalc=(((PI*2)*freq)/(size));
  double precalc2;
  for(i=0;i<size;i++){
    precalc2=precalc*i;
    psum1=psum1+buffer[i]*cos(precalc2);
    psum2=psum2+buffer[i]*sin(precalc2);
  }
  return (sqrt(psum1*psum1+psum2*psum2))/(size);
}
double* darray=NULL;
double* attwindow=NULL;
int gramsize=0;
void free_fourier_transform(){
	free(darray);
	darray=NULL;
	free(attwindow);
	attwindow=NULL;
}
void init_fourier_transform(int size,double depth){//this size is f16 char divided by 2
	 gramsize=0;
	 int i;
	 STEP_INC=depth;
	 free_fourier_transform();
	 int c=0;
	 double step=1;
	 for(i=0;i<(size>>1);i=i+step){
 	   step=step+STEP_INC;
 	   c++;
 	   gramsize++;
 	 }
 	  darray=malloc(sizeof(double)*(c));
 	  attwindow=malloc(sizeof(double)*size);
 	  double stepc=(PI)/(size-1);
 	  double pos=0;
 	  for(i=0;i<size;i++){
 	  	attwindow[i]=cos(pos);
 	  	pos=pos+stepc;
 	  }
}


double* produce_period_gram(int* buffer, int size){//this size is f16 char divided by 2
  int i;
  double step=1;
  int array_step=0; 
  for(i=0;i<(size>>1);i=i+step){
    step=step+STEP_INC;
    darray[array_step]=calc_amplitude(buffer,size,i);
    array_step++;
  }
  return darray;

}
int get_fourier_size(){
	return gramsize;
}
void f16_array_to_int(char* in,int size,int* buffer){//size of buffer: size/2
  int i;
  unsigned char r1;
  unsigned char r2;
  short perm;
  int oitr=0;
  for(i=0;i<size;i++){
    r1=in[i];
    i++;
    r2=in[i];
    perm=(r2<<8)|r1;
    perm=perm*attwindow[i];
    buffer[oitr]=perm;
    oitr++;
  }
}

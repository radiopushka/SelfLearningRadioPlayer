#include <math.h>
#include<stdlib.h>
#define PI 3.141592653589793

double STEP_INC=0.5;

double calc_amplitude(int* buffer,int size,int freq){
  register double psum1=0;
  register double psum2=0;
  register int i;
  register double precalc=(((PI*2)*freq)/(size));
  register double precalc2;
  register double precalc3;
  int dst=0;
  for(i=0;i<size;i++){
    precalc2=precalc*i;
    dst=i+1;
    precalc3=precalc*dst;
    psum1=psum1+buffer[i]*cos(precalc2);
    psum2=psum2+buffer[i]*sin(precalc2);
    i++;
    psum1=psum1+buffer[dst]*cos(precalc3);
    psum2=psum2+buffer[dst]*sin(precalc3);

  }
  return (sqrt(psum1*psum1+psum2*psum2))/(size);
}
double* darray=NULL;
double* attwindow=NULL;
int gramsize=0;
int* frequencymap=NULL;
void free_fourier_transform(){
	free(darray);
	darray=NULL;
	free(attwindow);
	attwindow=NULL;
  free(frequencymap);
  frequencymap=NULL;
}
void init_fourier_transform(int size,double depth){//this size is f16 char divided by 2
	 gramsize=0;
	 int i;
	 STEP_INC=depth;
	 free_fourier_transform();
	 double step=1;
	 for(i=0;i<(size>>1);i=i+step){
 	   step=step+STEP_INC;
 	   gramsize++;
 	 }
    frequencymap=malloc(sizeof(int)*gramsize);
 	  darray=malloc(sizeof(double)*(gramsize));
 	  attwindow=malloc(sizeof(double)*(size));
 	  double stepc=(PI)/(size-1);
 	  double pos=0;
 	  for(i=0;i<size;i++){
 	  	attwindow[i]=cos(pos);
 	  	pos=pos+stepc;
 	  }
}
int get_freq_at_index(int i){
  return frequencymap[i];
}
double* produce_period_gram(int* buffer, int size){//this size is f16 char divided by 2
  int i;
  double step=1;
  int array_step=0; 
  for(i=0;i<(size>>1);i=i+step){
    step=step+STEP_INC;
    darray[array_step]=calc_amplitude(buffer,size,i);
    frequencymap[array_step]=i;
    array_step++;
  }
  return darray;

}
int get_fourier_size(){
	return gramsize;
}
void f16_array_to_int(char* in,int size,int* buffer){//size of buffer: size/2
  int i;
  short perm;
  int ssize=size>>1;
  short* typecc=(short*)in;
  for(i=0;i<ssize;i++){
    perm=typecc[i];
    perm=perm*attwindow[i];
    buffer[i]=perm;
  }
}

#include <math.h>
#define PI 3.141592653589793

double calc_amplitude(int* buffer,int size,int freq){
  double psum1=0;
  int i;
  for(i=0;i<size;i++){
    psum1=psum1+buffer[i]*cos((((PI*2)*freq)/(size))*i);
  }
  return psum1/(size);

}

double* produce_period_gram(int* buffer, int size, int max, int step){
  int i;
  double* darray=malloc(sizeof(double)*(max/step));
  int array_step=0; 
  for(i=0;i<max;i=i+step){
    darray[array_step]=calc_amplitude(buffer,size,i);
    array_step++;
  }
  return darray;

}
int* f16_array_to_int(char* in,int size){
  int* output=malloc(sizeof(int)*(size/2));
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
    output[oitr]=perm;
    oitr++;
  }
  return output;
}

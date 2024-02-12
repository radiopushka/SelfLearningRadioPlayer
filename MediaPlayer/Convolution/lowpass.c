#include<stdlib.h>
#include<math.h>
#include "lowpass.h"
#include<stdio.h>

#define PI 3.141592653589793

double* convolution_buffer=NULL;
int convolution_size=0;
int* ringbuffer1=NULL;
int* ringbuffer2=NULL;

void destroy_filter(){
	if(convolution_buffer!=NULL){
		free(convolution_buffer);
		convolution_buffer=NULL;
	}
	if(ringbuffer2!=NULL){
		free(ringbuffer2);
		ringbuffer2=NULL;
	}
	if(ringbuffer1!=NULL){
		free(ringbuffer1);
		ringbuffer1=NULL;
	}
}

int shift=0;
void setup_low_pass(int freqhz,int sample_rate){
	destroy_filter();
	if(freqhz>sample_rate){
		return;
	}
	convolution_size=(sample_rate)/(freqhz);
	if(convolution_size<3){
		return;//dont allocate memory, exit
	}
	convolution_buffer=malloc(sizeof(double)*(convolution_size));
	ringbuffer2=malloc(sizeof(int)*convolution_size);
	ringbuffer1=malloc(sizeof(int)*convolution_size);
	int i;
	double indexing=PI/((double)(convolution_size-1));
	double position=0;
	for(i=0;i<convolution_size;i++){
		ringbuffer1[i]=0;
		ringbuffer2[i]=0;
		double value=(sin(position));

		value=value*((3500000.0)/(pow(sample_rate-freqhz,1.108)));
		
		convolution_buffer[i]=value;
		position=position+indexing;
	}
}
char sem=0;
int bufferize(int input){
	//printf("%d\n",input);
	register int* curr_buff_ptr;
	switch(sem){
		case 1:
			sem=0;
			curr_buff_ptr=ringbuffer1;
			break;
		case 0:
			sem=1;
			curr_buff_ptr=ringbuffer2;
			break;
	}
	register int i;
	register long sum;
	register long divresult;
	sum=0;
	if(convolution_buffer==NULL){
		sem=0;
		return input;	
	}
	register int bound=convolution_size-1;
	register int imsum=0;
	register int b2=bound;
	for(i=0;i<bound;i++){
		divresult=(convolution_buffer[b2]*(curr_buff_ptr[i]));
		imsum=i+1;
		b2--;
		curr_buff_ptr[i]=curr_buff_ptr[imsum];
		sum=sum+divresult;
	}
	divresult=(convolution_buffer[0]*(curr_buff_ptr[i]));
	curr_buff_ptr[bound]=input;
	sum=sum+divresult;
	return sum/(convolution_size<<4);
}

void filter_buffer(char* buffer, int buffer_size){
	register unsigned char r1;
	register unsigned char r2;
	register int i2;
	register short perm=0;
	register int result;
	register unsigned char second;
	for(i2=0;i2<buffer_size;i2++){
		r1=buffer[i2];
		if(i2+2>buffer_size){
			break;
		}
		i2++;
		r2=buffer[i2];
		perm=(r2<<8)|r1;
		result=bufferize(perm);
		//result=perm;
		buffer[i2-1]=result;
		second=(result>>8);
		buffer[i2]=second;
		//printf("%d %d \n",r2,second);
	}
}


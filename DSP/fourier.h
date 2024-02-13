#ifndef _FOURIER
#define _FOURIER
  double calc_amplitude(int* buffer, int size,int freq);
  double* produce_period_gram(int* buffer,int size);//initially you could set the step size but for audio I want to see all the frequencies so i made the step increse.
  int get_fourier_size();
  //do not free the array this returns
  void init_fourier_transform(int max,double depth);//depth must be 1 or less, the lower the depth the more points get put in
  void free_fourier_transform();
  void f16_array_to_int(char* in,int size,int* buffer);
  //size of array buffer should be size/2
#endif // !_FOURIER
#ifndef LOWPASS
#define LOWPASS

void filter_buffer(char* buffer, int buffer_size);
void destroy_filter();
void setup_low_pass(int freqhz,int sample_rate);



#endif

#ifndef WAVFILEPARSE
#define WAVFILEPARSE
#include<stdio.h>
#include<stdlib.h>
struct WavHeader{
	unsigned int chunk_id;
	unsigned int chunk_size;
	unsigned int format;
	unsigned int subchunk_size;
	unsigned short formata;
	unsigned short channels;
	unsigned int rate;
	unsigned short align;
	unsigned short bps;
	unsigned int subchunk_size2;
	unsigned int byterate;
	unsigned int seconds;
	unsigned int writes;
};

struct WavHeader* read_wav_header(FILE* f);
void print_header(struct WavHeader* header);

#endif

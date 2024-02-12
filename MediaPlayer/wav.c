
#include "wav.h"

unsigned int endian_swap(unsigned int input){
	unsigned int output=(input>>8) | (input<<8);
	return output;
}

int minutes_from_sec(int sec){
	 int min=0;
	 int secc=sec;
	 while(secc>60){
	 	secc=secc-60;
	 	min++;
	 }
	 return min;
}

struct WavHeader* read_wav_header(FILE* f){
	struct WavHeader* hdr=malloc(sizeof(struct WavHeader));
	fread(&(hdr->chunk_id),sizeof(unsigned int),1,f);
	hdr->chunk_id=endian_swap(hdr->chunk_id);
	if(hdr->chunk_id!=1179604553){//RIFF
		free(hdr);
		return NULL;
	}
	fread(&(hdr->chunk_size),sizeof(unsigned int),1,f);
	fread(&(hdr->format),sizeof(unsigned int),1,f);
	hdr->format=endian_swap(hdr->format);
	if(hdr->format!=1447384897){//WAVE
		free(hdr);
		return NULL;
	}
	fread(&(hdr->subchunk_size),sizeof(unsigned int),1,f);
	fread(&(hdr->subchunk_size),sizeof(unsigned int),1,f);
	fread(&(hdr->formata),sizeof(unsigned short),1,f);
	fread(&(hdr->channels),sizeof(unsigned short),1,f);
	fread(&(hdr->rate),sizeof(unsigned int),1,f);
	fread(&(hdr->byterate),sizeof(unsigned int),1,f);
	fread(&(hdr->align),sizeof(unsigned short),1,f);
	fread(&(hdr->bps),sizeof(unsigned short),1,f);
	fread(&(hdr->subchunk_size2),sizeof(unsigned int),1,f);
	fread(&(hdr->subchunk_size2),sizeof(unsigned int),1,f);
	hdr->seconds=(hdr->chunk_size)/(hdr->byterate);
	hdr->writes=hdr->seconds;
	return hdr;
}
void print_header(struct WavHeader* header){
	printf("chunk id: %d\n",header->chunk_id);
	printf("chunk size: %d\n",header->chunk_size);
	printf("format: %d\n",header->format);
	printf("sub_chunk: %d\n",header->subchunk_size);
	printf("audio format: %d\n",header->formata);
	printf("channels: %d\n",header->channels);
	printf("rate: %d\n",header->rate);
	printf("bits per sample: %d\n",header->bps);
	printf("byte rate: %d\n",header->byterate);
	printf("chunk size 2: %d\n",header->subchunk_size2);
	printf("seconds: %d\n",header->seconds);
	int minutes=minutes_from_sec(header->seconds);
	int over=header->seconds-minutes*60;
	printf("time: %d:%d\n",minutes,over);
}

#include "alsainterface.c"
#include <vorbis/vorbisfile.h>
//gcc wavtesting.c wav.c -lvorbis -lvorbisfile -lasound

void test_ogg(){
   	OggVorbis_File vf;
   	int eof = 0;
   	int current_section;
   	int err = ov_fopen("/home/ivan/radio/tenki-youhou - track2.ogg", &vf);
   	if(err!=0){return;}
	
	snd_pcm_t* inter= get_interface();
	char* buffer=malloc(sizeof(char)*BUFFER_SIZE);
	int i;
	
	while(1){
		long ret = ov_read(&vf, buffer, BUFFER_SIZE, 0, 2, 1,&current_section);
		if(ret==0){
			break;
		}
		write_buffer_ogg(inter,buffer,ret);
	
	}
	free(buffer);
	remove_alsa_channel(inter);
	ov_clear(&vf);
}


int main(){
	//test_ogg();
	//return 0;
	FILE* f=fopen("/home/ivan/radio/fmplayer/ongaku/giantsshyojyoremix.wav","rb");
	struct WavHeader* header=read_wav_header( f);
	print_header(header);
	snd_pcm_t* inter= get_interface();
	char* buffer=malloc(sizeof(char)*BUFFER_SIZE);
	int i;
	int writes=(header->writes)*((header->byterate)/BUFFER_SIZE);
	for(i=0;i<writes;i++){
		fread(buffer,sizeof(char),BUFFER_SIZE,f);
		write_buffer(inter,buffer);
	}
	free(buffer);
	remove_alsa_channel(inter);
	fclose(f);
	free(header);
	return 0;
}

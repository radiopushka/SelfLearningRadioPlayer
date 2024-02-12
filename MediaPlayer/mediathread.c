#include "../SongSelect/songselector.h"
#include <pthread.h>
#include "mediathread.h"
#include "alsainterface.c"
#include "../Logger/logging.h"
#include <vorbis/vorbisfile.h>

pthread_mutex_t data_mux; 
pthread_mutex_t signal_mux; 

snd_pcm_t* inter=NULL;

int fwd_speed=500;

int signal_var=0;

int low_pass_freq_m=9500;

int prog_top=0;
int prog_bottom=0;

void init_media_thread(){
	pthread_mutex_init(&data_mux, NULL);
	pthread_mutex_init(&signal_mux, NULL);
	pthread_mutex_unlock(&data_mux);
	pthread_mutex_unlock(&signal_mux);
}

void request_data_mutex(){
	pthread_mutex_lock(&data_mux);

}
void release_data_mutex(){
	pthread_mutex_unlock(&data_mux);
}
void signal(int signal){
  	pthread_mutex_lock(&signal_mux);
  	signal_var=signal;
  	pthread_mutex_unlock(&signal_mux);

}
int play_file(char* path){
	if(path==NULL){
		return 1;
	}
	int bufferskip=0;
	int format=get_audio_format(path);//1 is ogg, 2 is wav
	if(format<0){
		return -1;
	}
	if(inter==NULL){
		inter= get_interface();
	}
	char buffer[BUFFER_SIZE];
	if(format==1){
		OggVorbis_File vf;
   		int current_section;
   		int err = ov_fopen(path, &vf);
   		if(err!=0){
   			return -1;
   		}
   		vorbis_info *vi = ov_info(&vf, -1);
   		int rate=vi->rate;
   		if(vi->channels<2){
   			rate=rate/2;
   		}
   		set_sample_rate(inter,rate,2);
   		double time=ov_time_total(&vf,-1);
   		double ctime;
   		long ret;
   		while(1){
			ret = ov_read(&vf, buffer, BUFFER_SIZE, 0, 2, 1,&current_section);
			if(ret==0){
				break;
			}
			if(bufferskip<=0){
				write_buffer_ogg(inter,buffer,ret);
			}else{
				bufferskip--;
			}
			ctime=ov_time_tell(&vf);
			pthread_mutex_lock(&signal_mux);
			set_low_pass_frequency(low_pass_freq_m);
			prog_bottom=(int)time;
			prog_top=(int)ctime;
			if(signal_var==SIGNAL_EXIT){
				ov_clear(&vf);
				pthread_mutex_unlock(&signal_mux);
				return SIGNAL_EXIT;	
			}else if(signal_var==SIGNAL_PAUSE){
				pthread_mutex_unlock(&signal_mux);
				while(1){
					sleep(1);
					pthread_mutex_lock(&signal_mux);
					if(signal_var==SIGNAL_PLAY){
						pthread_mutex_unlock(&signal_mux);
						break;
					}
					if(signal_var==SIGNAL_EXIT){
						ov_clear(&vf);
						pthread_mutex_unlock(&signal_mux);
						return SIGNAL_EXIT;	
					}
					pthread_mutex_unlock(&signal_mux);
				}
			}else if(signal_var==SIGNAL_FWD){
				signal_var=0;
				pthread_mutex_unlock(&signal_mux);
				bufferskip=fwd_speed;
			}else if(signal_var==SIGNAL_SKIP){
				signal_var=0;
				pthread_mutex_unlock(&signal_mux);
				ov_clear(&vf);
				return 1;
			}else{
				pthread_mutex_unlock(&signal_mux);
			}
		}
		ov_clear(&vf);
	}else{
		FILE* f=fopen(path,"rb");
		struct WavHeader* header=read_wav_header( f);
		if(header==NULL){
			fclose(f);
			return -1;
		}
		int rate=header->rate;
		if(header->channels<2){
			rate=rate/2;
		}
		set_sample_rate(inter,header->rate,2);
		int i;
		int writes=(header->writes)*((header->byterate)/BUFFER_SIZE);
		free(header);
		for(i=0;i<writes;i++){
			fread(buffer,sizeof(char),BUFFER_SIZE,f);
			if(bufferskip<=0){
				write_buffer(inter,buffer);
			}else{
				bufferskip--;
			}
			pthread_mutex_lock(&signal_mux);
			set_low_pass_frequency(low_pass_freq_m);
			prog_top=i;
			prog_bottom=writes;
			if(signal_var==SIGNAL_EXIT){
				fclose(f);
				pthread_mutex_unlock(&signal_mux);
				return SIGNAL_EXIT;	
			}else if(signal_var==SIGNAL_PAUSE){
				pthread_mutex_unlock(&signal_mux);
				while(1){
					sleep(1);
					pthread_mutex_lock(&signal_mux);
					if(signal_var==SIGNAL_PLAY){
						pthread_mutex_unlock(&signal_mux);
						break;
					}
					if(signal_var==SIGNAL_EXIT){
						fclose(f);
						pthread_mutex_unlock(&signal_mux);
						return SIGNAL_EXIT;	
					}
					pthread_mutex_unlock(&signal_mux);
				}
			}else if(signal_var==SIGNAL_FWD){
				signal_var=0;
				pthread_mutex_unlock(&signal_mux);
				bufferskip=fwd_speed;
			
			}else if(signal_var==SIGNAL_SKIP){
				signal_var=0;
				pthread_mutex_unlock(&signal_mux);
				fclose(f);
				return 1;
			}else{
				pthread_mutex_unlock(&signal_mux);
			}
		}
		fclose(f);
	}
	return 1;
}
char* song_name_d=NULL;
void* media_runable(void* arg){
	pthread_detach(pthread_self()); 
	char* song_name=NULL;
	int stop=0;
	char* psong=NULL;
	int is_null=0;
	char* path_capture;
	int attempts=0;
	while(stop==0){
		pthread_mutex_lock(&data_mux);//critical section
		struct Song* tsugikyouku=get_next_song(psong);
		if(tsugikyouku->path==NULL){
			is_null=1;
			free_song_structure(tsugikyouku);
		}else{
			if(psong!=NULL){
			while(strcmp(psong,tsugikyouku->path)==0){
				free_song_structure(tsugikyouku);
				tsugikyouku=get_next_song(psong);
				attempts++;
				if(attempts>100){
					break;
				}
			}
			}
			is_null=0;
			attempts=0;
    			path_capture=pointerize(tsugikyouku->path);
    			song_name=pointerize(tsugikyouku->name);
    			song_name_d=song_name;
    			free_song_structure(tsugikyouku);
    		}
		pthread_mutex_unlock(&data_mux);
		int ccap=0;
		if(is_null==0){
			if((ccap=play_file(path_capture))==-1){
				sleep(1);
				log_line("failed to play song:");
				log_line(path_capture);
			}
		}
		if(ccap==SIGNAL_EXIT){
			stop=1;
		}
		if(is_null==0){
			if(psong!=NULL){
  				free(psong);
  			}
    			psong=pointerize(path_capture);
    			free(path_capture);
    			path_capture=NULL;
    			pthread_mutex_lock(&data_mux);
    			free(song_name);
    			song_name=NULL;
    			song_name_d=NULL;
    			pthread_mutex_unlock(&data_mux);
		}else{
			sleep(1);
		}
		pthread_mutex_lock(&signal_mux);
		if(signal_var==SIGNAL_EXIT){
			stop=1;
		}
		pthread_mutex_unlock(&signal_mux);
	}
	pthread_mutex_lock(&data_mux);
	if(psong!=NULL){
		free(psong);
	}
	if(song_name!=NULL){
		free(song_name);
	}
	if(path_capture!=NULL){
		free(path_capture);
	}
	if(inter!=NULL){
		remove_alsa_channel(inter);
	}
	pthread_mutex_unlock(&data_mux);
	pthread_mutex_destroy(&data_mux);
	pthread_mutex_destroy(&signal_mux);
	pthread_exit(NULL);
}
char* get_current_song_name(){
	char* memptr=NULL;
	pthread_mutex_lock(&data_mux);
	if(song_name_d!=NULL){
		memptr=pointerize(song_name_d);
	}
	pthread_mutex_unlock(&data_mux);
	return memptr;
}
int get_total_time(){
	int time=0;
	pthread_mutex_lock(&signal_mux);
	time=prog_bottom;
	pthread_mutex_unlock(&signal_mux);
	return time;
}
int get_position(){
	int time=0;
	pthread_mutex_lock(&signal_mux);
	time=prog_top;
	pthread_mutex_unlock(&signal_mux);
	return time;
}
void set_low_pass_freq(int freq){
	pthread_mutex_lock(&signal_mux);
	low_pass_freq_m=freq;
	pthread_mutex_unlock(&signal_mux);
}

#include <alsa/asoundlib.h>
#include <stdio.h>
#include "Convolution/lowpass.h"
#include "wav.h"

#define DEVICE "default"
#define RATE 44100

int BUFFER_SIZE=0;
snd_pcm_uframes_t frames;

int low_pass_freq=10000;

char resolve=0;

int c_rate=RATE;

snd_pcm_t* get_interface(){
	
	snd_pcm_t *pcm_handle;
	int channels=2;
	
	if ( snd_pcm_open(&pcm_handle, DEVICE,SND_PCM_STREAM_PLAYBACK, 0) < 0){
		return NULL;
	} 
	if(snd_pcm_set_params(pcm_handle, SND_PCM_FORMAT_S16_LE,SND_PCM_ACCESS_RW_INTERLEAVED, 2, RATE, 1, 500000)<0){
		return NULL;
	}
	frames=1024;
	setup_low_pass(low_pass_freq,RATE*channels);
	resolve=0;
	BUFFER_SIZE = frames * channels * 2;
	return pcm_handle;
}
		int pcm;
int write_buffer_ogg(snd_pcm_t* pcm_handle,char* buff,long frms){
		if(c_rate>low_pass_freq){
			filter_buffer(buff,frms);
		}
		if(resolve==1){
			setup_low_pass(low_pass_freq,c_rate);
			resolve=0;
		}
		if ((pcm = snd_pcm_writei(pcm_handle, buff, frms>>2)) == -EPIPE) {
			snd_pcm_prepare(pcm_handle);
		} else if (pcm < 0) {
			return -1;
		}
		return 1;
}
void set_low_pass_frequency(int freq){
	if(freq!=low_pass_freq){
		resolve=1;
	}
	low_pass_freq=freq;
}
void set_sample_rate(snd_pcm_t* iface,int rate,int channels){
	c_rate=rate;
	if(channels==1){
		c_rate=c_rate/2;
	}
	if(snd_pcm_set_params(iface, SND_PCM_FORMAT_S16_LE,SND_PCM_ACCESS_RW_INTERLEAVED, channels, c_rate, 1, 500000)<0){
	}
	resolve=0;
	setup_low_pass(low_pass_freq,c_rate);
}
int write_buffer(snd_pcm_t* pcm_handle,char* buff){
	return write_buffer_ogg(pcm_handle,buff,frames<<2);
}
void remove_alsa_channel(snd_pcm_t* alsa){
	snd_pcm_drain(alsa);
	snd_pcm_close(alsa);
	destroy_filter();
}

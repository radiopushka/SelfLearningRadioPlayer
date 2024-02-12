#include "mediathread.h"
#include "../SongSelect/songselector.h"
#include<stdio.h>
#include <pthread.h>

int main(){
  load_playlists();
  init_media_thread();
  pthread_t ptid; 
  pthread_create(&ptid, NULL, &media_runable, NULL); 
  //wait for thread to exit);
  set_low_pass_freq(7000);
  char c=0;
  while(c!='e'){
  	c=getchar();
  	if(c=='f'){
  		signal(SIGNAL_FWD);
  	}
  	int top=get_total_time();
  	int b=get_position();
  	printf("%d/%d\n",b,top);
  }
  signal(SIGNAL_EXIT);
  pthread_join(ptid, NULL);

  save_playlists();
  clear_all_data_structures();
  free_playlist_stack_mem();
  return 0;
}

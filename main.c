#include "./MediaPlayer/mediathread.h"
#include "./SongSelect/songselector.h"
#include <pthread.h>
#include "./UserInterface/playlisteditor.h"
#include "UserInterface/main.c"

int main(){
  load_playlists();
  init_media_thread();
  pthread_t ptid; 
  pthread_create(&ptid, NULL, &media_runable, NULL); 
  //wait for thread to exit);
  //set_low_pass_freq(7000);
  central_interface();


  save_playlists();
  clear_all_data_structures();
  free_playlist_stack_mem();
  return 0;
}


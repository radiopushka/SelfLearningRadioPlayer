#include "songselector.h"
#include<stdio.h>
#include<stdlib.h>

int main(){
  load_playlists();
  char* psong=NULL;
  int max=10;
  int i;
  struct Playlist* first=get_playlist_at_index(0);
 // add_element_for(first->problist,"/home/ivan/radio/orange berry kurakushyon - Sakura.ogg");
  //push_score_up_for(first->problist,"/home/ivan/radio/orange berry kurakushyon - Sakura.ogg","/home/ivan/radio/tenki-youhou-track3.ogg");
  queue_song(get_song_st("/home/ivan/radio/akihabara.ogg","akihabara"));
  for(i=0;i<max;i++){
  	
    struct Song* tsugikyouku=get_next_song(psong);
    if(psong!=NULL){
  		free(psong);
  	}
    psong=pointerize(tsugikyouku->path);
    printf("%s\n",tsugikyouku->path);
    free_song_structure(tsugikyouku);
  }

	dump_as_text(first->problist);
	queue_song(get_song_st("/home/ivan/radio/akihabara.ogg","akihabara"));
 if(psong!=NULL){
  	free(psong);
  }
  save_playlists();
  clear_all_data_structures();
  free_playlist_stack_mem();
  return 0;
}

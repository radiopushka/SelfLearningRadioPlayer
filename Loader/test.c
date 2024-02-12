#include "playlistloader.h"
#include <stdio.h>


//test case
//save 5 playlists, delete one in the middle
// clear and restart
//passed, no leaks
int main(int argn, char* argv){
  load_playlists();
  struct Playlist* pl=create_new_playlist("testing");
  //playlist_add_song(pl,"test","path");
  mass_add_directory(pl,"/home/ivan/radio/");
  add_playlist(pl);
  int size=playlist_count();
  int i;
  for(i=0;i<size;i++){
    struct Playlist* pll=get_playlist_at_index(i);
    pll->day_en=i;
    print_playlist(pll);
  }
  if(size>4){
  	remove_playlist(2);
  	size=playlist_count();
  	printf("--------------\n");
  	for(i=0;i<size;i++){
   		 struct Playlist* pll=get_playlist_at_index(i);
    	         print_playlist(pll);
 	 }
  	while(remove_playlist(0)==0);
  }
  save_playlists();
  free_playlist_stack_mem();
  return 0;

}

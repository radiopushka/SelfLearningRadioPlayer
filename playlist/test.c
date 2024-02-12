#include<stdio.h>
#include "playlist.h"

int main(){
  struct Playlist* pl=create_new_playlist("testing");
  playlist_add_song(pl,"poop","ass/poop");
  playlist_add_song(pl,"piss","ass/poop");
  playlist_add_song(pl,"sex","ass/poop");
  print_playlist(pl);
  push_index_down(pl,0);
  print_playlist(pl);
  push_index_down(pl,1);
  push_index_down(pl,0);
  print_playlist(pl);
  write_playlist_to_file(pl,"playlistsave");
  free_playlist_from_memory(pl);
  pl=get_playlist_from_file("playlistsave");
  print_playlist(pl);
  free_playlist_from_memory(pl);
  struct StackPointer* stack=NULL;
  add_item_to_stack(&stack,pointerize("poop"));
  add_item_to_stack(&stack,pointerize("shit"));
  dump_stack(&stack);
  free_stack(&stack);
}

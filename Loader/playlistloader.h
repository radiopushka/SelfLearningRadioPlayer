#ifndef _PLLOADER
#define _PLLOADER
#define BASE_DIR "/.AdvancedRadioPlayer/"
#define IS_PLAYLIST_RULE 'P'
#include "../playlist/playlist.h"
struct PlaylistContainer{
  struct Playlist* pl;
  struct PlaylistContainer* next;
};
//we need to load all the playlist present in the program's resource directory
//we will then put them all into memory as a data structure
//we will be able to write and read all of them in bulk
void load_playlists();
void save_playlists();
struct Playlist* get_playlist_at_index(int i);
void add_playlist(struct Playlist* input);
int remove_playlist(int index);
void free_playlist_stack_mem();
char* string_combine(char* one, char* two);
int playlist_count();
//usage:
/*
first load the playlists
check the playlist_count(); integer
add/remove playlist, remove playlist returns -1 if it could not remove, else returns 0

once dont save the playlists
then free the memory
*/
int get_audio_format(char* file);//the path given, returns if the file is ogg(1) or wav(2)
int is_not_directory(char* path);//returns 1 if it is a file
int mass_add_directory(struct Playlist* pl,char* dir);//adds all the new files in the directory to a playlist
char* get_true_name(char* filename);//returns the true name of the song without any file extensions
char* get_write_directory();

#endif // DEBUG

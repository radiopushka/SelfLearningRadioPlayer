#ifndef __SONGSELECT
#define __SONGSELECT
#include "../Loader/playlistloader.h"
/*
 *we need to take all the playlists in the scope 
 new struct called Song 
A stack of songs which is the queue
strategy:
-keep track of the size of the playlist's randomization
-if the max value of it changes, free the main data structure and re allocate memory
-the main data structure is an integer array of fixed size
-on every run the data structure gets populated from the start
-a random number is drawn
-the random number might fall on any spot in that array, that song will play 
-a song might have more than one probability
-user will be able to select wether to queu the song without incrementing its probability
-user will be able to clear propbability of it too
 */
struct Song{
  char* name;
  char* path;
};
void free_playlist_scope();//clears the memory of the playlist structure
void scope_playlists();
//holds and maintains a data structure containing all the playlists in scope
//if there is a scope mismatch, the data structure will be recreated
char* generate_song_path(char* ignorepath);
//this function creates that array and populates it, if there is a memory mismatch it re-allocates the memory before hand
struct Song* get_next_song(char* ignorepath);
//this method calls all previous
//the value returned by this method needs to be freed
//note: these methods access the main memory heap and need to be  
//done under a mutex 
void clear_all_data_structures();
void set_id_frequency(int minutes);
int get_id_frequency();
void free_song_structure(struct Song* song);
void queue_song(struct Song* song);//note: this function does not make a copy of struct Song
void resort_queue();
char* pop_queue_top();//returns copy of top song
struct Song* get_song_st(char* file,char* name);
void set_learn_speed(int speed);
//sets how often an ID is played, minimum value is 4 minutes

/*
procedure:
load_playlists(); (playlistloader.h)
launch player thread.
obtain semaphore green light (player.h)
manipulate,edit, etc.. markov chains
give back semaphore light (player.h)

player thread:
obtain semaphore green light
run get_next_song();
if last song is not null and song path is the same, free the song structure including all the strings and run again
give back semaphore light
if song plays succesfully, save its path to non-repeat qeue, if not, log it
if song is null, wait for 1 second,
if not null, free the strings inside the Song struct
free the Song struct either way
go back to top
*/
#endif // !__SONGSELECT

#ifndef __playlist
#define __playlist
#include "../SongSelect/markovchain.h"
//note: new songs are pushed from the top into the stack
struct PlaylistHead{
  struct PlaylistHead* next;
  char* name;
  char* file;
  int probability;
};

struct TimeFrame{
   char day_st;
   char hour_st;
   char min_st;
   char day_en;
   char hour_en;
   char min_en;


   struct TimeFrame* next;
};

struct Playlist{
  struct PlaylistHead* head;
  char* name;
  int type;
  int size;

  //suppoert for many time frames
  //char day_st;
  //char hour_st;
  //char min_st;
  //char day_en;
  //char hour_en;
  //char min_en;

  struct TimeFrame* tframes;

  struct PlaylistMarker* problist;
};

//methods:

struct Playlist* create_new_playlist(char* name);

void rename_playlist(struct Playlist* pl,char* name);

void free_song_from_memory(struct PlaylistHead* song);

void free_playlist_from_memory(struct Playlist* pl);

void playlist_add_song(struct Playlist* pl,char* name, char* file);
void playlist_add_song_adv(struct Playlist* pl,char* name, char* file,int prob);

char* get_songname_at_index(struct Playlist* pl, int index);

char* get_songfile_at_index(struct Playlist* pl, int index);

void rename_song_at(struct Playlist* pl,int index,char* name);

void repath_song_at(struct Playlist* pl,int index,char* path);

void delete_song_at_index(struct Playlist* pl,int index);

void push_index_down(struct Playlist* pl,int index);

void print_playlist(struct Playlist* pl);

void write_playlist_to_file(struct Playlist* pl,char* file);
struct Playlist* get_playlist_from_file(char* file);

int find_index_match_name(struct Playlist* pl, char* name);

int find_index_match_file(struct Playlist* pl, char* file);

struct PlaylistHead* get_head_location_at(struct Playlist* pl,int loc);
//do not free the value this returns


// date control functions:
//
void put_date(struct Playlist* pl,char d_s, char h_s, char m_s, char d_e, char h_e, char m_e);

// number of indexes in the date structure
int date_size(struct Playlist* pl);

//removes the date at index int the list
void remove_date(struct Playlist* pl, int index);

//gets the date at that index
struct TimeFrame* get_date_at(struct Playlist* pl, int index);

//returns 1 if the date falls in the range of the playlist
//returns 1 if no dates are set
int is_playlist_in_scope(struct Playlist* pl,char d, char h, char m);

#endif


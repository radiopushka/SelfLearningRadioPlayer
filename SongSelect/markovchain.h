#ifndef __MARKOV
#define __MARKOV
#include "../playlist/stack.h"
#include <stdio.h>
//note: clearing a songs probability will clear it from the Markov chain 
struct ProbabilityList{
  char* path;
  int score;
};
struct SongHeader{
  char* path;
  int stack_size;
  int total_prob_score;
  struct StackPointer* listhead;
};
struct PlaylistMarker{
  int stack_size;
  struct StackPointer* songheaderlist;
};
struct SongHeader* get_song_structure(struct PlaylistMarker* playlist,char* path);
void remove_path_from_song_header(struct SongHeader* header,char* path);
void remove_complete_header(struct SongHeader* header);
void add_element_for(struct PlaylistMarker* playlist,char* path);
void delete_element_for(struct PlaylistMarker* playlist,char* path);
void push_score_up_for(struct PlaylistMarker* playlist,char* path,char* targ);
struct PlaylistMarker* create_new_markov_chain();
int get_extra_probability_score_for_song(struct PlaylistMarker* playlist,char* path);
char* get_song_from_probability(struct PlaylistMarker* playlist,char* path,int prob);//takes 0- <extra prob score
//ex: score is 4, get song will take from 0 to 3
//note, return value needs to be freed

void write_chain_to_file(struct PlaylistMarker* playlist,FILE* f);
struct PlaylistMarker* read_chain_from_file(FILE* f);
void free_memory_markov(struct PlaylistMarker* playlist);

void dump_as_text(struct PlaylistMarker* playlist);


#endif // !__MARKOV

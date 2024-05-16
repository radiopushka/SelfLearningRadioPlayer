#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "playlist.h"
//memory management
struct Playlist* create_new_playlist(char* name){
  struct Playlist* pl=malloc(sizeof(struct Playlist));
  pl->head=NULL;
  int ns=strlen(name);
  pl->type=0;
  pl->name=malloc(sizeof(char)*(ns+1));
  sprintf(pl->name,"%s",name);
  pl->size=0;
  //date sets:
  pl -> tframes = NULL;
  //pl->day_st=0;
  //pl->hour_st=0;
  //pl->min_st=0;
  //
  //pl->day_en=0;
  //pl->hour_en=0;
  //pl->min_en=0;
  pl->problist=create_new_markov_chain();
  return pl;
}//rename
void rename_playlist(struct Playlist* pl,char* name){
  int ns=strlen(name);
  free(pl->name);
  pl->name=malloc(sizeof(char)*(ns+1));
  sprintf(pl->name,"%s",name);

}//clear the memory
void free_song_from_memory(struct PlaylistHead* song){
  free(song->name);
  free(song->file);
  free(song);
}
void free_playlist_from_memory(struct Playlist* pl){

  int size=pl->size;
  struct PlaylistHead* head=pl->head;
  struct TimeFrame* tf = pl -> tframes;

  while( tf != NULL ){
    
    struct TimeFrame* next = tf -> next;

    free(tf);

    tf = next;
  }

  int i;
  for(i=0;i<size;i++){
    struct PlaylistHead* tsugi=head->next;
    free_song_from_memory(head);
    head=tsugi;
  }
  free(pl->name);
  free_memory_markov(pl->problist);
  free(pl);
}
//-----
//helpers:
char* repoint_string(char* input){
  int len=strlen(input);
  char* newc=malloc(sizeof(char)*(len+1));
  sprintf(newc,"%s",input);
  return newc;
}
//---
//manipulations, add, push up, push down, delete, song rename,indexing
void playlist_add_song_adv(struct Playlist* pl,char* name, char* file,int prob){
  pl->size=pl->size+1;
  struct PlaylistHead* newsong=malloc(sizeof(struct PlaylistHead));
  newsong->name=repoint_string(name);
  newsong->file=repoint_string(file);
  newsong->probability=prob;
  newsong->next=pl->head;
  pl->head=newsong;
}
void playlist_add_song(struct Playlist* pl,char* name, char* file){
	playlist_add_song_adv(pl,name,file,1);
}
//get song name song file
char* get_songname_at_index(struct Playlist* pl, int index){
  int size=pl->size;
  if(index>=size||index<0){
  	return NULL;
  }
  int i;
  struct PlaylistHead* atama=pl->head;
  for(i=0;i<size;i++){
    if(atama==NULL){
    	return NULL;
    }
    if(i==index){
      return repoint_string(atama->name);
    }
    atama=atama->next;
  }
  return NULL;
}
char* get_songfile_at_index(struct Playlist* pl, int index){
  int size=pl->size;
  if(index>=size||index<0){
  	return NULL;
  }
  int i;
  struct PlaylistHead* atama=pl->head;
  for(i=0;i<size;i++){
    if(atama==NULL){
    	return NULL;
    }
    if(i==index){
      return repoint_string(atama->file);
    }
    atama=atama->next;
  }
  return NULL;
}
//rename song, change path
void rename_song_at(struct Playlist* pl,int index,char* name){
  int size=pl->size;
  if(index>=size||index<0){
  	return;
  }
  int i;
  struct PlaylistHead* atama=pl->head;
  for(i=0;i<size;i++){
    if(i==index){
      free(atama->name);
      atama->name=repoint_string(name);
    }
    atama=atama->next;
  }

}
void repath_song_at(struct Playlist* pl,int index,char* path){
  int size=pl->size;
  if(index>=size||index<0){
  	return;
  }
  int i;
  struct PlaylistHead* atama=pl->head;
  for(i=0;i<size;i++){
    if(i==index){
      delete_element_for(pl->problist,atama->file);
      free(atama->file);
      atama->file=repoint_string(path);
    }
    atama=atama->next;
  }

}
//delete a song from memory
//by far the most sophisticated function
void delete_song_at_index(struct Playlist* pl,int index){
  int size=pl->size;
  if(index>=size||index<0){
  	return;
  }
  struct PlaylistHead* atama=pl->head;
  if(index==0){
    atama=atama->next; 
    delete_element_for(pl->problist,(pl->head)->file);
    free_song_from_memory(pl->head);
    pl->head=atama;
    pl->size=pl->size-1;
    return;
  }
  int i;
  struct PlaylistHead* prev;
  for(i=0;i<size;i++){
    if(i==index){
      struct PlaylistHead* nexts=atama->next;
      delete_element_for(pl->problist,atama->file);
      free_song_from_memory(atama);
      prev->next=nexts;
      pl->size=pl->size-1;
      return;
    }
      prev=atama;
      atama=atama->next;
  }

}
//push and pull items in the stack
void push_index_down(struct Playlist* pl,int index){
  int size=pl->size;
   if(index>=size||index<0){
  	return;
  }
  if(size<2){
    return;
  }
  struct PlaylistHead* atama=pl->head;
  struct PlaylistHead* next=NULL;
  struct PlaylistHead* nextnext=NULL;
  if(index==0){
    next=atama->next; 
    nextnext=next->next;
    pl->head=next;
    atama->next=nextnext;
    next->next=atama;
    return;
  }

  struct PlaylistHead* prev1=NULL;
  next=NULL;
  int i;
  for(i=0;i<size-1;i++){
    if(i==index){
      next=atama->next;
      atama->next=next->next;
      next->next=atama;
      prev1->next=next;
      return;
    }
    prev1=atama;
    atama=atama->next;
  }
}
//debug methods:
/*
 * ---
 */
void print_playlist(struct Playlist* pl){
  int size=pl->size;
  struct PlaylistHead* atama=pl->head;
  int i;

  struct TimeFrame* tf = pl -> tframes;
  while( tf != NULL ){
    printf("%d %d %d  -  %d %d %d\n",tf->day_st,tf->hour_st,tf->min_st,tf->day_en,tf->hour_en,tf->min_en);
    tf = tf -> next;
  }

  printf("name: %s, type:%d\n",pl->name,pl->type);
  printf("size: %d\n",size);
  for(i=0;i<size;i++){
    printf("(%d)%d: %s %s\n",atama->probability,i,atama->file,atama->name);
    atama=atama->next;
  }

}
int find_index_match_name(struct Playlist* pl, char* name){
	int size=pl->size;
	int i;
	struct PlaylistHead* atama=pl->head;
	for(i=0;i<size;i++){
		if(strcmp(atama->name,name)==0){
			return i;
		}
		atama=atama->next;
	}
	return -1;
}
int find_index_match_file(struct Playlist* pl, char* file){
	int size=pl->size;
	int i;
	struct PlaylistHead* atama=pl->head;
	for(i=0;i<size;i++){
		if(strcmp(atama->file,file)==0){
			return i;
		}
		atama=atama->next;
	}
	return -1;
}
struct PlaylistHead* get_head_location_at(struct Playlist* pl,int loc){
	int size=pl->size;
	int i;
	struct PlaylistHead* atama=pl->head;
	for(i=0;i<size;i++){
		if(i==loc){
			return atama;
		}
		atama=atama->next;
	}
	return NULL;
}



void put_date(struct Playlist* pl,char d_s, char h_s, char m_s, char d_e, char h_e, char m_e){
  struct TimeFrame* tf = malloc(sizeof(struct TimeFrame));

  tf -> day_st = d_s;

  tf -> hour_st = h_s;

  tf -> min_st = m_s;


  tf -> day_en = d_e;

  tf -> hour_en = h_e;

  tf -> min_en = m_e;

  tf -> next = pl -> tframes;
  
  pl -> tframes = tf;

}

int date_size(struct Playlist* pl){

  struct TimeFrame* tf = pl -> tframes;

  int c = 0;

  while(tf != NULL){
    c++;
    tf = tf -> next;
  }

  return c;
}

void remove_date(struct Playlist* pl, int index){
  
  if(index == 0){
    struct TimeFrame* next = pl -> tframes -> next;

    free( pl -> tframes );
    pl -> tframes = next;
    return;
  }

  int i = 1;

  struct TimeFrame* it = pl -> tframes -> next;
  struct TimeFrame* prev = pl -> tframes;

  while ( it != NULL ){
    
    if( i == index ){
      prev -> next = it -> next;
      free(it);
      return;
    }

    i++;
    it = it -> next;
  }


}

struct TimeFrame* get_date_at(struct Playlist* pl, int index){

  int i = 0;



  struct TimeFrame* tf = pl -> tframes;

  while( tf != NULL ){
    if(i == index)
      return tf;

    i++;
    tf = tf -> next;
  }
  return NULL;
}

// inclusive, returns 0 if in range, 1 if over, -1 if under
//
int is_date_in_scope(struct TimeFrame* tf, char d, char h, char m){
  
  unsigned long long start_stamp = (tf -> day_st)<<16 | (tf -> hour_st)<<8 | (tf -> min_st);

  unsigned long long end_stamp = (tf -> day_en)<<16 | (tf -> hour_en)<<8 | (tf -> min_en);

  unsigned long long c_stamp = d<<16 | h<<8 | m;

  if( c_stamp > end_stamp )
    return 1;

  if( c_stamp < start_stamp )
    return -1;

  return 0;
}

int is_playlist_in_scope(struct Playlist* pl,char d, char h, char m){
  struct TimeFrame* tf = pl -> tframes;
  
  if( tf == NULL ){
    return 1;
  }

  while( tf != NULL ){
    if( is_date_in_scope(tf, d, h , m) == 0)
      return 1;
    tf = tf -> next;
  }
  return -1;
}



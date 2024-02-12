#include "playlist.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
/*
This is responsible for reading and writing playlist structures to and from files
these functions could be expanded to incorporate things like start and end times as well as other meta data
*/

void write_playlist_to_file_helper(struct Playlist* pl,char* file){
  FILE* f=fopen(file,"wb");
  /*
  pl->day_st
  pl->hour_st
  pl->min_st
  
  pl->day_en
  pl->hour_en
  pl->min_en
  */
   write_chain_to_file(pl->problist,f);
  fwrite(&(pl->day_st),sizeof(char),1,f);
  fwrite(&(pl->hour_st),sizeof(char),1,f);
  fwrite(&(pl->min_st),sizeof(char),1,f);
  fwrite(&(pl->day_en),sizeof(char),1,f);
  fwrite(&(pl->hour_en),sizeof(char),1,f);
  fwrite(&(pl->min_en),sizeof(char),1,f);
  int ppsize=pl->size;
  int rtpe=pl->type;
  fwrite(&rtpe,sizeof(int),1,f);
  fwrite(&ppsize,sizeof(int),1,f);
  char* name=pl->name;
  int size=strlen(name);
  fwrite(&size,sizeof(int),1,f);
  fwrite(name,sizeof(char),size,f);
  struct PlaylistHead* atama=pl->head;
  int i;
  for(i=0;i<ppsize;i++){
    fwrite(&(atama->probability),sizeof(int),1,f);
    size=strlen(atama->name);
    fwrite(&size,sizeof(int),1,f);
    size=strlen(atama->name);
    fwrite(atama->name,sizeof(char),size,f);
    size=strlen(atama->file);
    fwrite(&size,sizeof(int),1,f);
    size=strlen(atama->file);
    fwrite(atama->file,sizeof(char),size,f);
    atama=atama->next;
  }
  fclose(f);
}

struct Playlist* get_playlist_from_file(char* file){
	
  FILE* f=fopen(file,"rb");
  char day_st;
  char hour_st;
  char min_st;
  char day_en;
  char hour_en;
  char min_en;
  struct PlaylistMarker* mchain=read_chain_from_file(f);
  fread(&day_st,sizeof(char),1,f);
  fread(&hour_st,sizeof(char),1,f);
  fread(&min_st,sizeof(char),1,f);
  fread(&day_en,sizeof(char),1,f);
  fread(&hour_en,sizeof(char),1,f);
  fread(&min_en,sizeof(char),1,f);
  int ppsize;
    int typel;
  fread(&typel,sizeof(int),1,f);
  fread(&ppsize,sizeof(int),1,f);
  int size;
  fread(&size,sizeof(int),1,f);
  char* plname=malloc(sizeof(char)*(size+1));
  fread(plname,sizeof(char),size,f);
  plname[size]=0;
  struct Playlist* retp=create_new_playlist(plname);
  retp->day_st=day_st;
  retp->hour_st=hour_st;
  retp->min_st=min_st;
  retp->day_en=day_en;
  retp->hour_en=hour_en;
  retp->min_en=min_en;
  free_memory_markov(retp->problist);
  retp->problist=mchain;
  retp->type=typel;
  free(plname);
  int prob;
  int i;
  for(i=0;i<ppsize;i++){
    fread(&prob,sizeof(int),1,f);
    fread(&size,sizeof(int),1,f);
    char* namae=malloc(sizeof(char)*(size+1));
    fread(namae,sizeof(char),size,f);
    namae[size]=0;
    fread(&size,sizeof(int),1,f);
    char* path=malloc(sizeof(char)*(size+1));
    fread(path,sizeof(char),size,f);
    path[size]=0;
    playlist_add_song_adv(retp,namae,path,prob);
    free(namae);
    free(path);
  }
  fclose(f);
  return retp;
}
void write_playlist_to_file(struct Playlist* pl,char* file){
	write_playlist_to_file_helper(pl,file);
	struct Playlist* plg=get_playlist_from_file(file);
	write_playlist_to_file_helper(plg,file);
	free_playlist_from_memory(plg);
}


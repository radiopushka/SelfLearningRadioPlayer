#include "songselector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int scope_size=0;
struct Playlist** scope_list=NULL;

struct Playlist* previous_pointer=NULL;

int learn_speed=1;

struct StackPointer* queue=NULL;

int c_day=0;
int c_hour=0;
int c_min=0;

int id_freq=4;//in minutes
int p_id_min=0;
int p_id_h=0;

int p_rand=0;

void get_current_time() {
  time_t t = time(NULL);
  struct tm *now = localtime(&t);
  c_day=now->tm_wday;
  c_hour=now->tm_hour;
  c_min=now->tm_min;
}


void free_playlist_scope(){
	if(scope_list!=NULL){
		free(scope_list);
	}
	scope_size=0;
}
int is_in_time_scope(int d, int h, int m,int df,int hf, int mf){
	if(c_day>=d&&c_day<=df){
		if((c_hour>=h||d!=c_day)&&(c_hour<=hf||df!=c_day)){
			if((c_min>=m||d!=c_day||h!=c_hour)&&(c_min<=mf||df!=c_day||hf!=c_hour)){
				return 1;
			}
		}	
	}
	return -1;
}
void scope_playlists(){
	int c=playlist_count();
	int i;
	int count=0;
	for(i=0;i<c;i++){
		struct Playlist* pl=get_playlist_at_index(i);
		if(pl->type==0){
			if(pl->day_st==0&&pl->hour_st==0&&pl->min_st==0&&pl->day_en==0&&pl->hour_en==0&&pl->min_en==0){
				count++;
			}else{
				if(is_in_time_scope(pl->day_st,pl->hour_st,pl->min_st,pl->day_en,pl->hour_en,pl->min_en)==1){
					count++;
				}
			}
		}
	}
	if(scope_size!=count){
		if(scope_list!=NULL){
			free(scope_list);
		}
		scope_list=malloc(sizeof(struct Playlist*)*(count));
		scope_size=count;
	}
	int itr=0;
	for(i=0;i<c;i++){
		struct Playlist* pl=get_playlist_at_index(i);
		if(pl->type==0){
			if(pl->day_st==0&&pl->hour_st==0&&pl->min_st==0&&pl->day_en==0&&pl->hour_en==0&&pl->min_en==0){
				scope_list[itr]=pl;
				itr++;
			}else{
				if(is_in_time_scope(pl->day_st,pl->hour_st,pl->min_st,pl->day_en,pl->hour_en,pl->min_en)==1){
					scope_list[itr]=pl;
					itr++;
				}
			}
		}
	}
}
char* generate_song_path(char* ignorepath){
	int i;
	int t_size=0;
	int add=0;
	int extra=0;
	int size;
	for(i=0;i<scope_size;i++){
		struct Playlist* pl=scope_list[i];
		size=pl->size;
		t_size=t_size+size;
		struct PlaylistMarker* problist=pl->problist;
		if(ignorepath!=NULL){
			add=get_extra_probability_score_for_song(problist,ignorepath);
			extra=extra+add;
		}
		t_size=t_size+add;
	}
	if(t_size<1){
		return NULL;
	}
	int rsel=rand()%t_size;
	int msel=0;
	int rsel2;
	if(extra>0){
		msel=rand()%4;//between 0 1 2 3, if 1, 2, or 3 fall then use the markov chain only
	}
	if(msel>0){
		rsel2=rand()%extra;
		rsel=(t_size-extra)+rsel2;
	}
	
	while(rsel==p_rand&&t_size>1){
		rsel=rand()%t_size;
		msel=0;
		if(extra>0){
			msel=rand()%4;//between 0 1 2 3, if 1, 2, or 3 fall then use the markov chain only
		}
		if(msel>0){
			rsel2=rand()%extra;
			rsel=(t_size-extra)+rsel2;
		}
	}
	p_rand=rsel;
	t_size=0;
	add=0;
	char* retpath=NULL;
	for(i=0;i<scope_size;i++){
		struct Playlist* pl=scope_list[i];
		size=pl->size;
		if(t_size<=rsel&&rsel<(t_size+size)){
			retpath=get_songfile_at_index(pl,rsel-t_size);
			previous_pointer=pl;
			break;
		}
		t_size=t_size+size;
		struct PlaylistMarker* problist=pl->problist;
		if(ignorepath!=NULL){
			add=get_extra_probability_score_for_song(problist,ignorepath);
			
			if(t_size<=rsel&&rsel<(t_size+add)){
				retpath=get_song_from_probability(problist,ignorepath,rsel-t_size);
				previous_pointer=pl;
				break;
			}
			t_size=t_size+add;
		}
		
	}
	
	return retpath;
}
int prev_id_r=0;
int tpush=0;
char* get_id_path(){
	if(p_id_h==c_hour){
		if(tpush==1){
			return NULL;
		}
		if(c_min<p_id_min+id_freq){
			return NULL;
		}
	}else if(tpush==1){
		if(c_min<p_id_min){
			return NULL;
		}
	}
	p_id_h=c_hour;
	p_id_min=c_min;
	if(c_min+id_freq>60){
		int errd=(c_min+id_freq)-60;
		tpush=1;
		p_id_min=errd;
	}else{
		tpush=0;
	}
	int c=playlist_count();
	int i;
	int t_size=0;
	for(i=0;i<c;i++){
		struct Playlist* pl=get_playlist_at_index(i);
		if(pl->type!=0){
			if(pl->day_st==0&&pl->hour_st==0&&pl->min_st==0&&pl->day_en==0&&pl->hour_en==0&&pl->min_en==0){
				t_size=t_size+pl->size;
			}else{
				if(is_in_time_scope(pl->day_st,pl->hour_st,pl->min_st,pl->day_en,pl->hour_en,pl->min_en)==1){
					t_size=t_size+pl->size;
				}
			}
		}
	}
	if(t_size<1){
		return NULL;
	}
	int rsel=rand()%t_size;
	while(rsel==prev_id_r&&t_size>1){
		rsel=rand()%t_size;
	}
	prev_id_r=rsel;
	t_size=0;
	char* retpath=NULL;
	for(i=0;i<c;i++){
		struct Playlist* pl=get_playlist_at_index(i);
		if(pl->type!=0){
			if(pl->day_st==0&&pl->hour_st==0&&pl->min_st==0&&pl->day_en==0&&pl->hour_en==0&&pl->min_en==0){
				if(t_size<=rsel&&rsel<(t_size+pl->size)){
					retpath=get_songfile_at_index(pl,rsel-t_size);
					break;
				}
				t_size=t_size+pl->size;
			}else{
				if(is_in_time_scope(pl->day_st,pl->hour_st,pl->min_st,pl->day_en,pl->hour_en,pl->min_en)==1){
					if(t_size<=rsel&&rsel<(t_size+pl->size)){
						retpath=get_songfile_at_index(pl,rsel-t_size);
						break;
					}
					t_size=t_size+pl->size;
				}
			}
		}
	}
	return retpath;
}
char* prev_path_queue=NULL;
struct Song* get_next_song(char* ignorepath){//ignore path is the path of the previous song
//note that if you queued a Song, when it gets picked, this function will return the pointer to it so do not free it.
	get_current_time();
	char* id=get_id_path();
	if(id!=NULL){
		struct Song* structure=malloc(sizeof(struct Song));
		structure->path=id;
		structure->name=pointerize("Identification");
		return structure;
	}
	if(queue!=NULL){//learning algorithm call
		struct Song* atama=(struct Song*)read_stack_top(&queue);
		if(prev_path_queue!=NULL){
			if(strcmp(prev_path_queue,atama->path)!=0){
				if(previous_pointer!=NULL){
					int ione=find_index_match_file(previous_pointer, prev_path_queue);
					int itwo=find_index_match_file(previous_pointer, atama->path);
					if(ione!=-1&&itwo!=-1){
						int i;
						add_element_for(previous_pointer->problist,prev_path_queue);
						for(i=0;i<learn_speed;i++){
							push_score_up_for(previous_pointer->problist,prev_path_queue,atama->path);
						}
					}
				}
			}
			free(prev_path_queue);
		}
		prev_path_queue=pointerize(atama->path);
		pop_from_stack(&queue);
		return atama;
	}else{
		if(prev_path_queue!=NULL){
			free(prev_path_queue);
			prev_path_queue=NULL;
		}
	}
	scope_playlists();
	char* songptr=generate_song_path(ignorepath);
	struct Song* structure=malloc(sizeof(struct Song));
	structure->path=songptr;
	if(songptr!=NULL){
		int name_index= find_index_match_file(previous_pointer, songptr);
		structure->name=get_songname_at_index(previous_pointer,name_index);
	}else{
		structure->name=NULL;
	}
	return structure;
}
void free_song_structure(struct Song* song){
	if(song->path!=NULL){
		free(song->path);
	}
	if(song->name!=NULL){
		free(song->name);
	}
	free(song);
}
void clear_all_data_structures(){
	free(scope_list);
	int count=count_elements(&queue);
	int i;
	for(i=0;i<count;i++){
		struct Song* atama=(struct Song*)element_at(&queue,i);
		free_song_structure(atama);
	}
	free_stack(&queue);
	if(prev_path_queue!=NULL){
			free(prev_path_queue);
			prev_path_queue=NULL;
	}
}

void set_id_frequency(int minutes){
	if(minutes>2){
		id_freq=minutes;
	}
}
int get_id_frequency(){
	return id_freq;
}
void queue_song(struct Song* song){
	add_item_bottom(&queue,song);
}
char* pop_queue_top(){
	struct Song* atama=(struct Song*)read_stack_bottom(&queue);
	if(atama==NULL){
		return NULL;
	}
	char* retp=pointerize(atama->name);
	free(atama->name);
	free(atama->path);
	free(atama);
	pop_stack_bottom(&queue);
	return retp;
}
struct Song* get_song_st(char* file,char* name){
	struct Song* structure=malloc(sizeof(struct Song));
	structure->name=pointerize(name);
	structure->path=pointerize(file);
	return structure;
}
void set_learn_speed(int speed){
	if(speed>=0){
		learn_speed=speed;
	}
}
void resort_queue(){
	reorder_stack(&queue);
}

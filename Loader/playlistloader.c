#include "../playlist/playlist.h"
#include <dirent.h>
#include "playlistloader.h"
#include<string.h>
#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>

struct PlaylistContainer* DATA_HOLDER;
int LOADED_PLAYLISTS=0;


char* get_write_directory(){
	const char *homedir;

	if ((homedir = getenv("HOME")) == NULL) {
    		homedir = getpwuid(getuid())->pw_dir;
	}
	int len=strlen(homedir);
	int len2=strlen(BASE_DIR);
	char* retd=malloc(sizeof(char)*(len+len2+1));
	sprintf(retd,"%s%s",homedir,BASE_DIR);
	return retd;
}
char* string_combine(char* one, char* two){
	int len1=strlen(one);
	int len2=strlen(two);
	char* news=malloc(sizeof(char)*(len1+len2+1));
	sprintf(news,"%s%s",one,two);
	return news;
}

void load_playlists(){
	srand(time(NULL));  
	DIR* ll;
	struct dirent *ep;
	char* sdir=get_write_directory();
	ll=opendir(sdir);
	if(ll==NULL){
		return;
	}
	LOADED_PLAYLISTS=0;
	struct PlaylistContainer* last=NULL;
	while((ep=readdir(ll))!=NULL){
		char* name=ep->d_name;
		if(name[0]==IS_PLAYLIST_RULE){
			char* path=string_combine(sdir,name);
			struct Playlist* gotten=get_playlist_from_file(path);
			free(path);
			struct PlaylistContainer* atarashi=malloc(sizeof(struct PlaylistContainer));
			atarashi->pl=gotten;
			atarashi->next=last;
			last=atarashi;
			LOADED_PLAYLISTS++;
		}
	}
	closedir(ll);
	free(sdir);
	DATA_HOLDER=last;
}
void save_playlists(){
	DIR* ll;
	char* sdir=get_write_directory();
	mkdir(sdir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	ll=opendir(sdir);
	struct dirent *ep;
	while((ep=readdir(ll))!=NULL){
		char* name=ep->d_name;
		if(name[0]==IS_PLAYLIST_RULE){
			char* path=string_combine(sdir,name);
			remove(path);
			free(path);
		}
	}
	struct PlaylistContainer* holder=DATA_HOLDER;
	if(ll==NULL){
		return;
	}
	int i;
	for(i=0;i<LOADED_PLAYLISTS;i++){
		int ssz=i/10+1;
		char* number=malloc(sizeof(char)*(ssz+2));
		sprintf(number,"%c%d",IS_PLAYLIST_RULE,i);
		char* fname=string_combine(sdir,number);
		free(number);
		write_playlist_to_file(holder->pl,fname);
		free(fname);
		holder=holder->next;
	}
	closedir(ll);
	free(sdir);
}
void add_playlist(struct Playlist* pl){
	struct PlaylistContainer* atarashi=malloc(sizeof(struct PlaylistContainer));
	atarashi->pl=pl;
	atarashi->next=DATA_HOLDER;
	DATA_HOLDER=atarashi;
	LOADED_PLAYLISTS++;
}
void free_playlist_from_stack(struct PlaylistContainer* plc){
	free_playlist_from_memory(plc->pl);
	free(plc);
}
int remove_playlist(int index){
	struct PlaylistContainer* holder=DATA_HOLDER;
	if(holder==NULL){
		return -1;
	}
	if(index==0){
		struct PlaylistContainer* tmp=holder->next;
		free_playlist_from_stack(holder);
		DATA_HOLDER=tmp;
		LOADED_PLAYLISTS--;
		return 0;
	}
	int i;
	struct PlaylistContainer* prev=NULL;
	for(i=0;i<LOADED_PLAYLISTS;i++){
		if(i==index){
			struct PlaylistContainer* tmp=holder->next;
			prev->next=tmp;
			free_playlist_from_stack(holder);
			LOADED_PLAYLISTS--;
			return 0;
		}
		prev=holder;
		holder=holder->next;
	}
	return -1;
}
struct Playlist* get_playlist_at_index(int index){
	int i;
	struct PlaylistContainer* holder=DATA_HOLDER;
	for(i=0;i<LOADED_PLAYLISTS;i++){
		if(i==index){
			return holder->pl;
		}
		holder=holder->next;
	}
	return NULL;
}
void free_playlist_stack_mem(){
	int i;
	struct PlaylistContainer* tmp;
	struct PlaylistContainer* holder=DATA_HOLDER;
	for(i=0;i<LOADED_PLAYLISTS;i++){
		tmp=holder->next;
		free_playlist_from_stack(holder);
		holder=tmp;
	}
}

int playlist_count(){
	return LOADED_PLAYLISTS;
}
int is_not_directory(char* path){

	DIR* ll;
	ll=opendir(path);
	if(ll==NULL){
		return 1;
	}
	closedir(ll);
	return -1;
}
int get_audio_format(char* file){
	int len=strlen(file);
	int i;
	char dsize[10];
	int dpos=0;
	char* formats[]={"ggo","GGO","gGO","GgO","GGo","Ggo","gGo","ggO"};
	char* formats2[]={"vaw","VAW","vAW","VaW","VAw","Vaw","vAw","vaW"};
	int count=8;
	for(i=len-1;i>0;i--){
		if(file[i]=='.'){
			break;
		}
		if(dpos>=9){
			break;
		}
		dsize[dpos]=file[i];
		dpos++;
	}
	dsize[dpos]=0;
	int pass=-1;
	for(i=0;i<count;i++){
		if(strcmp(dsize,formats[i])==0){
			pass=1;
			break;
		}
		if(strcmp(dsize,formats2[i])==0){
			pass=2;
			break;
		}
	}
	return pass;
}
char* get_true_name(char* filename){
	int size=strlen(filename);
	char* ret=malloc(sizeof(char)*(size+1));
	int i;
	for(i=0;i<size;i++){
		ret[i]=filename[i];
	}
	ret[i]=0;
	for(i=size-1;i>=0;i--){
		if(ret[i]=='.'){
			ret[i]=0;
			break;
		}
	}
	return ret;
}
int mass_add_directory(struct Playlist* pl,char* dir){
	DIR* ll;
	struct dirent *ep;
	ll=opendir(dir);
	if(ll==NULL){
		return -1;
	}
	char *mpath;
	int ln=strlen(dir);
	if(dir[ln-1]!='/'){
		mpath=string_combine(dir,"/");
	}else{
		mpath=string_combine(dir,"");
	}
	while((ep=readdir(ll))!=NULL){
		char* name=ep->d_name;
		char* fullpath=string_combine(mpath,name);
		int isdir=is_not_directory(fullpath);
		if(isdir==1){
			int format=get_audio_format(fullpath);
			if(format>0){
				char* namae=get_true_name(name);
				if(find_index_match_file(pl, fullpath)<0){
					playlist_add_song(pl,namae, fullpath);
				}
				free(namae);
			}
		}
		free(fullpath);
	}
	free(mpath);
	closedir(ll);
	return 1;
}

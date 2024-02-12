#include "markovchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SongHeader* get_song_structure(struct PlaylistMarker* playlist,char* path){
	int size=playlist->stack_size;
	struct StackPointer* stack=playlist->songheaderlist;
	int i;
	for(i=0;i<size;i++){
		struct SongHeader* head=(struct SongHeader*)element_at(&stack,i);
		if(strcmp(head->path,path)==0){
			return head;
		}
	}
	return NULL;

}
void add_element_for(struct PlaylistMarker* playlist,char* path){
	if(get_song_structure(playlist,path)!=NULL){
		return;
	}
	int size=playlist->stack_size;
	struct StackPointer* stack=playlist->songheaderlist;
	struct SongHeader* new_header=malloc(sizeof(struct SongHeader));
	new_header->path=pointerize(path);
	new_header->listhead=NULL;
	new_header->stack_size=0;
	new_header->total_prob_score=0;
	add_item_to_stack(&stack,new_header);
	playlist->songheaderlist=stack;
	playlist->stack_size=size+1;
}
void remove_path_from_song_header(struct SongHeader* header,char* path){
	int size=header->stack_size;
	int prob=header->total_prob_score;
	struct StackPointer* stack=header->listhead;
	struct StackPointer* newstack=NULL;
	int i;
	int nsize=size;
	for(i=0;i<size;i++){
		struct ProbabilityList* plist=(struct ProbabilityList*)element_at(&stack,i);
		if(strcmp(plist->path,path)!=0){
			add_item_to_stack(&newstack,plist);
		}else{
			nsize=nsize-1;
			prob=prob-(plist->score);
			free(plist->path);
			free(plist);
			
		}
		
	}
	free_stack(&stack);
	header->listhead=newstack;
	header->stack_size=nsize;
	header->total_prob_score=prob;
}
void remove_complete_header(struct SongHeader* header){
	int size=header->stack_size;
	int i;
	struct StackPointer* stack=header->listhead;
	for(i=0;i<size;i++){
		struct ProbabilityList* plist=(struct ProbabilityList*)element_at(&stack,i);
		free(plist->path);
		free(plist);
	}
	free_stack(&stack);
	free(header->path);
	free(header);
}
void delete_element_for(struct PlaylistMarker* playlist,char* path){
	int size=playlist->stack_size;
	struct StackPointer* stack=playlist->songheaderlist;
	int i;
	int nsize=size;
	struct StackPointer* newstack=NULL;
	for(i=0;i<size;i++){
		struct SongHeader* header=(struct SongHeader*)element_at(&stack,i);
		if(strcmp(header->path,path)==0){
			nsize=nsize-1;
			remove_complete_header(header);
		}else{
			remove_path_from_song_header(header,path);
			add_item_to_stack(&newstack,header);
		}
	}
	free_stack(&stack);
	playlist->stack_size=nsize;
	playlist->songheaderlist=newstack;
}
void level_probabilities(struct PlaylistMarker* playlist){
	int size=playlist->stack_size;
	int i;
	int s2;
	struct StackPointer* listhead;
	int i2;
	struct StackPointer* stack=playlist->songheaderlist;
	int total_count=0;
	for(i=0;i<size;i++){
		struct SongHeader* header=(struct SongHeader*)element_at(&stack,i);
		listhead=header->listhead;
		s2=header->stack_size;
		for(i2=0;i2<s2;i2++){
			struct ProbabilityList* header=(struct ProbabilityList*)element_at(&listhead,i);
			header->score=(header->score)>>1;
			total_count=total_count+(header->score);
		}
	}
	if(total_count<0){
		level_probabilities(playlist);
	}
}
void push_score_up_for(struct PlaylistMarker* playlist,char* path,char* targ){
	struct SongHeader* structure=get_song_structure(playlist,path);
	if(structure==NULL){
		return;
	}
	int size=structure->stack_size;
	int prob=structure->total_prob_score;
	struct StackPointer* stack=structure->listhead;
	int i;
	for(i=0;i<size;i++){
		struct ProbabilityList* plist=(struct ProbabilityList*)element_at(&stack,i);
		if(strcmp(plist->path,targ)==0){
			plist->score=(plist->score)+1;
			prob++;
			structure->total_prob_score=prob;
			return;
		}
	}
	struct ProbabilityList* nlist=malloc(sizeof(struct ProbabilityList));
	nlist->score=1;
	prob++;
	size++;
	nlist->path=pointerize(targ);
	add_item_to_stack(&stack,nlist);
	structure->listhead=stack;
	structure->stack_size=size;
	structure->total_prob_score=prob;
	if(prob<0){
		level_probabilities(playlist);
	}
}
struct PlaylistMarker* create_new_markov_chain(){
	struct PlaylistMarker* plm=malloc(sizeof(struct PlaylistMarker));
	plm->stack_size=0;
	plm->songheaderlist=NULL;
	return plm;
}
int get_extra_probability_score_for_song(struct PlaylistMarker* playlist,char* path){
	struct SongHeader* structure=get_song_structure(playlist,path);
	if(structure==NULL){
		return -1;
	}
	return structure->total_prob_score;
}
char* get_song_from_probability(struct PlaylistMarker* playlist,char* path,int prob){
	struct SongHeader* structure=get_song_structure(playlist,path);
	int size= structure->stack_size;
	int pcount=0;
	int i;
	struct StackPointer* nstack=structure->listhead;
	int subsize;
	for(i=0;i<size;i++){
		struct ProbabilityList* plist=(struct ProbabilityList*)element_at(&nstack,i);
		subsize=plist->score;
		if(prob>=pcount&&(prob<(subsize+pcount)))
		{
			return pointerize(plist->path);
		}
		pcount=pcount+subsize;
	}
	return NULL;


}


void free_memory_markov(struct PlaylistMarker* playlist){
	int size=playlist->stack_size;
	int i;
	struct StackPointer* stack=playlist->songheaderlist;
	for(i=0;i<size;i++){
		struct SongHeader* header=(struct SongHeader*)element_at(&stack,i);
		remove_complete_header(header);
	}
	free_stack(&stack);
	free(playlist);
}

void dump_as_text(struct PlaylistMarker* playlist){
	printf("size: %d\n",playlist->stack_size);
	int size=playlist->stack_size;
	struct StackPointer* stack=playlist->songheaderlist;
	int i;
	int i2;
	int size2;
	for(i=0;i<size;i++){
		struct SongHeader* header=(struct SongHeader*)element_at(&stack,i);
		printf("-path: %s\n",header->path);
		printf("-size: %d prob: %d\n",header->stack_size,header->total_prob_score);
		struct StackPointer* stack2=header->listhead;
		size2=header->stack_size;
		for(i2=0;i2<size2;i2++){
			struct ProbabilityList* plist=(struct ProbabilityList*)element_at(&stack2,i2);
			printf("--path:%s score:%d\n",plist->path,plist->score);
		}
	}
}


//file IO operations:

void write_string_to_file(char* string,FILE* f){
	int stringsize=strlen(string);
	fwrite(&stringsize,sizeof(int),1,f);
	fwrite(string,sizeof(char),stringsize,f);
}
char* read_string_from_file(FILE* f){
	int size;
	fread(&size,sizeof(int),1,f);
	char* strret=malloc(sizeof(char)*(size+1));
	fread(strret,sizeof(char),size,f);
	strret[size]=0;
	return strret;
}

void write_song_header_to_file(FILE* f,struct SongHeader* header){
	write_string_to_file(header->path,f);
	fwrite(&(header->stack_size),sizeof(int),1,f);
	fwrite(&(header->total_prob_score),sizeof(int),1,f);
	int size=header->stack_size;
	struct StackPointer* listhead=header->listhead;
	int i;
	for(i=0;i<size;i++){
		struct ProbabilityList* plist=(struct ProbabilityList*)element_at(&listhead,i);
		write_string_to_file(plist->path,f);
		fwrite(&(plist->score),sizeof(int),1,f);
	}
	
}
struct SongHeader* read_song_header_from_file(FILE* f){
	struct SongHeader* new_header=malloc(sizeof(struct SongHeader));
	new_header->path=read_string_from_file(f);
	int stack_size;
	fread(&stack_size,sizeof(int),1,f);
	int pscore;
	fread(&pscore,sizeof(int),1,f);
	new_header->stack_size=stack_size;
	new_header->total_prob_score=pscore;
	struct StackPointer* listhead=NULL;
	int sscore;
	int i;
	for(i=0;i<stack_size;i++){
		struct ProbabilityList* plist=malloc(sizeof(struct ProbabilityList));
		plist->path=read_string_from_file(f);
		fread(&sscore,sizeof(int),1,f);
		plist->score=sscore;
		add_item_to_stack(&listhead,plist);
	}
	new_header->listhead=listhead;
	return new_header;
}

void write_chain_to_file(struct PlaylistMarker* playlist,FILE* f){
	int size=playlist->stack_size;
	fwrite(&size,sizeof(int),1,f);
	struct StackPointer* headerlist=playlist->songheaderlist;
	int i;
	for(i=0;i<size;i++){
		struct SongHeader* new_header=(struct SongHeader*)element_at(&headerlist,i);
		write_song_header_to_file(f,new_header);
	}

}
struct PlaylistMarker* read_chain_from_file(FILE* f){
	int size;
	fread(&size,sizeof(int),1,f);
	struct StackPointer* headerlist=NULL;
	int i;
	for(i=0;i<size;i++){
		struct SongHeader* hdr=read_song_header_from_file(f);
		add_item_to_stack(&headerlist,hdr);
	}
	struct PlaylistMarker* plm=malloc(sizeof(struct PlaylistMarker));
	plm->stack_size=size;
	plm->songheaderlist=headerlist;
	return plm;
}

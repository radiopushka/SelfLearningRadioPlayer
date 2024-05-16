#include <ncurses.h>
#include "../MediaPlayer/mediathread.h"
#include "../SongSelect/songselector.h"
#include "playlisteditor.h"
#include <stdlib.h>
#include <string.h>
#include "numberchooser.c"
#include "filechooser.c"



void number_screen_d(int setting,char* msg){
	clear();
	mvprintw(0,0,"%s",msg);
	mvprintw(1,0,"%d",setting);
	mvprintw(3,0,"use keys u and j to change and enter to exit");
	refresh();
}

int number_screen(int init,int step, int max,int min,char* msg){
	int filter_setting=init;
	number_screen_d(filter_setting,msg);
	char c=0;
	while((c=getch())!='\n'){
		if(c=='u'||c==3){
			filter_setting=filter_setting+step;
			if(filter_setting>=max){
				filter_setting=max;
			}
		}
		if(c=='j'||c==2){
			filter_setting=filter_setting-step;
			if(filter_setting<=min){
				filter_setting=min;
			}
		}
		
		number_screen_d(filter_setting,msg);
	}
	return filter_setting;
}



char* strip_sep(char* path){
	int s=strlen(path);
	char* ret=malloc(sizeof(char)*(s+1));
	int i;
	int win=0;
	for(i=0;i<s;i++){
		if(path[i]!='/'){
		ret[win]=path[i];
		win++;
		}else{
		win=0;
		}
		
	}
	ret[win]=0;
	return ret;
}

int draw_playlist_select_screen(int sel){
	int c=playlist_count();
	int i;
	clear();
	int disp=0;
	mvprintw(0,0,">");
	for(i=sel;i<c;i++){
		struct Playlist* plst= get_playlist_at_index(i);
		if(plst==NULL){
			break;
		}
		int type=plst->type;
		
		if(type==0){
			mvprintw(disp,1,"%s",plst->name);
		}else{
			mvprintw(disp,1,"[I]%s",plst->name);
		}
		disp++;
		if(disp>LINES){
			break;
		}
	}
	attron(A_STANDOUT);
	for(i=0;i<COLS;i++){
		mvprintw(LINES-2,i," ");
		mvprintw(LINES-1,i," ");
	}
	mvprintw(LINES-2,0," | k- edit playlist scope time, r- rename, i- toggle id list, o-set id frequency | ");
	mvprintw(LINES-1,0," | q- exit, enter - select, u,j- move up down, n new, d delete | ");
	attroff(A_STANDOUT);
	refresh();
	return c;
}
int edit_playlist_draw(int index,struct Playlist* host){
	int lst=host->size;
	clear();
	int i;
	int disp=0;
	mvprintw(0,0,">");
	for(i=index;i<lst;i++){
		char* namae=get_songname_at_index(host, i);
		if(namae==NULL){
			break;
		}
		mvprintw(disp,1,"%s",namae);
		free(namae);
		disp++;
		if(disp>LINES){
			break;
		}
	}
	attron(A_STANDOUT);
	for(i=0;i<COLS;i++){
		mvprintw(LINES-2,i," ");
		mvprintw(LINES-3,i," ");
		mvprintw(LINES-1,i," ");
	}
	mvprintw(LINES-3,0," | enter/bp - pop and push queu, u- add directory, s- re order queue | ");
	mvprintw(LINES-2,0," |press p- add song path, h- add directory by path, l show path, a- add song | ");
	mvprintw(LINES-1,0," |press q to exit,r rename song, d to delete song, g- clear song's ai | ");
	attroff(A_STANDOUT);
	refresh();
	return lst;
}
void edit_playlist(struct Playlist* host){
 	int size=edit_playlist_draw(0,host);
	char c=0;
	int pos=0;
	while((c=getch())!='q'){
		if(c=='\n'){
		
		}
		if(c=='u'||c==3){
			pos=pos-1;
			if(pos<=0){
				pos=0;
			}
		}
		if(c=='j'||c==2){
			pos=pos+1;
			if(pos>=size-1){
				pos=size-1;
			}
		}
		if(size==0&&(c!='u')&&(c!='a')&&(c!='p')&&(c!='h')){
			c=0;
		}
		if(c=='w'||c=='\n'){
			char* namae=get_songname_at_index(host, pos);
			char* path=get_songfile_at_index(host, pos);
			request_data_mutex();
			queue_song(get_song_st(path,namae));
			release_data_mutex();
			free(namae);
			free(path);
			mvprintw(LINES-1,0," |   song queued ! | ");
			getch();
		}
		if(c==127){//backspace
			request_data_mutex();
			char* poped=pop_queue_top();
			release_data_mutex();
			if(poped!=NULL){
				mvprintw(LINES-1,0," |   song %s poped from queue ! | ",poped);
				free(poped);
			}else{
				mvprintw(LINES-1,0," |   queue empty ! | ");
			}
			getch();
		}
		if(c=='d'){
			char c2=0;
			clear();
			mvprintw(0,0,"delete the song? press f to continue, enter to exit");
			refresh();
			while((c2=getch())!='\n'){
				if(c2=='f'){
					 request_data_mutex();
					 delete_song_at_index(host,pos);
					 save_playlists();
					 release_data_mutex();
					 break;
				}
				clear();
				mvprintw(0,0,"delete the song? press f to continue, enter to exit");
				refresh();
			}
		}
		if(c=='u'){
			char* home=get_write_directory_ni();
			char* chosen=file_chooser(home,1);
			free(home);
			if(chosen!=NULL){
				request_data_mutex();
				mass_add_directory(host,chosen);
				save_playlists();
				release_data_mutex();
				free(chosen);
			}
		}
		if(c=='a'){
			char* home=get_write_directory_ni();
			char* chosen=file_chooser(home,0);
			free(home);
			if(chosen!=NULL){
				char* tname=get_true_name(chosen);
				char* str=strip_sep(tname);
				char* namaes=string_prompt("enter song name",str);
				free(str);
				free(tname);
				if(namaes!=NULL){
					request_data_mutex();
					int path=find_index_match_file(host, chosen);
					if(path==-1){
						playlist_add_song(host,namaes, chosen);
					}else{
						pos=path;
					}
					save_playlists();
					release_data_mutex();
					free(namaes);
				}	
				free(chosen);
			}
		}
		if(c=='p'){
			char* namae=string_prompt("enter the song path","");
			if(namae!=NULL){
				FILE* f=fopen(namae,"rb");
				int type=get_audio_format(namae);
				if(f){
					if(type==1||type==2){
						char* tname=get_true_name(namae);
						char* str=strip_sep(tname);
						char* namaes=string_prompt("enter song name",str);
						free(str);
						free(tname);
						if(namaes!=NULL){
							request_data_mutex();
							int path=find_index_match_file(host, namae);
							if(path==-1){
								playlist_add_song(host,namaes, namae);
							}else{
								pos=path;
							}
							save_playlists();
							release_data_mutex();
							free(namaes);
						}
					}else{
						mvprintw(LINES-1,0," |  invalid song type | ");
						getch();
					}
					fclose(f);
				}else{
					mvprintw(LINES-1,0," |  file not found | ");
					getch();
				}
				free(namae);
			}
		}
		if(c=='g'){
			char* path=get_songfile_at_index(host, pos);
			request_data_mutex();
			delete_element_for(host->problist,path);
			save_playlists();
			release_data_mutex();
			mvprintw(LINES-1,0," |  probability for current song cleared | ");
			getch();
		}
		if(c=='h'){
			char* namae=string_prompt("enter the directory path","");
			if(namae!=NULL){
				request_data_mutex();
				mass_add_directory(host,namae);
				save_playlists();
				release_data_mutex();
				free(namae);
			}
		}
		if(c=='r'){
			char* nget=get_songname_at_index(host, pos);
			char* nn=string_prompt("enter the new name",nget);
			free(nget);
			if(nn!=NULL){
			 
				request_data_mutex();
				rename_song_at(host,pos,nn);
				release_data_mutex();
				free(nn);
			}
		}
		if(c=='l'){
			char* path=get_songfile_at_index(host, pos);
			clear();
			mvprintw(0,0,"%s",path);
			refresh();
			char c;
			while((c=getch())!='\n');
			free(path);
		}
		if(c=='s'){
			request_data_mutex();
			resort_queue();
			release_data_mutex();
			mvprintw(LINES-1,0," | queue order reversed | ");
			getch();
		}
		size=edit_playlist_draw(pos,host);
	}
}
void playlist_selection_screen(){
	int size=draw_playlist_select_screen(0);
	char c=0;
	int pos=0;
	while((c=getch())!='q'){
		if(c=='\n'){
			struct Playlist* plst= get_playlist_at_index(pos);
			edit_playlist(plst);
		}
		if(c=='u'||c==3){
			pos=pos-1;
			if(pos<=0){
				pos=0;
			}
		}
		if(c=='j'||c==2){
			pos=pos+1;
			if(pos>=size-1){
				pos=size-1;
			}
		}
		if(size==0&&c!='n'){
			c=0;
		}
		if(c=='d'){
			char c2=0;
			clear();
			mvprintw(0,0,"delete the playlist? press f to continue, enter to exit");
			refresh();
			while((c2=getch())!='\n'){
				if(c2=='f'){
					 request_data_mutex();
					 remove_playlist(pos);
					 save_playlists();
					 release_data_mutex();
					 break;
				}
				clear();
				mvprintw(0,0,"delete the playlist? press f to continue, enter to exit");
				refresh();
			}
		}
		if(c=='n'){
			char* namae=string_prompt("enter the new playlist name","");
			if(namae!=NULL){
			struct Playlist* newpl= create_new_playlist(namae);
			free(namae);
			request_data_mutex();
			add_playlist(newpl);
			save_playlists();
			release_data_mutex();
			}
		}
		if(c=='r'){
			struct Playlist* plst= get_playlist_at_index(pos);
			char* namae=string_prompt("enter the new playlist name",plst->name);
			if(namae!=NULL){
				request_data_mutex();
				rename_playlist(plst,namae);
				save_playlists();
				release_data_mutex();
				free(namae);
			}
		}
		if(c=='k'){
			struct Playlist* plst= get_playlist_at_index(pos);
			playlist_time(plst);
		}
		if(c=='i'){
			struct Playlist* plst= get_playlist_at_index(pos);
			request_data_mutex();
			int type=plst->type;
			if(type==0){
				plst->type=1;
			}else{
				plst->type=0;
			}
			release_data_mutex();
		}
		if(c=='o'){
			int ifr=get_id_frequency();
			int val=number_screen(ifr,1,20,4,"ID frequency in minutes");
			request_data_mutex();
			set_id_frequency(val);
			release_data_mutex();
		}
		size=draw_playlist_select_screen(pos);
	}

}

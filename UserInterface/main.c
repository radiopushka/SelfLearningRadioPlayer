#include <ncurses.h>
#include "../MediaPlayer/mediathread.h"
#include<stdio.h>
#include<stdlib.h>
#include <locale.h>
#include<unistd.h>
#include <string.h>





void draw_main_screen(char pl){
	char* song=get_current_song_name();
	clear();
	mvprintw(0,0,"-current song: %s",song);
	mvprintw(1,0,"-progress:");
	if(song!=NULL){
	free(song);
	}
	int mwidth=COLS/2;
	int time=get_total_time();
	int cur=get_position();
	int factor=1;
	int divisor=1;
	if(time!=0){
	if(time<mwidth){
		factor=mwidth/time;
	}
	if(time>mwidth){
		divisor=time/mwidth;
	}
	cur=(cur*factor)/divisor;
	int i;
	mvprintw(2,0,"|");
	for(i=0;i<cur;i++){
		if(i>COLS){
			break;
		}	
		mvprintw(2,i+1,"#");
	}
	mvprintw(2,(time*factor)/divisor,"|");
	}
	char* print="pause";
	if(pl==1){
		print="play";
	}
	mvprintw(3,0," commands: space - %s | f - seek | s - skip | e- exit",print);
	mvprintw(4,0,"m- go to playlist managment | l - low pas filter");
	mvprintw(5,0,"a - ai learning speed");
	refresh();

}
void low_pass_screen_d(int setting){
	clear();
	mvprintw(0,0,"Low pass filter config:");
	mvprintw(1,0,"%d hz",setting);
	mvprintw(3,0,"use keys u and j to change and enter to exit");
	refresh();
}
int filter_setting=9500;
void low_pass_screen(){
	low_pass_screen_d(filter_setting);
	char c=0;
	while((c=getch())!='\n'){
		if(c=='u'||c==3){
			filter_setting=filter_setting+100;
			if(filter_setting>=20000){
				filter_setting=20000;
			}
			set_low_pass_freq(filter_setting);
		}
		if(c=='j'||c==2){
			filter_setting=filter_setting-100;
			if(filter_setting<=200){
				filter_setting=200;
			}
			set_low_pass_freq(filter_setting);
		}
		
		low_pass_screen_d(filter_setting);
	}
}
int set_spd=20;
int central_interface(){
	 setlocale(LC_ALL, "");
	 initscr();
	 curs_set(0);
	 keypad( stdscr,TRUE);
	 noecho();
	 timeout(1000);
	 draw_main_screen(0);
	 char c=0;
	 char pp=0;
	 request_data_mutex();
	 set_learn_speed(set_spd);
	 release_data_mutex();
	 while((c=getch())!='e'){
	 	if(c==27){
	 		break;
	 	}
	 	if(c==' '){
	 		if(pp==0){
	 			pp=1;
	 			signal(SIGNAL_PAUSE);
	 		}else{
	 			signal(SIGNAL_PLAY);
	 			pp=0;
	 		}
	 	}
	 	if(c=='f'||c==5){
	 		signal(SIGNAL_FWD);
	 	}
	 	if(c=='s'){
	 		signal(SIGNAL_SKIP);
	 	}
	 	if(c=='l'){
	 		low_pass_screen();
	 	}
	 	if(c=='m'){
			playlist_selection_screen();
		}
		if(c=='a'){
			set_spd=number_screen(set_spd,1, 500,0,"set the learning speed of the ai, learns off of queue");
				 request_data_mutex();
				 set_learn_speed(set_spd);
				 release_data_mutex();
		}
	 	draw_main_screen(pp);
	 	mvprintw(0,COLS-3,"%d",c);
	 }
	 endwin();
	 return 0;

}

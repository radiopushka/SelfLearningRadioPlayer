#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "../MediaPlayer/mediathread.h"
#include "../SongSelect/songselector.h"
#include <time.h>

void draw_screen_time_chooser(int h,int m, int s,int h_e,int m_e,int s_e,int pos){
	clear();
	time_t t = time(NULL);
 	struct tm *now = localtime(&t);
  	int c_day=now->tm_wday;
  	int c_hour=now->tm_hour;
  	int c_min=now->tm_min;
	mvprintw(0,0,"start time:");
	if(pos==0){
		attron(A_STANDOUT);
	}
	mvprintw(1,0," %d:",h);
	if(pos==0){
		attroff(A_STANDOUT);
	}
	if(pos==1){
		attron(A_STANDOUT);
	}
	mvprintw(1,5,"%d:",m);
	if(pos==1){
		attroff(A_STANDOUT);
	}
	if(pos==2){
		attron(A_STANDOUT);
	}
	mvprintw(1,9,"%d",s);
	if(pos==2){
		attroff(A_STANDOUT);
	}
	mvprintw(2,0,"stop time:");
	if(pos==3){
		attron(A_STANDOUT);
	}
	mvprintw(3,0," %d:",h_e);
	if(pos==3){
		attroff(A_STANDOUT);
	}
	if(pos==4){
		attron(A_STANDOUT);
	}
	mvprintw(3,5,"%d:",m_e);
	if(pos==4){
		attroff(A_STANDOUT);
	}
	if(pos==5){
		attron(A_STANDOUT);
	}
	mvprintw(3,9,"%d",s_e);
	if(pos==5){
		attroff(A_STANDOUT);
	}
	if(m==0&&h==0&&s==0&&h_e==0&&m_e==0&&s_e==0){
		mvprintw(4,0,"playlist will play all the time");
	}
	if(h>h_e||(h==h_e&&m>m_e)||(h==h_e&&m==m_e&&s>s_e)){
		mvprintw(4,0,"playlist will never play");
	}
	mvprintw(5,0,"%d : %d : %d",c_day,c_hour,c_min);
	refresh();
}
void time_chooser(struct Playlist* pl){
	int d=pl->day_st;
	int h=pl->hour_st;
	int m=pl->min_st;
	int de=pl->day_en;
	int he=pl->hour_en;
	int me=pl->min_en;
	int pos=0;
	char c;
	int tmp=0;
	draw_screen_time_chooser(d,h,m,de,he,me,pos);
	while((c=getch())!='\n'){
		if(c==5){
			pos++;
			if(pos>=5){
				pos=5;
			}
		}
		if(c==4){
			pos--;
			if(pos<=0){
				pos=0;
			}
		}
		switch(pos){
			case 0:
			tmp=d;
			break;
			case 1:
			tmp=h;
			break;
			case 2:
			tmp=m;
			break;
			case 3:
			tmp=de;
			break;
			case 4:
			tmp=he;
			break;
			case 5:
			tmp=me;
			break;
		}
		if(c==3){
			tmp++;
		}
		if(c==2){
			tmp--;
			if(tmp<=0){
				tmp=0;
			}
		}
		
		switch(pos){
			case 0:
			d=tmp;
			break;
			case 1:
			h=tmp;
			break;
			case 2:
			m=tmp;
			break;
			case 3:
			de=tmp;
			break;
			case 4:
			he=tmp;
			break;
			case 5:
			me=tmp;
			break;
		}
		if(m>60){
			m=0;
			h++;
		}
		if(h>60){
			h=0;
			d++;
		}
		if(d>7){
			d=0;
			h=0;
			m=0;
		}
		if(me>60){
			me=0;
			he++;
		}
		if(he>60){
			he=0;
			de++;
		}
		if(de>7){
			de=0;
			he=0;
			me=0;
		}
		if(c=='q'||c==27||c==127){
			return;
		}
		draw_screen_time_chooser(d,h,m,de,he,me,pos);
	}
	request_data_mutex();
	pl->day_st=d;
	pl->hour_st=h;
	pl->min_st=m;
	pl->day_en=de;
	pl->hour_en=he;
	pl->min_en=me;
	save_playlists();
	release_data_mutex();
}

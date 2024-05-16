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
  unsigned long long start = h<<16 | m<<8 | s;

  unsigned long long end = h_e<<16 | m_e<<8 | s_e;

  if(start == end){
		mvprintw(4,0,"playlist will play for 1 minute");
	}

	if(end < start){
		mvprintw(4,0,"playlist will never play");
	}
	mvprintw(5,0,"%d : %d : %d",c_day,c_hour,c_min);
	refresh();
}
void time_chooser(struct TimeFrame* pl){
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
        if(pos == d || pos == de)
				  tmp=6;

        if((pos > 0 && pos < 3) || (pos > 3))
          tmp =59;

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
		if(m>59){
			m=0;
			h++;
		}
		if(h>59){
			h=0;
			d++;
		}
		if(d>6){
			d=0;
			h=0;
			m=0;
		}
		if(me>59){
			me=0;
			he++;
		}
		if(he>59){
			he=0;
			de++;
		}
		if(de>6){
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

void draw_pl_screen(struct TimeFrame* root, int selected){
  int i = 0;

  time_t t = time(NULL);

 	struct tm *now = localtime(&t);
  	int c_day=now->tm_wday;
  	int c_hour=now->tm_hour;
  	int c_min=now->tm_min;

  
  clear();
  attron(A_STANDOUT);
  mvprintw(0,0," n - new date, d - delete, i - edit, e - exit");
  attroff(A_STANDOUT);
  mvprintw(1,0,"%d : %d : %d", c_day, c_hour, c_min);

  while( root != NULL ){
      
    if( i == selected )
      attron(A_STANDOUT);

	  mvprintw(i + 3,1,"%d : %d : %d  ->  %d : %d : %d",root -> day_st , root -> hour_st ,root -> min_st, root -> day_en, root -> hour_en, root -> min_en);
   
    if( i ==  selected)
     attroff(A_STANDOUT);

    i++;

    root = root ->next;
  }
  refresh();

}

void playlist_time(struct Playlist* pl){
  
  int sel = 0;

  draw_pl_screen(pl -> tframes, sel);

  char c = 0;
  
  while ( (c = getch() ) != 'e' ){

    
    switch(c){
      case 2:
        sel++;
        break;

      case 3:
        sel--;
        break;


      case 'd':
        request_data_mutex();
        remove_date(pl, sel);
        release_data_mutex();
        break;

      case 'n':
        request_data_mutex();
        put_date(pl, 0,0,0,0,0,0);
        release_data_mutex();
        break;

      case '\n':
      case 'i':
        time_chooser(get_date_at(pl, sel));
        break;

    }


    int max = date_size(pl);

    if(sel < 0 )
      sel = 0;

    if(sel >= max )
      sel = max - 1;

    

    draw_pl_screen(pl -> tframes, sel);

  }

  request_data_mutex();
  save_playlists();
  release_data_mutex();

}

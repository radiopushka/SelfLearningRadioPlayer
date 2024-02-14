#include "fourier.h"
#include<stdio.h>
#include<stdlib.h>
#include<alsa/asoundlib.h>
#include <ncurses.h>
#include <math.h>
#include <locale.h>
#include <pthread.h>
#include "cursescolors.c"

#define SIZE 4000
#define LOWEST_F 11
#define FFT_SPEED_SPEC 40
#define FFT_SPEED_SINGLE 20 

#define DEVICE "default"

chtype printout[]={' '|COLOR_PAIR((1<<7)|(7&0)<<4|(7&0)),' '|COLOR_PAIR((1<<7)|(7&1)<<4|(7&0)),' '|COLOR_PAIR((1<<7)|(7&3)<<4|(7&0)),' '|COLOR_PAIR((1<<7)|(7&2)<<4|(7&0)),' '|COLOR_PAIR((1<<7)|(7&6)<<4|(7&0)),' '|COLOR_PAIR((1<<7)|(7&4)<<4|(7&0))};

int print_offset=0;
double amp=5.0;

pthread_mutex_t syncm;

chtype getfromdecimal(double ftout){
  if(ftout<10){
    return printout[0];
  }
  if(ftout<20){
    return printout[1];
  }
  if(ftout<30){
    return printout[2];
  }
  if(ftout<40){
    return printout[3]; 
  }
  if(ftout<50){
    return printout[4];
  }
  return printout[5];
}
chtype** screenbuff=NULL;
int bbsize=0;
void freescrbuff(){
  if(screenbuff==NULL){
    return;
  }
  int i;
  for(i=0;i<bbsize;i++){
    free(screenbuff[i]);
  }
  free(screenbuff);
  screenbuff=NULL;
}
int ppsize=0;
void printspect(double* trans,int size){
  int i;
  int i2;
  int scrnp=0;
  int abs;
  int nsize;
  if(screenbuff==NULL){
    bbsize=LINES;
    screenbuff=malloc(sizeof(chtype*)*(bbsize+1));
    ppsize=size;
    for(i=0;i<bbsize;i++){
      screenbuff[i]=malloc(sizeof(chtype)*(size+1));
      for(i2=0;i2<size;i2++){
        screenbuff[i][i2]=' ';
      }
    }
  }else if(ppsize!=size){
    nsize=LINES;
    chtype** nscrn=malloc(sizeof(chtype*)*(nsize+1));
    for(i=0;i<nsize;i++){
      nscrn[i]=malloc(sizeof(chtype)*(size+1));
      for(i2=0;i2<size;i2++){
        if(i2<ppsize&&i<bbsize){
          nscrn[i][i2]=screenbuff[i][i2];
        }else{
          nscrn[i][i2]=' ';
        }
      }
    }
    freescrbuff();
    screenbuff=nscrn;
    ppsize=size;
    bbsize=nsize;
  }
  clear();
  for(i=bbsize-1;i>0;i--){
    scrnp=0;
    for(i2=0;i2<size;i2++){
      screenbuff[i][i2]=screenbuff[i-1][i2];
      if(i2>print_offset&&scrnp<COLS){
        mvwaddch(stdscr,i,i2-print_offset,screenbuff[i][i2]);

        scrnp++;
      }
      if(scrnp>=COLS){
        break;
      }
    }
  }
  double step=amp/size;
  double coeff=step*2;
  int fpull;
  int fprint=0;
  scrnp=0;
  for(i=0;i<size;i++){
    coeff=coeff+step;
    abs=trans[i]*coeff;
    screenbuff[0][i]=getfromdecimal(abs);
    if(i>print_offset&&scrnp<COLS){
      //mvwaddch(stdscr,0,i-print_offset,screenbuff[0][i]);
      if(fprint%10==0){
        fpull=get_freq_at_index(i);
        mvprintw(0,i-print_offset-1,"|%dHz",fpull*LOWEST_F);
      }
      scrnp++;
      fprint++;
    }
    if(scrnp>=COLS){
      break;
    }
  }
  mvprintw(LINES-1,0,"gain: %d",(int)amp);

refresh();
}

void printft(double* trans,int size){
  clear();
  int i;
  int h;
  int hh=LINES;
  int abs;
  mvprintw(0,0,"%d",LINES);
  for(i=0;i<size;i++){
    abs=trans[i];
    if(i<(size>>2)){
      abs=abs>>2;
    }else if(i<(size>>1)){
      abs=abs>>1;
    }

    for(h=0;h<abs;h++){
      if(h>=hh){
        break;
      }
      mvprintw(hh-1-h,i,"*");
    }
  }
  refresh();
}
int* f16convert=NULL;
char* buffer=NULL;
int bsize=0;
void free_ft(){
  free(f16convert);
  free(buffer);
  f16convert=NULL;
  buffer=NULL;
  bsize=0;
  free_fourier_transform();
}
double gw;
void init_ft(int argn){
  free(f16convert);
  free(buffer);
  double w=0.4;
  gw=w;
  init_fourier_transform(SIZE,w); 
  bsize=SIZE*2;
  f16convert=malloc(sizeof(int)*SIZE);
  buffer=malloc(sizeof(char)*bsize);
}
void reset_ft(int direction){
  print_offset=0;
  if(direction==1){
    gw=gw-0.002;
    if(gw<0.00001){
      gw=0.00001;
      return;
    }
  }
  if(direction==0){
    gw=gw+0.002;
    if(gw>1){
      gw=1;
      return;
    }
  }
  free_fourier_transform();
  init_fourier_transform(SIZE,gw); 

}
int new_data=0;
int argnextrac=0;
void* fftthreadf(void*){
   double* ppointer;
   int ndcopy=0;
   while(ndcopy!=-1){
    pthread_mutex_lock(&syncm);
    ndcopy=new_data;
    if(new_data==1){
      new_data=0;
      ppointer=produce_period_gram(f16convert,SIZE);
      if(argnextrac>1){
        printft(ppointer,get_fourier_size());
      }else{
        printspect(ppointer,get_fourier_size());
      }
    }
    pthread_mutex_unlock(&syncm);
    sleep(0.01);
  }
  return NULL;
}
int main(int argn,char* argv[]){
  argnextrac=argn;
  pthread_t fftthread;
  setlocale(LC_ALL, "");
  pthread_mutex_init(&syncm,NULL);
  initscr();
  start_color();
  init_colorpairs();
  nodelay(stdscr, TRUE);
  keypad(stdscr,TRUE);
  int fftspeed=FFT_SPEED_SPEC;
  if(argn>1){
    fftspeed=FFT_SPEED_SINGLE;
  }
  curs_set(0);
  snd_pcm_t *pcm_handle;
	int channels=1;
	
	if ( snd_pcm_open(&pcm_handle, DEVICE,SND_PCM_STREAM_CAPTURE, 0) < 0){
		return 0;
	} 
	if(snd_pcm_set_params(pcm_handle, SND_PCM_FORMAT_S16_LE,SND_PCM_ACCESS_RW_INTERLEAVED, channels, 44100, 1, 500000)<0){
		return 0;
	}
  init_ft(argn);
  pthread_create(&fftthread,NULL,*fftthreadf,NULL);
  int err;
  int count=0;
  char c=-1;
  while(1){
      count++;
     if ((err = snd_pcm_readi (pcm_handle, buffer, 128)) != 128) {
      exit (1);
    }
    c=wgetch(stdscr);
    if(count>fftspeed){
      pthread_mutex_lock(&syncm);
      f16_array_to_int(buffer,bsize,f16convert);
      new_data=1;
      pthread_mutex_unlock(&syncm);
      count=0;
    
    }
  if(c!=-1&&c!=-102&&c!=3&&c!=2&&c!=5&&c!=4&&c!='w'&&c!='s'){

        break;
      }
      if(c=='w'){
        amp=amp+1;
        if(amp>20){
          amp=20;
        }
      }
      if(c=='s'){
        amp=amp-1;
        if(amp<1){
          amp=1;
        }
      }
      if(c==3){
           pthread_mutex_lock(&syncm);
          reset_ft(1);
           pthread_mutex_unlock(&syncm);
          
      }
      if(c==2){
        pthread_mutex_lock(&syncm);
        reset_ft(0);
        pthread_mutex_unlock(&syncm);
      }
      if(c==5){
       pthread_mutex_lock(&syncm);
        print_offset++;
        if(print_offset>(get_fourier_size()-COLS)){
            print_offset--;
        }
        pthread_mutex_unlock(&syncm);
      }
      if(c==4){
       pthread_mutex_lock(&syncm);
        print_offset--;
        if(print_offset<0){
          print_offset=0;
        }
        pthread_mutex_unlock(&syncm);
      }
    }

    pthread_mutex_lock(&syncm);
    new_data=-1;
    pthread_mutex_unlock(&syncm);
  pthread_join(fftthread,NULL);

  free_ft();
  freescrbuff();
  snd_pcm_close(pcm_handle);
  endwin();
  return 0;
}

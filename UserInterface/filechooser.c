#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include "../SongSelect/songselector.h"
#include <ncurses.h>



char* get_write_directory_ni(){
	const char *homedir;

	if ((homedir = getenv("HOME")) == NULL) {
    		homedir = getpwuid(getuid())->pw_dir;
	}
	int len=strlen(homedir);
	char* retd=malloc(sizeof(char)*(len+1));
	sprintf(retd,"%s",homedir);
	return retd;
}


char* string_prompt(char* msg,char* prev){
clear();
curs_set(1);
int add=strlen(prev);

char c=0;
int size=COLS-1;
int i;
char* string=malloc(sizeof(char)*(size+1));
for(i=0;i<add;i++){
	if(i>=size){
		break;
	}
	string[i]=prev[i];
}
string[i]=0;
mvprintw(0,0,"%s, press enter to exit or '\\ '/esc to cancel '`' to clear",msg);
mvprintw(1,0,"%s",string);
refresh();
int pos=i;
while((c=getch())!='\n'){
if(c=='\\'||c==27){
	free(string);
	curs_set(0);
	return NULL;
}
if(c==127){
	pos--;
	if(pos<0){
		pos=0;
		string[0]=0;
	}
	string[pos+1]=0;
	string[pos]=0;
	clear();
	mvprintw(0,0,"%s, press enter to exit or '\\ '/esc to cancel '`' to clear",msg);
	mvprintw(1,0,"%s",string);
	refresh();
}
if(c>31&&(c!=127)){
string[pos]=c;
string[pos+1]=0;
clear();
mvprintw(0,0,"%s, press enter to exit or '\\(bkwd slash)'/esc to cancel '`' to clear",msg);
mvprintw(1,0,"%s",string);
refresh();
pos++;
if(pos>=size){
pos=0;
}
}
if(c=='`'){
	for(i=0;i<pos;i++){
		string[i]=0;
	}
	pos=0;
	mvprintw(0,0,"%s, press enter to exit or '\\ '/esc to cancel '`' to clear",msg);
	mvprintw(1,0,"%s",string);
	refresh();
}
}
curs_set(0);
return string;
}

int draw_file(int loc,char** on,char* path,int dirsonly){
	clear();
	DIR* d=opendir(path);
	int max=0;
	struct dirent* dd;
	int p=0;
	int size1=strlen(path);
	char* rpath=NULL;
	int size2;
	int nextshow=1;
	int is_file=0;
	mvprintw(0,0,">");
	char* ptr;
	while((dd=readdir(d))!=NULL){
		char* namae=dd->d_name;
		if(strcmp(namae,".")!=0&&strcmp(namae,"..")!=0){
			size2=strlen(namae);
			is_file=0;
			if(dirsonly==1){
				ptr=malloc(sizeof(char)*(size2+size1+2));
				sprintf(ptr,"%s/%s",path,namae);
				
				int isndint=is_not_directory(ptr);
				free(ptr);
				if(isndint==-1){
					nextshow=1;
				}else{
					nextshow=0;
				}
			}else{
				ptr=malloc(sizeof(char)*(size2+size1+2));
				sprintf(ptr,"%s/%s",path,namae);
				int isndint=is_not_directory(ptr);
				free(ptr);
				int f=-1;
				if(isndint==1){
					is_file=1;
					f=get_audio_format(namae);
				}else{
					is_file=0;
				}
				if(f!=-1||(isndint==-1)){
					nextshow=1;
				}else{
					nextshow=0;
				}
			}
			if(nextshow==1){
			if(p>=loc){
				if(max==0){
					if(rpath==NULL){
					rpath=malloc(sizeof(char)*(size2+size1+2));
					sprintf(rpath,"%s/%s",path,namae);
					}
				}
				if(is_file==1){
					attron(A_BOLD);
				}
				mvprintw(max,1,"%s",namae);
				if(is_file==1){
					attroff(A_BOLD);
				}
				
				if(max>LINES-1){
					break;
				}
				max++;
			}
			p++;
			}
		}
	}
	*on=rpath;
	attron(A_STANDOUT);
	int i;
	for(i=0;i<COLS;i++){
		mvprintw(LINES-1,i," ");
		mvprintw(LINES-2,i," ");
		mvprintw(LINES-3,i," ");
	}
	mvprintw(LINES-3,0," |%s| ",path);
	mvprintw(LINES-2,0," |press p to enter path manually, press d to select the current dir| ");
	mvprintw(LINES-1,0," |press escape or q to go back, press enter to select| ");
	attroff(A_STANDOUT);
	closedir(d);
	refresh();
	return p;
}

char* file_chooser(char* path,int dirsonly){
	char* pth=path;
	if(path==NULL){
		pth=get_write_directory_ni();
	}
	char c;
	int cnt=0;
	char* cptr=NULL;
	int size=draw_file(cnt,&cptr,pth,dirsonly);
	while((c=getch())!=27){
		if(c=='q'){
			break;
		}
		if(c==2){
			cnt++;
			if(cnt>=size-1){
				cnt=size-1;
			}
		}
		if(c==3){
			cnt--;
			if(cnt<=0){
				cnt=0;
			}
		}
		if(c=='\n'){
			if(cptr!=NULL){
				int isndint=is_not_directory(cptr);
				if(isndint==1){
					return cptr;
				}else{
					char* np=file_chooser(cptr,dirsonly);
					if(np!=NULL){
						free(cptr);
						return np;
					}
				}
			}else{
				char* cp=pointerize(path);
				return cp;
			}
		}
		if(c=='d'&&dirsonly==1){
			return cptr;
		}
		if((c=='p'||c=='/')&&(cptr!=NULL)){
			char* prmpt=string_prompt("",cptr);
			if(prmpt!=NULL){
				int isndint=is_not_directory(prmpt);
				if(isndint==-1){
					char* np=file_chooser(prmpt,dirsonly);
					if(np!=NULL){
						free(cptr);
						free(prmpt);
						return np;
					}
				}
				free(prmpt);
			}
		}
		free(cptr);
		size=draw_file(cnt,&cptr,path,dirsonly);
	}
	if(cptr!=NULL){
		free(cptr);
	}
	return NULL;
}

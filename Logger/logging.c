#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Loader/playlistloader.h"

void log_line(char* line){
  char* wdir=get_write_directory();
  char* logfile=string_combine(wdir,"/LOGGFILE.log");
  free(wdir);
  char buffer[LOG_FILE_SIZE+1];
  FILE* f=fopen(logfile,"r");
  int counting=strlen(line);
  int i;
  for(i=0;i<counting;i++){
    buffer[i]=line[i];
  }
  buffer[i]='\n';
  counting++;
  if(f){
  char gg;
  while((gg=fgetc(f))!=0){
    if(gg==EOF){
        break;
      }
    if(counting>LOG_FILE_SIZE){
      break;
    }
    buffer[counting]=gg;
    counting++;
  }
  fclose(f);
  }
  f=fopen(logfile,"w");
  buffer[counting]=0;
  printf("%s\n",buffer);
  int llen=strlen(buffer);
  fwrite(buffer,sizeof(char),llen+1,f);
  fclose(f);

}

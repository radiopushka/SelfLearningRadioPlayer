#include "markovchain.h"
#include<stdlib.h>

//program to test functionality  of the markov chain data structure
void writepl(struct PlaylistMarker* pl){
	FILE* f=fopen("testsave","wb");
	write_chain_to_file(pl,f);
	fclose(f);
}
struct PlaylistMarker* readpl(){
	FILE* f=fopen("testsave","rb");
	struct PlaylistMarker* pl=read_chain_from_file(f);
	fclose(f);
	return pl;
}

int main(){

  struct PlaylistMarker* chain= create_new_markov_chain();
  add_element_for(chain,"test1");
  add_element_for(chain,"test2");
  add_element_for(chain,"test3");
  push_score_up_for(chain,"test1","path1");
  push_score_up_for(chain,"test1","path1");
  push_score_up_for(chain,"test1","path2");
  push_score_up_for(chain,"test1","path3");
  push_score_up_for(chain,"test2","path3");
  push_score_up_for(chain,"test3","path4");
  dump_as_text(chain);
  char* ongaku=get_song_from_probability(chain,"test1",2);
  printf("--\n%s\n",ongaku);
  free(ongaku);
  printf("file io:\n");
  writepl(chain);
  free_memory_markov(chain);
  printf("read\n");
  chain=readpl();
  dump_as_text(chain);
  printf("operations:\n");
  delete_element_for(chain,"path4");
  delete_element_for(chain,"test1");
  dump_as_text(chain);
  free_memory_markov(chain);
}

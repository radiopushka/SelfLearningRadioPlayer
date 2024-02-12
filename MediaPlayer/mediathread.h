#ifndef MEDIATHREAD
#define MEDIATHREAD
  #define SIGNAL_EXIT 10
  #define SIGNAL_FWD 11
  #define SIGNAL_PAUSE 12
  #define SIGNAL_PLAY 13
  #define SIGNAL_SKIP 14
  
  void init_media_thread();
  void request_data_mutex();//request write access to common stack
  void release_data_mutex();
  
  void signal(int signal);//listed above
  void* media_runable(void* arg);//run this in a pthread
  char* get_current_song_name();
  
  void set_low_pass_freq(int freq);
  
  int get_total_time();
  int get_position();
#endif // !MEDIATHREAD

ALGORITHMLIB := ./SongSelect/songselector.c ./Logger/logging.c ./Loader/playlistloader.c ./playlist/stack.c ./playlist/playlist.c ./playlist/playlist_io.c ./SongSelect/markovchain.c ./MediaPlayer/wav.c ./MediaPlayer/mediathread.c ./UserInterface/playlisteditor.c ./MediaPlayer/Convolution/lowpass.c
ARGS :=  -g -Wall -lasound -lvorbis -lvorbisfile -lpthread -lncursesw -lm -o 
NAME := advplayer


all:prog

prog:
	gcc main.c ${ALGORITHMLIB} ${ARGS} ${NAME}

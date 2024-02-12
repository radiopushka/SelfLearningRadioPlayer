ALGORITHMLIB := ../SongSelect/songselector.c ../Logger/logging.c ../Loader/playlistloader.c ../playlist/stack.c ../playlist/playlist.c ../playlist/playlist_io.c ../SongSelect/markovchain.c wav.c mediathread.c Convolution/lowpass.c
ARGS :=  -g -Wall -lasound -lvorbis -lvorbisfile -lpthread -lm -o 
NAME := medtest

all:
	gcc ${ALGORITHMLIB} mediathreadtest.c ${ARGS} ${NAME}


# SelfLearningRadioPlayer
An upgrade from the buggy glitchy program I wrote a while ago to run my pirate radio station. This one can analyze the songs i queue and optimize the shuffling of songs based upon this data.

Dependencies:
libncurses
libpthread
libasound
libvorbis


functionality:

actually has two programs, advplayer and ftaudio

ftaudio: (build in the DSP folder)
it can take audio from the monitor of your sound card or from your microphone and display the fourier transform waterfall of it in your terminal. it is kind of not beautiful but whatever.
use the arrow up key to zoom in and arrow down to zoom out left and right arrows to move left and right, w and s keys set the gain, space is pause and play waterfall.

advplayer:
the radio player itself, it shuffles songs based upon what you select in the queue, it will remeber those patterns and vary them each time. It has the play and pause and seek foreward options
launch the program, it will tell what each key does.

my code is ok but has not comments, sorry about that, writing them takes too much time and I am too busy. You can use any fragments or librariers I wrote for commercial use or whatever.
I have no life anyways and nothing to live for. Hopefully the function names will help you navigate. I take no responsibility for use.

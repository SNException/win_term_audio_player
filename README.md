# win_term_audio_player

Simple audio (.wav) player writen with the WinAPI.

## How to build
Simply run the build.bat file. Since this C program uses the GNU gcc compiler you need to have 'gcc' added to your PATH.

## How to use
Execute the play.exe file and pass the .wav file you wish to play as an argument. You can also pass a whole directory. The application
will then go through it and will play each .wav file in sequence. You can skip a file by simply pressing RETURN. If you wish to stop the execution
you can achieve that by pressing CTRL-C.

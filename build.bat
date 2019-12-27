@echo off

if not exist bin mkdir bin
del /Q /F bin\*

gcc -Wall -O3 -ftime-report src/main.c -o bin/play -lwinmm -lshlwapi

if exist bin/play.exe (
	echo Build succeeded.
) else (
	echo Build failure.
)

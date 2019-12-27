#include <stdio.h>
#include <string.h>

#include <windows.h>
#include <shlwapi.h>

typedef unsigned int u32;
#define loop_i register u32

#define file_scoped static

file_scoped void
play_wav_file(const char *file_name) {
	// TODO(nschultz): Make the volume adjustable
	waveOutSetVolume(0, 0x10101010);
	if (PlaySoundA(file_name, NULL, SND_FILENAME | SND_ASYNC)) {
		printf("Now playing '%s' ...\n", file_name);
		getchar();
	} else {
		printf("Failed to play sound file\n");
	}
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		printf("Either a file name or a directory name has to be specified.\n");
		return 1;
	}

	const char *file_name = argv[1];
	if (!PathFileExistsA(file_name)) {
		printf("The file/directory '%s' does not exist.\n", file_name);
		return 2;
	}

	if (GetFileAttributesA(file_name) == FILE_ATTRIBUTE_DIRECTORY) {
		// char *name = VirtualAlloc(0, sizeof(char) * 12, MEM_RESERVE | MEM_COMMIT, 0);
		char *name = calloc(strlen(file_name), sizeof(char));
		if (name == NULL) {
			printf("Memory allocation failed\n");
			return 3;
		}
		sprintf(name, "%s\\*", file_name);

		WIN32_FIND_DATA find_file_data;
		memset(&find_file_data, 0, sizeof(WIN32_FIND_DATA));

		HANDLE hFind = FindFirstFile(name, &find_file_data);
		if (hFind == INVALID_HANDLE_VALUE) {
			printf("Failed to read contents of directory.\n");
			return 4;
		}
		
		if (strcmp(strrchr(find_file_data.cFileName, '.'), ".wav") == 0) {
			char *full_name = calloc(strlen(file_name) + strlen(find_file_data.cFileName) + 2, sizeof(char));
			if (full_name == NULL) {
				printf("Memory allocation failed\n");
				return 5;
			}
			sprintf(full_name, "%s\\%s", file_name, find_file_data.cFileName);
			play_wav_file(full_name);
			free(full_name);
		}
		while (FindNextFile(hFind, &find_file_data)) {
			if (strcmp(strrchr(find_file_data.cFileName, '.'), ".wav") == 0) {
				char *full_name = calloc(strlen(file_name) + strlen(find_file_data.cFileName) + 2, sizeof(char));
				if (full_name == NULL) {
					printf("Memory allocation failed\n");
					return 5;
				}
				sprintf(full_name, "%s\\%s", file_name, find_file_data.cFileName);
				play_wav_file(full_name);
				free(full_name);
			}
		}
		FindClose(hFind);
		free(name);
	} else {
		if (waveOutGetNumDevs() >= 1) {
			play_wav_file(file_name);
		} else {
			printf("No waveform-audio devices are present or an error occured.\n");
		} 
	}
	return 0;
}

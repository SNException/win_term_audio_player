#include <stdio.h>
#include <string.h>

#include <windows.h>
#include <shlwapi.h>

typedef unsigned int u32;
#define loop_i register u32

#define file_scoped static

// TODO(nschultz): Create parse function which takes
// in an error code and produces a human readable text.
typedef enum error_codes {
	SUCCESS = 0,
	ERR_CALLOC = 1,
	ERR_IO = 1
} ERROR_CODES;

file_scoped void
play_wav_file(const char *file_name, u32 *err) {
	*err = SUCCESS;
	// TODO(nschultz): Make the volume adjustable
	// waveOutSetVolume(0, 0x10100000); // right
	// waveOutSetVolume(0, 0x00001010); // left
	waveOutSetVolume(0, 0x10101010);
	if (PlaySoundA(file_name, NULL, SND_FILENAME | SND_ASYNC)) {
		printf("Now playing '%s' ...\n", file_name);
		// TODO(nschultz): This blocks even after the audio file is done (obv).
		// Therefore we need to check when we are done to return control back to the callee
		// and hence continue with the 'playlist'.
		getchar();
	} else {
		*err = ERR_IO;
	}
}

file_scoped void
play_wav_files(const char *dir_name, u32 *err) {
	*err = SUCCESS;
	char *name = calloc(strlen(dir_name), sizeof(char));
	if (name == NULL) {
		*err = ERR_CALLOC;
		return;
	}
	sprintf(name, "%s\\*", dir_name);

	WIN32_FIND_DATA find_file_data;
	memset(&find_file_data, 0, sizeof(WIN32_FIND_DATA));

	HANDLE hFind = FindFirstFile(name, &find_file_data);
	if (hFind == INVALID_HANDLE_VALUE) {
		*err = ERR_IO;
		goto cleanup;
	}
	
	if (strcmp(strrchr(find_file_data.cFileName, '.'), ".wav") == 0) {
		char *full_name = calloc(strlen(dir_name) + strlen(find_file_data.cFileName) + 2, sizeof(char));
		if (full_name == NULL) {
			*err = ERR_CALLOC;
			goto cleanup;
		}
		sprintf(full_name, "%s\\%s", dir_name, find_file_data.cFileName);
		play_wav_file(full_name, err);
		free(full_name);
	}
	while (FindNextFile(hFind, &find_file_data)) {
		if (strcmp(strrchr(find_file_data.cFileName, '.'), ".wav") == 0) {
			char *full_name = calloc(strlen(dir_name) + strlen(find_file_data.cFileName) + 2, sizeof(char));
			if (full_name == NULL) {
				*err = ERR_CALLOC;
				goto cleanup;
			}
			sprintf(full_name, "%s\\%s", dir_name, find_file_data.cFileName);
			play_wav_file(full_name, err);
			free(full_name);
		}
	}
	cleanup:
	FindClose(hFind);
	free(name);
}

int
main(int argc, char **argv) {
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
		u32 err;
		play_wav_files(file_name, &err);
		if (err != SUCCESS) {
			printf("An error occured: %u\n", err);
		}
	} else {
		if (waveOutGetNumDevs() >= 1) {
			u32 err;
			play_wav_file(file_name, &err);
			if (err != SUCCESS) {
				printf("An error occured: %u\n", err);
			}
		} else {
			printf("No waveform-audio devices are present or an error occured.\n");
		} 
	}
	return 0;
}

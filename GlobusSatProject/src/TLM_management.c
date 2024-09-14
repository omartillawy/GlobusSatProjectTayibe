#include <satellite-subsystems/IsisTRXUV.h>
#include <satellite-subsystems/IsisAntS.h>
#include <satellite-subsystems/imepsv2_piu.h>
#include <hcc/api_mdriver_atmel_mcipdc.h>
#include <TLM_management.h>
#include <string.h>
#include <utils.h>
#include <stdlib.h>
#include <hal/errors.h>
#include <hcc/api_fat.h>
#include <hcc/api_hcc_mem.h>
#include <hcc/api_fs_err.h>
#include <hcc/api_fat_test.h>
#include <stdio.h>
#define SD_CARD_DRIVER_PRI 0
#define SD_CARD_DRIVER_SEC 1

FileSystemResult InitializeFS() {
	int err = hcc_mem_init();
	if (err != E_NO_SS_ERR) {
		logError(err, "hcc_mem_init failed");
		return err;
	}
	err = fn_init();
	if (err != E_NO_SS_ERR) {
		logError(err, "hcc_mem_init failed");
		return err;
	}
	err = f_enterFS();

	if (err != E_NO_SS_ERR) {
		logError(err, "fs_enterFS function  failed");
		return err;
	}

	err = f_initvolume(0, atmel_mcipdc_initfunc, 0); /* Initialize volID as safe */

	if ( F_ERR_NOTFORMATTED == err) {
		logError(err, "Filesystem not formated!\n\r");
		return -1;
	} else if ( F_NO_ERROR != err) {
		logError(err, "f_initvolume pb: %d\n\r");
		return -1;
	}
	return FS_SUCCSESS;
}

void DeInitializeFS(int SD_Card) {

	int err = fm_delvolume(SD_Card);
	if (err != 0) {
		logError(err, "deleting voulme");
		return;
	}

	f_releaseFS();
	err = hcc_mem_delete();
	if (err != 0) {
		logError(err, "cannot delete mememory voulume");
		return;
	}
}

FileSystemResult c_fileCreate(char* file_name, F_FILE ** file) {

	if (strlen(file_name) > MAX_FILE_NAME_SIZE) {
		logError(FS_TOO_LONG_NAME, "the file name size is too big");
		return FS_TOO_LONG_NAME;
	}

	F_FILE * res = fm_open(file_name, "w+");
	*file = res;

	if (res) {
		*file = res;
		return FS_SUCCSESS;
	} else {
		return fm_getlasterror();
	}

}

FileSystemResult c_fileWrite(char* file_name, void* element, int element_size) {
	F_FILE * file = fm_open(file_name, "r+");
	if (!file) {
		logError(FS_NOT_EXIST, "file isn't existing");
		return FS_NOT_EXIST;
	}

	unsigned int epoch;
	Time_getUnixEpoch(&epoch);
	unsigned int x = (unsigned) sizeof(epoch);

	fm_write(&epoch, x, 1, file);
	fm_write(&element_size, 4, 1, file);
	fm_write(element, element_size, 1, file);

	fm_close(file);

	return FS_SUCCSESS;

}
int c_fileGetNumOfElements(char* c_file_name, time_unix from_time,
		time_unix to_time) {

	int counter = 0;

	F_FILE *file = fm_open(c_file_name, "r");
	if (!file) {
		logError(FS_NOT_EXIST, "file isn't existing");
		return FS_NOT_EXIST;
	}

	int Pos1 = getTimePostion(file, from_time);
		int Pos2 = getTimePostion(file, to_time);

		if (Pos1 == -1 || Pos2 == -1)
			return -1;


	time_unix time;
	int seek;

	fm_seek(file,Pos1,SEEK_SET);
	while (f_eof(file) == 0) {

		   fm_read(&time, (int) sizeof(time), 1, file);
		   fm_read(&seek, 4, 1, file);
		   fm_seek(file, seek, SEEK_CUR);
		   counter++;


		if (time == to_time) {
			break;
		}

	}

	return counter;
}
FileSystemResult c_fileRead(char* c_file_name, byte* buffer, int size_of_buffer,
		time_unix from_time, time_unix to_time, int* read,
		time_unix* last_read_time) {

	int sizeRead = 0;
	*read = 0;

	F_FILE *file = fm_open(c_file_name, "r");
	if (!file) {
		logError(FS_NOT_EXIST, "file isn't existing");
		return FS_NOT_EXIST;
	}

	int Pos1 = getTimePostion(file, from_time);
	int Pos2 = getTimePostion(file, to_time);

	if (Pos1 == -1 || Pos2 == -1)
		return -1;

	time_unix time;

	int seek = 0;
	fm_seek(file, Pos1, SEEK_SET);

	while (fm_eof(file) == 0) {

		fm_read(&time, (int) sizeof(time), 1, file);

		if (time == to_time) {
			fm_close(file);
			*last_read_time = time;
			return FS_SUCCSESS;
		}

		if (sizeRead > size_of_buffer)
			return FS_BUFFER_OVERFLOW;
		char* data = (char*) malloc(seek);
		fm_read(data, seek, 1, file);
		strcat((char *) buffer, data);
		(*read)++;
		sizeRead += seek;

	}
	return FS_SUCCSESS;
}

FileSystemResult c_fileDeleteElements(char* c_file_name, time_unix from_time,
		time_unix to_time) {

	F_FILE *file = fm_open(c_file_name, "r+");
	if (!file) {
		logError(FS_NOT_EXIST, "file isn't existing");
		return FS_NOT_EXIST;
	}

	int Pos1 = getTimePostion(file, from_time);
	int Pos2 = getTimePostion(file, to_time);

	if (Pos1 == -1 || Pos2 == -1)
		return -1;

	int seek = 0;
	time_unix temp_time;
	while (fm_eof(file) == 0) {

		fm_seek(file, Pos2, SEEK_SET);
		fm_read(&temp_time, sizeof(time_unix), 1, file);
		fm_read(&seek, 4, 1, file);
		void *data = malloc(seek);
		fm_read(data, seek, 1, file);

		fm_seek(file, Pos1, SEEK_SET);
		fm_write(&temp_time, sizeof(time_unix), 1, file);
		fm_write(&seek, sizeof(int), 1, file);
		fm_write(data, seek, 1, file);

		Pos1 += sizeof(time_unix) + sizeof(seek) + seek;
		Pos2 += sizeof(time_unix) + sizeof(seek) + seek;
	}

	fm_close(file);
	return FS_SUCCSESS;
}

int getTimePostion(F_FILE * file, time_unix time) {

	int seek = 0;
	time_unix time_cur;
	while (fm_eof(file) == 0) {

		fm_read(&time_cur, (int) sizeof(time), 1, file);
		fm_read(&seek, 4, 1, file);

		if (time_cur == time)
			return fm_tell(file) - 4 - sizeof(time_cur);
		else {
			fm_seek(file, seek, SEEK_CUR);
		}

	}
	return -1;
}



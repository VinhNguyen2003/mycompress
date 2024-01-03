#ifndef _ZIP_HANDLER_H
#define _ZIP_HANDLER_H

#include "file_list.h"

#define BLOCK_SIZE 512
#define MAX_PATH_LENGTH 4096

int zip_compress(const char *archive_path, const file_list_t *files);
int zip_extract(const char *archive_path, const file_list_t *files, const char *output_dir);

#endif

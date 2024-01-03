#ifndef TAR_HANDLER_H
#define TAR_HANDLER_H

#include "file_list.h"

#define BLOCK_SIZE 512
#define MAX_PATH_LENGTH 4096
#define REGTYPE '0'

int tar_compress(const char *archive_name, const file_list_t *files);
int tar_extract(const char *archive_name, const file_list_t *files, const char *output_dir);

#endif

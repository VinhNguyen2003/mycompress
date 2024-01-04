#ifndef _7Z_HANDLER_H
#define _7Z_HANDLER_H

#include "file_list.h"

#define MAX_PATH_LENGTH 4096

int sevenz_compress(const char *archive_path, const file_list_t *files);
int sevenz_extract(const char *archive_path, const file_list_t *files, const char *output_dir);

#endif

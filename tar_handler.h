#ifndef TAR_HANDLER_H
#define TAR_HANDLER_H

#include "file_list.h"

int tar_compress(const char *archive_name, const file_list_t *files);
int tar_extract(const char *archive_name, const file_list_t *files);

#endif

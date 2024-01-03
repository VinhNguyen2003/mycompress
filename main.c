#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_list.h"
#include "tar_handler.h"

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <format> <command> <archive_name> [-o output_dir] [file...]\n", argv[0]);
        fprintf(stderr, "Formats: tar, zip\n");
        fprintf(stderr, "Commands: -c (create), -x (extract)\n");
        return 1;
    }

    const char *format = argv[1];
    const char *command = argv[2];
    const char *archive_name = argv[3];

    file_list_t files;
    file_list_init(&files);

    for (int i = 4; i < argc; i++) {
        file_list_add(&files, argv[i]);
    }

    int result = 0;

    // Check for format and perform corresponding actions
    if (strcmp(format, "tar") == 0) {
        if (strcmp(command, "-c") == 0) {
            result = tar_compress(archive_name, &files);
        } else if (strcmp(command, "-x") == 0) {
            result = tar_extract(archive_name, &files);
        } else {
            fprintf(stderr, "Invalid command for tar format.\n");
            result = 1;
        }
    } else {
        fprintf(stderr, "Unsupported format");
        result = 1;
    }

    file_list_clear(&files);
    return result;
}

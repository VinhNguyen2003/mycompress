#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_list.h"
#include "tar_handler.h"
#include "zip_handler.h"
#include "7z_handler.h"

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <format> <command> <archive_name> [-o output_dir] [file...]\n", argv[0]);
        fprintf(stderr, "Formats: tar, targz, zip, 7z\n");
        fprintf(stderr, "Commands: -c (create), -x (extract)\n");
        return 1;
    }

    const char *format = argv[1];
    const char *command = argv[2];
    const char *archive_name = argv[3];
    const char *output_dir = ".";

    file_list_t files;
    file_list_init(&files);

    int file_start_index = 4;
    if (argc > 4 && strcmp(argv[4], "-o") == 0) {
        if (argc < 6) {
            fprintf(stderr, "Output directory specified but no directory given.\n");
            return 1;
        }
        output_dir = argv[5];
        file_start_index = 6;
    }

    for (int i = file_start_index; i < argc; i++) {
        file_list_add(&files, argv[i]);
    }

    char full_archive_path[MAX_PATH_LENGTH];
    if (output_dir) {
        snprintf(full_archive_path, sizeof(full_archive_path), "%s/%s", output_dir, archive_name);
    } else {
        strncpy(full_archive_path, archive_name, sizeof(full_archive_path));
        full_archive_path[sizeof(full_archive_path) - 1] = '\0';
    }

    int result = 0;

    if (strcmp(format, "tar") == 0) {
        if (strcmp(command, "-c") == 0) { 
            result = tar_compress(full_archive_path, &files);
        } else if (strcmp(command, "-x") == 0) {
            result = tar_extract(archive_name, &files, output_dir);
        } else {
            fprintf(stderr, "Invalid command for tar format.\n");
            result = 1;
        }
    } else if (strcmp(format, "targz") == 0) {
        if (strcmp(command, "-c") == 0) {
            result = tar_gz_compress(full_archive_path, &files);
        } else if (strcmp(command, "-x") == 0) {
            result = tar_gz_extract(archive_name, &files, output_dir);
        } else {
            fprintf(stderr, "Invalid command for targz format.\n");
            result = 1;
        }
    } else if (strcmp(format, "zip") == 0) {
        if (strcmp(command, "-c") == 0) {
            result = zip_compress(full_archive_path, &files);
        } else if (strcmp(command, "-x") == 0) {
            result = zip_extract(archive_name, &files, output_dir);
        } else {
            fprintf(stderr, "Invalid command for zip format.\n");
            result = 1;
        }
    } else if (strcmp(format, "7z") == 0) {
        if (strcmp(command, "-c") == 0) {
            result = sevenz_compress(full_archive_path, &files);
        } else if (strcmp(command, "-x") == 0) {
            result = sevenz_extract(archive_name, &files, output_dir);
        } else {
            fprintf(stderr, "Invalid command for 7z format.\n");
            result = 1;
        }
    } else {
        fprintf(stderr, "Unsupported format");
        result = 1;
    }

    file_list_clear(&files);
    return result;
}

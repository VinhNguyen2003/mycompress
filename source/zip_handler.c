#include "zip_handler.h"
#include <zip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int zip_compress(const char *archive_path, const file_list_t *files) {
    int err = 0;
    zip_t *zip = zip_open(archive_path, ZIP_CREATE | ZIP_EXCL, &err);
    if (!zip) {
        fprintf(stderr, "Failed to create zip file: %s\n", archive_path);
        return -1;
    }

    node_t *current = files->head;
    while (current) {
        zip_file_add(zip, current->name, zip_source_file(zip, current->name, 0, 0), ZIP_FL_OVERWRITE);
        current = current->next;
    }

    if (zip_close(zip) < 0) {
        fprintf(stderr, "Failed to close zip: %s\n", zip_strerror(zip));
        return -1;
    }

    return 0;
}

int zip_extract(const char *archive_path, const file_list_t *files, const char *output_dir) {
    int err = 0;
    zip_t *zip = zip_open(archive_path, 0, &err);
    if (!zip) {
        fprintf(stderr, "Failed to open zip file: %s\n", archive_path);
        return -1;
    }

    zip_int64_t num_entries = zip_get_num_entries(zip, 0);

    for (zip_int64_t i = 0; i < num_entries; i++) {
        const char *name = zip_get_name(zip, i, 0);

        if (files->size == 0 || file_list_contains(files, name)) {
            zip_file_t *file = zip_fopen_index(zip, i, 0);
            if (!file) {
                fprintf(stderr, "Failed to open file in zip: %s\n", name);
                zip_close(zip);
                return -1;
            }

            char full_path[MAX_PATH_LENGTH];
            snprintf(full_path, sizeof(full_path), "%s/%s", output_dir, name);

            FILE *out = fopen(full_path, "wb");
            if (!out) {
                fprintf(stderr, "Failed to open output file: %s\n", full_path);
                zip_fclose(file);
                zip_close(zip);
                return -1;
            }

            // Manually copy the file contents
            char buffer[BLOCK_SIZE];
            zip_int64_t bytes_read;
            while ((bytes_read = zip_fread(file, buffer, sizeof(buffer))) > 0) {
                if (fwrite(buffer, 1, bytes_read, out) != bytes_read) {
                    fprintf(stderr, "Failed to write to output file: %s\n", full_path);
                    fclose(out);
                    zip_fclose(file);
                    zip_close(zip);
                    return -1;
                }
            }

            fclose(out);
            zip_fclose(file);
        }
    }

    zip_close(zip);
    return 0;
}

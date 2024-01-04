#include "7z_handler.h"
#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>

int sevenz_compress(const char *archive_path, const file_list_t *files) {
    struct archive *archive;
    struct archive_entry *entry;
    FILE *file;
    char buffer[8192];
    size_t len;

    archive = archive_write_new();
    if (archive_write_set_format_7zip(archive) != ARCHIVE_OK) {
        fprintf(stderr, "Failed to set 7zip format: %s\n", archive_error_string(archive));
        return -1;
    }

    if (archive_write_open_filename(archive, archive_path) != ARCHIVE_OK) {
        fprintf(stderr, "Failed to open archive for writing: %s\n", archive_error_string(archive));
        return -1;
    }

    node_t *current = files->head;
    while (current) {
        entry = archive_entry_new();
        archive_entry_set_pathname(entry, current->name);
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);

        file = fopen(current->name, "rb");
        if (!file) {
            fprintf(stderr, "Failed to open file: %s\n", current->name);
            return -1;
        }

        struct stat file_stat;
        if (stat(current->name, &file_stat) != 0) {
            fprintf(stderr, "Failed to stat file: %s\n", current->name);
            return -1;
        }
        archive_entry_set_size(entry, file_stat.st_size);

        archive_write_header(archive, entry);
        while ((len = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            ssize_t write_len;
            write_len = archive_write_data(archive, buffer, len);
            if (write_len < 0 || write_len != len) {
                fprintf(stderr, "Failed to write data to archive: %s\n", archive_error_string(archive));
                fclose(file);
                archive_entry_free(entry);
                archive_write_free(archive);
                return -1;
            }
        }
        fclose(file);
        archive_entry_free(entry);
        current = current->next;
    }

    archive_write_close(archive);
    archive_write_free(archive);

    return 0;
}

int sevenz_extract(const char *archive_path, const file_list_t *files, const char *output_dir) {
    struct archive *archive;
    struct archive_entry *entry;
    FILE *out;
    size_t len;
    char buff[8192];

    archive = archive_read_new();
    archive_read_support_format_7zip(archive);
    if (archive_read_open_filename(archive, archive_path, 10240) != ARCHIVE_OK) {
        fprintf(stderr, "Failed to open archive for reading: %s\n", archive_error_string(archive));
        return -1;
    }

    while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
        const char *pathname = archive_entry_pathname(entry);
        if (files->size == 0 || file_list_contains(files, pathname)) {
            char full_path[MAX_PATH_LENGTH];
            snprintf(full_path, sizeof(full_path), "%s/%s", output_dir, pathname);
            out = fopen(full_path, "wb");
            if (!out) {
                fprintf(stderr, "Failed to open output file: %s\n", full_path);
                return -1;
            }

            while ((len = archive_read_data(archive, &buff, sizeof(buff))) > 0) {
                if (fwrite(buff, 1, len, out) != len) {
                    fprintf(stderr, "Failed to write to output file: %s\n", full_path);
                    return -1;
                }
            }
            fclose(out);
        }
        else {
            archive_read_data_skip(archive);
        }
    }

    archive_read_close(archive);
    archive_read_free(archive);

    return 0;
}

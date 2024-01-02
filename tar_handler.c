#include "tar_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#define BLOCK_SIZE 512
#define REGTYPE '0'

typedef struct {
    // File's name, as a null-terminated string
    char name[100];
    // File's permission bits
    char mode[8];
    // Numerical ID of file's owner, 0-padded octal
    char uid[8];
    // Numerical ID of file's group, 0-padded octal
    char gid[8];
    // Size of file in bytes, 0-padded octal
    char size[12];
    // Modification time of file in Unix epoch time, 0-padded octal
    char mtime[12];
    // Checksum (simple sum) header bytes, 0-padded octal
    char chksum[8];
    // File type (use constants defined below)
    char typeflag;
    // Unused for this project
    char linkname[100];
    // Indicates which tar standard we are using
    char magic[6];
    char version[2];
    // Name of file's user, as a null-terminated string
    char uname[32];
    // Name of file's group, as a null-terminated string
    char gname[32];
    // Major device number, 0-padded octal
    char devmajor[8];
    // Minor device number, 0-padded octal
    char devminor[8];
    // String to prepend to file name above, if name is longer than 100 bytes
    char prefix[155];
    // Padding to bring total struct size up to 512 bytes
    char padding[12];
} tar_header;


// Forward declaration of helper functions
static void compute_checksum(tar_header *header);
static int fill_tar_header(tar_header *header, const char *file_name);
static int write_file_to_archive(FILE *arch, const char *file_name);
static int extract_file(FILE *arch, const tar_header *header);
static int add_footer_blocks(FILE *arch);

// Improved tar compress function
int tar_compress(const char *archive_name, const file_list_t *files) {
    // Validate the archive name and files list
    if (!archive_name || !files || files->size == 0) {
        fprintf(stderr, "Invalid arguments for tar compression.\n");
        return -1;
    }

    FILE *arch = fopen(archive_name, "wb");
    if (!arch) {
        perror("Failed to create archive");
        return -1;
    }

    // Iterate through the file list and add each file to the archive
    for (node_t *curr = files->head; curr != NULL; curr = curr->next) {
        if (write_file_to_archive(arch, curr->name) != 0) {
            fclose(arch);
            return -1; // write_file_to_archive prints its own error messages
        }
    }

    // Add the footer blocks to the archive
    if (add_footer_blocks(arch) != 0) {
        fclose(arch);
        return -1;
    }

    fclose(arch);
    return 0;
}

int tar_extract(const char *archive_name, const file_list_t *files) {
    FILE *arch = fopen(archive_name, "rb");
    if (!arch) {
        perror("Failed to open archive");
        return -1;
    }

    tar_header header;
    while (fread(&header, sizeof(header), 1, arch) == 1) {
        if (header.name[0] == '\0') break;  // End of archive

        unsigned long size;
        sscanf(header.size, "%lo", &size);

        if (files && files->size > 0 && !file_list_contains(files, header.name)) {
            fseek(arch, ((size + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE, SEEK_CUR);
            continue;
        }

        if (extract_file(arch, &header) != 0) {
            fclose(arch);
            return -1;
        }
    }

    fclose(arch);
    return 0;
}

// Implementations of helper functions
static void compute_checksum(tar_header *header) {
    unsigned char *bytes = (unsigned char *)header;
    unsigned int sum = 0;

    // Set checksum field to spaces before calculation
    memset(header->chksum, ' ', sizeof(header->chksum));

    for (size_t i = 0; i < sizeof(tar_header); i++) {
        sum += bytes[i];
    }

    // Format checksum and write it back to the header
    snprintf(header->chksum, sizeof(header->chksum), "%06o", sum);
}

static int fill_tar_header(tar_header *header, const char *file_name) {
    struct stat stat_buf;
    if (stat(file_name, &stat_buf) != 0) {
        perror("Failed to stat file");
        return -1;
    }

    memset(header, 0, sizeof(tar_header));
    strncpy(header->name, file_name, sizeof(header->name) - 1);
    snprintf(header->mode, sizeof(header->mode), "%07o", stat_buf.st_mode & 07777);
    snprintf(header->uid, sizeof(header->uid), "%07o", stat_buf.st_uid);
    snprintf(header->gid, sizeof(header->gid), "%07o", stat_buf.st_gid);
    snprintf(header->size, sizeof(header->size), "%011o", (unsigned)stat_buf.st_size);
    snprintf(header->mtime, sizeof(header->mtime), "%011o", (unsigned)stat_buf.st_mtime);
    header->typeflag = REGTYPE;
    strncpy(header->magic, "ustar", sizeof(header->magic));
    header->magic[sizeof(header->magic) - 1] = '\0';

    // Fetch and set owner and group names
    struct passwd *pwd = getpwuid(stat_buf.st_uid);
    if (pwd != NULL) {
        strncpy(header->uname, pwd->pw_name, sizeof(header->uname) - 1);
    }
    struct group *grp = getgrgid(stat_buf.st_gid);
    if (grp != NULL) {
        strncpy(header->gname, grp->gr_name, sizeof(header->gname) - 1);
    }

    // Set major and minor device numbers to zero for regular files
    memset(header->devmajor, 0, sizeof(header->devmajor));
    memset(header->devminor, 0, sizeof(header->devminor));

    compute_checksum(header);
    return 0;
}


static int write_file_to_archive(FILE *arch, const char *file_name) {
    tar_header header;
    if (fill_tar_header(&header, file_name) != 0) {
        return -1;
    }

    // Write the header to the archive
    if (fwrite(&header, sizeof(header), 1, arch) != 1) {
        perror("Failed to write header to archive");
        return -1;
    }

    // Open the file to be archived
    FILE *file = fopen(file_name, "rb");
    if (!file) {
        perror("Failed to open input file");
        return -1;
    }

    char buffer[BLOCK_SIZE] = {0}; // Initialize buffer to zero
    size_t bytes_read;
    size_t total_bytes_written = 0;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (fwrite(buffer, 1, bytes_read, arch) != bytes_read) {
            perror("Failed to write file data to archive");
            fclose(file);
            return -1;
        }
        total_bytes_written += bytes_read;
    }

    // Padding if the file size is not a multiple of BLOCK_SIZE
    size_t remainder = total_bytes_written % BLOCK_SIZE;
    if (remainder != 0) {
        size_t padding_size = BLOCK_SIZE - remainder;
        char padding[padding_size];
        memset(padding, 0, padding_size); // Fill padding with zeros

        if (fwrite(padding, 1, padding_size, arch) != padding_size) {
            perror("Failed to write padding to archive");
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}

static int extract_file(FILE *arch, const tar_header *header) {
    FILE *out = fopen(header->name, "wb");
    if (!out) {
        perror("Failed to open output file");
        return -1;
    }

    unsigned long size;
    sscanf(header->size, "%lo", &size);

    char buffer[BLOCK_SIZE];
    while (size > 0) {
        size_t bytes_to_read = (size < BLOCK_SIZE) ? size : BLOCK_SIZE;
        if (fread(buffer, 1, bytes_to_read, arch) != bytes_to_read) {
            perror("Failed to read from archive");
            fclose(out);
            return -1;
        }
        if (fwrite(buffer, 1, bytes_to_read, out) != bytes_to_read) {
            perror("Failed to write to output file");
            fclose(out);
            return -1;
        }
        size -= bytes_to_read;
    }

    fclose(out);
    return 0;
}


static int add_footer_blocks(FILE *arch) {
    char buffer[BLOCK_SIZE] = {0};
    // Write two empty blocks as footer
    for (int i = 0; i < 2; i++) {
        if (fwrite(buffer, sizeof(buffer), 1, arch) != 1) {
            perror("Failed to write footer to archive");
            return -1;
        }
    }
    return 0;
}

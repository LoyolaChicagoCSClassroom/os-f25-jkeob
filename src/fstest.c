#include "fat.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define SECTOR_SIZE 512

char sector_buf[SECTOR_SIZE];
char rde_region[16384];
int fd = 0;

int read_sector_from_disk_image(unsigned int sector_num, char *buf, unsigned int nsectors) {
    lseek(fd, sector_num * SECTOR_SIZE, SEEK_SET);
    int n = read(fd, buf, SECTOR_SIZE * nsectors);
    if (n <= 0) {
        perror("read_sector_from_disk_image");
        return -1;
    }
    return n;
}

void extract_filename(struct root_directory_entry *rde, char *fname) {
    int k = 0;
    while (((rde->file_name)[k] != ' ') && (k < 8)) {
        fname[k] = (rde->file_name)[k];
        k++;
    }
    fname[k] = '\0';

    if ((rde->file_extension)[0] == ' ')
        return;

    fname[k++] = '.';
    int n = 0;
    while (((rde->file_extension)[n] != ' ') && (n < 3)) {
        fname[k] = (rde->file_extension)[n];
        k++;
        n++;
    }
    fname[k] = '\0';
}

void strcpy_neil(char *dest, char *src) {
    int k = 0;
    while (src[k] != 0) {
        dest[k] = src[k];
        k++;
    }
    dest[k] = 0;
}

int main() {
    struct boot_sector *bs = (struct boot_sector *)sector_buf;
    struct root_directory_entry *rde_tbl = (struct root_directory_entry *)rde_region;

    fd = open("disk.img", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (read_sector_from_disk_image(0, sector_buf, 1) < 0) {
        printf("Failed to read boot sector\n");
        return 1;
    }

    printf("=== FAT Boot Sector Info ===\n");
    printf("Bytes per sector: %d\n", bs->bytes_per_sector);
    printf("Sectors per cluster: %d\n", bs->num_sectors_per_cluster);
    printf("Reserved sectors: %d\n", bs->num_reserved_sectors);
    printf("Num FAT tables: %d\n", bs->num_fat_tables);
    printf("Num RDEs: %d\n", bs->num_root_dir_entries);
    printf("Sectors per FAT: %d\n", bs->num_sectors_per_fat);
    printf("Hidden sectors: %d\n", bs->num_hidden_sectors);
    printf("FS Type: %.8s\n", bs->fs_type);
    printf("Boot Signature: 0x%X\n", bs->boot_signature);
    printf("=============================\n\n");

    if (bs->boot_signature != 0xAA55) {
        printf("Invalid boot signature. Exiting.\n");
        return 1;
    }

    unsigned int root_dir_start =
        bs->num_hidden_sectors +
        bs->num_reserved_sectors +
        bs->num_fat_tables * bs->num_sectors_per_fat;

    printf("Root directory starts at sector %u\n\n", root_dir_start);

    // Read root directory region
    if (read_sector_from_disk_image(root_dir_start, rde_region, 32) < 0) {
        printf("Failed to read root directory\n");
        return 1;
    }

    printf("=== Root Directory Entries ===\n");
    for (int k = 0; k < bs->num_root_dir_entries; k++) {
        struct root_directory_entry *rde = &rde_tbl[k];

        if (rde->file_name[0] == 0x00)
            break;
        if (rde->file_name[0] == 0xE5)
            continue;
        if (rde->attribute == FILE_ATTRIBUTE_SUBDIRECTORY)
            continue;

        char temp_str[16];
        extract_filename(rde, temp_str);

        if (strlen(temp_str) > 0)
            printf("File: %-12s  Size: %u bytes  Cluster: %u\n",
                   temp_str, rde->file_size, rde->cluster);
    }

    printf("===============================\n");
    close(fd);
    return 0;
}

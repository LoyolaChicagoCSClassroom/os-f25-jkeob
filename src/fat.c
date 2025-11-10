#include "fat.h"
#include "rprintf.h"
#include "ide.h"
#include <string.h>
#include <stdint.h>

static unsigned char bootSector[SECTOR_SIZE];
static unsigned char fat_table[8 * SECTOR_SIZE];
static unsigned char rde_region[32 * SECTOR_SIZE];

static struct boot_sector *bs;
static uint16_t current_cluster;
static uint32_t file_size;
static unsigned int root_dir_sector;

int fatInit(void) {
    ata_lba_read(0, bootSector, 1);
    bs = (struct boot_sector *)bootSector;

    if (bs->boot_signature != 0xAA55) {
        rprintf("Invalid boot signature: %x\n", bs->boot_signature);
        return -1;
    }

    if (strncmp(bs->fs_type, "FAT12", 5) != 0) {
        rprintf("Not a FAT12 filesystem: %s\n", bs->fs_type);
        return -1;
    }

    unsigned int fat_start = bs->num_reserved_sectors;
    ata_lba_read(fat_start, fat_table, bs->num_sectors_per_fat);

    root_dir_sector = bs->num_reserved_sectors +
                      bs->num_fat_tables * bs->num_sectors_per_fat;

    rprintf("FAT initialized. root_dir_sector=%d\n", root_dir_sector);
    return 0;
}

static void extract_filename(struct root_directory_entry *rde, char *fname) {
    int k = 0;
    while (rde->file_name[k] != ' ' && k < 8) {
        fname[k] = rde->file_name[k];
        k++;
    }
    fname[k] = '\0';

    if (rde->file_extension[0] != ' ') {
        fname[k++] = '.';
        int n = 0;
        while (rde->file_extension[n] != ' ' && n < 3) {
            fname[k++] = rde->file_extension[n++];
        }
        fname[k] = '\0';
    }
}

int fatOpen(const char *filename) {
    unsigned int dir_sectors = (bs->num_root_dir_entries * 32) / bs->bytes_per_sector;
    ata_lba_read(root_dir_sector, rde_region, dir_sectors);

    struct root_directory_entry *rde_tbl = (struct root_directory_entry *)rde_region;
    int total_entries = bs->num_root_dir_entries;

    for (int i = 0; i < total_entries; i++) {
        if (rde_tbl[i].file_name[0] == 0x00)
            break;
        if (rde_tbl[i].file_name[0] == 0xE5)
            continue;
        if (rde_tbl[i].attribute == FILE_ATTRIBUTE_SUBDIRECTORY)
            continue;

        char temp[16];
        extract_filename(&rde_tbl[i], temp);

        if (strcmp(temp, filename) == 0) {
            current_cluster = rde_tbl[i].cluster;
            file_size = rde_tbl[i].file_size;
            rprintf("File found: %s, cluster=%d, size=%d\n", temp, current_cluster, file_size);
            return 0;
        }
    }

    rprintf("File not found: %s\n", filename);
    return -1;
}

int fatRead(char *buffer) {
    unsigned int data_start = root_dir_sector +
                              (bs->num_root_dir_entries * 32) / bs->bytes_per_sector;
    unsigned int cluster = current_cluster;
    unsigned int bytes_read = 0;

    while (cluster < 0xFF8 && bytes_read < file_size) {
        unsigned int lba = data_start + (cluster - 2) * bs->num_sectors_per_cluster;
	ata_lba_read(lba, (unsigned char *)(buffer + bytes_read), bs->num_sectors_per_cluster);
        bytes_read += bs->num_sectors_per_cluster * bs->bytes_per_sector;

        uint16_t entry = *(uint16_t *)&fat_table[(cluster * 3) / 2];
        cluster = (cluster % 2 == 0) ? (entry & 0x0FFF) : (entry >> 4);
    }

    buffer[file_size] = '\0';
    return 0;
}
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, unsigned int n) {
    while (n-- && *s1 && (*s1 == *s2)) { s1++; s2++; }
    if (n == (unsigned int)-1) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

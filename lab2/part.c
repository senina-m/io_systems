#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif

#ifndef __USE_LARGEFILE64
#define __USE_LARGEFIEL64
#endif

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/queue.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

/* Размер элемента таблицы разделов */
#define PT_SIZE 0x10

/* Флаг загрузочного раздела */
#define BOOTABLE 0x80

#pragma pack(1)

struct systypes {
    u8 part_type;
    u8 * part_name;
};

typedef struct pt_entry {
    u8 bootable;
    u8 start_part[3];
    u8 type;
    u8 end_part[3];
    u32 sect_before;
    u32 sect_total;
    TAILQ_ENTRY(pt_entry) entries;
} pt_entry_t;

/* from fdisk.c */
struct systypes i386_sys_types[] = {
    {0x00, "Пустой раздел"},
    {0x01, "FAT12"},
    {0x04, "FAT16 <32M"},
    {0x05, "Расщиренный раздел DOS"}, /* DOS 3.3+ extended partition */
    {0x06, "FAT16"}, /* DOS 16-bit >=32M */
    {0x0b, "FAT32"},
    {0x0c, "FAT32 (LBA)"}, /* LBA really is `Extended Int 13h' */
    {0x0e, "FAT16 (LBA)"},
    {0x0f, "Расширенный раздел Windows"},
    {0x82, "раздел подкачки Linux (swap)"}, /* also Solaris */
    {0x83, "Linux"},
    {0x85, "Расширенный раздел Linux"},
    {0x07, "HPFS/NTFS"}
};

#define PART_NUM (sizeof(i386_sys_types)/sizeof(i386_sys_types[0]))

int read_main_ptable(int, u64 *);
int read_ext_ptable(int, u64);
int get_pt_info(int);
void info();

TAILQ_HEAD(,pt_entry) head;

/* Считываем основную таблицу разделов из MBR */
int read_main_ptable(int dev, u64 *seek)
{
    int i = 0;
    u8 mbr[512];
    pt_entry_t *part = NULL;

    if(read(dev, mbr, 512) < 0)
        return -1;

    if(*(u16 *)(mbr + 0x1FE) != 0xAA55)
        return -1;

    for(;i < 4; i++) {

        part = (pt_entry_t *)calloc(1, sizeof(pt_entry_t));
        if(!part)
            return -1;

        memcpy((void *)part, mbr + 0x1BE + PT_SIZE * i, PT_SIZE);

    /* Если присутствует расширенный раздел, запоминаем его номер */
        if(part->type == 0x0F ||
            part->type == 0x05 ||
            part->type == 0x0C)
                *seek = (u64)(part->sect_before) * 512;

        TAILQ_INSERT_TAIL(&head, part, entries);
    }

    return 0;
}

int read_ext_ptable(int dev, u64 seek)
{
    u8 smbr[512];
    pt_entry_t *part = NULL, tmp;

/* seek - смещение к расширенному разделу от начала диска (в байтах) */
    for(;;) {

        part = (pt_entry_t *)calloc(1, sizeof(pt_entry_t));
        if(!part)
            return -1;

        memset(smbr, 0, sizeof(smbr));

        lseek(dev, seek, SEEK_SET);
        if(read(dev, smbr, sizeof(smbr)) < 0)
            return -1;

        memcpy((void *)part, smbr + 0x1BE, PT_SIZE);

        part->sect_before += (u32)(seek/512);

        TAILQ_INSERT_TAIL(&head, part, entries);

        memset(&tmp, 0, PT_SIZE);
        memcpy((void *)&tmp, smbr + 0x1BE + PT_SIZE, PT_SIZE);

        if(!tmp.type)
            return 0;

        /* Вычисляем смещение к следующему SMBR */
        seek = (u64)(part->sect_before + part->sect_total) * 512;
    }
}

int get_pt_info(int dev)
{
    u64 seek = 0;

    TAILQ_INIT(&head);

    if(read_main_ptable(dev, &seek) < 0)
        return -1;

    if(seek)
        read_ext_ptable(dev, seek);

    return 0;
}

/* Вывести информацию */
void info()
{
    int i = 1, n = 0;
    pt_entry_t *part;

    for(part = (&head)->tqh_first;(part);part = part->entries.tqe_next) {

        fprintf(stderr,"%d\t", i++);
        fprintf(stderr,"%8u\t", part->sect_before);
        fprintf(stderr,"%8u\t", part->sect_total + part->sect_before - 1);
        fprintf(stderr,"%x\t", part->type);

        for(n = 0; n < PART_NUM; n++) {
                if(part->type == i386_sys_types[n].part_type) {
                fprintf(stderr, "%s\n", i386_sys_types[n].part_name);
                break;
            }
        }

        if(n == PART_NUM)
            fprintf(stderr,"неизвестный тип\n");
    }
}

int main(int argc, char **argv)
{
    int dev;

    if(!argv[1])
        return 0;

    dev = open(argv[1], O_RDONLY);
    if(dev < 0) {
        perror("open");
        return -1;
    }

    if(get_pt_info(dev) < 0)
        return -1;

    info();

    return 0;
}
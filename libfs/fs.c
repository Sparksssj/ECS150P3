#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

typedef struct Rootdirentry* Rootdirentry_t;
typedef struct FAT* FAT_t;
typedef struct Superblock* Superblock_t;


struct Superblock{
    uint64_t signiture;
    uint16_t totalblocks;
    uint16_t rootdirindex;
    uint16_t datablockindex;
    uint16_t datablocknum;
    uint8_t fatblocknum;
    uint8_t padding[4079];
};

struct FAT{
    uint16_t FAT[2048];
};

struct Rootdirentry{
    //uint64_t filename[2];
    char filename[16];
    uint32_t sizeoffile;
    uint16_t indexoffirstblock;
    uint8_t padding[5];
};




Superblock_t Superblock;
FAT_t* FATS;
Rootdirentry_t Rootdirentries;

int fs_mount(const char *diskname)
{
    if (!diskname) return -1;

    block_disk_open(diskname);
    Superblock = malloc(sizeof *Superblock);

    block_read(0, Superblock);

    FATS = malloc(sizeof(FAT_t) * Superblock->fatblocknum);

    FAT_t temp = malloc(sizeof *temp);
    for (int i = 0; i < Superblock->fatblocknum; ++i) {
        block_read(i+1, temp);
        FATS[i] = temp;
    }

    Rootdirentries = malloc(sizeof(Rootdirentry_t)*128);

    block_read(Superblock->fatblocknum + 1, Rootdirentries);

    return 0;
}

int fs_umount(void)
{
    for (int i = 0; i < Superblock->fatblocknum; ++i) {
        block_write(i+1, FATS[i]);
    }

    block_write(Superblock->fatblocknum + 1, Rootdirentries);

    block_disk_close();
    return 0;
}

int fs_info(void)
{
    fprintf(stderr, "FS Info:\n");
    fprintf(stderr, "total_blk_count=%d\n", Superblock->totalblocks);
    fprintf(stderr, "fat_blk_count=%d\n", Superblock->fatblocknum);
    fprintf(stderr, "rdir_blk=%d\n", Superblock->rootdirindex);
    fprintf(stderr, "data_blk=%d\n", Superblock->datablockindex);
    fprintf(stderr, "data_blk_count=%d\n", Superblock->datablocknum);
    fprintf(stderr, "fat_blk_count=%d\n", Superblock->fatblocknum);


//    printf("FS Info:\n");
//    printf("total_blk_count=%d\n", Superblock->totalblocks);
//    printf("fat_blk_count=%d\n", Superblock->fatblocknum);
//    printf("rdir_blk=%d\n", Superblock->rootdirindex);
//    printf("data_blk=%d\n", Superblock->datablockindex);
//    printf("data_blk_count=%d\n", Superblock->datablocknum);
//    printf("fat_blk_count=%d\n", Superblock->fatblocknum);

    return 0;

}



int fs_create(const char *filename)
{
    if (!filename) return -1;

    for (int i = 0; i < 128; ++i) {
        if (Rootdirentries[i].filename[0] == 0 && Rootdirentries[i].filename[1] == 0){

            if (strlen(filename) > 16){
                return -1;
            } else {

                strncpy(Rootdirentries[i].filename, filename, 16);
                Rootdirentries[i].sizeoffile = 0;
                Rootdirentries[i].indexoffirstblock = 0xFFFF;
                break;
            }
        }
    }
    return 0;
}


int fs_delete(const char *filename)
{
    if (!filename) return -1;
    for (int i = 0; i < 128; ++i) {
        if (!strcmp(Rootdirentries[i].filename, filename)){
            strcpy(Rootdirentries[i].filename, "");
            Rootdirentries[i].indexoffirstblock = 0;
            return 0;
        }
    }
    return -1;
}

int fs_ls(void)
{
	fprintf(stderr, "FS Ls:\n");

    for (int i = 0; i < 128; ++i) {
        if (strcmp(Rootdirentries[i].filename, "")){
           fprintf(stderr, "file: %s, size: %d, data_blk: %d\n", Rootdirentries[i].filename, Rootdirentries[i].sizeoffile, Rootdirentries[i].indexoffirstblock);
        }
    }

}

int fs_open(const char *filename)
{

}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

int main(){
    fs_mount("disk1.fs");
    fs_info();
    fs_create("testtest1.c");
    fs_create("testtest2");
    fs_create("test3");
    fs_delete("testtest2");
    fs_ls();
    return 0;
}
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
int openedfile[32] = {-1};
int offsets[32];
int freedatablk;


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

    freedatablk = Superblock->datablocknum -1;

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
    fprintf(stderr, "fat_free_ratio=%d/%d\n", freedatablk ,Superblock->datablocknum);
    int rootdirfree = 128;
    for (int i = 0; i < 128; ++i) {
        if (strcmp(Rootdirentries[i].filename, "")){
            rootdirfree --;
        }
    }
    fprintf(stderr, "rdir_free_ratio=%d/128\n",rootdirfree);

    return 0;

}



int fs_create(const char *filename)
{
    if (!filename) return -1;

    for (int i = 0; i < 128; ++i) {
        if (!strcmp(Rootdirentries[i].filename, filename)){
            return -1;
        }

        if (!strcmp(Rootdirentries[i].filename, "")){
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
    for (int i = 0; i < 32; ++i) {
        if (openedfile[i] == -1){
            for (int j = 0; j < 128; ++j) {
                if (!strcmp(Rootdirentries[j].filename, filename)){
                    openedfile[i] = j;
                    return i;
                }
            }
        }
    }
    return -1;
}

int fs_close(int fd)
{
    if (fd > 32 || !openedfile[fd]){
        return -1;
    } else{
        openedfile[fd] = 0;
    }
    return 0;
}

int fs_stat(int fd)
{
    if (fd > 32 || openedfile[fd] == -1){
        return -1;
    }
    fprintf(stderr, "Size of file '%s' is %d bytes\n",
            Rootdirentries[openedfile[fd]].filename, Rootdirentries[openedfile[fd]].sizeoffile);
    return 0;
}

int fs_lseek(int fd, size_t offset)
{
    if (fd > 32 || !openedfile[fd] || Rootdirentries[openedfile[fd]].sizeoffile < offset){
        return -1;
    } else{
        offsets[fd] = offset;
    }
    return 0;
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
    fs_mount("disk2.fs");
    //fs_info();
    fs_create("testtest1.c");
    fs_create("testtest2");
    fs_create("test3");
    fs_delete("testtest2");
    fs_delete("test3");
    fs_delete("testtest1.c");
    fs_delete("test_fs.c");
    fs_info();

//
//    fs_ls();
//
//    int i5 = fs_open("test3");
//    int i1 = fs_stat(0);
//    //int i2 = fs_stat(1);
//    int i3=fs_close(0);
//    //int i4=fs_close(1);
    fs_umount();
    return 0;
}
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "disk.h"
#include "fs.h"

int findIndex(int fd, int offset);
void extendDataBlock(int index);

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
    uint8_t padding[10];
};




Superblock_t Superblock;
FAT_t* FATS;
Rootdirentry_t Rootdirentries;
int openedfile[32] = {[0 ... 31] = -1};
unsigned int offsets[32];
bool mounted = false;


int fs_mount(const char *diskname)
{
    if (!diskname) return -1;


    if (block_disk_open(diskname))
        return -1;
    Superblock = malloc(sizeof *Superblock);

    block_read(0, Superblock);

    if (Superblock->signiture != 6000536558536704837 || Superblock->totalblocks != block_disk_count()){
        return -1;
    }

    FATS = malloc(sizeof(FAT_t) * Superblock->fatblocknum);

    if (Superblock->fatblocknum == 1){
        FAT_t temp1 = malloc(sizeof *temp1);
        block_read(1, temp1);
        FATS[0] = temp1;

    } else if (Superblock->fatblocknum == 2){
        FAT_t temp1 = malloc(sizeof *temp1);
        FAT_t temp2 = malloc(sizeof *temp2);
        block_read(1, temp1);
        block_read(2, temp2);
        FATS[0] = temp1;
        FATS[1] = temp2;

    } else if (Superblock->fatblocknum == 3){
        FAT_t temp1 = malloc(sizeof *temp1);
        FAT_t temp2 = malloc(sizeof *temp2);
        FAT_t temp3 = malloc(sizeof *temp3);
        block_read(1, temp1);
        block_read(2, temp2);
        block_read(3, temp3);
        FATS[0] = temp1;
        FATS[1] = temp2;
        FATS[2] = temp3;


    } else if (Superblock->fatblocknum == 4) {
        FAT_t temp1 = malloc(sizeof *temp1);
        FAT_t temp2 = malloc(sizeof *temp2);
        FAT_t temp3 = malloc(sizeof *temp3);
        FAT_t temp4 = malloc(sizeof *temp4);
        block_read(1, temp1);
        block_read(2, temp2);
        block_read(3, temp3);
        block_read(4, temp4);
        FATS[0] = temp1;
        FATS[1] = temp2;
        FATS[2] = temp3;
        FATS[3] = temp4;
    }


    FATS[0]->FAT[0] = 0XFFFF;


    Rootdirentries = malloc(sizeof(struct Rootdirentry)*128);

    block_read(Superblock->fatblocknum + 1, Rootdirentries);

    /* free(temp);*/
    mounted = true;

    return 0;
}

int fs_umount(void)
{
    if (!mounted) return -1;
    for (int i = 0; i < Superblock->fatblocknum; ++i) {
        block_write(i+1, FATS[i]);
    }

    block_write(Superblock->fatblocknum + 1, Rootdirentries);

    free(Superblock);
    free(FATS);
    free(Rootdirentries);

    for (int i = 0; i < 32; ++i) {
        if (openedfile[i] != -1) return -1;
    }

    if (block_disk_close() == -1) return -1;
    mounted = false;
    return 0;
}

int fs_info(void)
{
    if (!mounted) return -1;

    int freedatablk_h = 0;
    for (int i = 0; i < Superblock->datablocknum; i++){
        if (!(FATS[i/2048]->FAT[i%2048])){
            freedatablk_h++;
        }
    }
    printf("FS Info:\n");
    printf("total_blk_count=%d\n", Superblock->totalblocks);
    printf("fat_blk_count=%d\n", Superblock->fatblocknum);
    printf("rdir_blk=%d\n", Superblock->rootdirindex);
    printf("data_blk=%d\n", Superblock->datablockindex);
    printf("data_blk_count=%d\n", Superblock->datablocknum);
    printf("fat_free_ratio=%d/%d\n", freedatablk_h ,Superblock->datablocknum);
    int rootdirfree = 128;
    for (int i = 0; i < 128; ++i) {
        if (strcmp(Rootdirentries[i].filename, "")){
            rootdirfree --;
        }
    }
    printf("rdir_free_ratio=%d/128\n",rootdirfree);

    return 0;

}



int fs_create(const char *filename)
{
    if (!filename || !mounted) return -1;

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
    if (!filename || !mounted || strlen(filename) > 16) return -1;
    for (int i = 0; i < 128; ++i) {
        if (!strcmp(Rootdirentries[i].filename, filename)){
            for (int j = 0; j < 32; j++){
                if (openedfile[j] == i){
                    return -1;
                }
            }
            uint16_t currentfat = Rootdirentries->indexoffirstblock;
            while (currentfat != 0xFFFF){
                uint16_t temp;
                temp = FATS[currentfat/2048]->FAT[currentfat%2048];
                FATS[currentfat/2048]->FAT[currentfat%2048] = 0;

                currentfat = temp;
            }
            //block_write(Rootdirentries[i].indexoffirstblock+Superblock->datablockindex, bounce);
            FATS[(Rootdirentries[i].indexoffirstblock)/2048]->FAT[(Rootdirentries[i].indexoffirstblock)%2048] = 0;
            strcpy(Rootdirentries[i].filename, "");
            Rootdirentries[i].indexoffirstblock = 0;
            return 0;
        }
    }
    return -1;
}

int fs_ls(void)
{
    if (!mounted){
        return -1;
    }
    printf("FS Ls:\n");

    for (int i = 0; i < 128; ++i) {
        if (strcmp(Rootdirentries[i].filename, "")){
            printf("file: %s, size: %d, data_blk: %d\n", Rootdirentries[i].filename, Rootdirentries[i].sizeoffile, Rootdirentries[i].indexoffirstblock);
        }
    }
    return 0;
}

int fs_open(const char *filename)
{
    if (!mounted || strlen(filename) > 16 || !filename){
        return -1;
    }
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
    if (fd > 31 || fd < 0 || openedfile[fd] == -1 || !mounted){
        return -1;
    } else{
        openedfile[fd] = -1;
        offsets[fd] = 0;
    }
    return 0;
}

int fs_stat(int fd)
{
    if (fd > 31 || fd < 0 || openedfile[fd] == -1 || !mounted){
        return -1;
    }
    printf("Size of file '%s' is %d bytes\n",
           Rootdirentries[openedfile[fd]].filename, Rootdirentries[openedfile[fd]].sizeoffile);
    return Rootdirentries[openedfile[fd]].sizeoffile;
}

int fs_lseek(int fd, size_t offset)
{
    if (fd > 31 || fd < 0 || openedfile[fd] == -1 || Rootdirentries[openedfile[fd]].sizeoffile < offset || !mounted){
        return -1;
    } else{
        offsets[fd] = offset;
    }
    return 0;
}

int findIndex (int fd, int offset){
    int index = 0;
    if (Rootdirentries[fd].indexoffirstblock != 0xFFFF){
        if (offset < 4096){
            index = Rootdirentries[fd].indexoffirstblock;
        } else {
            index = Rootdirentries[fd].indexoffirstblock;
            while (offset >= 4096) {
                index = FATS[index/2048]->FAT[index%2048];
                offset -= 4096;
            }
        }
    } else {
        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 2048; j++){
                if (!i && !j) continue;
                if ((!FATS[i]->FAT[j])){
                    FATS[i]->FAT[j] = 0xFFFF;
                    index = i*2048+j;
                    Rootdirentries[fd].indexoffirstblock = index;
                    return index;
                }
            }
        }
    }
    return index;
}

void extendDataBlock(int index){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 2048; j++){
            if (!i && !j) continue;
            if (!(FATS[i]->FAT[j])){
                FATS[i]->FAT[j] = 0xFFFF;
                FATS[index/2048]->FAT[index%2048] = i*2048+j;
                return;
            }
        }
    }
}

int fs_write(int fd, void *buf, size_t count)
{
    if (!buf || openedfile[fd] == -1 || fd > 31 || fd < 0 || !mounted){
        return -1;
    }
    if (!count){
        return count;
    }
    unsigned int freedatablk = 0;
    for (int i = 0; i < Superblock->datablocknum; i++){
        if (!(FATS[i/2048]->FAT[i%2048])){
            freedatablk++;
        }
    }
    bool offset_aligned_on_beginning = false;
    bool exactly_size_of_data = false;
    char bounce[4096];
    if (Rootdirentries[fd].indexoffirstblock == 0xFFFF){
        if (freedatablk * 4096 < count){
            count = freedatablk*4096;
        }
    } else {
        unsigned int minus = Rootdirentries[openedfile[fd]].sizeoffile%4096;
        if (Rootdirentries[openedfile[fd]].sizeoffile%4096 == 0){
            minus = 4096;
        }
        if (freedatablk * 4096 + Rootdirentries[openedfile[fd]].sizeoffile - offsets[fd] + 4096 - minus < count) {
            count = freedatablk * 4096 + Rootdirentries[openedfile[fd]].sizeoffile - offsets[fd] + 4096 - minus;
        }
    }
    if (!(offsets[fd]%4096)) offset_aligned_on_beginning = true;
    if (!((offsets[fd]+count)%4096)) exactly_size_of_data = true;
    int countcopy = count;
    int index = findIndex(openedfile[fd], offsets[fd]);
    if (offset_aligned_on_beginning){
        if (exactly_size_of_data){
            while (countcopy){
                block_write(index+Superblock->datablockindex, buf+count-countcopy);
                if ((FATS[index/2048]->FAT[index%2048] == 0xFFFF) && (countcopy>4096)){
                    extendDataBlock(index);
                }
                index = FATS[index/2048]->FAT[index%2048];
                countcopy-=4096;
            }
        } else {
            while (countcopy > 4096){
                block_write(index+Superblock->datablockindex, buf+count-countcopy);
                if ((FATS[index/2048]->FAT[index%2048] == 0xFFFF) && (countcopy>4096)){
                    extendDataBlock(index);
                }
                index = FATS[index/2048]->FAT[index%2048];
                countcopy-=4096;
            }
            block_read(index+Superblock->datablockindex, bounce);
            memcpy(bounce, buf+count-countcopy, countcopy);
            block_write(index+Superblock->datablockindex, bounce);
        }
    } else {
        if (offsets[fd]%4096+count<=4096){
            block_read(index+Superblock->datablockindex, bounce);
            memcpy(bounce+offsets[fd]%4096, buf, countcopy);
            block_write(index+Superblock->datablockindex, bounce);
        } else {
            block_read(index+Superblock->datablockindex, bounce);
            memcpy(bounce+offsets[fd]%4096, buf, 4096-offsets[fd]%4096);
            block_write(index+Superblock->datablockindex, bounce);
            if (FATS[index/2048]->FAT[index%2048] == 0xFFFF){
                extendDataBlock(index);
            }
            index = FATS[index/2048]->FAT[index%2048];
            countcopy-=4096-offsets[fd]%4096;
            while (countcopy > 4096){
                block_write(index+Superblock->datablockindex, buf+count-countcopy);
                if ((FATS[index/2048]->FAT[index%2048] == 0xFFFF) && (countcopy>4096)){
                    extendDataBlock(index);
                }
                index = FATS[index/2048]->FAT[index%2048];
                countcopy-=4096;
            }
            block_read(index+Superblock->datablockindex, bounce);
            memcpy(bounce, buf+count-countcopy, countcopy);
            block_write(index+Superblock->datablockindex, bounce);
        }
    }
    offsets[fd]+=count;
    if (Rootdirentries[openedfile[fd]].sizeoffile < offsets[fd]){
        Rootdirentries[openedfile[fd]].sizeoffile = offsets[fd];
    }
    return count;
}

int fs_read(int fd, void *buf, size_t count)
{
    if (!buf || openedfile[fd] == -1 || fd > 31 || fd < 0 || !mounted){
        return -1;
    }
    if (!count){
        return count;
    }
    bool offset_aligned_on_beginning = false;
    bool exactly_size_of_data = false;
    char bounce[4096];
    if (Rootdirentries[openedfile[fd]].sizeoffile - offsets[fd] < count){
        count = Rootdirentries[openedfile[fd]].sizeoffile - offsets[fd];
    }
    int countcopy = count;
    int index = findIndex(openedfile[fd], offsets[fd]);
    if (!(offsets[fd]%4096)) offset_aligned_on_beginning = true;
    if (!((offsets[fd]+count)%4096)) exactly_size_of_data = true;
    if (offset_aligned_on_beginning){
        if (exactly_size_of_data){
            while (countcopy){
                block_read(index+Superblock->datablockindex, buf+count-countcopy);
                index = FATS[index/2048]->FAT[index%2048];
                countcopy-=4096;
            }
        } else {
            while (countcopy > 4096){
                block_read(index+Superblock->datablockindex, buf+count-countcopy);
                index = FATS[index/2048]->FAT[index%2048];
                countcopy-=4096;
            }
            block_read(index+Superblock->datablockindex, bounce);
            memcpy(buf+count-countcopy, bounce, countcopy);
        }
    } else {
        if (offsets[fd]%4096+count<=4096){
            block_read(index+Superblock->datablockindex, bounce);
            memcpy(buf, bounce+offsets[fd]%4096, countcopy);
        } else {
            block_read(index+Superblock->datablockindex, bounce);
            index = FATS[index/2048]->FAT[index%2048];
            memcpy(buf, bounce+offsets[fd]%4096, 4096-offsets[fd]%4096);
            countcopy-=4096-offsets[fd]%4096;
            while (countcopy > 4096){
                block_read(index+Superblock->datablockindex, buf+count-countcopy);
                index = FATS[index/2048]->FAT[index%2048];
                countcopy-=4096;
            }
            block_read(index+Superblock->datablockindex, bounce);
            memcpy(buf+count-countcopy, bounce, countcopy);
        }
    }
    offsets[fd]+=count;
    return count;
}
//
//int main(){
//    int fd_fs;
//    struct stat st;
//    fd_fs = open("test_fs.c", O_RDONLY);
//    stat("test_fs.c", &st);
//    char* buf = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd_fs, 0);
//    fs_mount("disk.fs");
//    fs_create("test_fs");
//    int fd = fs_open("test_fs");
//    fs_write(fd, buf, st.st_size);
//    fs_close(fd);
//    fs_delete("test_fs");
//    fs_umount();
//}
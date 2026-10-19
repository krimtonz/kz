/*
* sd.c
*
* sd card access for kz-lite on n64 flashcarts (64drive, everdrive64,
* summercart64), using libcart for the cartridge and FatFs for the filesystem.
* implements the posix-like api in sys.h, which kz-full and kz-vc implement
* with homeboy instead (sys.c).
*/

#ifdef LITE
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "z2.h"
#include "mem.h"
#include "cache.h"
#include "libcart_ultra.h"
#include "cart.h"
#define DIR FF_DIR
#include "ff.h"
#include "diskio.h"
#undef DIR
#include "sys.h"

/*
* the pi is shared with the game's pi manager thread, which loads data from
* rom.  libcart takes libultra's pi access lock around every card operation,
* and kz finds the lock functions inside osEPiReadIo, which is
*
*   __osPiGetAccess(); __osEPiRawReadIo(...); __osPiRelAccess();
*
* and checks that they really call osRecvMesg and osSendMesg on the lock queue.
*/
static void (*pi_get_access)(void);
static void (*pi_rel_access)(void);

#define MIPS_OP_JAL     3
#define MIPS_JR_RA      0x03E00008

static void *jal_target(uint32_t op) {
    return (void*)(0x80000000 | ((op & 0x03FFFFFF) << 2));
}

static int function_calls(void *fn, void *target) {
    uint32_t *p = fn;
    for(int i = 0; i < 32; i++) {
        if((p[i] >> 26) == MIPS_OP_JAL && jal_target(p[i]) == target) {
            return 1;
        }
        if(p[i] == MIPS_JR_RA) {
            break;
        }
    }
    return 0;
}

static int pi_access_init(void) {
    if(pi_get_access != NULL) {
        return 1;
    }

    uint32_t *p = (uint32_t*)osEPiReadIo;
    void *calls[3];
    int n = 0;
    for(int i = 0; i < 32 && n < 3; i++) {
        if((p[i] >> 26) == MIPS_OP_JAL) {
            calls[n++] = jal_target(p[i]);
        }
        if(p[i] == MIPS_JR_RA) {
            break;
        }
    }

    if(n < 3 || !function_calls(calls[0], osRecvMesg) || !function_calls(calls[2], osSendMesg)) {
        return 0;
    }

    pi_get_access = calls[0];
    pi_rel_access = calls[2];
    return 1;
}

void sd_pi_get_access(void) {
    pi_get_access();
}

void sd_pi_rel_access(void) {
    pi_rel_access();
}

/*
* pi dma for libcart.  while kz holds the pi access lock the game's pi manager
* is idle, so the dma is polled with interrupts disabled, and the pi interrupt
* is cleared before interrupts are enabled again.  this way the pi manager never
* receives a completion event for a dma it did not start.
*/
static uint32_t int_disable(void) {
    uint32_t sr;
    __asm__ volatile("mfc0 %0, $12" : "=r"(sr));
    __asm__ volatile("mtc0 %0, $12; nop; nop" :: "r"(sr & ~1));
    return sr;
}

static void int_restore(uint32_t sr) {
    __asm__ volatile("mtc0 %0, $12; nop; nop" :: "r"(sr));
}

static void pi_wait(void) {
    while(IO_READ(PI_STATUS_REG) & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY));
}

/*
* the cache maintenance happens with interrupts disabled too, so no other thread
* can dirty a cache line shared with the edge of the buffer during the dma.
*/
static void pi_dma(uint32_t len_reg, void *dram, u32 cart, u32 size) {
    uint32_t sr = int_disable();
    osWritebackDCache(dram, size);
    if(len_reg == PI_WR_LEN_REG) {
        osInvalDCache(dram, size);
    }
    pi_wait();
    IO_WRITE(PI_DRAM_ADDR_REG, (uint32_t)dram & 0x1FFFFFFF);
    IO_WRITE(PI_CART_ADDR_REG, cart);
    IO_WRITE(len_reg, size - 1);
    pi_wait();
    IO_WRITE(PI_STATUS_REG, PI_STATUS_CLR_INTR);
    int_restore(sr);
}

void sd_pi_dma_rd(void *dram, u32 cart, u32 size) {
    pi_dma(PI_WR_LEN_REG, dram, cart, size);
}

void sd_pi_dma_wr(const void *dram, u32 cart, u32 size) {
    pi_dma(PI_RD_LEN_REG, (void*)dram, cart, size);
}

/* FatFs disk interface */
static DSTATUS card_status = STA_NOINIT;

DSTATUS disk_status(BYTE pdrv) {
    return pdrv == 0 ? card_status : STA_NOINIT;
}

DSTATUS disk_initialize(BYTE pdrv) {
    if(pdrv != 0) {
        return STA_NOINIT;
    }
    card_status = cart_card_init() ? STA_NOINIT : 0;
    return card_status;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
    if(pdrv != 0) {
        return RES_PARERR;
    }
    return cart_card_rd_dram(buff, sector, count) ? RES_ERROR : RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
    if(pdrv != 0) {
        return RES_PARERR;
    }
    return cart_card_wr_dram(buff, sector, count) ? RES_ERROR : RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    if(pdrv != 0) {
        return RES_PARERR;
    }
    return cmd == CTRL_SYNC ? RES_OK : RES_PARERR;
}

/* sys.h implementation */
#define SD_FILE_MAX 2

static FATFS   *sd_fs;
static FIL     *sd_files[SD_FILE_MAX];
static _Bool    sd_cart_ready;

static int fresult_errno(FRESULT res) {
    switch(res) {
        case FR_OK:                 return 0;
        case FR_NOT_READY:
        case FR_DISK_ERR:           return ENODEV;
        case FR_NO_FILESYSTEM:      return ENOENT;
        case FR_NO_FILE:
        case FR_NO_PATH:
        case FR_INVALID_NAME:       return ENOENT;
        case FR_EXIST:              return EEXIST;
        case FR_DENIED:
        case FR_WRITE_PROTECTED:    return EACCES;
        case FR_NOT_ENOUGH_CORE:    return ENOMEM;
        case FR_TOO_MANY_OPEN_FILES:return EMFILE;
        default:                    return EIO;
    }
}

static int fail(FRESULT res) {
    errno = fresult_errno(res);
    return -1;
}

/* detect the flashcart and mount the card the first time it is used */
static int sd_mount(void) {
    if(sd_fs != NULL) {
        return 0;
    }

    if(!pi_access_init()) {
        errno = ENODEV;
        return -1;
    }

    if(!sd_cart_ready) {
        if(cart_init() != 0) {
            errno = ENODEV;
            return -1;
        }
        sd_cart_ready = 1;
    }

    FATFS *fs = malloc(sizeof(*fs));
    if(fs == NULL) {
        errno = ENOMEM;
        return -1;
    }

    FRESULT res = f_mount(fs, "", 1);
    if(res != FR_OK) {
        f_mount(NULL, "", 0);
        free(fs);
        return fail(res);
    }

    sd_fs = fs;
    return 0;
}

int reset_disk(void) {
    for(int i = 0; i < SD_FILE_MAX; i++) {
        if(sd_files[i] != NULL) {
            f_close(sd_files[i]);
            free(sd_files[i]);
            sd_files[i] = NULL;
        }
    }
    if(sd_fs != NULL) {
        f_mount(NULL, "", 0);
        free(sd_fs);
        sd_fs = NULL;
    }
    card_status = STA_NOINIT;
    return sd_mount();
}

static FIL *sd_file(int file) {
    if(file < 0 || file >= SD_FILE_MAX || sd_files[file] == NULL) {
        errno = EBADF;
        return NULL;
    }
    return sd_files[file];
}

int open(const char *path, int open_flags, ...) {
    if(sd_mount()) {
        return -1;
    }

    int fd;
    for(fd = 0; fd < SD_FILE_MAX && sd_files[fd] != NULL; fd++);
    if(fd == SD_FILE_MAX) {
        errno = EMFILE;
        return -1;
    }

    BYTE mode;
    switch(open_flags & O_ACCMODE) {
        case O_WRONLY:  mode = FA_WRITE; break;
        case O_RDWR:    mode = FA_READ | FA_WRITE; break;
        default:        mode = FA_READ; break;
    }
    if(open_flags & O_CREAT) {
        if(open_flags & O_EXCL) {
            mode |= FA_CREATE_NEW;
        } else if(open_flags & O_TRUNC) {
            mode |= FA_CREATE_ALWAYS;
        } else {
            mode |= FA_OPEN_ALWAYS;
        }
    }

    FIL *fil = malloc(sizeof(*fil));
    if(fil == NULL) {
        errno = ENOMEM;
        return -1;
    }

    FRESULT res = f_open(fil, path, mode);
    if(res == FR_OK && (open_flags & O_TRUNC) && !(open_flags & O_CREAT)) {
        res = f_truncate(fil);
    }
    if(res != FR_OK) {
        free(fil);
        return fail(res);
    }

    if(open_flags & O_APPEND) {
        f_lseek(fil, f_size(fil));
    }

    sd_files[fd] = fil;
    return fd;
}

int creat(const char *path, mode_t mode) {
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

int close(int file) {
    FIL *fil = sd_file(file);
    if(fil == NULL) {
        return -1;
    }
    FRESULT res = f_close(fil);
    free(fil);
    sd_files[file] = NULL;
    return res == FR_OK ? 0 : fail(res);
}

int read(int file, void *buf, uint32_t byte_cnt) {
    FIL *fil = sd_file(file);
    if(fil == NULL) {
        return -1;
    }
    UINT n;
    FRESULT res = f_read(fil, buf, byte_cnt, &n);
    return res == FR_OK ? (int)n : fail(res);
}

int write(int file, void *buf, uint32_t byte_cnt) {
    FIL *fil = sd_file(file);
    if(fil == NULL) {
        return -1;
    }
    UINT n;
    FRESULT res = f_write(fil, buf, byte_cnt, &n);
    if(res == FR_OK && n < byte_cnt) {
        errno = ENOSPC;
        return -1;
    }
    return res == FR_OK ? (int)n : fail(res);
}

static mode_t fattrib_mode(BYTE attrib) {
    mode_t mode = S_IRUSR | S_IRGRP | S_IROTH;
    if(!(attrib & AM_RDO)) {
        mode |= S_IWUSR;
    }
    return mode | ((attrib & AM_DIR) ? S_IFDIR : S_IFREG);
}

int stat(const char *path, struct stat *buf) {
    if(sd_mount()) {
        return -1;
    }
    FILINFO info;
    FRESULT res = f_stat(path, &info);
    if(res != FR_OK) {
        return fail(res);
    }
    memset(buf, 0, sizeof(*buf));
    buf->st_size = info.fsize;
    buf->st_mode = fattrib_mode(info.fattrib);
    return 0;
}

int mkdir(const char *path, mode_t mode) {
    if(sd_mount()) {
        return -1;
    }
    FRESULT res = f_mkdir(path);
    return res == FR_OK ? 0 : fail(res);
}

int chdir(const char *path) {
    if(sd_mount()) {
        return -1;
    }
    FRESULT res = f_chdir(path);
    return res == FR_OK ? 0 : fail(res);
}

char *getcwd(char *buf, size_t size) {
    if(sd_mount()) {
        return NULL;
    }
    FRESULT res = f_getcwd(buf, size);
    if(res != FR_OK) {
        fail(res);
        return NULL;
    }
    return buf;
}

struct sd_dir {
    FF_DIR      dir;
    dirent_t    dirent;
};

DIR *opendir(const char *path) {
    if(sd_mount()) {
        return NULL;
    }
    struct sd_dir *d = malloc(sizeof(*d));
    if(d == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    FRESULT res = f_opendir(&d->dir, path);
    if(res != FR_OK) {
        free(d);
        fail(res);
        return NULL;
    }
    return (DIR*)d;
}

dirent_t *readdir(DIR *dir) {
    struct sd_dir *d = (struct sd_dir*)dir;
    FILINFO info;
    FRESULT res = f_readdir(&d->dir, &info);
    if(res != FR_OK) {
        fail(res);
        return NULL;
    }
    if(info.fname[0] == 0) {
        return NULL;
    }
    memset(&d->dirent, 0, sizeof(d->dirent));
    strncpy(d->dirent.dir_name, info.fname, sizeof(d->dirent.dir_name) - 1);
    d->dirent.mode = fattrib_mode(info.fattrib);
    d->dirent.size = info.fsize;
    return &d->dirent;
}

int closedir(DIR *dir) {
    struct sd_dir *d = (struct sd_dir*)dir;
    f_closedir(&d->dir);
    free(d);
    return 0;
}

#endif

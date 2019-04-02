#ifndef _IO_H
#define _IO_H

#include "z2.h"

#define IO_BLOCK_SIZE       128
#define FLASH_ADDR          0xA8000000
#define KZ_START_BLOCK      900
#define KZ_MAX_SIZE         (0x40000 - (BLOCK_SIZE * KZ_START_BLOCK))

#define IO_FLASH_CMD_WRITE  0xB4
#define IO_FLASH_CMD_READ   0xF0
#define IO_FLASH_CMD_EXE    0xD2
#define IO_FLASH_CMD_BLOCK  0xA5

#define ioCmdWrite(x)        z2_setFlashStatus(x,(FLASH_ADDR | 0x00010000),(IO_FLASH_CMD_WRITE << 24))
#define ioCmdRead(x)         z2_setFlashStatus(x,(FLASH_ADDR),(IO_FLASH_CMD_READ << 24))
#define ioCmdSetBlock(x,y)   z2_setFlashStatus(x,FLASH_ADDR,((IO_FLASH_CMD_BLOCK << 24) | y))
#define ioCmdExe(x)          z2_setFlashStatus(x,(FLASH_ADDR | 0x00010000),(IO_FLASH_CMD_EXE << 24))

void kz_io(void *dram, uint16_t block, size_t size, uint8_t direction);

#endif
#ifndef _IO_H
#define _IO_H

#include "z2.h"

#define IO_BLOCK_SIZE       128
#define FLASH_ADDR          0xA8010000
#define KZ_START_BLOCK      900
#define KZ_MAX_SIZE         (0x40000 - (BLOCK_SIZE * KZ_START_BLOCK))

#define IO_FLASH_CMD_WRITE  0xB4
#define IO_FLASH_CMD_READ   0xF0
#define IO_FLASH_CMD_EXE    0xD2
#define IO_FLASH_CMD_BLOCK  0xA5
#define IO_FLASH_CMD_BLOCK2 0x4B

#define ioCmdWrite()         osEPiWriteIo(&z2_pi_io_handle,FLASH_ADDR | 0x00010000,(IO_FLASH_CMD_WRITE << 24))
#define ioCmdRead()          osEPiWriteIo(&z2_pi_io_handle,FLASH_ADDR | 0x00010000,(IO_FLASH_CMD_READ << 24))
#define ioCmdSetBlock(x)     osEPiWriteIo(&z2_pi_io_handle,FLASH_ADDR | 0x00010000,((IO_FLASH_CMD_BLOCK << 24) | (x)))
#define ioCmdExe()           osEPiWriteIo(&z2_pi_io_handle,FLASH_ADDR | 0x00010000,(IO_FLASH_CMD_EXE << 24))
#define ioCmdSetBlock2(x)    osEPiWriteIo(&z2_pi_io_handle,FLASH_ADDR | 0x00010000,((IO_FLASH_CMD_BLOCK2 << 24) | (x)))

void kz_io(void *dram, uint16_t block, size_t size, uint8_t direction);

#endif
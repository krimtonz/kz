/*
* libcart_ultra.h
*
* the parts of libultra that libcart (src/libcart.c) needs, provided by kz.
*
* libcart normally takes the pi access lock with __osPiGetAccess and waits on
* libultra's pi event queue for dma completion.  kz does not know the address
* of those in every version of the game, so it finds the access lock functions
* at runtime (see sd.c), and polls for dma completion with interrupts disabled
* so that no pi event reaches the game's pi manager.
*/

#ifndef _LIBCART_ULTRA_H
#define _LIBCART_ULTRA_H

#include <stdint.h>

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;

#define PHYS_TO_K1(x)           ((u32)(x) | 0xA0000000)
#define IO_READ(addr)           (*(volatile u32*)PHYS_TO_K1(addr))
#define IO_WRITE(addr, data)    (*(volatile u32*)PHYS_TO_K1(addr) = (u32)(data))

#define MI_BASE_REG             0x04300000
#define MI_VERSION_REG          (MI_BASE_REG + 0x04)
#define PI_BASE_REG             0x04600000
#define PI_DRAM_ADDR_REG        (PI_BASE_REG + 0x00)
#define PI_CART_ADDR_REG        (PI_BASE_REG + 0x04)
#define PI_RD_LEN_REG           (PI_BASE_REG + 0x08)
#define PI_WR_LEN_REG           (PI_BASE_REG + 0x0C)
#define PI_STATUS_REG           (PI_BASE_REG + 0x10)
#define PI_BSD_DOM1_LAT_REG     (PI_BASE_REG + 0x14)
#define PI_BSD_DOM1_PWD_REG     (PI_BASE_REG + 0x18)
#define PI_BSD_DOM1_PGS_REG     (PI_BASE_REG + 0x1C)
#define PI_BSD_DOM1_RLS_REG     (PI_BASE_REG + 0x20)
#define PI_BSD_DOM2_LAT_REG     (PI_BASE_REG + 0x24)
#define PI_BSD_DOM2_PWD_REG     (PI_BASE_REG + 0x28)
#define PI_BSD_DOM2_PGS_REG     (PI_BASE_REG + 0x2C)
#define PI_BSD_DOM2_RLS_REG     (PI_BASE_REG + 0x30)
#define PI_STATUS_DMA_BUSY      0x01
#define PI_STATUS_IO_BUSY       0x02
#define PI_STATUS_ERROR         0x04
#define PI_STATUS_CLR_INTR      0x02

/* provided by sd.c */
void    sd_pi_get_access    (void);
void    sd_pi_rel_access    (void);
void    sd_pi_dma_rd        (void *dram, u32 cart, u32 size);
void    sd_pi_dma_wr        (const void *dram, u32 cart, u32 size);

#endif

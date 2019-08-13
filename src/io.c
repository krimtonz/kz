#include "io.h"

static void kz_io_read(void *dram, uint16_t block, size_t size, OSIoMesg *iomb, OSMesg *msg, uint32_t *status){
    for(int i=0;i<=size/IO_BLOCK_SIZE;i++){
        iomb->devAddr = ((block * IO_BLOCK_SIZE) + (IO_BLOCK_SIZE * i)) >> 1;
        ioCmdRead();
        osEPiStartDma(&z2_pi_io_handle,iomb,OS_READ);
        osRecvMesg(iomb->hdr.retQueue,msg,OS_MESG_BLOCK);
        iomb->dramAddr+=IO_BLOCK_SIZE;
    }
}

static void kz_io_write(void *dram, uint16_t block, size_t size, OSIoMesg *iomb, OSMesg *msg, uint32_t *status){
    ioCmdRead();
    ioCmdSetBlock2(KZ_START_BLOCK);
    ioCmdExe();
    for(int i=0;i<=size/IO_BLOCK_SIZE;i++){
        ioCmdWrite();
        osEPiStartDma(&z2_pi_io_handle,iomb,OS_WRITE);
        osRecvMesg(iomb->hdr.retQueue,msg,OS_MESG_BLOCK);
        ioCmdSetBlock(block + i);
        ioCmdExe();
        iomb->dramAddr+=IO_BLOCK_SIZE;
    }
}

void kz_io(void *dram, uint16_t block, size_t size, uint8_t direction){
    OSIoMesg ioMesg = {0};
    OSMesgQueue queue;
    OSMesg mesg;
    uint32_t status = 0;
    osCreateMesgQueue(&queue,&mesg,1);
    ioMesg.hdr.pri = OS_MESG_PRI_NORMAL;
    ioMesg.hdr.retQueue = &queue;
    ioMesg.dramAddr = dram;
    ioMesg.size = size;
    ioMesg.devAddr = 0;
    switch(direction){
        case OS_READ:
            kz_io_read(dram,block,size,&ioMesg,&mesg, &status);
            break;
        case OS_WRITE:
            kz_io_write(dram,block,size,&ioMesg,&mesg, &status);
            break;
        default:
            break;
    }
}
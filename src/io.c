#include "io.h"

static void kz_io_read(void *dram, uint16_t block, size_t size, OSIoMesg *iomb, OSMesg *msg){
    for(int i=0;i<=size/IO_BLOCK_SIZE;i++){
        iomb->devAddr = block + IO_BLOCK_SIZE * i;
        ioCmdRead(&z2_pi_io_handle);
        osEPiStartDma(&z2_pi_io_handle,iomb,OS_READ);
        z2_osRecvMessage(iomb->hdr.retQueue,msg,OS_MESG_BLOCK);
        iomb->dramAddr+=IO_BLOCK_SIZE;
    }
}

static void kz_io_write(void *dram, uint16_t block, size_t size, OSIoMesg *iomb, OSMesg *msg){
    ioCmdRead(&z2_pi_io_handle);
    ioCmdSetBlock2(&z2_pi_io_handle,KZ_START_BLOCK);
    ioCmdExe(&z2_pi_io_handle);
    ioCmdNop(&z2_pi_io_handle);
    for(int i=0;i<=size/IO_BLOCK_SIZE;i++){
        ioCmdWrite(&z2_pi_io_handle);
        osEPiStartDma(&z2_pi_io_handle,iomb,OS_WRITE);
        z2_osRecvMessage(iomb->hdr.retQueue,msg,OS_MESG_BLOCK);
        ioCmdSetBlock(&z2_pi_io_handle,(block + i));
        ioCmdExe(&z2_pi_io_handle);
        ioCmdNop(&z2_pi_io_handle);
        iomb->dramAddr+=IO_BLOCK_SIZE;
    }
}

void kz_io(void *dram, uint16_t block, size_t size, uint8_t direction){
    OSIoMesg ioMesg;
    OSMesgQueue queue;
    OSMesg mesg;
    osCreateOSMesgQueue(&queue,&mesg,1);
    ioMesg.hdr.pri = OS_MESG_PRI_NORMAL;
    ioMesg.hdr.retQueue = &queue;
    ioMesg.dramAddr = dram;
    ioMesg.size = size;
    switch(direction){
        case OS_READ:
            kz_io_read(dram,block,size,&ioMesg,&mesg);
            break;
        case OS_WRITE:
            kz_io_write(dram,block,size,&ioMesg,&mesg);
            break;
        default:
            break;
    }
}
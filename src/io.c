#include "io.h"

void kz_io(void *dram, uint16_t block, size_t size, uint8_t direction){
    osWritebackDCache(dram,size);
    if(direction == OS_WRITE){
        ioCmdWrite(&z2_pi_io_handle);
    }else{
        ioCmdRead(&z2_pi_io_handle);
        ioCmdSetBlock(&z2_pi_io_handle,block);
    }

    OSIoMesg ioMesg;
    OSMesgQueue queue;
    OSMesg mesg;
    osCreateOSMesgQueue(&queue,&mesg,1);
    ioMesg.hdr.pri = OS_MESG_PRI_NORMAL;
    ioMesg.hdr.retQueue = &queue;
    ioMesg.dramAddr = dram;
    ioMesg.size = size;
    ioMesg.devAddr = (direction==OS_WRITE)?0:((block * IO_BLOCK_SIZE) >> 1);
    osEPiStartDma(&z2_pi_io_handle,&ioMesg,direction);
    z2_osRecvMessage(&queue,&mesg,OS_MESG_BLOCK);
    ioCmdExe(&z2_pi_io_handle);
}
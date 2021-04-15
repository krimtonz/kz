#include "vc.h"
extern void *hb_frame_buffer[2];
int frame_buffer_idx = 0;

void kz_rdp_frame_buffer_copy(void *frame_buffer) {
    gSystem->framebuffer->tex = hb_frame_buffer[frame_buffer_idx++];
    rdp_frame_buffer_copy(gSystem->framebuffer->tex);
    frame_buffer_idx &= 1;
}
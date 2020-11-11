#ifndef _AUDIO_H
#define _AUDIO_H

#include "z2.h"

#define afx_start_seq(player_idx, seq_idx) z2_AfxCmdWord(0x82000000 | (player_idx << 16) | (seq_idx << 8), 0x00000000)

#endif

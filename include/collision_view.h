/*
* commands.h
*
* definitions for collision viewers
*/

#ifndef _COLLISION_VIEW_H
#define _COLLISION_VIEW_H

enum col_view_status {
    COL_VIEW_INACTIVE,
    COL_VIEW_START,
    COL_VIEW_ACTIVE,
    COL_VIEW_BEGIN_STOP,
    COL_VIEW_STOP,
    COL_VIEW_BEGIN_RESTART,
    COL_VIEW_RESTART
};

void    kz_col_view     ();
void    kz_hitbox_view  ();

#endif
/*
* commands.h
*
* definitions for collision viewers
*/

#ifndef _COLLISION_VIEW_H
#define _COLLISION_VIEW_H

enum col_view_status {
    COL_VIEW_NONE = -1,
    COL_VIEW_SHOW,
    COL_VIEW_DESTROY,
    COL_VIEW_KILL,
    COL_VIEW_GENERATE,
    COL_VIEW_REGENERATE,
};

void    kz_col_view     ();
void    kz_hitbox_view  ();

#endif
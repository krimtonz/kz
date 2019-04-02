#ifndef _COLLISION_VIEW_H
#define _COLLISION_VIEW_H

enum col_view_status {
    COL_VIEW_NONE = -1,
    COL_VIEW_SHOW,
    COL_VIEW_DESTROY,
    COL_VIEW_GENERATE,
};

enum col_view_settings {
    COL_VIEW_REDUX = 1,
    COL_VIEW_OPAQUE = 2,
};

void kz_col_view();

#endif
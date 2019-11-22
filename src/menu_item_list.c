#include <stdlib.h>
#include <math.h>
#include "resource.h"
#include "menu.h"

#ifndef LITE
static Gfx wheel_state[] = {
    gsDPPipeSync(),
    gsSPLoadGeometryMode(0),
    /* rdp state */
    gsDPSetCycleType(G_CYC_1CYCLE),
    /* texture engine */
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsDPSetTextureDetail(G_TD_CLAMP),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetTextureLOD(G_TL_TILE),
    /* texture filter */
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetTextureConvert(G_TC_FILT),
    /* color combiner */
    gsDPSetCombineKey(G_OFF),
    gsDPSetCombineMode(G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM),
    /* blender */
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
    gsDPSetPrimColor(0,0,0xFF,0xFF,0xFF,0xFF),

    /* memory interface */
    gsDPSetColorDither(G_CD_DISABLE),
    gsDPSetAlphaDither(G_AD_DISABLE),
    gsDPPipelineMode(G_PM_NPRIMITIVE),
    gsSPEndDisplayList(),
};
#endif

struct item_list_data {
    int                     selected_idx;
    _Bool                   active;
    menu_generic_callback   callback;
    void                   *callback_data;
    int8_t                 *options;
    uint8_t                 option_cnt;
    int8_t                 *value_ptr;
    uint16_t                start_tile;
    uint8_t                *ovl_values;
    int                     tiles_cnt;
    draw_info_t            *draw_info;
    struct tilebg_info     *null_item;
#ifndef LITE
    float                   wheel_rotation;
    struct menu_item       *wheel;
#endif
};

static int8_t get_option_idx(struct item_list_data *data){
    if(data->options){
        for(int i=0;i<data->option_cnt;i++){
            if(data->options[i]==*(data->value_ptr)) return i;
        }
    }else{
        return *data->value_ptr;
    }
    return 0;
}

#ifndef LITE
static int8_t get_item_id(struct item_list_data *data, int idx){
    if(data->options){
        return data->options[idx];
    }else{
        return idx - 1;
    }
}

static _Bool has_null(struct item_list_data *data){
    if(data->options){
        for(int i=0;i<data->option_cnt;i++){
            if(data->options[i] == Z2_ITEM_NULL) return 1;
        }
    }else{
        return data->start_tile == Z2_ITEM_NULL;
    }
    return 0;
}

static void wheel_scroll(struct item_list_data *data, float speed){
    if(data->wheel){
        float dest = M_PI * 2.f * -data->selected_idx/data->option_cnt;
        float diff = dest - data->wheel_rotation;
        if(fabsf(diff)<0.001f){
            data->wheel_rotation = dest;
        }else{
            data->wheel_rotation+=diff*speed;
        }
    }
}

static void draw_item(struct item_list_data *data, int item, float rot){
    static Vtx mesh[] = {
        gdSPDefVtx(-1,1,0,0,0),
        gdSPDefVtx(1,1,0,62,0),
        gdSPDefVtx(-1,-1,0,0,62),
        gdSPDefVtx(1,-1,0,62,62),
        gdSPDefVtx(0,0,0,0,0),
        gdSPDefVtx(1,0,0,31,0),
        gdSPDefVtx(0,-1,0,0,31),
        gdSPDefVtx(1,-1,0,31,31),
    };

    Mtx m;
    MtxF mf;
    MtxF mt;
    {
        guRotateF(&mf,rot,0.f,1.f,0.f);
    }
    {
        guTranslateF(&mt,0.f,0.f,(float)data->option_cnt/M_PI);
        guMtxCatF(&mt,&mf,&mf);
    }
    guMtxF2L(&mf,&m);

    Mtx *p_mtx = gfx_data_push(&m,sizeof(m));
    gfx_push(gsSPMatrix(p_mtx,G_MTX_MODELVIEW | G_MTX_LOAD),
             gsSPVertex(&mesh,8,0));
    gfx_texture *texture;
    if(item>=0){
        int ovl_idx = item;
        if(has_null(data)) ovl_idx++;
        if(item<data->start_tile) item = item + data->start_tile;
        if(data->tiles_cnt>0 && item>data->tiles_cnt) item = data->start_tile;
        texture = get_item_texture(item);
        gfx_load_tile(texture,0);
        gfx_push(gsSP2Triangles(0,1,2,0,2,1,3,0));
        if(data->ovl_values){
            gfx_load_tile(resource_get(R_KZ_AMOUNTS),data->ovl_values[ovl_idx]);
            gfx_push(gsSP2Triangles(4,5,6,4,6,5,7,4));
        }
    }
}

static void draw_wheel(struct menu_item *item){
    struct item_list_data *data = item->data;
    if(!data->active) return;
    gfx_push(gsDisplayList(&wheel_state,0));

    Mtx m;
    MtxF mf;
    MtxF mt;
        
    guPerspectiveF(&mf, NULL, M_PI / 4.f,(float)Z2_SCREEN_WIDTH / (float)Z2_SCREEN_HEIGHT,1.f, 100.f, 1.f);
    guTranslateF(&mt, 0.f, 0.f, 1.f - (Z2_SCREEN_WIDTH / 32.f * 2.f + data->option_cnt / M_PI));
    guMtxCatF(&mt, &mf, &mf);
    guMtxF2L(&mf, &m);

    gfx_push(gsSPMatrix(gfx_data_push(&m, sizeof(m)), G_MTX_PROJECTION | G_MTX_LOAD));

    wheel_scroll(data,1.f/3.f);
    int n = (lroundf(-data->wheel_rotation / (M_PI * 2.f) * data->option_cnt) +
           (data->option_cnt + 3) / 4) % data->option_cnt;
    
    if (n < 0)
        n += data->option_cnt;

    for(int i =0;i<data->option_cnt;i++){
        float rota = data->wheel_rotation + M_PI *2.f / data->option_cnt * n;
        draw_item(data,get_item_id(data,n),rota);
        n = (n+1) % data->option_cnt;
    }

    gfx_begin();
}

 static int nav_wheel(struct menu_item *item, enum menu_nav nav){
    struct item_list_data *data = item->data;
    int n = 0;
    switch(nav){
        case MENU_NAV_DOWN:     n = -3; break;
        case MENU_NAV_UP:       n = 3;  break;
        case MENU_NAV_LEFT:     n = -1; break;
        case MENU_NAV_RIGHT:    n = 1;  break;
        case MENU_NAV_NONE:     return 0;
    }

    data->selected_idx += n;
    while(data->selected_idx>=data->option_cnt){
        data->selected_idx -= data->option_cnt;
        data->wheel_rotation += M_PI * 2.f;
    }
    while(data->selected_idx<0){
        data->selected_idx += data->option_cnt;
        data->wheel_rotation -= M_PI * 2.f;
    }
    return 1;
}

static void create_wheel(struct menu *menu, struct item_list_data *data){
    if(data->wheel == NULL){
        struct menu_item *item = menu_add(menu,0,0,NULL);
        item->data = data;
        item->draw_proc = draw_wheel;
        item->navigate_proc = nav_wheel;
        data->wheel = item;
        data->wheel_rotation = 0.f;
    }
    wheel_scroll(data,1.f);
}
#endif

static void draw_item_list(struct menu_item *item){
    struct item_list_data *data = item->data;
    draw_info_t *draw = data->draw_info;
    struct tilebg_info *bg = draw->background;
    uint32_t color;
    if(bg){
        if(data->active){
            color = bg->on_color.color;
        }
        else if(item->owner->selected_item==item){
            color = 0x0000FFFF;
        }else{
            color = bg->off_color.color;
        }
        gfx_draw_sprite_scale_color(bg->texture,get_item_x_pos(item),get_item_y_pos(item),
                                    bg->tile,
                                    16,16,
                                    draw->x_scale,draw->y_scale,
                                    color);
    }
    rdp_mode_set_apply(RDP_MODE_COLOR,0xFFFFFFFF);
    int idx = data->active?data->selected_idx:get_option_idx(data);
    int val;
    if(data->options){
        val = data->options[idx];
    }else{
        if(data->active)idx--;
        val = idx;
        idx++;
    }
    if(val!=Z2_ITEM_NULL){
        int tile_idx = idx;
        if(data->tiles_cnt>0 && tile_idx>=data->tiles_cnt-1) tile_idx = 1;
        tile_idx = tile_idx + data->start_tile - 1;
        gfx_texture *texture = get_item_texture(tile_idx);
        gfx_draw_sprite_scale(texture,get_item_x_pos(item),get_item_y_pos(item),0, 16,16,draw->x_scale,draw->y_scale);
        if(data->ovl_values){
            gfx_draw_sprite_scale(resource_get(R_KZ_AMOUNTS),get_item_x_pos(item)+8,get_item_y_pos(item)+11,data->ovl_values[idx],16,8,0.5f,0.5f);
        }
    }else if(data->null_item!=NULL){
        struct tilebg_info *null_item = data->null_item;
        uint32_t color;
        if(data->active){
            color = null_item->on_color.color;
        }
        else if(item->owner->selected_item==item){
            color = 0x0000FFFF;
        }else{
            color = null_item->off_color.color;
        }
        gfx_draw_sprite_scale_color(data->null_item->texture,get_item_x_pos(item),get_item_y_pos(item),data->null_item->tile,
                                    16,16, draw->x_scale, draw->y_scale,
                                    color);
    }
}

static int navigate_item_list(struct menu_item *item, enum menu_nav nav){
    struct item_list_data *data = item->data;
    if(!data->active) return 0;
#ifdef LITE
    if(nav==MENU_NAV_DOWN || nav == MENU_NAV_LEFT){
        data->selected_idx += data->option_cnt - 1;
        data->selected_idx %= data->option_cnt;
    }else if(nav==MENU_NAV_UP || nav == MENU_NAV_RIGHT){
        data->selected_idx++;
        data->selected_idx %= data->option_cnt;
    }
    return 1;
#else
    return nav_wheel(data->wheel,nav);
#endif
}

static void activate_item_list(struct menu_item *item){
    struct item_list_data *data = item->data;
    if(data->active){
        int8_t val;
        if(data->options)
            val = data->options[data->selected_idx];
        else
            val = data->selected_idx - 1;
        *(data->value_ptr) = val;
        if(data->callback){
            data->callback(item,MENU_CALLBACK_ACTIVATE,data->callback_data);
        }
    }else{
        int8_t idx = get_option_idx(data) + (data->options?0:1);
        data->selected_idx = idx;
#ifndef LITE
        create_wheel(item->owner,data);
#endif
    }
    data->active = !data->active;
}

static void update_item_list(struct menu_item *item){
    struct item_list_data *data = item->data;
    if(!data->active){
        if(data->callback){
            data->callback(item,MENU_CALLBACK_UPDATE,data->callback_data);
        }
    }
}

struct menu_item *menu_add_item_list(struct menu *menu, uint16_t x, uint16_t y, menu_generic_callback callback,
                                     void *callback_data, uint16_t start_tile, int8_t *options,
                                     uint8_t option_cnt, int8_t *value_ptr, uint8_t *ovl_values, int tiles_cnt,
                                     draw_info_t *drawinfo, struct tilebg_info *null_item, char *tooltip){
    struct menu_item *item = menu_add(menu,x,y,NULL);
    if(item){
        struct item_list_data *data = malloc(sizeof(*data));
        data->callback = callback;
        data->callback_data = callback_data;
        data->selected_idx = 0;
        data->active = 0;
        data->options = options;
        data->option_cnt = option_cnt;
        data->value_ptr = value_ptr;
        data->start_tile = start_tile;
        data->ovl_values = ovl_values;
        data->tiles_cnt = tiles_cnt;
        data->draw_info = drawinfo;
        data->null_item = null_item;
#ifndef LITE
        data->wheel = NULL;
        data->wheel_rotation = 0.f;
#endif
        item->data = data;
        item->draw_proc = draw_item_list;
        item->interactive = 1;
        item->navigate_proc = navigate_item_list;
        item->activate_proc = activate_item_list;
        item->update_proc = update_item_list;
        item->tooltip = tooltip;
    }
    return item;
}
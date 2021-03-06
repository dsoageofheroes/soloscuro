#ifndef GFF_IMAGE_H
#define GFF_IMAGE_H

#include "gff-common.h"

#define PALETTE_SIZE (256)

typedef struct _gff_color_t {
    unsigned short r, g, b;
} gff_color_t;

typedef struct _gff_palette_t {
    gff_color_t color[PALETTE_SIZE];
} gff_palette_t;

typedef struct gff_palettes_s {
    uint16_t len;
    gff_palette_t palettes[];
} gff_palettes_t;

typedef struct gff_image_entry_s {
    uint16_t frame_num;
    uint32_t data_len;
    char data[];
} gff_image_entry_t;

typedef struct gff_frame_entry_s {
    uint16_t width, height;
    char rgba[];
} gff_frame_entry_t;

void gff_image_init();
gff_palette_t* create_palettes(int gff_index, unsigned int *len);
gff_palettes_t* read_palettes(int gff_idx);
gff_palettes_t* read_palettes_type(int gff_idx, int res_id);
int get_frame_count(int gff_index, int type_id, int res_id);
int get_frame_width(int gff_index, int type_id, int res_id, int frame_id);
int get_frame_height(int gff_index, int type_id, int res_id, int frame_id);
unsigned char* get_frame_rgba(int gff_index, int type_id, int res_id, int frame_id);
unsigned char* get_frame_rgba_with_palette(int gff_index, int type_id, int res_id, int frame_id, int palette_id);
unsigned char* get_frame_rgba_palette(int gff_index, int type_id, int res_id, int frame_id, const gff_palette_t *pal);
unsigned char* get_frame_rgba_palette_img(gff_image_entry_t *img, int frame_id, const gff_palette_t *pal);
unsigned char* get_portrait(unsigned char* bmp_table, unsigned int *width, unsigned int *height);
unsigned char* create_font_rgba(int gff_index, int c, int fg_color, int bg_color);
extern int gff_get_number_of_palettes();

#endif

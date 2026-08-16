#ifndef WM_BRET_SPRITES_H
#define WM_BRET_SPRITES_H
#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char *source_frame;
    const char *source_container;
    uint16_t width;
    uint16_t height;
    int16_t xani;
    int16_t yani;
    const uint8_t *pixels_ci8;
    /* N64-ready RGBA5551 TLUT. Entry 0 has alpha=0 for source transparency. */
    uint16_t *palette_rgba5551;
    uint16_t palette_colors;
} wm_source_sprite;

const wm_source_sprite *wm_bret_sprite_find(const char *source_frame);
size_t wm_bret_sprite_count(void);

#endif

/**
 * @file lv_draw_rbasic..h
 * 
 */

#ifndef LV_DRAW_RBASIC_H
#define LV_DRAW_RBASIC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_rpx(lv_coord_t x, lv_coord_t y, const lv_area_t * mask_p, lv_color_t color, lv_opa_t opa);

/**
 * Fill an area on the display
 * @param cords_p coordinates of the area to fill
 * @param mask_p fill only o this mask
 * @param color fill color
 * @param opa opacity (ignored, only for compatibility with lv_vfill)
 */
void lv_rfill(const lv_area_t * cords_p, const lv_area_t * mask_p,
                lv_color_t color, lv_opa_t opa);

/**
 * Draw a letter to the display
 * @param pos_p left-top coordinate of the latter
 * @param mask_p the letter will be drawn only on this area
 * @param font_p pointer to font
 * @param letter a letter to draw
 * @param color color of letter
 * @param opa opacity of letter (ignored, only for compatibility with lv_vletter)
 */
void lv_rletter(const lv_point_t * pos_p, const lv_area_t * mask_p,
                const lv_font_t * font_p, uint32_t letter,
                lv_color_t color, lv_opa_t opa);

/**
 * Draw a color map to the display
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area
 * @param map_p pointer to a lv_color_t array
 * @param opa opacity of the map (ignored, only for compatibility with lv_vmap)
 * @param transp true: enable transparency of LV_IMG_LV_COLOR_TRANSP color pixels
 * @param upscale true: upscale to double size (not supported)
 * @param recolor mix the pixels with this color (not supported)
 * @param recolor_opa the intense of recoloring (not supported)
 */
void lv_rmap(const lv_area_t * cords_p, const lv_area_t * mask_p,
             const lv_color_t * map_p, lv_opa_t opa, bool transp, bool upscale,
			 lv_color_t recolor, lv_opa_t recolor_opa);
/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_DRAW_RBASIC_H*/

/**
 * @file lv_test_kb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>  /*For printf in the action*/

#include "lv_test_kb.h"

#if USE_LV_KB != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a default object and test the basic functions
 */
void lv_test_kb_1(void)
{

    lv_obj_t *ta = lv_ta_create(lv_scr_act(), NULL);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);

    /* Default object*/
    lv_obj_t *kb1 = lv_kb_create(lv_scr_act(), NULL);
    lv_obj_align(kb1, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_kb_set_ta(kb1, ta);
}

/**
 * Create a styles keyboard
 */
void lv_test_kb_2(void)
{

    lv_obj_t *ta = lv_ta_create(lv_scr_act(), NULL);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);

    /* Default object*/
    lv_obj_t *kb1 = lv_kb_create(lv_scr_act(), NULL);
    lv_obj_set_size(kb1, LV_HOR_RES / 2, LV_VER_RES / 4);
    lv_obj_align(kb1, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_kb_set_ta(kb1, ta);

    static lv_style_t bg;
    static lv_style_t rel;
    static lv_style_t pr;

    lv_style_copy(&bg, &lv_style_plain_color);
    bg.body.main_color = LV_COLOR_NAVY;
    bg.body.grad_color = LV_COLOR_NAVY;
    bg.body.padding.hor = 0;
    bg.body.padding.ver = 10;
    bg.body.padding.inner = 0;

    lv_style_copy(&rel, &lv_style_plain);
    rel.body.border.width = 1;
    rel.body.main_color = LV_COLOR_WHITE;
    rel.body.grad_color = LV_COLOR_SILVER;
    rel.body.grad_color = LV_COLOR_SILVER;
    rel.text.color = LV_COLOR_NAVY;
    lv_style_copy(&pr, &lv_style_plain_color);

    lv_kb_set_style(kb1, LV_KB_STYLE_BG, &bg);
    lv_kb_set_style(kb1, LV_KB_STYLE_BTN_REL, &rel);
    lv_kb_set_style(kb1, LV_KB_STYLE_BTN_PR, &pr);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*USE_LV_KB*/

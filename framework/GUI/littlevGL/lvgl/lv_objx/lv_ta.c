/**
 * @file lv_ta.c
 * 
 */


/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_TA != 0

#include "lv_ta.h"
#include "../lv_core/lv_group.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
/*Test configuration*/

#ifndef LV_TA_CURSOR_BLINK_TIME
#define LV_TA_CURSOR_BLINK_TIME 400    /*ms*/
#endif

#ifndef LV_TA_PWD_SHOW_TIME
#define LV_TA_PWD_SHOW_TIME 1500    /*ms*/
#endif

#define LV_TA_DEF_WIDTH     (2 * LV_DPI)
#define LV_TA_DEF_HEIGHT    (1 * LV_DPI)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_ta_design(lv_obj_t * ta, const lv_area_t * mask, lv_design_mode_t mode);
static bool lv_ta_scrollable_design(lv_obj_t * scrl, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_ta_signal(lv_obj_t * ta, lv_signal_t sign, void * param);
static lv_res_t lv_ta_scrollable_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);
#if USE_LV_ANIMATION
static void cursor_blink_anim(lv_obj_t * ta, uint8_t show);
static void pwd_char_hider_anim(lv_obj_t * ta, int32_t x);
#endif
static void pwd_char_hider(lv_obj_t * ta);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_func_t ancestor_design;
static lv_design_func_t scrl_design;
static lv_signal_func_t ancestor_signal;
static lv_signal_func_t scrl_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a text area objects
 * @param par pointer to an object, it will be the parent of the new text area
 * @param copy pointer to a text area object, if not NULL then the new object will be copied from it
 * @return pointer to the created text area
 */
lv_obj_t * lv_ta_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor object*/
    lv_obj_t * new_ta = lv_page_create(par, copy);
    lv_mem_assert(new_ta);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_ta);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_ta);
    if(scrl_signal == NULL) scrl_signal = lv_obj_get_signal_func(lv_page_get_scrl(new_ta));
    if(scrl_design == NULL) scrl_design = lv_obj_get_design_func(lv_page_get_scrl(new_ta));

    /*Allocate the object type specific extended data*/
    lv_ta_ext_t * ext = lv_obj_allocate_ext_attr(new_ta, sizeof(lv_ta_ext_t));
    lv_mem_assert(ext);
    ext->cursor.state = 0;
    ext->pwd_mode = 0;
    ext->pwd_tmp = NULL;
    ext->cursor.style = NULL;
    ext->cursor.pos = 0;
    ext->cursor.type = LV_CURSOR_LINE;
    ext->cursor.valid_x = 0;
    ext->one_line = 0;
    ext->label = NULL;


    lv_obj_set_signal_func(new_ta, lv_ta_signal);
    lv_obj_set_signal_func(lv_page_get_scrl(new_ta), lv_ta_scrollable_signal);
    lv_obj_set_design_func(new_ta, lv_ta_design);

    /*Init the new text area object*/
    if(copy == NULL) {
    	ext->label = lv_label_create(new_ta, NULL);

    	lv_obj_set_design_func(ext->page.scrl, lv_ta_scrollable_design);

    	lv_label_set_long_mode(ext->label, LV_LABEL_LONG_BREAK);
    	lv_label_set_text(ext->label, "Text area");
    	lv_obj_set_click(ext->label, false);
    	lv_obj_set_size(new_ta, LV_TA_DEF_WIDTH, LV_TA_DEF_HEIGHT);
        lv_ta_set_sb_mode(new_ta, LV_SB_MODE_DRAG);
        lv_page_set_style(new_ta, LV_PAGE_STYLE_SCRL, &lv_style_transp_tight);

        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            lv_ta_set_style(new_ta, LV_TA_STYLE_BG, th->ta.area);
            lv_ta_set_style(new_ta, LV_TA_STYLE_SB, th->ta.sb);
        } else {
            lv_ta_set_style(new_ta, LV_TA_STYLE_BG, &lv_style_pretty);
        }
    }
    /*Copy an existing object*/
    else {
    	lv_obj_set_design_func(ext->page.scrl, lv_ta_scrollable_design);
    	lv_ta_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	ext->label = lv_label_create(new_ta, copy_ext->label);
        ext->pwd_mode = copy_ext->pwd_mode;
        ext->cursor.style = copy_ext->cursor.style;
        ext->cursor.pos = copy_ext->cursor.pos;
        ext->cursor.valid_x = copy_ext->cursor.valid_x;
        ext->cursor.type = copy_ext->cursor.type;
    	if(copy_ext->one_line) lv_ta_set_one_line(new_ta, true);

    	lv_ta_set_style(new_ta, LV_TA_STYLE_CURSOR, lv_ta_get_style(copy, LV_TA_STYLE_CURSOR));

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_ta);
    }

#if USE_LV_ANIMATION
    /*Create a cursor blinker animation*/
    lv_anim_t a;
    a.var = new_ta;
    a.fp = (lv_anim_fp_t)cursor_blink_anim;
    a.time = LV_TA_CURSOR_BLINK_TIME;
    a.act_time = 0;
    a.end_cb = NULL;
    a.start = 1;
    a.end = 0;
    a.repeat = 1;
    a.repeat_pause = 0;
    a.playback = 1;
    a.playback_pause = 0;
    a.path = lv_anim_path_step;
    lv_anim_create(&a);
#endif

    return new_ta;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Insert a character to the current cursor position
 * @param ta pointer to a text area object
 * @param c a character
 */
void lv_ta_add_char(lv_obj_t * ta, char c)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);

    if(ext->pwd_mode != 0) pwd_char_hider(ta);  /*Make sure all the current text contains only '*'*/
    char letter_buf[2];
    letter_buf[0] = c;
    letter_buf[1] = '\0';

    lv_label_ins_text(ext->label, ext->cursor.pos, letter_buf);    /*Insert the character*/

    if(ext->pwd_mode != 0) {

        ext->pwd_tmp = lv_mem_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + 2);  /*+2: the new char + \0 */
        lv_mem_assert(ext->pwd_tmp);
        lv_txt_ins(ext->pwd_tmp, ext->cursor.pos, letter_buf);

#if USE_LV_ANIMATION
        /*Auto hide characters*/
        lv_anim_t a;
        a.var = ta;
        a.fp = (lv_anim_fp_t)pwd_char_hider_anim;
        a.time = LV_TA_PWD_SHOW_TIME;
        a.act_time = 0;
        a.end_cb = (lv_anim_cb_t)pwd_char_hider;
        a.start = 0;
        a.end = 1;
        a.repeat = 0;
        a.repeat_pause = 0;
        a.playback = 0;
        a.playback_pause = 0;
        a.path = lv_anim_path_step;
        lv_anim_create(&a);
#endif
    }

    /*Move the cursor after the new character*/
    lv_ta_set_cursor_pos(ta, lv_ta_get_cursor_pos(ta) + 1);
}

/**
 * Insert a text to the current cursor position
 * @param ta pointer to a text area object
 * @param txt a '\0' terminated string to insert
 */
void lv_ta_add_text(lv_obj_t * ta, const char * txt)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);


    if(ext->pwd_mode != 0) pwd_char_hider(ta);  /*Make sure all the current text contains only '*'*/
    /*Insert the text*/

    lv_label_ins_text(ext->label, ext->cursor.pos, txt);
    if(ext->pwd_mode != 0) {
        ext->pwd_tmp = lv_mem_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + strlen(txt) + 1);
        lv_mem_assert(ext->pwd_tmp);

        lv_txt_ins(ext->pwd_tmp, ext->cursor.pos, txt);

#if USE_LV_ANIMATION
        /*Auto hide characters*/
        lv_anim_t a;
        a.var = ta;
        a.fp = (lv_anim_fp_t)pwd_char_hider_anim;
        a.time = LV_TA_PWD_SHOW_TIME;
        a.act_time = 0;
        a.end_cb = (lv_anim_cb_t)pwd_char_hider;
        a.start = 0;
        a.end = 1;
        a.repeat = 0;
        a.repeat_pause = 0;
        a.playback = 0;
        a.playback_pause = 0;
        a.path = lv_anim_path_step;
        lv_anim_create(&a);
#endif
    }

    /*Move the cursor after the new text*/
    lv_ta_set_cursor_pos(ta, lv_ta_get_cursor_pos(ta) + lv_txt_get_length(txt));
}

/**
 * Delete a the left character from the current cursor position
 * @param ta pointer to a text area object
 */
void lv_ta_del_char(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
    uint16_t cur_pos = ext->cursor.pos;

    if(cur_pos == 0) return;

    char * label_txt = lv_label_get_text(ext->label);
    /*Delete a character*/
    lv_txt_cut(label_txt, ext->cursor.pos - 1, 1);
    /*Refresh the label*/
    lv_label_set_text(ext->label, label_txt);

    /*Don't let 'width == 0' because cursor will not be visible*/
    if(lv_obj_get_width(ext->label) == 0) {
        lv_style_t * style = lv_obj_get_style(ext->label);
        lv_obj_set_width(ext->label, style->line.width);
    }

    if(ext->pwd_mode != 0) {
#if LV_TXT_UTF8 == 0
        lv_txt_cut(ext->pwd_tmp, ext->cursor.pos - 1, 1);
#else
        uint32_t byte_pos = txt_utf8_get_byte_id(ext->pwd_tmp, ext->cursor.pos - 1);
        lv_txt_cut(ext->pwd_tmp, ext->cursor.pos - 1, lv_txt_utf8_size(label_txt[byte_pos]));
#endif
        ext->pwd_tmp = lv_mem_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + 1);
        lv_mem_assert(ext->pwd_tmp);
    }

    /*Move the cursor to the place of the deleted character*/
    lv_ta_set_cursor_pos(ta, ext->cursor.pos - 1);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void lv_ta_set_text(lv_obj_t * ta, const char * txt)
{
	lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
	lv_label_set_text(ext->label, txt);
	lv_ta_set_cursor_pos(ta, LV_TA_CURSOR_LAST);

	/*Don't let 'width == 0' because cursor will not be visible*/
	if(lv_obj_get_width(ext->label) == 0) {
	    lv_style_t * style = lv_obj_get_style(ext->label);
	    lv_obj_set_width(ext->label, lv_font_get_width_scale(style->text.font, ' '));
	}

    if(ext->pwd_mode != 0) {
        ext->pwd_tmp = lv_mem_realloc(ext->pwd_tmp, strlen(txt) + 1);
        strcpy(ext->pwd_tmp, txt);

#if USE_LV_ANIMATION
        /*Auto hide characters*/
        lv_anim_t a;
        a.var = ta;
        a.fp = (lv_anim_fp_t)pwd_char_hider_anim;
        a.time = LV_TA_PWD_SHOW_TIME;
        a.act_time = 0;
        a.end_cb = (lv_anim_cb_t)pwd_char_hider;
        a.start = 0;
        a.end = 1;
        a.repeat = 0;
        a.repeat_pause = 0;
        a.playback = 0;
        a.playback_pause = 0;
        a.path = lv_anim_path_step;
        lv_anim_create(&a);
#endif
    }
}

/**
 * Set the cursor position
 * @param obj pointer to a text area object
 * @param pos the new cursor position in character index
 *             < 0 : index from the end of the text
 *             LV_TA_CURSOR_LAST: go after the last character
 */
void lv_ta_set_cursor_pos(lv_obj_t * ta, int16_t pos)
{
	lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
    uint16_t len = lv_txt_get_length(lv_label_get_text(ext->label));

	if(pos < 0) pos = len + pos;

	if(pos > len || pos == LV_TA_CURSOR_LAST) pos = len;

	ext->cursor.pos = pos;

	/*Position the label to make the cursor visible*/
	lv_obj_t * label_par = lv_obj_get_parent(ext->label);
	lv_point_t cur_pos;
	lv_style_t * style = lv_obj_get_style(ta);
	const lv_font_t * font_p = style->text.font;
	lv_area_t label_cords;
    lv_area_t ta_cords;
	lv_label_get_letter_pos(ext->label, pos, &cur_pos);
	lv_obj_get_coords(ta, &ta_cords);
    lv_obj_get_coords(ext->label, &label_cords);

	/*Check the top*/
    lv_coord_t font_h = lv_font_get_height_scale(font_p);
	if(lv_obj_get_y(label_par) + cur_pos.y < 0) {
		lv_obj_set_y(label_par, - cur_pos.y + style->body.padding.ver);
	}

	/*Check the bottom*/
	if(label_cords.y1 + cur_pos.y + font_h + style->body.padding.ver > ta_cords.y2) {
		lv_obj_set_y(label_par, -(cur_pos.y - lv_obj_get_height(ta) +
				                     font_h + 2 * style->body.padding.ver));
	}
	/*Check the left (use the font_h as general unit)*/
    if(lv_obj_get_x(label_par) + cur_pos.x < font_h) {
        lv_obj_set_x(label_par, - cur_pos.x + font_h);
    }

    /*Check the right (use the font_h as general unit)*/
    if(label_cords.x1 + cur_pos.x + font_h + style->body.padding.hor > ta_cords.x2) {
        lv_obj_set_x(label_par, -(cur_pos.x - lv_obj_get_width(ta) +
                                     font_h + 2 * style->body.padding.hor));
    }

    ext->cursor.valid_x = cur_pos.x;

#if USE_LV_ANIMATION
    /*Reset cursor blink animation*/
    lv_anim_t a;
    a.var = ta;
    a.fp = (lv_anim_fp_t)cursor_blink_anim;
    a.time = LV_TA_CURSOR_BLINK_TIME;
    a.act_time = 0;
    a.end_cb = NULL;
    a.start = 1;
    a.end= 0;
    a.repeat = 1;
    a.repeat_pause = 0;
    a.playback = 1;
    a.playback_pause = 0;
    a.path = lv_anim_path_step;
    lv_anim_create(&a);
#endif

	lv_obj_invalidate(ta);
}

/**
 * Set the cursor type.
 * @param ta pointer to a text area object
 * @param cur_type: element of 'lv_ta_cursor_type_t'
 */
void lv_ta_set_cursor_type(lv_obj_t * ta, lv_cursor_type_t cur_type)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
    ext->cursor.type = cur_type;
    lv_obj_invalidate(ta);
}

/**
 * Enable/Disable password mode
 * @param ta pointer to a text area object
 * @param pwd_en true: enable, false: disable
 */
void lv_ta_set_pwd_mode(lv_obj_t * ta, bool pwd_en)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);

    /*Pwd mode is now enabled*/
    if(ext->pwd_mode == 0 && pwd_en != false) {
        char * txt = lv_label_get_text(ext->label);
        uint16_t len = strlen(txt);
        ext->pwd_tmp = lv_mem_alloc(len + 1);
        strcpy(ext->pwd_tmp, txt);

        uint16_t i;
        for(i = 0; i < len; i++) {
            txt[i] = '*';       /*All char to '*'*/
        }
        txt[i] = '\0';

        lv_label_set_text(ext->label, NULL);
    }
    /*Pwd mode is now disabled*/
    else if(ext->pwd_mode == 1 && pwd_en == false) {
        lv_label_set_text(ext->label, ext->pwd_tmp);
        lv_mem_free(ext->pwd_tmp);
        ext->pwd_tmp = NULL;
    }

    ext->pwd_mode = pwd_en == false ? 0 : 1;
}

/**
 * Configure the text area to one line or back to normal
 * @param ta pointer to a Text area object
 * @param en true: one line, false: normal
 */
void lv_ta_set_one_line(lv_obj_t * ta, bool en)
{
    if(en != false) {
        lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
        lv_style_t * style_ta = lv_obj_get_style(ta);
        lv_style_t * style_scrl = lv_obj_get_style(lv_page_get_scrl(ta));
        lv_style_t * style_label = lv_obj_get_style(ext->label);
        lv_coord_t font_h =  lv_font_get_height_scale(style_label->text.font);

        ext->one_line = 1;
        lv_page_set_scrl_fit(ta, true, true);
        lv_obj_set_height(ta, font_h + (style_ta->body.padding.ver + style_scrl->body.padding.ver) * 2);
        lv_label_set_long_mode(ext->label, LV_LABEL_LONG_EXPAND);
        lv_label_set_no_break(ext->label, true);
        lv_obj_set_pos(lv_page_get_scrl(ta), style_ta->body.padding.hor, style_ta->body.padding.ver);
    } else {
        lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
        lv_style_t * style_ta = lv_obj_get_style(ta);

        ext->one_line = 0;
        lv_page_set_scrl_fit(ta, false, true);
        lv_label_set_long_mode(ext->label, LV_LABEL_LONG_BREAK);
        lv_label_set_no_break(ext->label, false);
        lv_obj_set_height(ta, LV_TA_DEF_HEIGHT);
        lv_obj_set_pos(lv_page_get_scrl(ta), style_ta->body.padding.hor, style_ta->body.padding.ver);
    }
}

/**
 * Set a style of a text area
 * @param ta pointer to a text area object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_ta_set_style(lv_obj_t *ta, lv_ta_style_t type, lv_style_t *style)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);

    switch (type) {
        case LV_TA_STYLE_BG:
            lv_page_set_style(ta, LV_PAGE_STYLE_BG, style);
            break;
        case LV_TA_STYLE_SB:
            lv_page_set_style(ta, LV_PAGE_STYLE_SB, style);
            break;
        case LV_TA_STYLE_CURSOR:
            ext->cursor.style = style;
            lv_obj_refresh_ext_size(lv_page_get_scrl(ta)); /*Refresh ext. size because of cursor drawing*/
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a text area
 * @param ta pointer to a text area object
 * @return pointer to the text
 */
const char * lv_ta_get_text(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);

	const char * txt;
	if(ext->pwd_mode == 0) {
	    txt = lv_label_get_text(ext->label);
	} else {
	    txt = ext->pwd_tmp;
	}

	return txt;
}


/**
 * Get the label of a text area
 * @param ta pointer to a text area object
 * @return pointer to the label object
 */
lv_obj_t * lv_ta_get_label(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->label;
}


/**
 * Get the current cursor position in character index
 * @param ta pointer to a text area object
 * @return the cursor position
 */
uint16_t lv_ta_get_cursor_pos(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
	return ext->cursor.pos;
}

/**
 * Get the current cursor type.
 * @param ta pointer to a text area object
 * @return element of 'lv_ta_cursor_type_t'
 */
lv_cursor_type_t lv_ta_get_cursor_type(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->cursor.type;
}

/**
 * Get the password mode attribute
 * @param ta pointer to a text area object
 * @return true: password mode is enabled, false: disabled
 */
bool lv_ta_get_pwd_mode(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->pwd_mode == 0 ? false : true;
}

/**
 * Get the one line configuration attribute
 * @param ta pointer to a text area object
 * @return true: one line configuration is enabled, false: disabled
 */
bool lv_ta_get_one_line(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
    return ext->one_line == 0 ? false : true;
}

/**
 * Get a style of a text area
 * @param ta pointer to a text area object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_ta_get_style(lv_obj_t *ta, lv_ta_style_t type)
{
    lv_ta_ext_t *ext = lv_obj_get_ext_attr(ta);

    switch (type) {
        case LV_TA_STYLE_BG:     return lv_page_get_style(ta, LV_PAGE_STYLE_BG);
        case LV_TA_STYLE_SB:     return lv_page_get_style(ta, LV_PAGE_STYLE_SB);
        case LV_TA_STYLE_CURSOR: return ext->cursor.style;
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Move the cursor one character right
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_right(lv_obj_t * ta)
{
    uint16_t cp = lv_ta_get_cursor_pos(ta);
    cp++;
    lv_ta_set_cursor_pos(ta, cp);
}

/**
 * Move the cursor one character left
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_left(lv_obj_t * ta)
{
    uint16_t cp = lv_ta_get_cursor_pos(ta);
    if(cp > 0)  {
        cp--;
        lv_ta_set_cursor_pos(ta, cp);
    }
}

/**
 * Move the cursor one line down
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_down(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
    lv_point_t pos;

    /*Get the position of the current letter*/
    lv_label_get_letter_pos(ext->label, lv_ta_get_cursor_pos(ta), &pos);

    /*Increment the y with one line and keep the valid x*/
    lv_style_t * label_style = lv_obj_get_style(ext->label);
    const lv_font_t * font_p = label_style->text.font;
    lv_coord_t font_h = lv_font_get_height_scale(font_p);
    pos.y += font_h + label_style->text.line_space + 1;
    pos.x = ext->cursor.valid_x;

    /*Do not go below the last line*/
    if(pos.y < lv_obj_get_height(ext->label)) {
        /*Get the letter index on the new cursor position and set it*/
        uint16_t new_cur_pos = lv_label_get_letter_on(ext->label, &pos);

        lv_coord_t cur_valid_x_tmp = ext->cursor.valid_x;   /*Cursor position set overwrites the valid positon */
        lv_ta_set_cursor_pos(ta, new_cur_pos);
        ext->cursor.valid_x = cur_valid_x_tmp;
    }
}

/**
 * Move the cursor one line up
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_up(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
    lv_point_t pos;

    /*Get the position of the current letter*/
    lv_label_get_letter_pos(ext->label, lv_ta_get_cursor_pos(ta), &pos);

    /*Decrement the y with one line and keep the valid x*/
    lv_style_t * label_style = lv_obj_get_style(ext->label);
    const lv_font_t * font = label_style->text.font;
    lv_coord_t font_h = lv_font_get_height_scale(font);
    pos.y -= font_h + label_style->text.line_space - 1;
    pos.x = ext->cursor.valid_x;


    /*Get the letter index on the new cursor position and set it*/
    uint16_t new_cur_pos = lv_label_get_letter_on(ext->label, &pos);
    lv_coord_t cur_valid_x_tmp = ext->cursor.valid_x;   /*Cursor position set overwrites the valid positon */
    lv_ta_set_cursor_pos(ta, new_cur_pos);
    ext->cursor.valid_x = cur_valid_x_tmp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the text areas
 * @param ta pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_ta_design(lv_obj_t * ta, const lv_area_t * masp, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return ancestor_design(ta, masp, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Draw the object*/
		ancestor_design(ta, masp, mode);

    } else if(mode == LV_DESIGN_DRAW_POST) {
		ancestor_design(ta, masp, mode);
    }
    return true;
}

/**
 * An extended scrollable design of the page. Calls the normal design function and draws a cursor.
 * @param scrl pointer to the scrollable part of the Text area
 * @param mask  the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @return return true/false, depends on 'mode'
 */
static bool lv_ta_scrollable_design(lv_obj_t * scrl, const lv_area_t * mask, lv_design_mode_t mode)
{
	if(mode == LV_DESIGN_COVER_CHK) {
		/*Return false if the object is not covers the mask_p area*/
		return scrl_design(scrl, mask, mode);
	} else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Draw the object*/
		scrl_design(scrl, mask, mode);
	} else if(mode == LV_DESIGN_DRAW_POST) {
		scrl_design(scrl, mask, mode);

		/*Draw the cursor too*/
		lv_obj_t * ta = lv_obj_get_parent(scrl);
		lv_ta_ext_t * ta_ext = lv_obj_get_ext_attr(ta);
        lv_style_t * label_style = lv_obj_get_style(ta_ext->label);
		if(ta_ext->cursor.type == LV_CURSOR_NONE ||
           (ta_ext->cursor.type & LV_CURSOR_HIDDEN) ||
		   ta_ext->cursor.state == 0 ||
		   label_style->body.opa == LV_OPA_TRANSP)
		{
		    return true; 	/*The cursor is not visible now*/
		}


        lv_style_t cur_style;
        if(ta_ext->cursor.style != NULL) {
            lv_style_copy(&cur_style, ta_ext->cursor.style);
        }
        else {
            /*If cursor style is not specified then use the modified label style */
        	lv_style_copy(&cur_style, label_style);
        	lv_color_t clv_color_tmp = cur_style.text.color;		/*Make letter color to cursor color*/
        	cur_style.text.color = cur_style.body.main_color;		/*In block mode the letter color will be current background color*/
        	cur_style.body.main_color = clv_color_tmp;
        	cur_style.body.grad_color = clv_color_tmp;
        	cur_style.body.border.color = clv_color_tmp;
        	cur_style.body.border.opa = LV_OPA_COVER;
        	cur_style.body.border.width = 1;
        	cur_style.body.shadow.width = 0;
        	cur_style.body.radius = 0;
        	cur_style.body.empty = 0;
        	cur_style.body.padding.hor = 0;
            cur_style.body.padding.ver = 0;
            cur_style.line.width = 1;
            cur_style.body.opa = LV_OPA_COVER;
        }

		uint16_t cur_pos = lv_ta_get_cursor_pos(ta);
		const char * txt = lv_label_get_text(ta_ext->label);
        uint32_t byte_pos;
#if LV_TXT_UTF8 != 0
        byte_pos = txt_utf8_get_byte_id(txt, cur_pos);
#else
        byte_pos = cur_pos;
#endif

		uint32_t letter = lv_txt_utf8_next(&txt[byte_pos], NULL);
		lv_coord_t letter_h = lv_font_get_height_scale(label_style->text.font);
		/*Set letter_w (set not 0 on non printable but valid chars)*/
        lv_coord_t letter_w;
		if(letter == '\0' || letter == '\n' || letter == '\r') {
		    letter_w = lv_font_get_width_scale(label_style->text.font, ' ');
		} else {
            letter_w = lv_font_get_width_scale(label_style->text.font, letter);
		}

		lv_point_t letter_pos;
		lv_label_get_letter_pos(ta_ext->label, cur_pos, &letter_pos);

		/*If the cursor is out of the text (most right) draw it to the next line*/
		if(letter_pos.x + ta_ext->label->coords.x1 + letter_w > ta_ext->label->coords.x2 && ta_ext->one_line == 0) {
		    letter_pos.x = 0;
		    letter_pos.y += letter_h + label_style->text.line_space;

		    if(letter != '\0'){
		        byte_pos += lv_txt_utf8_size(txt[byte_pos]);
		        letter = lv_txt_utf8_next(&txt[byte_pos], NULL);
		    }

		    if(letter == '\0' || letter == '\n' || letter == '\r') {
                letter_w = lv_font_get_width_scale(label_style->text.font, ' ');
            } else {
                letter_w = lv_font_get_width_scale(label_style->text.font, letter);
            }
		}

		/*Draw he cursor according to the type*/
		lv_area_t cur_area;
		if(ta_ext->cursor.type == LV_CURSOR_LINE) {
			cur_area.x1 = letter_pos.x + ta_ext->label->coords.x1 + cur_style.body.padding.hor - (cur_style.line.width >> 1) - (cur_style.line.width & 0x1);
			cur_area.y1 = letter_pos.y + ta_ext->label->coords.y1 + cur_style.body.padding.ver;
			cur_area.x2 = letter_pos.x + ta_ext->label->coords.x1 + cur_style.body.padding.hor + (cur_style.line.width >> 1);
			cur_area.y2 = letter_pos.y + ta_ext->label->coords.y1 + cur_style.body.padding.ver + letter_h;
			lv_draw_rect(&cur_area, mask, &cur_style);
		} else if(ta_ext->cursor.type == LV_CURSOR_BLOCK) {
			cur_area.x1 = letter_pos.x + ta_ext->label->coords.x1 - cur_style.body.padding.hor;
			cur_area.y1 = letter_pos.y + ta_ext->label->coords.y1 - cur_style.body.padding.ver;
			cur_area.x2 = letter_pos.x + ta_ext->label->coords.x1 + cur_style.body.padding.hor + letter_w;
			cur_area.y2 = letter_pos.y + ta_ext->label->coords.y1 + cur_style.body.padding.ver + letter_h;

			lv_draw_rect(&cur_area, mask, &cur_style);

			/*Get the current letter*/
#if LV_TXT_UTF8 == 0
			char letter_buf[2];
			letter_buf[0] = txt[byte_pos];
            letter_buf[1] = '\0';
#else
            char letter_buf[8] = {0};
            memcpy(letter_buf, &txt[byte_pos], lv_txt_utf8_size(txt[byte_pos]));
#endif
            cur_area.x1 += cur_style.body.padding.hor;
            cur_area.y1 += cur_style.body.padding.ver;
			lv_draw_label(&cur_area, mask, &cur_style, letter_buf, LV_TXT_FLAG_NONE, 0);

		} else if(ta_ext->cursor.type == LV_CURSOR_OUTLINE) {
			cur_area.x1 = letter_pos.x + ta_ext->label->coords.x1 - cur_style.body.padding.hor;
			cur_area.y1 = letter_pos.y + ta_ext->label->coords.y1 - cur_style.body.padding.ver;
			cur_area.x2 = letter_pos.x + ta_ext->label->coords.x1 + cur_style.body.padding.hor + letter_w;
			cur_area.y2 = letter_pos.y + ta_ext->label->coords.y1 + cur_style.body.padding.ver+ letter_h;

			cur_style.body.empty = 1;
			if(cur_style.body.border.width == 0) cur_style.body.border.width = 1; /*Be sure the border will be drawn*/
			lv_draw_rect(&cur_area, mask, &cur_style);
		} else if(ta_ext->cursor.type == LV_CURSOR_UNDERLINE) {
			cur_area.x1 = letter_pos.x + ta_ext->label->coords.x1 + cur_style.body.padding.hor;
			cur_area.y1 = letter_pos.y + ta_ext->label->coords.y1 + cur_style.body.padding.ver + letter_h - (cur_style.line.width >> 1);
			cur_area.x2 = letter_pos.x + ta_ext->label->coords.x1 + cur_style.body.padding.hor + letter_w;
			cur_area.y2 = letter_pos.y + ta_ext->label->coords.y1 + cur_style.body.padding.ver + letter_h + (cur_style.line.width >> 1) + (cur_style.line.width & 0x1);

			lv_draw_rect(&cur_area, mask, &cur_style);
		}

	}

	return true;
}

/**
 * Signal function of the text area
 * @param ta pointer to a text area object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_ta_signal(lv_obj_t * ta, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(ta, sign, param);
    if(res != LV_RES_OK) return res;

    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
    if(sign == LV_SIGNAL_CLEANUP) {
        if(ext->pwd_tmp != NULL) lv_mem_free(ext->pwd_tmp);

        /* (The created label will be deleted automatically) */
    } else if(sign == LV_SIGNAL_STYLE_CHG) {
        if(ext->label) {
            lv_obj_t * scrl = lv_page_get_scrl(ta);
            lv_style_t * style_ta = lv_obj_get_style(ta);
            lv_style_t * style_scrl = lv_obj_get_style(scrl);
            if(ext->one_line) {
                /*In one line mode refresh the Text Area height because 'vpad' can modify it*/
                lv_style_t * style_label = lv_obj_get_style(ext->label);
                lv_coord_t font_h =  lv_font_get_height_scale(style_label->text.font);
                lv_obj_set_height(ta, font_h + (style_ta->body.padding.ver + style_scrl->body.padding.ver) * 2);
            } else {
                /*In not one line mode refresh the Label width because 'hpad' can modify it*/
                lv_obj_set_width(ext->label, lv_obj_get_width(scrl) - 2 * style_scrl->body.padding.hor);
                lv_obj_set_pos(ext->label, style_scrl->body.padding.hor, style_scrl->body.padding.ver);         /*Be sure the Label is in the correct position*/
            }
            lv_label_set_text(ext->label, NULL);

        }
    } else if(sign == LV_SIGNAL_CORD_CHG) {
        /*Set the label width according to the text area width*/
        if(ext->label) {
            if(lv_obj_get_width(ta) != lv_area_get_width(param) ||
              lv_obj_get_height(ta) != lv_area_get_height(param)) {
                lv_obj_t * scrl = lv_page_get_scrl(ta);
                lv_style_t * style_scrl = lv_obj_get_style(scrl);
                lv_obj_set_width(ext->label, lv_obj_get_width(scrl) - 2 * style_scrl->body.padding.hor);
                lv_obj_set_pos(ext->label, style_scrl->body.padding.hor, style_scrl->body.padding.ver);
                lv_label_set_text(ext->label, NULL);    /*Refresh the label*/
            }
        }
    }
    else if (sign == LV_SIGNAL_CONTROLL) {
        uint32_t c = *((uint32_t*)param);       /*uint32_t because can be UTF-8*/
        if(c == LV_GROUP_KEY_RIGHT)  lv_ta_cursor_right(ta);
        else if(c == LV_GROUP_KEY_LEFT)  lv_ta_cursor_left(ta);
        else if(c == LV_GROUP_KEY_UP)  lv_ta_cursor_up(ta);
        else if(c == LV_GROUP_KEY_DOWN) lv_ta_cursor_down(ta);
        else {
#if LV_TXT_UTF8 != 0
            /*Swap the bytes (UTF-8 is big endian, but the MCUs are little endian)*/
            if((c & 0x80) == 0) {   /*ASCII*/
                lv_ta_add_char(ta, (char)c);
            }
            else {
                uint32_t swapped[2] = {0, 0};   /*the 2. element is the closing '\0'*/
                uint8_t c8[4];
                memcpy(c8, &c, 4);
                swapped[0] = (c8[0] << 24) + (c8[1] << 16) + (c8[2] << 8) + (c8[3]);
                char *p = (char*)swapped;
                uint8_t i;
                for(i = 0; i < 4; i++) {
                    if(p[0] == 0) p++; /*Ignore leading zeros (they were in the end originally)*/
                }
                lv_ta_add_text(ta, p);
            }
#else
            lv_ta_add_char(ta, (char)c);

#endif
        }
    }

    return res;
}

/**
 * Signal function of the scrollable part of the text area
 * @param scrl pointer to scrollable part of a text area object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_ta_scrollable_signal(lv_obj_t * scrl, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = scrl_signal(scrl, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_REFR_EXT_SIZE) {
        /*Set ext. size because the cursor might be out of this object*/
        lv_obj_t * ta = lv_obj_get_parent(scrl);
        lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
        lv_style_t * style_label = lv_obj_get_style(ext->label);
        lv_coord_t font_h = lv_font_get_height_scale(style_label->text.font);
        scrl->ext_size = LV_MATH_MAX(scrl->ext_size, style_label->text.line_space + font_h);
    }

    return res;
}

#if USE_LV_ANIMATION

/**
 * Called to blink the cursor
 * @param ta pointer to a text area
 * @param hide 1: hide the cursor, 0: show it
 */
static void cursor_blink_anim(lv_obj_t * ta, uint8_t show)
{
	lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
	if(show != ext->cursor.state) {
        ext->cursor.state = show == 0 ? 0 : 1;
        if(ext->cursor.type != LV_CURSOR_NONE &&
           (ext->cursor.type & LV_CURSOR_HIDDEN) == 0) {
            lv_obj_invalidate(ta);
        }
	}
}


/**
 * Dummy function to animate char hiding in pwd mode.
 * Does nothing, but a function is required in car hiding anim.
 * (pwd_char_hider callback do the real job)
 * @param ta unused
 * @param x unused
 */
static void pwd_char_hider_anim(lv_obj_t * ta, int32_t x)
{
    (void)ta;
    (void)x;
}

#endif

/**
 * Hide all characters (convert them to '*')
 * @param ta: pointer to text area object
 */
static void pwd_char_hider(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext_attr(ta);
    if(ext->pwd_mode != 0) {
        char * txt = lv_label_get_text(ext->label);
        int16_t len = lv_txt_get_length(txt);
        bool refr = false;
        uint16_t i;
        for(i = 0; i < len; i++) txt[i] = '*';

        txt[i] = '\0';

        if(refr != false) lv_label_set_text(ext->label, txt);
    }
}

#endif

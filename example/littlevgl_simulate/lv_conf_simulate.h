/**
 * @file lv_conf_simulate.h
 * 
 */

#ifndef LV_CONF_SIMULATE_H
#define LV_CONF_SIMULATE_H

/*----------------
 * Dynamic memory
 *----------------*/
/*
 * Memory size which will be used by the library
 * to store the graphical objects and other data
 */
#define LV_MEM_CUSTOM      0                /*1: use custom malloc/free, 0: use the built-in lv_mem_alloc/lv_mem_free*/
#if LV_MEM_CUSTOM == 0
#define LV_MEM_SIZE    (32U * 1024U)        /*Size memory used by mem_alloc (in bytes)*/
#define LV_MEM_ATTR                         /*Complier prefix for big array declaration*/
#define LV_MEM_AUTO_DEFRAG  1               /*Automatically defrag on free*/
#else       /*LV_MEM_CUSTOM*/
#define LV_MEM_CUSTOM_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
#define LV_MEM_CUSTOM_ALLOC   malloc       /*Wrapper to malloc*/
#define LV_MEM_CUSTOM_FREE    free         /*Wrapper to free*/
#endif     /*LV_MEM_CUSTOM*/

/*===================
   Graphical settings
 *=====================*/

/* Horizontal and vertical resolution of the library.*/
#define LV_HOR_RES          (480)
#define LV_VER_RES          (320)
#define LV_DPI              100

/* Size of internal graphics buffer (required for buffered drawing)
 * VDB means Virtual Display Buffer (the internal graphics buffer)
 * Set to 0 for unbuffered drawing
 * Set to >= LV_HOR_RES for buffered drawing if LV_ANTIALIAS = 0
 * Set to >= 2 * LV_HOR_RES for buffered drawing if LV_ANTIALIAS = 1
 * More info: https://littlevgl.com/basics#drawing-and-rendering*/
#define LV_VDB_SIZE         (20 * 1024)  /*Size of VDB in pixel count*/
#define LV_VDB_ADR          0            /*Place VDB to a specific address (e.g. in external RAM) (0: allocate automatically  into RAM)*/

/* Use two Virtual Display buffers (VDB) parallelize rendering and flushing (optional)
 * The flushing should use DMA to write the frame buffer in the background*/
#define LV_VDB_DOUBLE       0       /*1: Enable the use of 2 VDBs*/
#define LV_VDB2_ADR         0       /*Place VDB2 to a specific address (e.g. in external RAM) (0: allocate automatically into RAM)*/

/* Enable anti-aliasing
 * If enabled everything will be rendered in double size and filtered to normal size.
 * Fonts and Images will be downscaled */
#define LV_ANTIALIAS        1       /*1: Enable anti-aliasing*/

/* Enable anti-aliasing only for fonts (texts)
 * It downscales the fonts to half size so you should use double sized fonts
 * Much faster then normal anti-aliasing  */
#define LV_FONT_ANTIALIAS   0       /*1: Enable font anti-aliasing*/

/*Screen refresh settings*/
#define LV_REFR_PERIOD      50    /*Screen refresh period in milliseconds*/
#define LV_INV_FIFO_SIZE    32    /*The average count of objects on a screen */

/*=================
   Misc. setting
 *=================*/

/*Input device settings*/
#define LV_INDEV_READ_PERIOD            50                     /*Input device read period in milliseconds*/
#define LV_INDEV_POINT_MARKER           0                      /*Mark the pressed points*/
#define LV_INDEV_DRAG_LIMIT             10                     /*Drag threshold in pixels */
#define LV_INDEV_DRAG_THROW             20                     /*Drag throw slow-down in [%]. Greater value means faster slow-down */
#define LV_INDEV_LONG_PRESS_TIME        400                    /*Long press time in milliseconds*/
#define LV_INDEV_LONG_PRESS_REP_TIME    100                    /*Repeated trigger period in long press [ms] */

/*Color settings*/
#define LV_COLOR_DEPTH     24                     /*Color depth: 1/8/16/24*/
#define LV_COLOR_TRANSP    LV_COLOR_LIME          /*Images pixels with this color will not be drawn (chroma keying)*/

/*Text settings*/
#define LV_TXT_UTF8             1                /*Enable UTF-8 coded Unicode character usage */
#define LV_TXT_BREAK_CHARS     " ,.;:-_"         /*Can break texts on these chars*/

/*Graphics feature usage*/
#define USE_LV_ANIMATION        1               /*1: Enable all animations*/
#define USE_LV_SHADOW           1               /*1: Enable shadows*/
#define USE_LV_GROUP            1               /*1: Enable object groups (for keyboards)*/
#define USE_LV_GPU              1               /*1: Enable GPU interface*/
#define USE_LV_FILESYSTEM       1               /*1: Enable file system (required by images*/

/*================
 *  THEME USAGE
 *================*/
#define USE_LV_THEME_TEMPL      0       /*Just for test*/
#define USE_LV_THEME_DEFAULT    0       /*Built mainly from the built-in styles. Consumes very few RAM*/
#define USE_LV_THEME_ALIEN      0       /*Dark futuristic theme*/
#define USE_LV_THEME_NIGHT      0       /*Dark elegant theme*/
#define USE_LV_THEME_MONO       0       /*Mono color theme for monochrome displays*/
#define USE_LV_THEME_MATERIAL   0       /*Flat theme with bold colors and light shadows*/
#define USE_LV_THEME_ZEN        0       /*Peaceful, mainly light theme */

/*==================
 *    FONT USAGE
 *===================*/
/*More info about fonts: https://littlevgl.com/basics#fonts*/
#define LV_FONT_DEFAULT        &lv_font_dejavu_40     /*Always set a default font from the built-in fonts*/

#define USE_LV_FONT_DEJAVU_10              0
#define USE_LV_FONT_DEJAVU_10_SUP          0
#define USE_LV_FONT_DEJAVU_10_LATIN_EXT_A  0
#define USE_LV_FONT_DEJAVU_10_LATIN_EXT_B  0
#define USE_LV_FONT_DEJAVU_10_CYRILLIC     0
#define USE_LV_FONT_SYMBOL_10_BASIC        0
#define USE_LV_FONT_SYMBOL_10_FILE         0
#define USE_LV_FONT_SYMBOL_10_FEEDBACK     0

#define USE_LV_FONT_DEJAVU_20              0
#define USE_LV_FONT_DEJAVU_20_SUP          0
#define USE_LV_FONT_DEJAVU_20_LATIN_EXT_A  0
#define USE_LV_FONT_DEJAVU_20_LATIN_EXT_B  0
#define USE_LV_FONT_DEJAVU_20_CYRILLIC     0
#define USE_LV_FONT_SYMBOL_20_BASIC        0
#define USE_LV_FONT_SYMBOL_20_FILE         0
#define USE_LV_FONT_SYMBOL_20_FEEDBACK     0

#define USE_LV_FONT_DEJAVU_30              0
#define USE_LV_FONT_DEJAVU_30_SUP          0
#define USE_LV_FONT_DEJAVU_30_LATIN_EXT_A  0
#define USE_LV_FONT_DEJAVU_30_LATIN_EXT_B  0
#define USE_LV_FONT_DEJAVU_30_CYRILLIC     0
#define USE_LV_FONT_SYMBOL_30_BASIC        0
#define USE_LV_FONT_SYMBOL_30_FILE         0
#define USE_LV_FONT_SYMBOL_30_FEEDBACK     0

#define USE_LV_FONT_DEJAVU_40              1
#define USE_LV_FONT_DEJAVU_40_SUP          1
#define USE_LV_FONT_DEJAVU_40_LATIN_EXT_A  1
#define USE_LV_FONT_DEJAVU_40_LATIN_EXT_B  1
#define USE_LV_FONT_DEJAVU_40_CYRILLIC     1
#define USE_LV_FONT_SYMBOL_40_BASIC        1
#define USE_LV_FONT_SYMBOL_40_FILE         1
#define USE_LV_FONT_SYMBOL_40_FEEDBACK     1

#define USE_LV_FONT_DEJAVU_60              0
#define USE_LV_FONT_DEJAVU_60_SUP          0
#define USE_LV_FONT_DEJAVU_60_LATIN_EXT_A  0
#define USE_LV_FONT_DEJAVU_60_LATIN_EXT_B  0
#define USE_LV_FONT_DEJAVU_60_CYRILLIC     0
#define USE_LV_FONT_SYMBOL_60_BASIC        0
#define USE_LV_FONT_SYMBOL_60_FILE         0
#define USE_LV_FONT_SYMBOL_60_FEEDBACK     0

#define USE_LV_FONT_DEJAVU_80              0
#define USE_LV_FONT_DEJAVU_80_SUP          0
#define USE_LV_FONT_DEJAVU_80_LATIN_EXT_A  0
#define USE_LV_FONT_DEJAVU_80_LATIN_EXT_B  0
#define USE_LV_FONT_DEJAVU_80_CYRILLIC     0
#define USE_LV_FONT_SYMBOL_80_BASIC        0
#define USE_LV_FONT_SYMBOL_80_FILE         0
#define USE_LV_FONT_SYMBOL_80_FEEDBACK     0

/*===================
 *  LV_OBJ SETTINGS
 *==================*/
#define LV_OBJ_FREE_NUM_TYPE    uint32_t    /*Type of free number attribute (comment out disable free number)*/
#define LV_OBJ_FREE_PTR         1           /*Enable the free pointer attribute*/

/*==================
 *  LV OBJ X USAGE 
 *================*/
/*
 * Documentation of the object types: https://littlevgl.com/object-types
 */

/*****************
 * Simple object
 *****************/

/*Label (dependencies: -*/
#define USE_LV_LABEL    1
#if USE_LV_LABEL != 0
#define LV_LABEL_SCROLL_SPEED       25     /*Hor, or ver. scroll speed (px/sec) in 'LV_LABEL_LONG_SCROLL/ROLL' mode*/
#endif

/*Image (dependencies: lv_label*/
#define USE_LV_IMG      1

/*Line (dependencies: -*/
#define USE_LV_LINE     1

/*******************
 * Container objects
 *******************/

/*Container (dependencies: -*/
#define USE_LV_CONT     1

/*Page (dependencies: lv_cont)*/
#define USE_LV_PAGE     1

/*Window (dependencies: lv_cont, lv_btn, lv_label, lv_img, lv_page)*/
#define USE_LV_WIN      1

/*Tab (dependencies: lv_page, lv_btnm)*/
#define USE_LV_TABVIEW      1
#if USE_LV_TABVIEW != 0
#define LV_TABVIEW_ANIM_TIME    300     /*Time of slide animation [ms] (0: no animation)*/
#endif

/*************************
 * Data visualizer objects
 *************************/

/*Bar (dependencies: -)*/
#define USE_LV_BAR      1

/*Line meter (dependencies: *;)*/
#define USE_LV_LMETER   1

/*Gauge (dependencies:bar, lmeter)*/
#define USE_LV_GAUGE    1

/*Chart (dependencies: -)*/
#define USE_LV_CHART    1

/*LED (dependencies: -)*/
#define USE_LV_LED      1

/*Message box (dependencies: lv_rect, lv_btnm, lv_label)*/
#define USE_LV_MBOX     1

/*Text area (dependencies: lv_label, lv_page)*/
#define USE_LV_TA       1
#if USE_LV_TA != 0
#define LV_TA_CURSOR_BLINK_TIME 400     /*ms*/
#define LV_TA_PWD_SHOW_TIME     1500    /*ms*/
#endif

/*************************
 * User input objects
 *************************/

/*Button (dependencies: lv_cont*/
#define USE_LV_BTN      1

/*Button matrix (dependencies: -)*/
#define USE_LV_BTNM     1

/*Keyboard (dependencies: lv_btnm)*/
#define USE_LV_KB       1

/*Check box (dependencies: lv_btn, lv_label)*/
#define USE_LV_CB       1

/*List (dependencies: lv_page, lv_btn, lv_label, (lv_img optionally for icons ))*/
#define USE_LV_LIST     1
#if USE_LV_LIST != 0
#define LV_LIST_FOCUS_TIME  100 /*Default animation time of focusing to a list element [ms] (0: no animation)  */
#endif

/*Drop down list (dependencies: lv_page, lv_label)*/
#define USE_LV_DDLIST    1
#if USE_LV_DDLIST != 0
#define LV_DDLIST_ANIM_TIME     200     /*Open and close default animation time [ms] (0: no animation)*/
#endif

/*Roller (dependencies: lv_ddlist)*/
#define USE_LV_ROLLER    1
#if USE_LV_ROLLER != 0
#define LV_ROLLER_ANIM_TIME     200     /*Focus animation time [ms] (0: no animation)*/
#endif

/*Slider (dependencies: lv_bar)*/
#define USE_LV_SLIDER    1

/*Switch (dependencies: lv_slider)*/
#define USE_LV_SW       1

#endif /*LV_CONF_SIMULATE_H*/

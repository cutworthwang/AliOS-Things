#include "../../../lv_conf.h"
#if USE_LV_FONT_SYMBOL_10_FILE

#include <stdint.h>
#include "../lv_font.h"

/*Store the image of the letters (glyph) */
static const uint8_t symbol_10_file_bitmap[] = 
{
    // ASCII: 57376, char width: 9
    0x00, 0x00,  // ---------.......
    0x03, 0x80,  // ------OOO.......
    0x1f, 0x80,  // ---OOOOOO.......
    0x1f, 0x80,  // ---OOOOOO.......
    0x1c, 0x80,  // ---OOO--O.......
    0x10, 0x80,  // ---O----O.......
    0x10, 0x80,  // ---O----O.......
    0x17, 0x80,  // ---O-OOOO.......
    0xf3, 0x00,  // OOOO--OO-.......
    0xf0, 0x00,  // OOOO-----.......

    // ASCII: 57377, char width: 11
    0x00, 0x00,  // -----------.....
    0xff, 0xe0,  // OOOOOOOOOOO.....
    0xa0, 0xa0,  // O-O-----O-O.....
    0xe0, 0xe0,  // OOO-----OOO.....
    0xa0, 0xa0,  // O-O-----O-O.....
    0xff, 0xe0,  // OOOOOOOOOOO.....
    0xa0, 0xa0,  // O-O-----O-O.....
    0xe0, 0xe0,  // OOO-----OOO.....
    0xa0, 0xa0,  // O-O-----O-O.....
    0xa0, 0xa0,  // O-O-----O-O.....

    // ASCII: 57378, char width: 8
    0x00,  // --------
    0x3c,  // --OOOO--
    0xff,  // OOOOOOOO
    0x02,  // ------O-
    0x02,  // ------O-
    0x76,  // -OOO-OO-
    0x76,  // -OOO-OO-
    0x66,  // -OO--OO-
    0x02,  // ------O-
    0x7e,  // -OOOOOO-

    // ASCII: 57379, char width: 9
    0x0c, 0x00,  // ----OO---.......
    0x0c, 0x00,  // ----OO---.......
    0x0c, 0x00,  // ----OO---.......
    0x3e, 0x00,  // --OOOOO--.......
    0x1e, 0x00,  // ---OOOO--.......
    0x0c, 0x00,  // ----OO---.......
    0xf3, 0x80,  // OOOO--OOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0x00, 0x00,  // ---------.......

    // ASCII: 57380, char width: 9
    0x00, 0x00,  // ---------.......
    0x00, 0x00,  // ---------.......
    0x7e, 0x00,  // -OOOOOO--.......
    0x43, 0x00,  // -O----OO-.......
    0xc1, 0x00,  // OO-----O-.......
    0x81, 0x80,  // O------OO.......
    0xe7, 0x80,  // OOO--OOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0x00, 0x00,  // ---------.......

    // ASCII: 57381, char width: 11
    0x00, 0x00,  // -----------.....
    0xff, 0xe0,  // OOOOOOOOOOO.....
    0x80, 0x20,  // O---------O.....
    0xa0, 0x20,  // O-O-------O.....
    0xa3, 0x20,  // O-O---OO--O.....
    0x87, 0xa0,  // O----OOOO-O.....
    0xbf, 0xa0,  // O-OOOOOOO-O.....
    0xbf, 0xa0,  // O-OOOOOOO-O.....
    0xbf, 0xa0,  // O-OOOOOOO-O.....
    0xff, 0xc0,  // OOOOOOOOOO-.....

    // ASCII: 57382, char width: 9
    0x00, 0x00,  // ---------.......
    0x02, 0x00,  // ------O--.......
    0x07, 0x00,  // -----OOO-.......
    0x0f, 0x00,  // ----OOOO-.......
    0x1e, 0x00,  // ---OOOO--.......
    0x3e, 0x00,  // --OOOOO--.......
    0x7c, 0x00,  // -OOOOO---.......
    0xf8, 0x00,  // OOOOO----.......
    0xb0, 0x00,  // O-OO-----.......
    0xe0, 0x00,  // OOO------.......

    // ASCII: 57383, char width: 6
    0x00,  // ------..
    0x84,  // O----O..
    0x8c,  // O---OO..
    0x9c,  // O--OOO..
    0xfc,  // OOOOOO..
    0xfc,  // OOOOOO..
    0xbc,  // O-OOOO..
    0x9c,  // O--OOO..
    0x8c,  // O---OO..
    0x00,  // ------..

    // ASCII: 57384, char width: 8
    0x00,  // --------
    0x80,  // O-------
    0xe0,  // OOO-----
    0xf8,  // OOOOO---
    0xfc,  // OOOOOO--
    0xff,  // OOOOOOOO
    0xfc,  // OOOOOO--
    0xf0,  // OOOO----
    0xe0,  // OOO-----
    0x80,  // O-------

    // ASCII: 57385, char width: 9
    0x00, 0x00,  // ---------.......
    0xf7, 0x80,  // OOOO-OOOO.......
    0xf7, 0x80,  // OOOO-OOOO.......
    0xf7, 0x80,  // OOOO-OOOO.......
    0xf7, 0x80,  // OOOO-OOOO.......
    0xf7, 0x80,  // OOOO-OOOO.......
    0xf7, 0x80,  // OOOO-OOOO.......
    0xf7, 0x80,  // OOOO-OOOO.......
    0xf7, 0x80,  // OOOO-OOOO.......
    0xe7, 0x00,  // OOO--OOO-.......

    // ASCII: 57386, char width: 9
    0x00, 0x00,  // ---------.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x00,  // OOOOOOOO-.......

    // ASCII: 57387, char width: 6
    0x00,  // ------..
    0x08,  // ----O-..
    0x88,  // O---O-..
    0xc8,  // OO--O-..
    0xf8,  // OOOOO-..
    0xf8,  // OOOOO-..
    0xe8,  // OOO-O-..
    0xc8,  // OO--O-..
    0x88,  // O---O-..
    0x00,  // ------..

    // ASCII: 57388, char width: 9
    0x00, 0x00,  // ---------.......
    0x00, 0x00,  // ---------.......
    0x18, 0x00,  // ---OO----.......
    0x3c, 0x00,  // --OOOO---.......
    0x7e, 0x00,  // -OOOOOO--.......
    0xff, 0x00,  // OOOOOOOO-.......
    0x00, 0x00,  // ---------.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0x00, 0x00,  // ---------.......

    // ASCII: 57389, char width: 10
    0x00, 0x00,  // ----------......
    0x00, 0x80,  // --------O-......
    0xe7, 0xc0,  // OOO--OOOOO......
    0x3c, 0x80,  // --OOOO--O-......
    0x18, 0x00,  // ---OO-----......
    0x18, 0x00,  // ---OO-----......
    0x3c, 0x80,  // --OOOO--O-......
    0xe7, 0xc0,  // OOO--OOOOO......
    0x00, 0x80,  // --------O-......
    0x00, 0x00,  // ----------......

    // ASCII: 57390, char width: 11
    0x00, 0x00,  // -----------.....
    0x00, 0x00,  // -----------.....
    0x00, 0x00,  // -----------.....
    0x6f, 0x80,  // -OO-OOOOO--.....
    0xf0, 0x80,  // OOOO----O--.....
    0x70, 0x80,  // -OOO----O--.....
    0x21, 0xe0,  // --O----OOOO.....
    0x31, 0xc0,  // --OO---OOO-.....
    0x3e, 0x80,  // --OOOOO-O--.....
    0x00, 0x00,  // -----------.....

    // ASCII: 57391, char width: 9
    0x00, 0x00,  // ---------.......
    0xf0, 0x00,  // OOOO-----.......
    0xf8, 0x00,  // OOOOO----.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0x00, 0x00,  // ---------.......

    // ASCII: 57392, char width: 9
    0x00, 0x00,  // ---------.......
    0x0c, 0x00,  // ----OO---.......
    0x1e, 0x00,  // ---OOOO--.......
    0x3f, 0x00,  // --OOOOOO-.......
    0x0c, 0x00,  // ----OO---.......
    0x0c, 0x00,  // ----OO---.......
    0x0c, 0x00,  // ----OO---.......
    0xf3, 0x80,  // OOOO--OOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......

    // ASCII: 57393, char width: 10
    0x00, 0x00,  // ----------......
    0x00, 0x00,  // ----------......
    0xe0, 0x00,  // OOO-------......
    0x90, 0x00,  // O--O------......
    0xf0, 0x00,  // OOOO------......
    0x08, 0x00,  // ----O-----......
    0x7c, 0x00,  // -OOOOO----......
    0x92, 0x00,  // O--O--O---......
    0x90, 0x40,  // O--O-----O......
    0xe0, 0x00,  // OOO-------......

    // ASCII: 57394, char width: 10
    0x1e, 0x00,  // ---OOOO---......
    0x32, 0x00,  // --OO--O---......
    0x53, 0x80,  // -O-O--OOO-......
    0xf3, 0x40,  // OOOO--OO-O......
    0x84, 0x40,  // O----O---O......
    0x8e, 0x40,  // O---OOO--O......
    0x80, 0x40,  // O--------O......
    0xf8, 0x40,  // OOOOO----O......
    0x00, 0x40,  // ---------O......
    0x00, 0x40,  // ---------O......

    // ASCII: 57395, char width: 9
    0x00, 0x00,  // ---------.......
    0xfe, 0x00,  // OOOOOOO--.......
    0xf5, 0x00,  // OOOO-O-O-.......
    0xf4, 0x80,  // OOOO-O--O.......
    0xbc, 0x80,  // O-OOOO--O.......
    0x80, 0x80,  // O-------O.......
    0xbe, 0x80,  // O-OOOOO-O.......
    0xc0, 0x80,  // OO------O.......
    0xc0, 0x80,  // OO------O.......
    0xff, 0x80,  // OOOOOOOOO.......

    // ASCII: 57396, char width: 9
    0x78, 0x00,  // -OOOO----.......
    0xfa, 0x00,  // OOOOO-O--.......
    0xfb, 0x00,  // OOOOO-OO-.......
    0xf8, 0x00,  // OOOOO----.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
    0xff, 0x80,  // OOOOOOOOO.......
};

/*Store the start index of the glyphs in the bitmap array*/
static const uint32_t symbol_10_file_map[] = 
{
     0, 20, 40, 50, 70, 90, 110, 130, 
    140, 150, 170, 190, 200, 220, 240, 260, 
    280, 300, 320, 340, 360, 
};

/*Store the width (column count) of each glyph*/
static const uint8_t symbol_10_file_width[] = 
{
     9, 11,  8,  9,  9, 11,  9,  6, 
     8,  9,  9,  6,  9, 10, 11,  9, 
     9, 10, 10,  9,  9, 
};

lv_font_t lv_font_symbol_10_file = 
{
#if LV_TXT_UTF8 == 0
    224,        /*First letter's unicode */
    255,        /*Last letter's unicode */
#else
    57376,        /*First letter's unicode */
    57397,        /*Last letter's unicode */
#endif
    10,        /*Letters height (rows) */
    symbol_10_file_bitmap,    /*Glyph's bitmap*/
    symbol_10_file_map,    /*Glyph start indexes in the bitmap*/
    symbol_10_file_width,    /*Glyph widths (columns)*/
    NULL         /*No next page by default*/
};

#endif /*USE_LV_FONT_SYMBOL_10_FILE*/

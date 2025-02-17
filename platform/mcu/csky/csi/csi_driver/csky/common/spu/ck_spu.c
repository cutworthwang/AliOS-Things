/* ---------------------------------------------------------------------------
 * Copyright (C) 2016 CSKY Limited. All rights reserved.
 *
 * Redistribution and use of this software in source and binary forms,
 * with or without modification, are permitted provided that the following
 * conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *   * Neither the name of CSKY Ltd. nor the names of CSKY's contributors may
 *     be used to endorse or promote products derived from this software without
 *     specific prior written permission of CSKY Ltd.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 * -------------------------------------------------------------------------- */

/******************************************************************************
 * @file
 * @brief
 * @version  V1.0
 * @date
 ******************************************************************************/

#include <csi_config.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "csi_core.h"
#include "drv_spu.h"
#include "ck_spu.h"

#include "soc.h"
#include "spu_pin_planning.h"

#define ERR_SPU(errno) (CSI_DRV_ERRNO_SPU_BASE | errno)

extern spu_pgpio_info_t get_pgpio_info(uint32_t spu_idx, uint8_t pgpio);
extern void ck_spu_usart_irqhandler(int32_t idx,spu_dev_signal_e signal);
extern void ck_spu_spi_irqhandler(int32_t idx,spu_dev_signal_e signal);
extern void ck_spu_i2c_irqhandler(int32_t idx,spu_dev_signal_e signal);
extern void ck_spu_can_irqhandler(int32_t idx,spu_dev_signal_e signal);

//------------------------------------------
//            data private
//------------------------------------------
typedef struct {
    uint32_t base;
    uint32_t irq;

    uint32_t clk_en_vec;
    bool clk_en;
    uint32_t prog_used;
    uint32_t usart_func_index_base;
    uint32_t spi_func_index_base;
    uint32_t i2c_func_index_base;
    uint32_t can_func_index_base;
} ck_spu_priv_t;

static ck_spu_priv_t spu_instance[CONFIG_SPU_NUM];

static const spu_capabilities_t spu_capabilities = {
    .spu_usart = 1,   //supports USART device
    .spu_spi   = 1,   //supports SPI device
    .spu_i2c   = 1,   //supports I2C device
    .spu_can   = 0,   //supports CAN device
};

//usart function
#define SPU_USART_FUNC_WORDS 180
#define SPU_USART_FUNC_BYTES SPU_USART_FUNC_WORDS*4
const static int32_t SPU_USART_FUNC_INDEX[16] = {
0x00000000,0x00000090,0x000000ba,0x00000000,
0x00000000,0x000000d8,0x00000184,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
};
const static int32_t SPU_USART_FUNC[180] = {
0x2000d84d,0x3acd74c8,0xd80d082c,0xd82d2003,
0xe0002004,0x3ac800ef,0x4b620817,0x08083ac9,
0x08053aca,0x080a3acb,0x08093acc,0x36003ba4,
0xb680a664,0x3b848ea1,0x35010403,0x714c3500,
0x23006d14,0x44814362,0x6420c104,0xe00003ce,
0x0c1100db,0x2002d8ed,0xdced3fa3,0x040b2002,
0x6024c120,0xc1243cbd,0x03d46420,0x08033ac8,
0x37002303,0xc1466d8c,0x751d6420,0x6dde4ff0,
0x3c40691c,0x34010c04,0x6420c164,0x4c20c000,
0x2000d84d,0xd80d4a50,0xd82d2003,0xe0002004,
0x080300a5,0x040213dd,0x34ff13dd,0x6420c104,
0xc1466d88,0xc0006420,0x340e4c20,0x6420c144,
0x2002d8ed,0xdced3fa2,0x3fd22002,0x34010804,
0x6420c164,0x4c20c000,0x2000d8cd,0x4b6274d8,
0xc1003420,0x3ec86022,0x2300081f,0x7091610e,
0x08043ec9,0x0c0a3eca,0x35003ba4,0xb540b561,
0x3cd09580,0x7491082a,0x34010411,0x710c2b00,
0x3ecb6908,0x3c400804,0x04030820,0x0c1d3c40,
0x68906d12,0x610e0403,0xd80d7091,0xd82d2003,
0xe0002004,0xd88d006f,0x2c002001,0x2001dc8d,
0x0072e000,0xd8ed0c06,0x3fa12002,0x2002dced,
0x4ed012b4,0x040a6d58,0x2002d8ed,0xdced3fa4,
0x12ad2002,0x230374d8,0x751d6d4c,0x6dde4ff0,
0x3c40691c,0x34010c04,0x6420c164,0x6420c145,
0x4c20c000,0x2000d8cd,0x3ec874d8,0x23030802,
0x26004ed2,0x715912a4,0x6027c100,0x6dde6dd4,
0x0c073f40,0x2002d8ed,0xdced3fa5,0x040a2002,
0x2001d88d,0x080a3c40,0x2002d8ed,0xdced3fa0,
0xc1402002,0x3e806026,0x11d30403,0x751d6d8c,
0x6dde4ff0,0x3c40691c,0x34010c04,0x6420c164,
0x6420c146,0x4c20c000,0x75457501,0x783c6552,
0x69004898,0x611449b0,0x94804482,0xdc0d2000,
0x783c1006,0x75115981,0x755448b0,0x783c6514,
0x69044898,0x611449b0,0xb4404482,0xdc2d2100,
0x783c1008,0x75115981,0x755448b0,0x783c6515,
0x31003000,0x33003200,0x35003400,0x37003600,
0xc002105a,0xc0006421,0x3aa66022,0xc0023aa8,
0x10d76420,0x5e8d3340,0x24033500,0x6518b4a0,
0x10f40bfd,0x78001014,0x6c036c03,0x6c036c03,
0x10726c03,0x0c0764c2,0x6021c000,0x642bc001,
0xb020100b,0x642bc001,0xb0201009,0x000007fc,
0xc1e71003,0xe1e71201,0xc0e31083,0xc0e31783,
0xc0e35281,0xffffffff,0x00000000,0x6000fff8,
0x000003c2,0x0000040c,0x1324ffbb,0x6c036c03,
0x6c036c03,0x6c036c03,0x6c036c03,0x00000000
};

//spi function
#define SPU_SPI_FUNC_WORDS 816
#define SPU_SPI_FUNC_BYTES SPU_SPI_FUNC_WORDS*4
const static int32_t SPU_SPI_FUNC_INDEX[128]={
0x00000000,0x00000048,0x000000a2,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x0000016c,0x000001b4,0x0000020e,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x000002d8,0x000002ee,0x0000036e,0x000003fe,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000498,0x000004ae,0x00000536,0x00000590,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x0000062a,0x00000634,0x000006ac,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000762,0x00000000,
0x00000776,0x00000780,0x000007f8,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x000008ea,0x00000000,
0x000008fe,0x000009a8,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x0000090e,0x00000000,
0x00000a5e,0x00000b08,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000b6e,0x00000000
};
const static int32_t SPU_SPI_FUNC[816] = {
0x2001d84d,0x2000d8cd,0x0c0d3ac0,0x6d180093,
0x6421c144,0x2005d80d,0x2006d82d,0x05d5e000,
0x6421c104,0x0c053ac1,0x6d180099,0x6422c144,
0xc1043400,0x34036423,0x6d5800bc,0x6420c144,
0x6423c145,0x4c20c000,0xc1463602,0xd8ed6420,
0x3fc12001,0xd84d0c1b,0x75082000,0x24004c82,
0x6c463100,0x0c023adc,0x70507051,0xc1026c46,
0x68846022,0x2007d80d,0x2008d82d,0x05b5e000,
0x2003d88d,0xdc8d2c00,0x3fc22003,0x34fe0803,
0x34000402,0x6423c104,0xc14601d3,0xc0006423,
0x37004c20,0x2001d84d,0x0c2c3ac0,0x2005d80d,
0x2006d82d,0x0585e000,0x3ac30810,0x340e0805,
0x6423c144,0x34000404,0x6423c104,0x2004d8ed,
0xdced3fa2,0x04162004,0x0577e000,0x6421c104,
0x02a63403,0x2000d8cd,0xc1456d58,0xc1446423,
0xe0006420,0x0c060574,0x2004d8ed,0xdced3fa3,
0x3ac12004,0xd88d0c29,0x3c402003,0x3ac30810,
0x340e0805,0x6423c144,0x34000404,0x6423c104,
0x2004d8ed,0xdced3fa0,0x04162004,0x02b93403,
0x2000d8cd,0xc1456d58,0xc1446423,0xd80d6420,
0xd82d2007,0xe0002008,0x0c06055a,0x2004d8ed,
0xdced3fa1,0x751d2004,0x6dde4ff0,0x3c40691c,
0x34010c04,0x6423c164,0x4c20c000,0x2001d84d,
0x2000d8cd,0x0c0d3ac0,0x6d18038a,0x6421c144,
0x2005d80d,0x2006d82d,0x051fe000,0x6421c104,
0x0c053ac1,0x6d180390,0x6422c144,0xc1043400,
0x340f6423,0x6d5803b3,0x6420c144,0x6423c145,
0x4c20c000,0xc146360e,0xd8ed6420,0x3fc12001,
0xd84d0c1b,0x75082000,0x24004c82,0x6c463100,
0x0c023adc,0x70507051,0xc1026c46,0x68846022,
0x2007d80d,0x2008d82d,0x04ffe000,0x2003d88d,
0xdc8d2c00,0x3fc22003,0x34fe0803,0x34000402,
0x6423c104,0xc14613d1,0xc0006423,0x37004c20,
0x2001d84d,0x0c2c3ac0,0x2005d80d,0x2006d82d,
0x04cfe000,0x3ac30810,0x340e0805,0x6423c144,
0x34000404,0x6423c104,0x2004d8ed,0xdced3fa2,
0x04162004,0x04c1e000,0x6421c104,0x13a2340f,
0x2000d8cd,0xc1456d58,0xc1446423,0xe0006420,
0x0c0604be,0x2004d8ed,0xdced3fa3,0x3ac12004,
0xd88d0c29,0x3c402003,0x3ac30810,0x340e0805,
0x6423c144,0x34000404,0x6423c104,0x2004d8ed,
0xdced3fa0,0x04162004,0x12af340f,0x2000d8cd,
0xc1456d58,0xc1446423,0xd80d6420,0xd82d2007,
0xe0002008,0x0c0604a4,0x2004d8ed,0xdced3fa1,
0x751d2004,0x6dde4ff0,0x3c40691c,0x34010c04,
0x6423c164,0x4c20c000,0xc1043400,0x11bf6423,
0xc1453602,0xc1466423,0xc0006420,0x37004c20,
0x003bea88,0x6420c148,0x2001d84d,0x2000d8cd,
0x0c153ac0,0x2005d80d,0x2006d82d,0x045de000,
0x6421c104,0x6d181193,0x6421c144,0x045fe000,
0xd8ed0c06,0x3fa32004,0x2004dced,0x0c113ac1,
0x6d18118d,0x6422c144,0x2007d80d,0x2008d82d,
0x045de000,0xd8ed0c06,0x3fa12004,0x2004dced,
0x4ff0751d,0x691c6dde,0x0c043c40,0xc1643401,
0x11a26423,0xc1456d58,0xc0006423,0x36024c20,
0x6420c146,0x2001d8ed,0x0c1b3fc1,0x2000d84d,
0x4c827508,0x31002400,0x3adc6c46,0x70510c02,
0x6c467050,0x6022c102,0xd80d6884,0xd82d2007,
0xe0002008,0xd88d0422,0x2c002003,0x2003dc8d,
0x3fc210cf,0x34fe081e,0x0000041d,0x80008803,
0x80008001,0x80009803,0xc0001087,0x80004003,
0x80004001,0x80005003,0xc0001003,0x00000007,
0x8000409f,0x8000c09d,0x8000589f,0xc0001107,
0xc1043400,0xc1466423,0xc0006423,0x37004c20,
0x2001d84d,0x0c203ac0,0x2005d80d,0x2006d82d,
0x03d7e000,0x3ac30810,0x340e0805,0x6423c144,
0x34000404,0x6423c104,0x2004d8ed,0xdced3fa2,
0x040a2004,0xc1043400,0x34026423,0xc14500ae,
0xc1446423,0x3ac16420,0xd88d0c1d,0x3c402003,
0x3ac30810,0x340e0805,0x6423c144,0x34000404,
0x6423c104,0x2004d8ed,0xdced3fa0,0x040a2004,
0xc1043400,0x34026423,0xc14500bd,0xc1446423,
0x751d6420,0x6dde4ff0,0x3c40691c,0x34010c04,
0x6423c164,0x4c20c000,0xc1043400,0x01a66423,
0xc145360e,0xc1466423,0xc0006420,0x37004c20,
0x00d5ea88,0x6420c148,0x2001d84d,0x2000d8cd,
0x0c193ac0,0x2005d80d,0x2006d82d,0x037de000,
0x6421c104,0x6d180192,0x6421c144,0x2005d80d,
0x2006d82d,0x037be000,0xd8ed0c06,0x3fa32004,
0x2004dced,0x0c113ac1,0x6d18019a,0x6422c144,
0x2007d80d,0x2008d82d,0x0379e000,0xd8ed0c06,
0x3fa12004,0x2004dced,0x4ff0751d,0x691c6dde,
0x0c043c40,0xc1643401,0x02a56423,0xc1456d58,
0xc0006423,0x360e4c20,0x6420c146,0x2001d8ed,
0x0c1b3fc1,0x2000d84d,0x4c827508,0x31002400,
0x3adc6c46,0x70510c02,0x6c467050,0x6022c102,
0xd80d6884,0xd82d2007,0xe0002008,0xd88d033e,
0x2c002003,0x2003dc8d,0x3fc202d8,0x34fe0803,
0x34000402,0x6423c104,0x6423c146,0x4c20c000,
0xd84d3700,0x3ac02001,0xd80d0c20,0xd82d2005,
0xe0002006,0x0810030e,0x08053ac3,0xc144340e,
0x04046423,0xc1043400,0xd8ed6423,0x3fa22004,
0x2004dced,0x3400040a,0x6423c104,0x03b2340e,
0x6423c145,0x6420c144,0x0c1d3ac1,0x2003d88d,
0x08103c40,0x08053ac3,0xc144340e,0x04046423,
0xc1043400,0xd8ed6423,0x3fa02004,0x2004dced,
0x3400040a,0x6423c104,0x13be340e,0x6423c145,
0x6420c144,0x4ff0751d,0x691c6dde,0x0c043c40,
0xc1643401,0xc0006423,0x139c4c20,0x6423c144,
0x4c20c000,0xd8cd3700,0xd84d2000,0x3ac02001,
0x13970c20,0xc1446d18,0xd80d6422,0xd82d2005,
0xe0002006,0x080902b6,0x2004d8ed,0xdced3fa2,
0xd88d2004,0x040b2002,0x02b9e000,0xd8ed0c06,
0x3fa32004,0x2004dced,0x02a7e000,0x6422c104,
0x0c063ac1,0x6d181387,0x6421c144,0x751d3700,
0x6dde4ff0,0x3c40691c,0x34010c04,0x6423c164,
0x6d181381,0x6423c144,0x4c20c000,0xd8cd3700,
0xd84d2000,0x3ac02001,0xd80d0c1c,0xd82d2005,
0xe0002006,0x0809027e,0x2004d8ed,0xdced3fa2,
0xd88d2004,0x040b2002,0x0281e000,0xd8ed0c06,
0x3fa32004,0x2004dced,0x026fe000,0x6422c104,
0x0c293ac1,0x4c827518,0x31002400,0x3edc6c46,
0x70510c02,0x6c467050,0x6022c101,0xd80d6884,
0xd82d2007,0xe0002008,0xd88d0268,0x2c002003,
0x2003dc8d,0x08073c40,0x2004d8ed,0xdced3fa0,
0x04092004,0x0263e000,0xd8ed0c06,0x3fa12004,
0x2004dced,0x4ff0751d,0x691c6dde,0x0c043c40,
0xc1643401,0x11956423,0xc1446d18,0xc0006423,
0xd86d4c20,0x2b002005,0x2005dc6d,0xc144118b,
0xc0006423,0x11894c20,0x6423c144,0x4c20c000,
0xd8cd3700,0xd84d2000,0x3ac02001,0x11880c20,
0xc1446d18,0xd80d6422,0xd82d2005,0xe0002006,
0x08090210,0x2004d8ed,0xdced3fa2,0xd88d2004,
0x040b2002,0x0213e000,0xd8ed0c06,0x3fa32004,
0x2004dced,0x0201e000,0x6422c104,0x0c063ac1,
0x6d181098,0x6421c144,0x751d3700,0x6dde4ff0,
0x3c40691c,0x34010c04,0x6423c164,0x6d181092,
0x6423c144,0x4c20c000,0xd8cd3700,0x041e2000,
0xc0001003,0x0000000b,0x8000809f,0x8000009d,
0x8000909f,0xc0001107,0xe0201001,0x80008083,
0x80008081,0x80009881,0xa0009881,0x80004083,
0x80004081,0x80005081,0xd84d6c03,0x3ac02001,
0xd80d0c1c,0xd82d2005,0xe0002006,0x080901ba,
0x2004d8ed,0xdced3fa2,0xd88d2004,0x040b2002,
0x01bde000,0xd8ed0c06,0x3fa32004,0x2004dced,
0x01abe000,0x6422c104,0x0c293ac1,0x4c827518,
0x31002400,0x3edc6c46,0x70510c02,0x6c467050,
0x6022c101,0xd80d6884,0xd82d2007,0xe0002008,
0xd88d01a4,0x2c002003,0x2003dc8d,0x08073c40,
0x2004d8ed,0xdced3fa0,0x04092004,0x019fe000,
0xd8ed0c06,0x3fa12004,0x2004dced,0x4ff0751d,
0x691c6dde,0x0c043c40,0xc1643401,0x009d6423,
0xc1446d18,0xc0006423,0xd86d4c20,0x2b002005,
0x2005dc6d,0xc1440182,0xc0006423,0xd8cd4c20,
0x01842000,0xc1446d18,0xc0006423,0x37004c20,
0x6024c083,0x083c3cdf,0x2000d8cd,0x2001d84d,
0x0c203ac0,0x6d18018c,0x6422c144,0x2005d80d,
0x2006d82d,0x0145e000,0xd8ed0809,0x3fa22004,
0x2004dced,0x2002d88d,0xe000040b,0x0c060148,
0x2004d8ed,0xdced3fa3,0xe0002004,0xc1040136,
0x3ac16422,0x019b0c06,0xc1446d18,0x37006421,
0x4ff0751d,0x691c6dde,0x0c043c40,0xc1643401,
0x02816423,0xc1446d18,0xc0006423,0xd86d4c20,
0x2b002006,0x2006dc6d,0xd8cd028a,0x6d182000,
0x6423c144,0x4c20c000,0xd8cd3700,0xd84d2000,
0x3ac02001,0xd80d0c1c,0xd82d2005,0xe0002006,
0x08090100,0x2004d8ed,0xdced3fa2,0xd88d2004,
0x040b2002,0x0103e000,0xd8ed0c06,0x3fa32004,
0x2004dced,0x00f1e000,0x6422c104,0x0c293ac1,
0x4c827518,0x31002400,0x3edc6c46,0x70510c02,
0x6c467050,0x6022c101,0xd80d6884,0xd82d2007,
0xe0002008,0xd88d00ea,0x2c002003,0x2003dc8d,
0x08073c40,0x2004d8ed,0xdced3fa0,0x04092004,
0x00e5e000,0xd8ed0c06,0x3fa12004,0x2004dced,
0x4ff0751d,0x691c6dde,0x0c043c40,0xc1643401,
0x03946423,0xc1446d18,0xc0006423,0xd8cd4c20,
0x039c2000,0xc1446d18,0xc0006423,0x37004c20,
0x6024c083,0x083c3cdf,0x2000d8cd,0x2001d84d,
0x0c203ac0,0x6d18139f,0x6422c144,0x2005d80d,
0x2006d82d,0x0095e000,0xd8ed0809,0x3fa22004,
0x2004dced,0x2002d88d,0xe000040b,0x0c060098,
0x2004d8ed,0xdced3fa3,0xe0002004,0xc1040086,
0x3ac16422,0x13900c06,0xc1446d18,0x37006421,
0x4ff0751d,0x691c6dde,0x0c043c40,0xc1643401,
0x138a6423,0xc1446d18,0xc0006423,0xd86d4c20,
0x2b002006,0x2006dc6d,0x2000d8cd,0x6d18129c,
0x6423c144,0x4c20c000,0xd8cd3700,0xd84d2000,
0x3ac02001,0xd80d0c1c,0xd82d2005,0xe0002006,
0x08090050,0x2004d8ed,0xdced3fa2,0xd88d2004,
0x040b2002,0x0053e000,0xd8ed0c06,0x3fa32004,
0x2004dced,0x0041e000,0x6422c104,0x0c293ac1,
0x4c827518,0x31002400,0x3edc6c46,0x70510c02,
0x6c467050,0x6022c101,0xd80d6884,0xd82d2007,
0xe0002008,0xd88d003a,0x2c002003,0x2003dc8d,
0x08073c40,0x2004d8ed,0xdced3fa0,0x04092004,
0x0035e000,0xd8ed0c06,0x3fa12004,0x2004dced,
0x4ff0751d,0x691c6dde,0x0c043c40,0xc1643401,
0x11976423,0xc1446d18,0xc0006423,0x75014c20,
0x65527545,0x4898783c,0x49b06900,0x44826114,
0x20009480,0x100adc0d,0x5981783c,0x48b07511,
0x65147554,0x4898783c,0x49b06904,0x44826114,
0x2100b440,0x1010dc2d,0x5981783c,0x48b07511,
0x65157554,0x3000783c,0x32003100,0x34003300,
0x36003500,0x105f3700,0x6421c002,0x6022c000,
0x3aa83aa6,0x6420c002,0x334010db,0x35005e8d,
0xb4a02403,0x0bfd6518,0x101910f8,0x6c037800,
0x6c036c03,0x6c036c03,0x64c21076,0xc0000c07,
0xc0016021,0x1010642b,0xc001b020,0x100e642b,
0x07fcb020,0xa0005081,0xe0201001,0xa0001000,
0x80004003,0x8000c001,0x8000d801,0xa000d801,
0x80008003,0x80000001,0x80001001,0xa0001001,
0x00000000,0x6000fff8,0x00000f58,0x00000fb4,
0x1324ffbb,0x6c036c03,0x6c036c03,0x6c036c03,
0x6c036c03,0x00000000,0x00000000,0x00000000
};

//i2c function
#define SPU_I2C_FUNC_WORDS 228
#define SPU_I2C_FUNC_BYTES SPU_I2C_FUNC_WORDS*4
const static int32_t SPU_I2C_FUNC_INDEX[16]={
0x00000000,0x0000004c,0x00000072,0x00000082,
0x0000010c,0x00000192,0x00000194,0x000001b0,
0x000001d4,0x00000206,0x0000022e,0x00000246,
0x00000000,0x00000000,0x00000000,0x00000286,
};
const static int32_t SPU_I2C_FUNC[228] = {
0x0004d80d,0x38c03413,0xc1440c0d,0xc1206421,
0x3cbe6024,0x6420c124,0x2004d80d,0x2005d82d,
0x38c1044c,0xc1440c09,0xc1206421,0x3cbe6024,
0x6420c124,0x34000464,0x6420c104,0x38c20280,
0x3caa0c02,0x6420c144,0x4c20c000,0xc1443413,
0xd80d6421,0xd82d2004,0xe0002005,0xc104011e,
0x02886420,0x2000d8ad,0xc1446d14,0xc0006420,
0x028b4c20,0x2000d8ad,0xc1446d14,0xc0006420,
0xc1004c20,0x3cdf6024,0x34020c05,0x1006dc8d,
0xd84d0482,0x3ac50004,0xd80d08cb,0xd82d2004,
0xe0002005,0x080900f6,0x08293ac3,0x08933ac4,
0xdc8d3408,0x046f1006,0x00efe000,0x6420c104,
0x0004d8ad,0x0c043cc0,0xdcad3da5,0x03830004,
0x2000d8ad,0xc1446d14,0xe0006420,0x0cfc00e8,
0xdc8d3401,0xd8ad1006,0x6d561007,0x3c406914,
0x34010cf3,0x6420c164,0x4c20c000,0xd8ad038d,
0x6d142000,0x6420c144,0x4c20c000,0x2006d80d,
0x2007d82d,0x6022c100,0x00cfe000,0x2002d88d,
0xdc8d2c00,0x3c402002,0x34000c1b,0x6420c104,
0xd8ad0399,0x6d142000,0x6420c144,0x00c7e000,
0x34040ccb,0x1006dc8d,0x1007d8ad,0x69146d56,
0x0cc23c40,0xc1643401,0xc0006420,0x139c4c20,
0x2000d8ad,0xd80d6d14,0x38c40004,0x35000c09,
0x6420c105,0xc1443ca8,0xc0006420,0x350f4c20,
0x6420c105,0x6420c144,0xdc8d3408,0xc0001006,
0x07b54c20,0xc10434fe,0x138e6421,0x6421c144,
0xc10535fc,0x3cac6420,0x6420c144,0x4c20c000,
0x2003d88d,0x6d564cb0,0x69147511,0x0c043c40,
0xc1643401,0x340e6420,0x6420c144,0x6421c144,
0x4c20c000,0x6024c120,0xc1243c9e,0x340e6420,
0x6420c144,0xc1443402,0x34106421,0x1006dc8d,
0x1007d8ad,0x69146d56,0x0c6e3c40,0xc1643401,
0xc0006420,0x34134c20,0x6421c144,0x2004d80d,
0x2005d82d,0x0041e000,0xc1043cb7,0x128e6420,
0x2000d8ad,0xc1446d14,0xc0006420,0x34024c20,
0x6421c144,0x3cbf34ff,0x6420c104,0xc1441287,
0xc0006420,0xd80d4c20,0x38852001,0x0c0c38c2,
0x6024c120,0x44974c97,0x6d1448a8,0x6420c124,
0x6421c124,0xdc0d3882,0x34ff2001,0x6421c104,
0xc144119b,0x35016421,0x6420c105,0xc1443cac,
0xc0006420,0x34204c20,0x1006dc8d,0x75010792,
0x65527545,0x4898783c,0x49b06900,0x44826114,
0x20009480,0x1008dc0d,0x5981783c,0x48b07511,
0x65147554,0x4898783c,0x49b06904,0x44826114,
0x2100b440,0x100edc2d,0x5981783c,0x48b07511,
0x65157554,0xc000783c,0x30004c20,0x32003100,
0x34003300,0x36003500,0x105e3700,0x6421c002,
0x6022c000,0x3aa83aa6,0x6420c002,0x334010da,
0x35005e8d,0xb4a02403,0x0bfd6518,0x101810f7,
0x6c037800,0x6c036c03,0x6c036c03,0x64c21075,
0xc0000c07,0xc0016021,0x100f642b,0xc001b020,
0x100d642b,0x07fcb020,0xc0003003,0x901fb89f,
0x80009901,0x9000999d,0x801fba03,0x801fba83,
0xc000030b,0x901fbca3,0xc0001507,0xc0000007,
0x00000000,0x6000fff8,0x0000046c,0x000004c4,
0x1324ffbb,0x6c036c03,0x6c036c03,0x6c036c03,
0x6c036c03,0x00000000,0x00000000,0x00000000
};

//can function
#define SPU_CAN_FUNC_WORDS 0
#define SPU_CAN_FUNC_BYTES SPU_CAN_FUNC_WORDS*4
const static int32_t SPU_CAN_FUNC_INDEX[16]={0};
const static int32_t SPU_CAN_FUNC[1] = {0};

//------------------------------------------
//                  ISR
//------------------------------------------
extern void ck_spu_usart_irqhandler(int32_t idx,spu_dev_signal_e signal);
extern void ck_spu_spi_irqhandler(int32_t idx,spu_dev_signal_e signal);
extern void ck_spu_i2c_irqhandler(int32_t idx,spu_dev_signal_e signal);
extern void ck_spu_can_irqhandler(int32_t idx,spu_dev_signal_e signal);

void ck_spu_irqhandler(int32_t idx)
{
    ck_spu_priv_t *priv = &spu_instance[idx];

    //ISR of particular device
    int32_t base;
    int32_t data;
    int32_t intr_src;
    int32_t i;

    base = priv->base + GLOBAL_BASE;
    getData(data,base,OFFSET_PGPIO_INTR_STATUS);

    spu_pgpio_info_t pgpio_info;

    for(i=0; i<32; i++){
        intr_src = data & (1 << i);
        if(intr_src!=0){
            pgpio_info = get_pgpio_info(idx, i);
            switch(pgpio_info.dev_type){
                case ENUM_SPU_DEV_USART: ck_spu_usart_irqhandler(pgpio_info.dev_idx,pgpio_info.dev_signal); break;
                case ENUM_SPU_DEV_SPI: ck_spu_spi_irqhandler(pgpio_info.dev_idx,pgpio_info.dev_signal); break;
                case ENUM_SPU_DEV_I2C: ck_spu_i2c_irqhandler(pgpio_info.dev_idx,pgpio_info.dev_signal); break;
                //case ENUM_SPU_DEV_CAN: ck_spu_can_irqhandler(pgpio_info.dev_idx,pgpio_info.dev_signal); break;
                default: break;
            }
        }
    }
}

//------------------------------------------
//            CSI_DRIVER
//------------------------------------------
extern void csi_vic_enable_irq(int32_t irq_num);
extern void csi_vic_disable_irq(int32_t irq_num);
extern int32_t target_spu_init(int32_t pgpio[],uint32_t *base, uint32_t *irq);

spu_capabilities_t csi_spu_get_capabilities(spu_handle_t handle)
{
    return spu_capabilities;
}

spu_handle_t csi_spu_initialize(int32_t pgpio[])
{
    //get base address & irq
    int32_t idx;
    uint32_t base = 0u;
    uint32_t irq;
    ck_spu_priv_t *priv;

    idx = target_spu_init(pgpio, &base, &irq);

    if (idx<0){
        return NULL;
    }

    priv = &spu_instance[idx];
    priv->base = base;
    priv->irq  = irq;
    csi_vic_enable_irq(priv->irq);

    //initialize variable
    priv->clk_en_vec = 0;
    priv->clk_en = false;
    priv->prog_used = 0;

    return priv;
}

int32_t csi_spu_config(spu_handle_t handle,spu_capabilities_t capabilities)
{
    ck_spu_priv_t *priv = handle;

    //check if INST_RAM size enough or not
    uint32_t func_index_base = 256u;
    uint32_t inst_len = 0u;

    if(capabilities.spu_usart==1){
        priv->usart_func_index_base = func_index_base;
        func_index_base+=64;
        inst_len+=(SPU_USART_FUNC_BYTES);
    }

    if(capabilities.spu_spi==1){
        priv->spi_func_index_base = func_index_base;
        func_index_base+=512;
        inst_len+=(SPU_SPI_FUNC_BYTES);
    }

    if(capabilities.spu_i2c==1){
        priv->i2c_func_index_base = func_index_base;
        func_index_base+=64;
        inst_len+=(SPU_I2C_FUNC_BYTES);
    }

    if(capabilities.spu_can==1){
        priv->can_func_index_base = func_index_base;
        func_index_base+=64;
        inst_len+=(SPU_CAN_FUNC_BYTES);
    }

    inst_len += func_index_base;

    if((inst_len)>SPU_INST_RAM_SIZE){
        return ERR_SPU(EDRV_SPU_INST_OVERFLOW);
    }

    //write INST_RAM
    int i;
    uint32_t func_base;
    uint32_t glb_func_index_base;
    uint32_t glb_func_base;
    uint32_t data;
    uint32_t offset;

    func_base = func_index_base;
    glb_func_index_base = 256 + (priv->base+INST_BASE);
    glb_func_base = func_index_base + (priv->base+INST_BASE);

    putData(0x1,priv->base+GLOBAL_BASE,OFFSET_BLOCKS_CLK_EN);  //open BLOCKS-CLOCK

    for(i=0;i<256;i+=4){
        putData(0x0,priv->base+INST_BASE,i);
    }

    if(capabilities.spu_usart==1){
        for(i=0;i<16;i++){
            data=SPU_USART_FUNC_INDEX[i]+func_base;
            offset = i<<2;
            putData(data,glb_func_index_base,offset);
        }
        glb_func_index_base+=64;

        for(i=0;i<SPU_USART_FUNC_WORDS;i++){
            offset = i<<2;
            putData(SPU_USART_FUNC[i],glb_func_base,offset);
        }
        func_base+=(SPU_USART_FUNC_BYTES);
        glb_func_base+=(SPU_USART_FUNC_BYTES);
    }

    if(capabilities.spu_spi==1){
        for(i=0;i<128;i++){
            data=SPU_SPI_FUNC_INDEX[i]+func_base;
            offset = i<<2;
            putData(data,glb_func_index_base,offset);
        }
        glb_func_index_base+=512;

        for(i=0;i<SPU_SPI_FUNC_WORDS;i++){
            offset = i<<2;
            putData(SPU_SPI_FUNC[i],glb_func_base,offset);
        }
        func_base+=(SPU_SPI_FUNC_BYTES);
        glb_func_base+=(SPU_SPI_FUNC_BYTES);
    }

    if(capabilities.spu_i2c==1){
        for(i=0;i<16;i++){
            data=SPU_I2C_FUNC_INDEX[i]+func_base;
            offset = i<<2;
            putData(data,glb_func_index_base,offset);
        }
        glb_func_index_base+=64;

        for(i=0;i<SPU_I2C_FUNC_WORDS;i++){
            offset = i<<2;
            putData(SPU_I2C_FUNC[i],glb_func_base,offset);
        }
        func_base+=(SPU_I2C_FUNC_BYTES);
        glb_func_base+=(SPU_I2C_FUNC_BYTES);
    }

    if(capabilities.spu_can==1){
        for(i=0;i<16;i++){
            data=SPU_CAN_FUNC_INDEX[i]+func_base;
            offset = i<<2;
            putData(data,glb_func_index_base,offset);
        }
        glb_func_index_base+=64;

        for(i=0;i<SPU_CAN_FUNC_WORDS;i++){
            offset = i<<2;
            putData(SPU_CAN_FUNC[i],glb_func_base,offset);
        }
        func_base+=(SPU_CAN_FUNC_BYTES);
        glb_func_base+=(SPU_CAN_FUNC_BYTES);
    }

    putData(0x0,priv->base+GLOBAL_BASE,OFFSET_BLOCKS_CLK_EN);  //close BLOCKS-CLOCK

    return 0;
}

int32_t csi_spu_uninitialize(spu_handle_t handle)
{
    ck_spu_priv_t *priv = handle;

    csi_vic_disable_irq(priv->irq);  //close interrupt
    putData(0x0,priv->base+GLOBAL_BASE,OFFSET_BLOCKS_CLK_EN);  //close BLOCKS-CLOCK
    putData(0x0,priv->base+GLOBAL_BASE,OFFSET_PGPIO_CLK_EN);  //close CLOCK

    return 0;
}

//------------------------------------------
//          function for device
//------------------------------------------
uint32_t spu_get_base(int32_t idx){
    ck_spu_priv_t *priv = &spu_instance[idx];
    return priv->base;
}

void spu_enable_clock(int32_t idx,int32_t pgpio)
{
    ck_spu_priv_t *priv = &spu_instance[idx];

    priv->clk_en_vec |= (1 << pgpio);

    if(!priv->clk_en){
        priv->clk_en = true;
        putData(0x1,priv->base+GLOBAL_BASE,OFFSET_BLOCKS_CLK_EN);
    }
}

void spu_disable_clock(int32_t idx,int32_t pgpio)
{
    ck_spu_priv_t *priv = &spu_instance[idx];

    priv->clk_en_vec &= ~(1 << pgpio);

    if((priv->clk_en_vec==0) && priv->clk_en){
        priv->clk_en = false;
        putData(0x0,priv->base+GLOBAL_BASE,OFFSET_BLOCKS_CLK_EN);
    }
}


int32_t spu_apply_prog(int32_t idx,int32_t bytes)
{
    ck_spu_priv_t *priv = &spu_instance[idx];

    int32_t tmp;

    tmp = priv->prog_used;
    priv->prog_used += bytes;

    if(priv->prog_used > (SPU_PROG_RAM_SIZE)){
        priv->prog_used = tmp;
        return -1;
    }

    return tmp;
}

int32_t spu_get_usart_func_index_base(int32_t idx)
{
    ck_spu_priv_t *priv = &spu_instance[idx];
    return priv->usart_func_index_base;
}

int32_t spu_get_spi_func_index_base(int32_t idx)
{
    ck_spu_priv_t *priv = &spu_instance[idx];
    return priv->spi_func_index_base;
}

int32_t spu_get_i2c_func_index_base(int32_t idx)
{
    ck_spu_priv_t *priv = &spu_instance[idx];
    return priv->i2c_func_index_base;
}

int32_t spu_get_can_func_index_base(int32_t idx)
{
    ck_spu_priv_t *priv = &spu_instance[idx];
    return priv->can_func_index_base;
}

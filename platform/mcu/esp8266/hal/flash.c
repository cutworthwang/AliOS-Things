#include <aos/aos.h>
#include <string.h>
#include "hal/soc/soc.h"
#include "c_types.h"
#include "spi_flash.h"
#include "k_api.h"

#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define FLASH_ALIGN_MASK ~(sizeof(uint32_t) - 1)
#define FLASH_ALIGN sizeof(uint32_t)

extern void vPortETSIntrLock(void);
extern void vPortETSIntrUnlock(void);

extern const hal_logic_partition_t hal_partitions[];

hal_logic_partition_t *hal_flash_get_info(hal_partition_t pno)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ pno ];

    return logic_partition;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    uint32_t start_addr, len, left_off;
    int32_t ret = 0;
    uint8_t *buffer = NULL;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( pno );
    start_addr = partition_info->partition_start_addr + *poff;

    left_off = start_addr % FLASH_ALIGN;
    len = ((buf_size + left_off) + ~FLASH_ALIGN_MASK) & FLASH_ALIGN_MASK;
    
    if (len > buf_size || left_off > 0) {
        buffer = (uint8_t *)aos_malloc(len);
        if (!buffer)
            return -1;
        memset(buffer, 0xff, len);
        memcpy(buffer + left_off, buf, buf_size);
        vPortETSIntrLock();
        ret = spi_flash_write(start_addr - left_off, (uint32_t *)buffer, len);
        vPortETSIntrUnlock();
        aos_free(buffer);
    } else {
        vPortETSIntrLock();
        ret = spi_flash_write(start_addr, (uint32_t *)buf, len);
        vPortETSIntrUnlock();
    }

    *poff += buf_size;
    return ret;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    int32_t ret = 0;
    uint32_t start_addr, len, left_off;
    uint8_t *buffer = NULL;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( pno );

    if(poff == NULL || buf == NULL || *poff + buf_size > partition_info->partition_length)
        return -1;
    start_addr = partition_info->partition_start_addr + *poff;

    left_off = start_addr % FLASH_ALIGN;
    len = ((buf_size + left_off) + ~FLASH_ALIGN_MASK) & FLASH_ALIGN_MASK;

    if (len > buf_size || left_off > 0) {
        buffer = (uint8_t *)aos_malloc(len);
        if (!buffer)
            return -1;
        memset(buffer, 0, len);
        ret = spi_flash_read(start_addr - left_off, (uint32_t *)buffer, len);
        memcpy(buf, buffer + left_off, buf_size);
        aos_free(buffer);
    } else
        ret = spi_flash_read(start_addr, buf, buf_size);
    *poff += buf_size;

    return ret;
}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set,
                        uint32_t size)
{
    uint32_t addr;
    uint32_t start_addr, end_addr;
    int32_t ret = 0;
    hal_logic_partition_t *partition_info;

    partition_info = hal_flash_get_info( pno );
    if(size + off_set > partition_info->partition_length)
        return -1;

    start_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set), SPI_FLASH_SEC_SIZE);
    end_addr = ROUND_DOWN((partition_info->partition_start_addr + off_set + size - 1), SPI_FLASH_SEC_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SPI_FLASH_SEC_SIZE) {
        vPortETSIntrLock();
        ret = spi_flash_erase_sector(addr/SPI_FLASH_SEC_SIZE);
        vPortETSIntrUnlock();
        if (ret != 0)
            return ret;
    }

    return 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}



/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     dw_gpio.c
 * @brief    CSI Source File for GPIO Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>
#include <stdbool.h>
#include <stdio.h>
#include "drv_gpio.h"
#include "dw_gpio.h"
#include "csi_core.h"
#include "pin_name.h"

#define ERR_GPIO(errno) (CSI_DRV_ERRNO_GPIO_BASE | errno)
#define GPIO_NULL_PARAM_CHK(para)                         \
        do {                                        \
            if (para == NULL) {                     \
                return ERR_GPIO(DRV_ERROR_PARAMETER);   \
            }                                       \
        } while (0)

typedef void *gpio_port_handle_t;

typedef struct {
    uint32_t base;              ///< handle register base
    uint32_t irq;               ///< irq of this handle
    uint32_t pin_num;           ///< pin number of this handle
    gpio_mode_e mode;           ///< gpio mode
    gpio_direction_e dir;       ///< gpio direction
    uint32_t mask;              ///< gpio mask bit
    uint32_t value;             ///< gpio value
} dw_gpio_priv_t;

typedef struct {
    uint8_t     portidx;
    uint8_t     idx;
    gpio_event_cb_t cb;
} dw_gpio_pin_priv_t;

extern int32_t target_gpio_port_init(port_name_t port, uint32_t *base, uint32_t *irq, uint32_t *pin_num);
extern int32_t target_gpio_pin_init(int32_t gpio_pin, uint32_t *port_idx);

static dw_gpio_priv_t gpio_handle[CONFIG_GPIO_NUM];
static dw_gpio_pin_priv_t gpio_pin_handle[CONFIG_GPIO_PIN_NUM];

//
// Functions
//
static int32_t gpio_set_direction(
    void *port,
    gpio_direction_e direction
)
{
    dw_gpio_priv_t *gpio_priv = port;
    dw_gpio_reg_t *gpio_reg = (dw_gpio_reg_t *)(gpio_priv->base);

    if (direction == GPIO_DIRECTION_INPUT) {
        gpio_reg->SWPORT_DDR &= (~gpio_priv->mask);
    } else if (direction == GPIO_DIRECTION_OUTPUT) {
        gpio_reg->SWPORT_DDR |= gpio_priv->mask;
    } else {
        return ERR_GPIO(GPIO_ERROR_DIRECTION);
    }

    return 0;
}

/*
 * Read the statu of the Port choosed.
 * Parameters:
 *   port:  use to choose a I/O port among Port A, B, or C.
 * return: the value of the corresponding Port.
 */

static int32_t gpio_read(void *port, uint32_t *value)
{
    dw_gpio_priv_t *gpio_priv = port;
    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(gpio_priv->base + 0x30);
    *value = gpio_control_reg->EXT_PORTA;
    return 0;
}


/*
 * Write an output value to corresponding Port.
 * Parameters:
 *   port:  use to choose a I/O port among Port A, B, or C.
 *   output: value that will be written to the corresponding Port.
 * return: SUCCESS
 */

static int32_t gpio_write(void *port, uint32_t mask)
{
    dw_gpio_priv_t *gpio_priv = port;
    dw_gpio_reg_t *gpio_reg = (dw_gpio_reg_t *)(gpio_priv->base);

    uint32_t value = gpio_reg->SWPORT_DR;

    value &= ~(mask);
    value |= gpio_priv->value;
    gpio_reg->SWPORT_DR = value;
    return 0;
}

/**
 * Configure a GPIO gpio_set_irq_mode.
 * @param[in]   pin         the addr store the pin num.
 * @param[in]   _irqmode    the irqmode of gpio
 * @return      zero on success. -1 on falure.
 */
static int32_t gpio_set_irq_mode(gpio_pin_handle_t pin, gpio_irq_mode_e irq_mode)
{
    dw_gpio_pin_priv_t *gpio_pin_priv = pin;

    /* convert portidx to port handle */
    dw_gpio_priv_t * port_handle = &gpio_handle[gpio_pin_priv->portidx];

    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(port_handle->base + 0x30);
    uint32_t offset = gpio_pin_priv->idx;
    uint32_t mask = 1 << offset;

    switch (irq_mode) {
        /* rising edge interrupt mode */
        case GPIO_IRQ_MODE_RISING_EDGE:
            gpio_control_reg->INTTYPE_LEVEL |= mask;
            gpio_control_reg->INT_POLARITY |= mask;
            break;

        /* falling edge interrupt mode */
        case GPIO_IRQ_MODE_FALLING_EDGE:
            gpio_control_reg->INTTYPE_LEVEL |= mask;
            gpio_control_reg->INT_POLARITY &= (~mask);
            break;

        /* low level interrupt mode */
        case GPIO_IRQ_MODE_LOW_LEVEL:
            gpio_control_reg->INTTYPE_LEVEL &= (~mask);
            gpio_control_reg->INT_POLARITY &= (~mask);
            break;

        /* high level interrupt mode */
        case GPIO_IRQ_MODE_HIGH_LEVEL:
            gpio_control_reg->INTTYPE_LEVEL &= (~mask);
            gpio_control_reg->INT_POLARITY |= mask;
            break;

        /* double edge interrupt mode */
        case GPIO_IRQ_MODE_DOUBLE_EDGE:
            return ERR_GPIO(DRV_ERROR_UNSUPPORTED);

        default:
            return ERR_GPIO(GPIO_ERROR_IRQ_MODE);
    }

    return 0;
}

/*
 * Clear one or more interrupts of PortA.
 * Parameters:
 *   pinno:
 * return: SUCCESS.
 */

static void gpio_irq_clear(gpio_pin_handle_t pin, uint32_t idx)
{
    dw_gpio_pin_priv_t *gpio_pin_priv = pin;

    /* convert portidx to port handle */
    dw_gpio_priv_t * port_handle = &gpio_handle[gpio_pin_priv->portidx];

    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(port_handle->base + 0x30);

    gpio_control_reg->PORTA_EOI = idx;
}


/*
 * Enable one or more interrupts of PortA.
 * Parameters:
 *   pinno:
 * return: SUCCESS.
 */
static void gpio_irq_enable(gpio_pin_handle_t pin)
{
    dw_gpio_pin_priv_t *gpio_pin_priv = pin;

    /* convert portidx to port handle */
    dw_gpio_priv_t * port_handle = &gpio_handle[gpio_pin_priv->portidx];

    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(port_handle->base + 0x30);
    uint32_t offset = gpio_pin_priv->idx;
    uint32_t val = gpio_control_reg->INTEN;
    val |= (1 << offset);
    gpio_control_reg->INTEN = val;
}


/*
 * Disable one or more interrupts of PortA.
 * Parameters:
 *   pinno:
 * return: SUCCESS.
 */

static void gpio_irq_disable(gpio_pin_handle_t pin)
{
    dw_gpio_pin_priv_t *gpio_pin_priv = pin;
    uint32_t offset = gpio_pin_priv->idx;

    /* convert portidx to port handle */
    dw_gpio_priv_t * port_handle = &gpio_handle[gpio_pin_priv->portidx];

    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(port_handle->base + 0x30);
    uint32_t val = gpio_control_reg->INTEN;
    val &= ~(1 << offset);
    gpio_control_reg->INTEN = val;
}

void dw_gpio_irqhandler(int idx)
{
    dw_gpio_control_reg_t *gpio_control_reg = (dw_gpio_control_reg_t *)(gpio_handle[idx].base + 0x30);

    uint32_t value = gpio_control_reg->INTSTATUS;
    uint8_t i;

    /* find the interrput pin */
    for (i = 0; i < 32; i++) {
        if (value == (1 << i)) {
            break;
        }
    }

    uint32_t offset = i;
    uint32_t pin_idx = offset;

    for (i = 0; i < idx; i++) {
        pin_idx += gpio_handle[i].pin_num;
    }

    dw_gpio_pin_priv_t *gpio_pin_priv = (dw_gpio_pin_priv_t *)&gpio_pin_handle[pin_idx];

    /* execute the callback function */
    if ((gpio_event_cb_t)(gpio_pin_priv->cb) && offset != 32) {
        ((gpio_event_cb_t)(gpio_pin_priv->cb))(pin_idx);
    }
    gpio_irq_clear(gpio_pin_priv, value);  //clear the gpio interrupt
}

/**
  \brief       Initialize GPIO module. 1. Initializes the resources needed for the GPIO handle 2.registers event callback function
                3.get gpio_port_handle
  \param[in]   port      port_name.
  \return      gpio_port_handle
*/
gpio_port_handle_t csi_gpio_port_initialize(int32_t port)
{
    uint32_t i;
    dw_gpio_priv_t *gpio_priv = NULL;

    for (i = 0; i <= port; i++) {
        /* obtain the gpio port information */
        uint32_t base = 0u;
        uint32_t pin_num;
        uint32_t irq;
        uint32_t idx = target_gpio_port_init(i, &base, &irq, &pin_num);

        if (idx < 0 || idx >= CONFIG_GPIO_NUM) {
            return NULL;
        }

        gpio_priv = &gpio_handle[idx];

        gpio_priv->base = base;
        gpio_priv->irq  = irq;
        gpio_priv->pin_num  = pin_num;
    }

    csi_vic_enable_irq(gpio_priv->irq);

    return (gpio_port_handle_t)gpio_priv;
}

/**
  \brief       De-initialize GPIO handle. stops operation and releases the software resources used by the handle
  \param[in]   handle   gpio port handle to operate.
  \return      error code
*/
int32_t csi_gpio_port_uninitialize(gpio_port_handle_t handle)
{
    GPIO_NULL_PARAM_CHK(handle);

    dw_gpio_priv_t *gpio_priv = handle;

    csi_vic_disable_irq(gpio_priv->irq);

    return 0;
}

/**
  \brief       config multiple pin within one handle
  \param[in]   handle    gpio port handle to operate.
  \param[in]   mask      the bitmask to identify which bits in the handle should be included (0 - ignore)
  \param[in]   mode      \ref gpio_mode_e
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_port_config(gpio_port_handle_t handle, uint32_t mask, gpio_mode_e mode, gpio_direction_e dir)
{
    if (mask < 0) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }
    GPIO_NULL_PARAM_CHK(handle);

    dw_gpio_priv_t *gpio_priv = handle;
    /*config the gpio mode direction mask bits */
    switch (mode) {
        case GPIO_MODE_PULLNONE:
            gpio_priv->mode = mode;
            break;
        case GPIO_MODE_PULLUP:
        case GPIO_MODE_PULLDOWM:
        case GPIO_MODE_OPEN_DRAIN:
        case GPIO_MODE_PUSH_PULL:
            return ERR_GPIO(DRV_ERROR_UNSUPPORTED);
        default:
            return ERR_GPIO(GPIO_ERROR_MODE);
    }

    gpio_priv->dir = dir;
    gpio_priv->mask = mask;
    uint32_t ret = gpio_set_direction(gpio_priv, dir);

    return ret;
}

/**
  \brief       Write value to the handle(write value to multiple pins on one handle at the same time)
  \param[in]   handle    gpio port handle to operate.
  \param[in]   mask      The bitmask to identify which bits in the handle should be included (0 - ignore)
  \param[in]   value     the value to be set
  \return      error code
*/
int32_t csi_gpio_port_write(gpio_port_handle_t handle, uint32_t mask, uint32_t value)
{
    if (mask < 0 || value < 0) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }
    GPIO_NULL_PARAM_CHK(handle);

    uint32_t port_value = mask & value;

    dw_gpio_priv_t *gpio_priv = handle;
    gpio_priv->value = port_value;
    gpio_write(gpio_priv, mask);

    return 0;

}

/**
  \brief       Read the current value on the handle(read value of multiple pins on one handle at the same time)
  \param[in]   handle    gpio port handle to operate.
  \param[in]   mask      The bitmask to identify which bits in the handle should be included (0 - ignore)
  \param[out]  value     an integer with each bit corresponding to an associated handle pin setting
  \return      error code
*/
int32_t csi_gpio_port_read(gpio_port_handle_t handle, uint32_t mask, uint32_t *value)
{
    if (mask < 0) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }
    GPIO_NULL_PARAM_CHK(handle);
    GPIO_NULL_PARAM_CHK(value);

    uint32_t port_value = 0;

    gpio_read(handle, &port_value);
    *value = (mask & port_value);

    return 0;

}

/**
  \brief       Initialize GPIO handle.
  \param[in]   gpio_pin    Pointer to the int32_t.
  \param[in]   cb_event    Pointer to \ref gpio_event_cb_t.
  \param[in]   arg    Pointer to \ref arg used for the callback.
  \return      gpio_pin_handle
*/
gpio_pin_handle_t csi_gpio_pin_initialize(int32_t gpio_pin, gpio_event_cb_t cb_event)
{

    if (gpio_pin < 0 || gpio_pin >= CONFIG_GPIO_PIN_NUM) {
        return NULL;
    }
    uint32_t i;
    for (i = 0; i < CONFIG_GPIO_NUM; i++) {
        csi_gpio_port_initialize(i);
    }
    /* obtain the gpio pin information */
    uint32_t port_idx;
    uint32_t pin_idx = target_gpio_pin_init(gpio_pin, &port_idx);

    uint32_t idx = pin_idx;

    for (i = 0; i < port_idx; i++) {
        idx += (gpio_handle[i].pin_num);
    }

    dw_gpio_pin_priv_t *gpio_pin_priv = &(gpio_pin_handle[idx]);
    gpio_pin_priv->portidx = port_idx;


    gpio_pin_priv->idx = pin_idx;
    gpio_pin_priv->cb = cb_event;

    return (gpio_pin_handle_t)gpio_pin_priv;
}

/**
  \brief       De-initialize GPIO pin handle. stops operation and releases the software resources used by the handle
  \param[in]   handle   gpio pin handle to operate.
  \return      error code
*/
int32_t csi_gpio_pin_uninitialize(gpio_pin_handle_t handle)
{
    if (handle == NULL) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    dw_gpio_pin_priv_t *gpio_pin_priv = (dw_gpio_pin_priv_t *)handle;
    gpio_pin_priv->cb = NULL;
    dw_gpio_priv_t * gpio_priv = &gpio_handle[gpio_pin_priv->portidx];
    csi_vic_disable_irq(gpio_priv->irq);
    return 0;
}

/**
  \brief       config pin mode
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   mode      \ref gpio_mode_e
  \return      error code
*/
int32_t csi_gpio_pin_config_mode(gpio_pin_handle_t handle,
                            gpio_mode_e mode)
{
    return ERR_GPIO(DRV_ERROR_UNSUPPORTED);
}
/**
  \brief       config pin direction
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_pin_config_direction(gpio_pin_handle_t handle,
                            gpio_direction_e dir)
{
    GPIO_NULL_PARAM_CHK(handle);

    /* config the gpio pin mode direction mask bits */
    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    /* convert portidx to port handle */
    dw_gpio_priv_t * gpio_priv = &gpio_handle[gpio_pin_priv->portidx];

    gpio_priv->dir = dir;
    gpio_priv->mask = 1 << gpio_pin_priv->idx;

    uint32_t ret = gpio_set_direction(gpio_priv, dir);
    if(ret) {
        return ret;
    }

    return 0;
}

/**
  \brief       config pin
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   mode      \ref gpio_mode_e
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_pin_config(gpio_pin_handle_t handle,
                            gpio_mode_e mode,
                            gpio_direction_e dir)
{
    GPIO_NULL_PARAM_CHK(handle);

    /* config the gpio pin mode direction mask bits */
    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    /* convert portidx to port handle */
    dw_gpio_priv_t * gpio_priv = &gpio_handle[gpio_pin_priv->portidx];

    gpio_priv->mode = mode;
    gpio_priv->dir = dir;
    gpio_priv->mask = 1 << gpio_pin_priv->idx;

    uint32_t ret = gpio_set_direction(gpio_priv, dir);
    if(ret) {
        return ret;
    }

    return 0;

}

/**
  \brief       Set one or zero to the selected GPIO pin.
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   value     the value to be set
  \return      error code
*/
int32_t csi_gpio_pin_write(gpio_pin_handle_t handle, bool value)
{
    GPIO_NULL_PARAM_CHK(handle);
    if ((int32_t)value < 0) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    /* convert portidx to port handle */
    dw_gpio_priv_t * port_handle = &gpio_handle[gpio_pin_priv->portidx];

    uint8_t offset = gpio_pin_priv->idx;
    uint32_t port_value = value << offset;

    port_handle->value = port_value;
    gpio_write(port_handle, (1 << offset));

    return 0;

}

/**
  \brief       Get the value of  selected GPIO pin.
  \param[in]   handle       gpio pin handle to operate.
  \param[out]  value     buf to store the pin value
  \return      error code
*/
int32_t csi_gpio_pin_read(gpio_pin_handle_t handle, bool *value)
{
    GPIO_NULL_PARAM_CHK(handle);
    if (value <= 0) {
        return ERR_GPIO(DRV_ERROR_PARAMETER);
    }

    dw_gpio_pin_priv_t *gpio_pin_priv = handle;
    uint32_t port_value;
    uint8_t offset = gpio_pin_priv->idx;

    /* convert portidx to port handle */
    dw_gpio_priv_t * port_handle = &gpio_handle[gpio_pin_priv->portidx];

    gpio_read(port_handle, &port_value);
    *value = (port_value & (1 << offset)) >> offset;

    return 0;
}

/**
  \brief       set GPIO interrupt mode.
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   mode      the irq mode to be set
  \param[in]   enable    the enable flag
  \return      error code
*/
int32_t csi_gpio_pin_set_irq(gpio_pin_handle_t handle, gpio_irq_mode_e mode, bool enable)
{
    GPIO_NULL_PARAM_CHK(handle);

    uint32_t ret = 0;

    if (enable) {
        ret = gpio_set_irq_mode(handle, mode);

        if (ret) {
            return ret;
        }
        gpio_irq_enable(handle);

    } else {
        gpio_irq_disable(handle);

    }

    return ret;

}


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
 * @file     drv_gpio.h
 * @brief    header file for gpio driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef _CSI_GPIO_H_
#define _CSI_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <drv_common.h>

/// definition for gpio pin handle.
typedef void *gpio_pin_handle_t;

/****** GPIO specific error codes *****/
typedef enum {
    GPIO_ERROR_MODE  = (DRV_ERROR_SPECIFIC + 1),      ///< Specified Mode not suphandleed
    GPIO_ERROR_DIRECTION,                        ///< Specified direction not suphandleed
    GPIO_ERROR_IRQ_MODE,                         ///< Specified irq mode not suphandleed
} gpio_error_e;

/*----- GPIO Control Codes: Mode -----*/
typedef enum {
    GPIO_MODE_PULLNONE         = 0,    ///< pull none for input
    GPIO_MODE_PULLUP              ,    ///< pull up for input
    GPIO_MODE_PULLDOWM            ,    ///< pull down for input
    GPIO_MODE_OPEN_DRAIN          ,    ///< open drain mode for output
    GPIO_MODE_PUSH_PULL           ,    ///< push-pull mode for output
} gpio_mode_e;

/*----- GPIO Control Codes: Mode Parameters: Data Bits -----*/
typedef enum {
    GPIO_DIRECTION_INPUT             = 0,    ///< gpio as input
    GPIO_DIRECTION_OUTPUT               ,    ///< gpio as output
} gpio_direction_e;

/*----- GPIO Control Codes: Mode Parameters: Parity -----*/
typedef enum {
    GPIO_IRQ_MODE_RISING_EDGE     = 0,       ///< interrupt mode for rising edge
    GPIO_IRQ_MODE_FALLING_EDGE       ,       ///< interrupt mode for falling edge
    GPIO_IRQ_MODE_DOUBLE_EDGE        ,       ///< interrupt mode for double edge
    GPIO_IRQ_MODE_LOW_LEVEL          ,       ///< interrupt mode for low level
    GPIO_IRQ_MODE_HIGH_LEVEL         ,       ///< interrupt mode for high level
} gpio_irq_mode_e;

typedef void (*gpio_event_cb_t)(int32_t idx);   ///< gpio Event call back.

/**
  \brief       Initialize GPIO handle.
  \param[in]   gpio_pin    gpio pin idx.
  \param[in]   cb_event  Pointer to \ref gpio_event_cb_t
  \param[in]   arg  Pointer to \ref arg used for the callback
  \return      gpio_pin_handle
*/
gpio_pin_handle_t csi_gpio_pin_initialize(int32_t gpio_pin, gpio_event_cb_t cb_event);

/**
  \brief       De-initialize GPIO pin handle.stops operation and releases the software resources used by the handle.
  \param[in]   handle    gpio pin handle to operate.
  \return      error code
*/
int32_t csi_gpio_pin_uninitialize(gpio_pin_handle_t handle);

/**
  \brief       config pin mode
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   mode      \ref gpio_mode_e
  \return      error code
*/
int32_t csi_gpio_pin_config_mode(gpio_pin_handle_t handle,
                            gpio_mode_e mode);

/**
  \brief       config pin direction
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_pin_config_direction(gpio_pin_handle_t handle,
                            gpio_direction_e dir);

/**
  \brief       config pin
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   mode      \ref gpio_mode_e
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_pin_config(gpio_pin_handle_t handle,
                                    gpio_mode_e mode,
                            gpio_direction_e dir);

/**
  \brief       Set one or zero to the selected GPIO pin.
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   value     the value to be set
  \return      error code
*/
int32_t csi_gpio_pin_write(gpio_pin_handle_t handle, bool value);

/**
  \brief       Get the value of  selected GPIO pin.
  \param[in]   pin       gpio pin handle to operate.
  \param[out]  value     buf to store the pin value
  \return      error code
*/
int32_t csi_gpio_pin_read(gpio_pin_handle_t handle, bool *value);

/**
  \brief       set GPIO interrupt mode.
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   mode      the irq mode to be set
  \param[in]   enable    the enable flag
  \return      error code
*/
int32_t csi_gpio_pin_set_irq(gpio_pin_handle_t handle, gpio_irq_mode_e mode, bool enable);


#ifdef __cplusplus
}
#endif

#endif /* _CSI_GPIO_H_ */

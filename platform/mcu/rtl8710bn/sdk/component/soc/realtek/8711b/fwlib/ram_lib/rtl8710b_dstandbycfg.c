/**
  ******************************************************************************
  * @file    rtl8710b_dstandbycfg.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of pin control:
  *           - dstandby wakeup event
  *           - dstandby power management
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"

/* if X can wakeup dsleep, it can wakeup dstandby & sleep */
/* if X can wakeup dstandby, it can wakeup sleep */
const PWRCFG_TypeDef dstandby_wevent_config[]=
{
//  	Module									Status
	{BIT_SYSON_WEVT_GPIO_DSTBY_MSK,		ON},	/* dstandby:	wakepin 0~3 wakeup */
	{BIT_SYSON_WEVT_A33_AND_A33GPIO_MSK,	ON},	/* dsleep:	REGU A33 Timer & A33 wakepin wakeup*/
	{BIT_SYSON_WEVT_RTC_MSK,				OFF},	/* dstandby:	RTC Wakeup */
	{BIT_SYSON_WEVT_SYSTIM_MSK,			OFF},	/* dstandby:	SYS Timer(ANA Timer) Wakeup */
	
	{0xFFFFFFFF,								OFF},	/* Table end */
};

/* you should set BIT_SYSON_WEVT_GPIO_DSTBY_MSK ON if you use wakepin */
const WAKEPIN_TypeDef dstandby_wakepin_config[]=
{
//  	Module			Status		Polarity
	{WAKUP_0,		OFF,			0},	/* wakeup_0: GPIOA_18 */
	{WAKUP_1,		ON,			0},	/* wakeup_1: GPIOA_5 */
	{WAKUP_2,		OFF,			0},	/* wakeup_2: GPIOA_22 */
	{WAKUP_3,		OFF,			0},	/* wakeup_3: GPIOA_23 */
	
	{0xFFFFFFFF,		OFF,			0},	/* Table end */
};
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

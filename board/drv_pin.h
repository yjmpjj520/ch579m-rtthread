/*
 * Copyright (c) 2019 Winner Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-15     flyingcys    1st version
 */

#ifndef DRV_PIN_H__
#define DRV_PIN_H__

#include "rtthread.h"
#define CH_PIN_DEFAULT (-1)

const struct pinindex *ch_get_pin(rt_base_t pin_index);
int ch_hw_pin_init(void);

#endif


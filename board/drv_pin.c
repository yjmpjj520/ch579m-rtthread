/*
 * Copyright (c) 2019 Winner Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-15     flyingcys    1st version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include "CH57x_gpio.h"
#include "drv_pin.h"

#define GPIOA  0
#define GPIOB  1
#define __CH_PIN(index, gpio,gpio_index) {index,GPIO##gpio,gpio_index}
#define __CH_PIN_DEFAULT    {-1,0,0}
#define CH_PIN_NUM(items) (sizeof(items) / sizeof(items[0]))
	
struct pinindex
	{
	int index;
	int gpio_name;
	int gpio_pin;
};

/* WM chip GPIO map*/
static const struct pinindex pins[] =
{
    __CH_PIN_DEFAULT,
    __CH_PIN_DEFAULT,
    __CH_PIN_DEFAULT,
    __CH_PIN_DEFAULT,
    __CH_PIN(4,A,GPIO_Pin_7),
    __CH_PIN(5,A,GPIO_Pin_8),
    __CH_PIN(6,A,GPIO_Pin_9),
    __CH_PIN(7,B,GPIO_Pin_9),
    __CH_PIN(8,B,GPIO_Pin_8),
    __CH_PIN(9,B,GPIO_Pin_17),
    __CH_PIN(10,B,GPIO_Pin_16),
    __CH_PIN(11,B,GPIO_Pin_15),
    __CH_PIN(12,B,GPIO_Pin_14),
    __CH_PIN(13,B,GPIO_Pin_13),
    __CH_PIN(14,B,GPIO_Pin_12),
    __CH_PIN(15,B,GPIO_Pin_11),
    __CH_PIN(16,B,GPIO_Pin_10),
    __CH_PIN(17,B,GPIO_Pin_7),
    __CH_PIN(18,B,GPIO_Pin_6),
    __CH_PIN(19,B,GPIO_Pin_5),
    __CH_PIN(20,B,GPIO_Pin_4),
    __CH_PIN(21,B,GPIO_Pin_3),
    __CH_PIN(22,B,GPIO_Pin_2),
    __CH_PIN(23,B,GPIO_Pin_1),
    __CH_PIN(24,B,GPIO_Pin_0),
    __CH_PIN(25,B,GPIO_Pin_23),
    __CH_PIN(26,B,GPIO_Pin_22),
    __CH_PIN(27,B,GPIO_Pin_21),
    __CH_PIN(28,B,GPIO_Pin_20),
    __CH_PIN(29,B,GPIO_Pin_19),
    __CH_PIN(30,B,GPIO_Pin_18),
    __CH_PIN_DEFAULT,
    __CH_PIN_DEFAULT,
    __CH_PIN_DEFAULT,
    __CH_PIN_DEFAULT,
    __CH_PIN_DEFAULT,
    __CH_PIN(36,A,GPIO_Pin_4),
    __CH_PIN(37,A,GPIO_Pin_5),
    __CH_PIN(38,A,GPIO_Pin_6),
    __CH_PIN(39,A,GPIO_Pin_0),
    __CH_PIN(40,A,GPIO_Pin_1),
    __CH_PIN(41,A,GPIO_Pin_2),
    __CH_PIN(42,A,GPIO_Pin_3),
    __CH_PIN(43,A,GPIO_Pin_15),
    __CH_PIN(44,A,GPIO_Pin_14),
    __CH_PIN(45,A,GPIO_Pin_13),
    __CH_PIN(46,A,GPIO_Pin_12),
    __CH_PIN(47,A,GPIO_Pin_11),
    __CH_PIN(48,A,GPIO_Pin_10),
};

const struct pinindex *ch_get_pin(rt_base_t pin_index)
{
    const struct pinindex *index;
    if (pin_index < CH_PIN_NUM(pins))
    {
        index = &pins[pin_index];
    }
    return index;
}

#ifdef RT_USING_PIN

static void ch_pin_mode(struct rt_device *device, rt_base_t pin, rt_base_t mode)
{
    const struct pinindex *index;
    index = ch_get_pin(pin);
    if (!index)
    {
        return;
    }
    if(index->gpio_name==0) {
        if (mode == GPIO_ModeIN_Floating)
        {
            GPIOA_ModeCfg(index->gpio_pin, GPIO_ModeIN_Floating);
        }
        else if (mode == GPIO_ModeIN_PU)
        {
            GPIOA_ModeCfg(index->gpio_pin, GPIO_ModeIN_PU);
        }
        else if (mode == GPIO_ModeIN_PD)
        {
            GPIOA_ModeCfg(index->gpio_pin, GPIO_ModeIN_PD);
        }
        else if (mode == GPIO_ModeOut_PP_5mA)
        {
            GPIOA_ModeCfg(index->gpio_pin,GPIO_ModeOut_PP_5mA);
        }
        else if (mode == GPIO_ModeOut_PP_20mA)
        {
            GPIOA_ModeCfg(index->gpio_pin,GPIO_ModeOut_PP_20mA);
        }
    } else {
        if (mode == GPIO_ModeIN_Floating)
        {
            GPIOB_ModeCfg(index->gpio_pin, GPIO_ModeIN_Floating);
        }
        else if (mode == GPIO_ModeIN_PU)
        {
            GPIOB_ModeCfg(index->gpio_pin, GPIO_ModeIN_PU);
        }
        else if (mode == GPIO_ModeIN_PD)
        {
            GPIOB_ModeCfg(index->gpio_pin, GPIO_ModeIN_PD);
        }
        else if (mode == GPIO_ModeOut_PP_5mA)
        {
            GPIOB_ModeCfg(index->gpio_pin,GPIO_ModeOut_PP_5mA);
        }
        else if (mode == GPIO_ModeOut_PP_20mA)
        {
            GPIOB_ModeCfg(index->gpio_pin,GPIO_ModeOut_PP_20mA);
        }
    }
    return;
}

static void ch_pin_write(struct rt_device *device, rt_base_t pin, rt_base_t value)
{
    const struct pinindex *index;
    index = ch_get_pin(pin);
    if (!index)
    {
        return;
    }
    if(index->gpio_name==0) {
        if(value) {
            GPIOA_SetBits(index->gpio_pin);
        } else {
            GPIOA_ResetBits(index->gpio_pin);
        }
    } else {
        if(value) {
            GPIOB_SetBits(index->gpio_pin);
        } else {
            GPIOB_ResetBits(index->gpio_pin);
        }
    }
    return;
}

static int ch_pin_read(struct rt_device *device, rt_base_t pin)
{
    const struct pinindex *index;
    index = ch_get_pin(pin);
    if (!index)
    {
        return PIN_LOW;
    }
    if(index->gpio_name==0) {
        return GPIOA_ReadPortPin(index->gpio_pin);
    } else {
        return GPIOB_ReadPortPin(index->gpio_pin);
    }
}

static rt_err_t ch_pin_attach_irq(struct rt_device *device, rt_int32_t pin,
                                  rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
    const struct pinindex *index;
    rt_base_t level;

    index = ch_get_pin(pin);
    if (!index)
    {
        return RT_ENOSYS;
    }

    level = rt_hw_interrupt_disable();
    //irq mode set
    if(index->gpio_name==0) {
        switch (mode)
        {
        case GPIO_ITMode_LowLevel:
            GPIOA_ITModeCfg(index->gpio_pin, GPIO_ITMode_LowLevel);
            break;
        case GPIO_ITMode_HighLevel:
            GPIOA_ITModeCfg(index->gpio_pin, GPIO_ITMode_HighLevel);
            break;
        case GPIO_ITMode_FallEdge:
            GPIOA_ITModeCfg(index->gpio_pin, GPIO_ITMode_FallEdge);
            break;
        case GPIO_ITMode_RiseEdge:
            GPIOA_ITModeCfg(index->gpio_pin, GPIO_ITMode_RiseEdge);
            break;
        default:
            rt_hw_interrupt_enable(level);
            return RT_ENOSYS;
        }
    } else {
        switch (mode)
        {
        case GPIO_ITMode_LowLevel:
            GPIOB_ITModeCfg(index->gpio_pin, GPIO_ITMode_LowLevel);
            break;
        case GPIO_ITMode_HighLevel:
            GPIOB_ITModeCfg(index->gpio_pin, GPIO_ITMode_HighLevel);
            break;
        case GPIO_ITMode_FallEdge:
            GPIOB_ITModeCfg(index->gpio_pin, GPIO_ITMode_FallEdge);
            break;
        case GPIO_ITMode_RiseEdge:
            GPIOB_ITModeCfg(index->gpio_pin, GPIO_ITMode_RiseEdge);
            break;
        default:
            rt_hw_interrupt_enable(level);
            return RT_ENOSYS;
        }
    }

    rt_hw_interrupt_enable(level);
    return RT_EOK;
}

static rt_err_t ch_pin_detach_irq(struct rt_device *device, rt_int32_t pin)
{
    return RT_EOK;
}

struct rt_pin_ops _ch_pin_ops =
{
    ch_pin_mode,
    ch_pin_write,
    ch_pin_read,
    ch_pin_attach_irq,
    ch_pin_detach_irq,
};

int ch_hw_pin_init(void)
{
    int ret = rt_device_pin_register("pin", &_ch_pin_ops, RT_NULL);
    return ret;
}
INIT_BOARD_EXPORT(ch_hw_pin_init);


#endif // RT_USING_PIN 


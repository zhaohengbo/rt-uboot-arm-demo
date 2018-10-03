/*
 *  linux/drivers/clocksource/arm_arch_timer.c
 *
 *  Copyright (C) 2011 ARM Ltd.
 *  All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <rthw.h>
#include <rtthread.h>

#include <asm/io.h>

#include "gic.h"
#include "armv7.h"
#include "interrupt.h"

#define TIMER_LOAD(hw_base)     __REG32(hw_base + 0x00)
#define TIMER_VALUE(hw_base)    __REG32(hw_base + 0x04)
#define TIMER_CTRL(hw_base)     __REG32(hw_base + 0x08)
#define TIMER_CTRL_ONESHOT      (1 << 0)
#define TIMER_CTRL_32BIT        (1 << 1)
#define TIMER_CTRL_DIV1         (0 << 2)
#define TIMER_CTRL_DIV16        (1 << 2)
#define TIMER_CTRL_DIV256       (2 << 2)
#define TIMER_CTRL_IE           (1 << 5)        /* Interrupt Enable (versatile only) */
#define TIMER_CTRL_PERIODIC     (1 << 6)
#define TIMER_CTRL_ENABLE       (1 << 7)

#define TIMER_INTCLR(hw_base)   __REG32(hw_base + 0x0c)
#define TIMER_RIS(hw_base)      __REG32(hw_base + 0x10)
#define TIMER_MIS(hw_base)      __REG32(hw_base + 0x14)
#define TIMER_BGLOAD(hw_base)   __REG32(hw_base + 0x18)

#define SYS_CTRL                __REG32(0x1001A000)

#define TIMER_HW_BASE           0x10012000

#define IRQ_TIMER2_3			35

#define SOC_IRQ_TIMER_LEVEL IRQ_TYPE_LEVEL

void soc_timer_isr_handler(void)
{
	TIMER_INTCLR(TIMER_HW_BASE) = 0x01;
}

void soc_timer_init(void)
{
	rt_uint32_t val;

    SYS_CTRL |= 0;

    /* Setup Timer0 for generating irq */
    val = TIMER_CTRL(TIMER_HW_BASE);
    val &= ~TIMER_CTRL_ENABLE;
    val |= (TIMER_CTRL_32BIT | TIMER_CTRL_PERIODIC | TIMER_CTRL_IE);
    TIMER_CTRL(TIMER_HW_BASE) = val;

    TIMER_LOAD(TIMER_HW_BASE) = 1000;

    /* enable timer */
    TIMER_CTRL(TIMER_HW_BASE) |= TIMER_CTRL_ENABLE;    
}

extern void rt_hw_timer_isr(int vector, void *param);

void soc_timer_isr_install(void)
{
	rt_hw_interrupt_install(IRQ_TIMER2_3, rt_hw_timer_isr, RT_NULL, "tick");
	rt_hw_interrupt_mask(IRQ_TIMER2_3);
	rt_hw_interrupt_config(IRQ_TIMER2_3,SOC_IRQ_TIMER_LEVEL);
    rt_hw_interrupt_umask(IRQ_TIMER2_3);
}
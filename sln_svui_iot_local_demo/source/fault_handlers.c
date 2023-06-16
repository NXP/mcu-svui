/*
 * Copyright 2018, 2020 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */



void HardFault_Handler(void)
{
    __asm("BKPT #0");
}

void MemManage_Handler(void)
{
    __asm("BKPT #1");
}

void BusFault_Handler(void)
{
    __asm("BKPT #2");
}

void UsageFault_Handler(void)
{
    __asm("BKPT #3");
}

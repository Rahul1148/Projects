/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "hal_data.h"
#include <stdio.h>
#include <unistd.h>

void R_BSP_WarmStart(bsp_warm_start_event_t event);

static volatile bool txComplete = false;

int puts(const char *str)
{
    txComplete = false;
    fsp_err_t err = FSP_SUCCESS;
    uint8_t data[256] = {0};
    int len = 0;
    len = sprintf((char*)data, str);

    err =  R_SCI_UART_Write(&g_uart0_ctrl, data, (uint32_t)len);
    assert(err == FSP_SUCCESS);
    return 0;
}


/* Callback function */
void user_uart_callback(uart_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    if(p_args->event == UART_EVENT_TX_COMPLETE)
    {
        txComplete = true;
    }
}

/*******************************************************************************************************************//**
 * @brief  Blinky example application
 *
 * Blinks all leds at a rate of 1 second using the software delay function provided by the BSP.
 *
 **********************************************************************************************************************/
void hal_entry (void)
{

#if BSP_TZ_SECURE_BUILD

    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif

    fsp_err_t err = FSP_SUCCESS;
    R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    assert(err == FSP_SUCCESS);

  while(1)
  {
      printf("Hello World ....\r\n\n");
      R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
  }

}


/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);
    }
}

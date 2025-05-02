/***********************************************************************************************************************
* Copyright (c) 2021 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : user_main.c
 * Version      : .
 * OS           : .
 * H/W Platform : FPB-RA4E1
 * Description  : This file includes the main tutorial code.
 * Operation    : See readme.txt
 * Limitations  : .
 *********************************************************************************************************************/



#include "hal_data.h"
#include "user_main.h"

extern bsp_leds_t g_bsp_leds;

/* Define the units to be used with the software delay function */


/* Set the blink frequency*/


/**********************************************************************************************************************
 * Function Name: st_irq_pins_t;
 * Description  : stores location of the pins used for irqs
 * Argument     : s_irq_pins
 * Return Value : .
 *********************************************************************************************************************/
static st_irq_pins_t s_irq_pins[] =
{
    { &g_external_irq1ds }
};
/**********************************************************************************************************************
 End of function st_irq_pins_t
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: icu_initialize
 * Description  : initialises irq pins to allow for button presses
 * Return Value : fsp_err
 *********************************************************************************************************************/
static fsp_err_t icu_initialize(void)
{
    uint8_t g_irq_switch = ((sizeof(s_irq_pins)) / (sizeof(s_irq_pins[0]))); /*  */

    fsp_err_t fsp_err = FSP_SUCCESS;



    for (uint32_t i = 0; i < g_irq_switch; i++ )
    {
        fsp_err = R_ICU_ExternalIrqOpen (s_irq_pins[i].p_irq->p_ctrl, s_irq_pins[i].p_irq->p_cfg);

        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }

        fsp_err = R_ICU_ExternalIrqEnable (s_irq_pins[i].p_irq->p_ctrl);

        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }
    return fsp_err;
}
/**********************************************************************************************************************
 End of function icu_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: user_main
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void user_main(void)
{


    icu_initialize();

    R_BSP_PinAccessEnable();
    R_BSP_PinWrite(BSP_IO_PORT_04_PIN_08, BSP_IO_LEVEL_LOW);
    R_BSP_PinWrite(BSP_IO_PORT_04_PIN_07, BSP_IO_LEVEL_HIGH);
    R_BSP_PinAccessDisable();

}
/**********************************************************************************************************************
 End of function user_main
 *********************************************************************************************************************/

/** External IRQ for SW 1 on ICU Instance. */
/**********************************************************************************************************************
 * Function Name: callback_irq1ds_button
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void callback_irq1ds_button(external_irq_callback_args_t *p_args)
{
    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    R_BSP_PinAccessEnable();

    R_BSP_PinWrite(BSP_IO_PORT_04_PIN_08, BSP_IO_LEVEL_HIGH);
    R_BSP_PinWrite(BSP_IO_PORT_04_PIN_07, BSP_IO_LEVEL_LOW);

    R_BSP_PinAccessDisable();


}
/**********************************************************************************************************************
 End of function callback_irq1ds_button
 *********************************************************************************************************************/

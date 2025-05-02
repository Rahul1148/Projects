#include "new_thread1.h"
/* New Thread entry function */
/* pvParameters contains TaskHandle_t */

extern TaskHandle_t new_thread0;

void new_thread1_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* TODO: add your own code here */
    while (1)
    {
        R_BSP_PinAccessEnable ();
        R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_04_PIN_07, BSP_IO_LEVEL_HIGH);

        R_BSP_PinAccessDisable ();
        vTaskDelay (1);
    }
}

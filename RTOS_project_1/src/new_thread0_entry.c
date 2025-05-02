#include "new_thread0.h"
#include "hal_data.h"
#include "common_utils.h"
#include "i2c_sensor.h"
/* New Thread entry function */
/* pvParameters contains TaskHandle_t */

extern TaskHandle_t new_thread1;
extern TaskHandle_t new_thread0;

void new_thread0_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* TODO: add your own code here */
    while (1)
    {
        R_BSP_PinAccessEnable ();
        R_BSP_PinWrite ((bsp_io_port_pin_t) BSP_IO_PORT_04_PIN_08, BSP_IO_LEVEL_HIGH);
        fsp_err_t err = FSP_SUCCESS;

            err = init_sensor ();
            if (FSP_SUCCESS != err)
            {
                /* Sensor init failed, so cleanup the sensor specific initialization */
                APP_ERR_PRINT("** SENSOR INIT FAILED ** \r\n");
                APP_ERR_TRAP(err);
            }
        R_BSP_PinAccessDisable ();
        vTaskDelay (1);

    }
}

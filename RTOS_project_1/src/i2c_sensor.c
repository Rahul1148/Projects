/***********************************************************************************************************************
 * File Name    : i2c_sensor.c
 * Description  : Contains data structures and functions used in i2c_sensor.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "common_utils.h"
#include "i2c_sensor.h"
#include <string.h>


/*******************************************************************************************************************//**
 * @addtogroup r_iic_master_ep
 * @{
 **********************************************************************************************************************/

/*
 * Global Variables
 */
/* Reading I2C call back event through i2c_Master callback */
static volatile i2c_master_event_t i2c_event = I2C_MASTER_EVENT_ABORTED;


uint8_t write_ptr = RESET_VALUE;
uint8_t read_ptr = RESET_VALUE;
//uint8_t PPG_RDY_REG =RESET_VALUE;


uint8_t write_ptr_data_1[3];
uint32_t ppg_data[200];
uint32_t ppg_output[64];

uint8_t RESET[MEASURE_PAYLOAD_SIZE] = {0X0D,0X01};
uint8_t clear_fifo_1[MEASURE_PAYLOAD_SIZE] = {0X04,0X00};
uint8_t clear_fifo_2[MEASURE_PAYLOAD_SIZE] = {0X05,0X00};
uint8_t clear_fifo_3[MEASURE_PAYLOAD_SIZE] = {0X06,0X00};
/*
 * private function declarations
 */
static fsp_err_t get_device_id(uint8_t *dev_id);
static fsp_err_t validate_i2c_event(void);

/* **************************************************** PART_B***********************************************/

/*init_sensor with get part_id*/
//fsp_err_t init_sensor(void)
//{
//    uint8_t device_id = RESET_VALUE;
//    fsp_err_t err     = FSP_SUCCESS;
//    err = R_IIC_MASTER_Open(&g_i2c_master_ctrl, &g_i2c_master_cfg);
//
//    /* handle error */
//    if (FSP_SUCCESS != err)
//    {
//        APP_ERR_PRINT("** R_IIC_MASTER_Open API failed ** \r\n");
//        return err;
//    }
//
//    /* Get device ID as a start of communication */
//
////    read_reg();
//    err = get_device_id (&device_id);
//
//    if (err != FSP_SUCCESS)
//    {
//        APP_ERR_PRINT("** Get device id failed ** \r\n");
//        return err;
//    }
//    SEGGER_RTT_printf(0,"div_id: %d\r\n",device_id);
//    return err;
//}





int read_reg(void)
{
    uint8_t dev_id = RESET_VALUE;
    uint8_t reg_address   = 0xFF;

     R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &reg_address, ONE_BYTE, true);
     R_IIC_MASTER_Read(&g_i2c_master0_ctrl, &dev_id, ONE_BYTE, false);

     SEGGER_RTT_printf(0,"div_id: %d\r\n",dev_id);
     return 0;

}



/* **************************************************** PART_B***********************************************/

fsp_err_t measure_enable_payload(uint8_t *measure_enable_payload)
{
    fsp_err_t err     = FSP_SUCCESS;

    err = R_IIC_MASTER_Write(&g_i2c_master0_ctrl, measure_enable_payload, TWO_BYTE, false);
            if (FSP_SUCCESS == err)
            {
                err = validate_i2c_event();
                /* handle error */
                if(FSP_ERR_TRANSFER_ABORTED == err)
                {
                    APP_ERR_PRINT("** POWER_CTL reg I2C write failed!!! ** \r\n");
                }
            }
            /* handle error */
            else
            {
                /* Write API returns itself is not successful */
                APP_ERR_PRINT("** R_IIC_MASTER_Write API failed ** \r\n");
            }
    return err;
}



/*******************************************************************************************************************//**
 *  @brief       initialize IIC master module and set up MAX86150 sensor
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS                  Upon successful open and start of timer
 *  @retval      Any Other Error code apart from FSP_SUCCESS is  Unsuccessful open or start
 **********************************************************************************************************************/
fsp_err_t init_sensor(void)
{
    uint8_t device_id = RESET_VALUE;
    fsp_err_t err     = FSP_SUCCESS;
    err = R_IIC_MASTER_Open(&g_i2c_master0_ctrl, &g_i2c_master0_cfg);

    /* handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Open API failed ** \r\n");
        return err;
    }

    /* Get device ID as a start of communication */
    err = get_device_id (&device_id);

    uint8_t FLUSH[MEASURE_PAYLOAD_SIZE] = {0X0D, 0X04};
    if (err == FSP_SUCCESS)
    {
        err = SETUP_PPG ();
        if (err == FSP_SUCCESS)
        {

            for (int k = 0; k < 100; k += 32)
            {
                for (int i = k; i < k + 32; i++)
                {

                    uint8_t WRT_PTR_INC[MEASURE_PAYLOAD_SIZE] =
                    { 0X04, i - k };
                    uint8_t RED_PTR_INC[MEASURE_PAYLOAD_SIZE] =
                    { 0X06, i - k };
                    err = measure_enable_payload (WRT_PTR_INC);
                    err = measure_enable_payload (RED_PTR_INC);
                    err = WRITE_DATA_REGISTER ();
                    ppg_data[i] = (uint32_t) write_ptr_data_1[2] << 16 | (uint32_t) write_ptr_data_1[1] << 8
                            | (uint32_t) write_ptr_data_1[0] << 0;
                    SEGGER_RTT_printf (0, "%d\r\n", (ppg_data[i] & 0x7ffff));
                }
                err = measure_enable_payload (FLUSH);

            }

        }
    }
    if (err != FSP_SUCCESS)
    {
        APP_ERR_PRINT("** Get device id failed ** \r\n");
        return err;
    }
    return err;
}


fsp_err_t CAPTURE_PPG(void)
{
    fsp_err_t err     = FSP_SUCCESS;
    uint8_t FLUSH[MEASURE_PAYLOAD_SIZE] = {0X0D, 0X04};
    uint8_t FIFO_ENABLE[MEASURE_PAYLOAD_SIZE] = {FIFO_ENABLE_reg, FIFO_ENABLE_data};
    err = SETUP_PPG();
    if (err == FSP_SUCCESS)
    {
        for(int k=0;k<20;k++)
        {
            err =measure_enable_payload(FIFO_ENABLE);
            for (int i = 0; i < 32; i++)

            {
//                uint8_t WRT_PTR_INC[MEASURE_PAYLOAD_SIZE] = {0X04, i};
//                uint8_t RED_PTR_INC[MEASURE_PAYLOAD_SIZE] = {0X06, i};
//
//                err = measure_enable_payload (WRT_PTR_INC);
//                err = measure_enable_payload (RED_PTR_INC);
                err = WRITE_DATA_REGISTER ();
            }

            err = measure_enable_payload (FLUSH);
        }


    }



    return err;
}

fsp_err_t SETUP_PPG(void)
{
    fsp_err_t err     = FSP_SUCCESS;
    /* ADC Configuration of ppg */
    uint8_t PPG_ADC_RGE[MEASURE_PAYLOAD_SIZE] = {PPG_ADC_RGE_reg, PPG_ADC_RGE_data};

    /*LED1,LED2 Pulse width configuration */
    uint8_t PPG_LED1[MEASURE_PAYLOAD_SIZE] = {PPG_LED1_reg, PPG_LED1_data}; //50mA
    uint8_t PPG_LED2[MEASURE_PAYLOAD_SIZE] = {PPG_LED2_reg, PPG_LED2_data}; //50mA

    /* configuration of fifo enable & rollover */
    uint8_t FIFO_ENABLE[MEASURE_PAYLOAD_SIZE] = {FIFO_ENABLE_reg, FIFO_ENABLE_data};
    uint8_t FIFO_ROLLS_ON_FULL[MEASURE_PAYLOAD_SIZE] = {FIFO_ROLLS_ON_FULL_reg, FIFO_ROLLS_ON_FULL_data};
    uint8_t FIFO_A_FULL_ENABLE[MEASURE_PAYLOAD_SIZE] = { A_FULL_ENABLE_REG, A_FULL_ENABLE_DATA};

    /*ppg enable */
    uint8_t PPG_SLOT[MEASURE_PAYLOAD_SIZE] = {PPG_SLOT_reg, 0b00100001};  //PPG_SLOT_data
    uint8_t ECG_SLOT[MEASURE_PAYLOAD_SIZE] = {ECG_SLOT_reg, 0b00000000};  //ECG_SLOT_data  0b00001001
    /* set sample average  */
    uint8_t LED_START_INTG_DELAY[MEASURE_PAYLOAD_SIZE] = {LED_START_INTG_DELAY_reg, LED_START_INTG_DELAY_data};

    uint8_t PPG_INTR_ENABLE[MEASURE_PAYLOAD_SIZE] = {0X02, 0b10000000};
   uint8_t LED_RANGE[MEASURE_PAYLOAD_SIZE] = {0X14,0x00};

   err = measure_enable_payload (RESET);
   if(err == FSP_SUCCESS)
   {
       err =measure_enable_payload(FIFO_ENABLE);
       if(err == FSP_SUCCESS)
         {
           err= measure_enable_payload (PPG_SLOT);
           if(err == FSP_SUCCESS)
             {
               err =measure_enable_payload (ECG_SLOT);
               if(err == FSP_SUCCESS)
                {
                   err =  measure_enable_payload (PPG_ADC_RGE);
                    if (err == FSP_SUCCESS)
                    {
                        err =  measure_enable_payload (LED_START_INTG_DELAY);
                         if (err == FSP_SUCCESS)
                         {
                             err =  measure_enable_payload (LED_RANGE);
                              if (err == FSP_SUCCESS)
                              {
                                  err =  measure_enable_payload (PPG_LED1);
                                   if (err == FSP_SUCCESS)
                                   {
                                       err =  measure_enable_payload (PPG_LED2);
                                        if (err == FSP_SUCCESS)
                                        {
                                            err =  measure_enable_payload (FIFO_ROLLS_ON_FULL);
                                             if (err == FSP_SUCCESS)
                                             {
                                                 err =  measure_enable_payload (FIFO_A_FULL_ENABLE);
                                                  if (err == FSP_SUCCESS)
                                                  {
                                                      err =  measure_enable_payload (PPG_INTR_ENABLE);
                                                  }
                                             }
                                        }
                                   }
                              }
                         }
                    }
                }

             }
         }
   }

//    err = (measure_enable_payload (RESET) && measure_enable_payload(FIFO_ENABLE) && measure_enable_payload (PPG_SLOT) &&  measure_enable_payload (ECG_SLOT) && measure_enable_payload (PPG_ADC_RGE) &&  measure_enable_payload (LED_START_INTG_DELAY) &&
//            measure_enable_payload (LED_RANGE) && measure_enable_payload (PPG_LED1) && measure_enable_payload (PPG_LED2) && measure_enable_payload (FIFO_ROLLS_ON_FULL) && measure_enable_payload (FIFO_A_FULL_ENABLE) && measure_enable_payload (PPG_INT_ENABLE));

    return err;
}
fsp_err_t WRITE_PTR_REGISTER(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg = 0x04;


    /* Write  Data register   */
    err = R_IIC_MASTER_Write (&g_i2c_master0_ctrl, &reg, ONE_BYTE, true);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Write API failed ** \r\n");
        return err;
    }
    else
    {
        err = validate_i2c_event ();

        /* Performs read operation only when write event is successful, failure part is handled at next stage*/
        if (FSP_SUCCESS == err)
        {
            err = R_IIC_MASTER_Read (&g_i2c_master0_ctrl, &write_ptr, DATA_REGISTERS, false);
            /* handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("** R_IIC_MASTER_Read API failed ** \r\n");
                return err;
            }
            else
            {
                err = validate_i2c_event ();
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("** Data register, I2C read failed ** \r\n");
                    return err;
                }
            }
        }
        else
        {
            APP_ERR_PRINT("** Accel Data register, I2C write failed ** \r\n");
            return err;
        }

    }
    SEGGER_RTT_printf(0,"write_ptr:-%d\r\n",write_ptr);
    return err;
}



fsp_err_t READ_PTR_REGISTER(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg = 0x06;


    /* Write  Data register   */
    err = R_IIC_MASTER_Write (&g_i2c_master0_ctrl, &reg, ONE_BYTE, true);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Write API failed ** \r\n");
        return err;
    }
    else
    {
        err = validate_i2c_event ();

        /* Performs read operation only when write event is successful, failure part is handled at next stage*/
        if (FSP_SUCCESS == err)
        {
            err = R_IIC_MASTER_Read (&g_i2c_master0_ctrl, &read_ptr, DATA_REGISTERS, false);
            /* handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("** R_IIC_MASTER_Read API failed ** \r\n");
                return err;
            }
            else
            {
                err = validate_i2c_event ();
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("** Data register, I2C read failed ** \r\n");
                    return err;
                }
            }
        }
        else
        {
            APP_ERR_PRINT("** Accel Data register, I2C write failed ** \r\n");
            return err;
        }

    }
    SEGGER_RTT_printf(0,"read_ptr:-%d\r\n",read_ptr);
    return err;
}

fsp_err_t FULL_INT_STATUS(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t PPG_RDY_REG = RESET_VALUE;
    uint8_t reg = 0x00;

    /* Write  Data register   */
    err = R_IIC_MASTER_Write (&g_i2c_master0_ctrl, &reg, ONE_BYTE, true);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Write API failed ** \r\n");
        return err;
    }
    else
    {
        err = validate_i2c_event ();

        /* Performs read operation only when write event is successful, failure part is handled at next stage*/
        if (FSP_SUCCESS == err)
        {
            err = R_IIC_MASTER_Read (&g_i2c_master0_ctrl, &PPG_RDY_REG, DATA_REGISTERS, false);
            /* handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("** R_IIC_MASTER_Read API failed ** \r\n");
                return err;
            }
            else
            {
                err = validate_i2c_event ();
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("** Data register, I2C read failed ** \r\n");
                    return err;
                }
            }
        }
        else
        {
            APP_ERR_PRINT("** Accel Data register, I2C write failed ** \r\n");
            return err;
        }

    }
    SEGGER_RTT_printf(0,"PPG_RDY_REG:-%d\r\n",PPG_RDY_REG);
    return err;
}




fsp_err_t WRITE_DATA_REGISTER(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t reg = 0x07;

//    uint32_t ppg_data_1 = RESET_VALUE;
//    uint8_t led1[3];
//    uint8_t led2[3];

    /* Write  Data register   */
    err = R_IIC_MASTER_Write (&g_i2c_master0_ctrl, &reg, ONE_BYTE, true);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Write API failed ** \r\n");
        return err;
    }
    else
    {
        err = validate_i2c_event ();
        /* Performs read operation only when write event is successful, failure part is handled at next stage*/
        if (FSP_SUCCESS == err)
        {
            err = R_IIC_MASTER_Read (&g_i2c_master0_ctrl, write_ptr_data_1, 0X03, false);
            /* handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("** R_IIC_MASTER_Read API failed ** \r\n");
                return err;
            }
            else
            {
                err = validate_i2c_event ();

                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("** Data register, I2C read failed ** \r\n");
                    return err;
                }

            }

        }
        else
        {
            APP_ERR_PRINT("** ppg Data register, I2C write failed ** \r\n");
            return err;
        }

    }

//    SEGGER_RTT_printf(0,"%d\r\n",(ppg_data & 0x7ffff));

//    if(err == FSP_SUCCESS)
//    {
//
//       err = WRITE_PTR_REGISTER();
//       if(err == FSP_SUCCESS)
//       {
//           err = READ_PTR_REGISTER();
//       }
//
//    }

    return err;
}




/*******************************************************************************************************************//**
 *  @brief       DeInitialize IIC master module
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void deinit_sensor(void)
{
    fsp_err_t err     = FSP_SUCCESS;

    /* close open modules */
    err =  R_IIC_MASTER_Close (&g_i2c_master0_ctrl);

    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Close API failed ** \r\n");
    }
}
/*******************************************************************************************************************//**
 *  @brief       Read The ACL Sensor data for X,Y and Z axis. And prints the raw data on the RTT console
 *  @param[IN]   Array pointer to get the sensor data
 *  @retval      FSP_SUCCESS                  Upon successful open and start of timer
 *  @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful open or start
 **********************************************************************************************************************/
fsp_err_t READ_REGISTER(uint8_t reg)
{
    fsp_err_t err           = FSP_SUCCESS;
    uint8_t read_data = RESET_VALUE;

    /* Write  Data register   */
    err = R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &reg, ONE_BYTE, true);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Write API failed ** \r\n");
        return err;
    }
    else
    {
        err = validate_i2c_event();

        /* Performs read operation only when write event is successful, failure part is handled at next stage*/
        if (FSP_SUCCESS == err)
        {
            err = R_IIC_MASTER_Read(&g_i2c_master0_ctrl, &read_data, DATA_REGISTERS, false);
            /* handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("** R_IIC_MASTER_Read API failed ** \r\n");
                return err;
            }
            else
            {
                err = validate_i2c_event();
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("** Data register, I2C read failed ** \r\n");
                    return err;
                }
            }
        }
        else
        {
            APP_ERR_PRINT("** Accel Data register, I2C write failed ** \r\n");
            return err;
        }

    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief     Read Device signature/ID from sensor
 *
 *  @param[in] dev_id                       retrieve PMOD ACL device ID
 *
 *  @retval    FSP_SUCCESS                  on successful I2C transaction
 *             FSP_ERR_INVALID_POINTER      if invalid parameters passed
 *             err                          whichever occurs on either API or on I2C event failure
 **********************************************************************************************************************/
static fsp_err_t get_device_id(uint8_t *dev_id)
{
    fsp_err_t err         = FSP_SUCCESS;
    uint8_t reg_address   = DEVICE_ID_REG;

    /* Parameter checking */
    if (NULL == dev_id)
    {
        err = FSP_ERR_INVALID_POINTER;
        APP_ERR_PRINT("** NULL Pointer check fail ** \r\n");
        return err;
    }



    /* Send register address to sensor */
    err = R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &reg_address, ONE_BYTE, true);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_IIC_MASTER_Write API failed ** \r\n");
    }
    else
    {
        err = validate_i2c_event();
        /* handle error */
        if(FSP_ERR_TRANSFER_ABORTED == err)
        {
            APP_ERR_PRINT("** Device ID reg, I2C write failed ** \r\n");
        }
        else
        {
            /* Read only when I2C write and its callback event is successful */
            err  = R_IIC_MASTER_Read(&g_i2c_master0_ctrl, dev_id, ONE_BYTE, false);
            /* handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("** R_IIC_MASTER_Read API failed ** \r\n");
                //  Do nothing, the error is returned in the end
            }
            else
            {
                err = validate_i2c_event();
                /* handle error */
                if(FSP_ERR_TRANSFER_ABORTED == err)
                {
                    APP_ERR_PRINT("** Device ID read,  I2C read failed ** \r\n");
                }
            }
        }
    }
    /* On successful I2C transaction with PMOD-ACL return from here */
    return err;
}

/*******************************************************************************************************************//**
 *  @brief      User callback function
 *  @param[in]  p_args
 *  @retval None
 **********************************************************************************************************************/
void i2c_master_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        /* capture callback event for validating the i2c transfer event*/
        i2c_event = p_args->event;
    }
}

/*******************************************************************************************************************//**
 *  @brief       Validate i2c receive/transmit  based on required write read operation
 *
 *  @param[in]   None
 *
 *  @retval      FSP_SUCCESS                       successful event receiving returns FSP_SUCCESS
 *               FSP_ERR_TRANSFER_ABORTED          Either on timeout elapsed or received callback event is
 *                                                 I2C_MASTER_EVENT_ABORTED
 **********************************************************************************************************************/
static fsp_err_t validate_i2c_event(void)
{
    uint16_t local_time_out = UINT16_MAX;

    /* resetting call back event capture variable */
    i2c_event = (i2c_master_event_t)RESET_VALUE;

    do
    {
        /* This is to avoid infinite loop */
        --local_time_out;

        if(RESET_VALUE == local_time_out)
        {
            return FSP_ERR_TRANSFER_ABORTED;
        }

    }while(i2c_event == RESET_VALUE);

    if(i2c_event != I2C_MASTER_EVENT_ABORTED)
    {
        i2c_event = (i2c_master_event_t)RESET_VALUE;  // Make sure this is always Reset before return
        return FSP_SUCCESS;
    }

    i2c_event = (i2c_master_event_t)RESET_VALUE; // Make sure this is always Reset before return
    return FSP_ERR_TRANSFER_ABORTED;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_iic_master_ep)
 **********************************************************************************************************************/

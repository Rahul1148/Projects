/***********************************************************************************************************************
 * File Name    : i2c_sensor.h
 * Description  : Contains data structures and functions used in i2c_sensor.h/c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/


#ifndef I2C_SENSOR_H_
#define I2C_SENSOR_H_

#define ONE_BYTE                0x01
#define TWO_BYTE                0x02

#define MEASURE_PAYLOAD_SIZE    0x03        //measurement enable data length
#define ACCELERO_DELAY          0xC8
#define SENSOR_READ_DELAY       0x03
#define ENABLE_BIT              0x08
#define DATA_REGISTERS          0x06

/* Accelerometer internal register whichever consumed here */
#define DEVICE_ID_REG           0xFF
#define DEVICE_SIGNATURE        0x1E

#define POWER_CTL_REG           0x2D
#define AXIS_DATA               0x32

/* configure the  fifo_register */
#define A_FULL_ENABLE_REG       0X02
#define A_FULL_ENABLE_DATA      0x80

#define Reset_part_reg          0x0D
#define Reset_part_data         0X01

#define FIFO_ENABLE_reg         0X0D
#define FIFO_ENABLE_data        0X04

#define FIFO_ROLLS_ON_FULL_reg  0x08
#define FIFO_ROLLS_ON_FULL_data 0x10  //1F TO 10,0F.7f

/* Enable the pgg and ecg mode registers*/
#define PPG_SLOT_reg                0x09
#define PPG_SLOT_data               0x01     // 01 for led1 only

#define ECG_SLOT_reg                0x0A
#define ECG_SLOT_data               0x00

/* Configuration of the Acquisition Settings for the Best PPG Performance */

#define PPG_LED1_reg                0x11
#define PPG_LED2_reg                0x12

#define PPG_LED1_data               0xFF
#define PPG_LED2_data               0xFF

#define PPG_ADC_RGE_reg             0X0E
#define PPG_ADC_RGE_data            0x51   // CHANGED D6 --> 0XDB ,11 ,51

#define LED_START_INTG_DELAY_reg    0x0F
#define LED_START_INTG_DELAY_data   0x05      // CHANGED 00 to 03 ,05

/* Configuration the Acquisition Settings for the Best ECG Performance */
#define ECG_ADC_OSR_reg             0x3C
#define ECG_ADC_OSR_data            0x03

#define PGA_ECG_Gain_reg            0x3E
#define PGA_ECG_Gain_data           0x0D

/* Configuration of the AFE Settings for the Best ECG Performance*/
#define AFE_ONE_reg                 0XFF
#define AFE_ONE_data                0X54

#define AFE_TWO_reg                 0XFF
#define AFE_TWO_data                0X4D

#define AFE_THREE_reg               0XCE
#define AFE_THREE_data              0X0A

#define AFE_FOUR_reg                0XCF
#define AFE_FOUR_data               0X18

#define AFE_FIVE_reg                0XFF
#define AFE_FIVE_data               0X00

#define SENSOR_DATA_SIZE            0x06

#define fifo_read_pointer           0x06
#define fifo_write_pointer          0x04
/*
 * function declarations
 */
fsp_err_t init_sensor(void);
void deinit_sensor(void);
fsp_err_t measure_enable_payload(uint8_t *measure_enable_payload);

/*updated code from partB */
int read_reg(void);
int write_reg(int);

// old code
fsp_err_t CAPTURE_PPG(void);
fsp_err_t WRITE_PTR_REGISTER(void);
fsp_err_t READ_PTR_REGISTER(void);
fsp_err_t WRITE_DATA_REGISTER(void);
fsp_err_t FULL_INT_STATUS(void);
fsp_err_t READ_REGISTER(uint8_t reg);
fsp_err_t SETUP_PPG(void);
fsp_err_t read_sensor_data();





#endif /* I2C_SENSOR_H_ */

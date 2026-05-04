/**
  ******************************************************************************
  * @file    BMI088.h
  * @brief   BMI088 6轴惯性传感器驱动头文件 (SPI接口)
  ******************************************************************************
  */

#ifndef __BMI088_H__
#define __BMI088_H__

#include "spi.h"

#define BMI088_SPI_PORT hspi1
#define BMI088_CS_ACCEL_PORT GPIOA
#define BMI088_CS_ACCEL_PIN GPIO_PIN_4
#define BMI088_CS_GYRO_PORT GPIOB
#define BMI088_CS_GYRO_PIN GPIO_PIN_0

#define BMI088_REG_ACC_X_L 0x12
#define BMI088_REG_ACC_X_H 0x13
#define BMI088_REG_ACC_Y_L 0x14
#define BMI088_REG_ACC_Y_H 0x15
#define BMI088_REG_ACC_Z_L 0x16
#define BMI088_REG_ACC_Z_H 0x17
#define BMI088_REG_GYRO_X_L 0x02
#define BMI088_REG_GYRO_X_H 0x03
#define BMI088_REG_GYRO_Y_L 0x04
#define BMI088_REG_GYRO_Y_H 0x05
#define BMI088_REG_GYRO_Z_L 0x06
#define BMI088_REG_GYRO_Z_H 0x07

#define BMI088_ACC_RANGE_3G 0x00
#define BMI088_ACC_RANGE_6G 0x01
#define BMI088_ACC_RANGE_12G 0x02
#define BMI088_ACC_RANGE_24G 0x03
#define BMI088_GYRO_RANGE_2000DPS 0x00
#define BMI088_GYRO_RANGE_1000DPS 0x01
#define BMI088_GYRO_RANGE_500DPS 0x02
#define BMI088_GYRO_RANGE_250DPS 0x03
#define BMI088_GYRO_RANGE_125DPS 0x04

#define BMI088_ACC_RANGE_3G_SENS 0.00059814453125f
#define BMI088_ACC_RANGE_6G_SENS 0.0011962890625f
#define BMI088_ACC_RANGE_12G_SENS 0.002392578125f
#define BMI088_ACC_RANGE_24G_SENS 0.00478515625f
#define BMI088_GYRO_RANGE_2000DPS_SENS 0.06103515625f
#define BMI088_GYRO_RANGE_1000DPS_SENS 0.030517578125f
#define BMI088_GYRO_RANGE_500DPS_SENS 0.0152587890625f
#define BMI088_GYRO_RANGE_250DPS_SENS 0.00762939453125f
#define BMI088_GYRO_RANGE_125DPS_SENS 0.003814697265625f

void BMI088_ACC_Write_Reg(uint8_t addr, uint8_t data);
void BMI088_ACC_Write_Multi_Reg(uint8_t addr, const uint8_t *data, uint16_t len);
void BMI088_ACC_Read_Reg(uint8_t addr, uint8_t *data);
void BMI088_ACC_Read_Multi_Reg(uint8_t addr, uint8_t *data, uint16_t len);

void BMI088_GYRO_Write_Reg(uint8_t addr, uint8_t data);
void BMI088_GYRO_Write_Multi_Reg(uint8_t addr, const uint8_t *data, uint16_t len);
void BMI088_GYRO_Read_Reg(uint8_t addr, uint8_t *data);
void BMI088_GYRO_Read_Multi_Reg(uint8_t addr, uint8_t *data, uint16_t len);

#endif /* __BMI088_H__ */
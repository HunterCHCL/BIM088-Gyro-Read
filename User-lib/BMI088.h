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

#define BMI088_REG_ACC_RANGE 0x41
#define BMI088_REG_GYRO_RANGE 0x0F

#define BMI088_REG_ACC_PWR_CONF 0x7C
#define BMI088_REG_ACC_PWR_CTRL 0x7D
#define BMI088_REG_GYRO_LPM1 0x11
#define BMI088_REG_GYRO_SOFTRESET 0x14
#define BMI088_REG_ACC_SOFTRESET 0x7E

#define BMI088_ACC_RANGE_3G 0x00
#define BMI088_ACC_RANGE_6G 0x01
#define BMI088_ACC_RANGE_12G 0x02
#define BMI088_ACC_RANGE_24G 0x03
#define BMI088_GYRO_RANGE_2000DPS 0x00
#define BMI088_GYRO_RANGE_1000DPS 0x01
#define BMI088_GYRO_RANGE_500DPS 0x02
#define BMI088_GYRO_RANGE_250DPS 0x03
#define BMI088_GYRO_RANGE_125DPS 0x04

#define BMI088_ACC_SENS_3G 0.0008979988098144531f
#define BMI088_ACC_SENS_6G 0.0017959976196289062f
#define BMI088_ACC_SENS_12G 0.0035919952392578125f
#define BMI088_ACC_SENS_24G 0.007183990478515625f
#define BMI088_GYRO_SENS_2000DPS 0.06103515625f
#define BMI088_GYRO_SENS_1000DPS 0.030517578125f
#define BMI088_GYRO_SENS_500DPS 0.0152587890625f
#define BMI088_GYRO_SENS_250DPS 0.00762939453125f
#define BMI088_GYRO_SENS_125DPS 0.003814697265625f

typedef struct {
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
} BMI088_Data_t;

extern BMI088_Data_t BMI088_Data;

/* 角度解算数据结构 */
typedef struct {
    float roll;   /* 横滚角 (°) */
    float pitch;  /* 俯仰角 (°) */
    float yaw;    /* 偏航角 (°) */
} BMI088_Angle_t;

/* 陀螺仪零偏校准数据结构 */
typedef struct {
    float gx_offset;
    float gy_offset;
    float gz_offset;
    uint8_t calibrated;
} BMI088_GyroCalib_t;

/* 卡尔曼滤波器结构 (单轴) */
typedef struct {
    float Q_angle;     /* 角度过程噪声协方差 */
    float Q_gyro;      /* 角速度过程噪声协方差 */
    float R_angle;     /* 角度测量噪声协方差 (加速度计) */
    float angle;       /* 估计角度 (°) */
    float bias;        /* 陀螺仪零偏估计 (°/s) */
    float P[2][2];     /* 误差协方差矩阵 */
} BMI088_Kalman_t;

extern BMI088_Angle_t BMI088_Angle;

void BMI088_Init(void);
void BMI088_Read_Accel(BMI088_Data_t *data);
void BMI088_Read_Gyro(BMI088_Data_t *data);
void BMI088_Set_Accel_Range(uint8_t range);
void BMI088_Set_Gyro_Range(uint8_t range);
uint8_t BMI088_Get_Accel_ID(void);
uint8_t BMI088_Get_Gyro_ID(void);

/* 启动校准 (陀螺仪零偏) */
void BMI088_Calib_Init(void);
uint8_t BMI088_Is_Calibrated(void);

/* 角度解算 */
void BMI088_Kalman_Init(BMI088_Kalman_t *kf, float Q_angle, float Q_gyro, float R_angle);
float BMI088_Kalman_Update(BMI088_Kalman_t *kf, float gyro_rate, float acc_angle, float dt);
void BMI088_Update_Angle(float dt);

#endif /* __BMI088_H__ */
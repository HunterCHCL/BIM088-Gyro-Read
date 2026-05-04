/**
  ******************************************************************************
  * @file    BMI088.h
  * @brief   BMI088 6轴惯性传感器驱动头文件 (SPI接口)
  *          BMI088 = 3轴加速度计 + 3轴陀螺仪
  ******************************************************************************
  */

#ifndef __BMI088_H__
#define __BMI088_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "spi.h"
#include <stdint.h>

/* ========================== 硬件接口宏定义 ========================== */
#define BMI088_SPI_PORT         SPI1
#define BMI088_CS_ACCEL_PORT    GPIOA
#define BMI088_CS_ACCEL_PIN     GPIO_PIN_4
#define BMI088_CS_GYRO_PORT     GPIOB
#define BMI088_CS_GYRO_PIN      GPIO_PIN_0

/* ========================== BMI088 加速度计 (ACC) 寄存器 ========================== */
#define BMI088_ACC_CHIP_ID             0x00U   /* Chip ID, 应返回 0x1E */
#define BMI088_ACC_ERR_REG             0x02U   /* 错误寄存器 */
#define BMI088_ACC_STATUS              0x03U   /* 状态寄存器 */

/* 加速度计数据寄存器 (6字节: X_LSB, X_MSB, Y_LSB, Y_MSB, Z_LSB, Z_MSB) */
#define BMI088_ACC_DATA_START          0x12U

/* 温度数据寄存器 */
#define BMI088_ACC_TEMP_MSB            0x22U
#define BMI088_ACC_TEMP_LSB            0x23U

/* 传感器时间寄存器 */
#define BMI088_ACC_SENSORTIME_START    0x24U

/* 配置寄存器 */
#define BMI088_ACC_CONF                0x40U   /* 输出数据速率(ODR)和带宽配置 */
#define BMI088_ACC_RANGE               0x41U   /* 量程配置 */
#define BMI088_ACC_PWR_CONF            0x7CU   /* 电源模式配置 */
#define BMI088_ACC_PWR_CTRL            0x7DU   /* 电源控制 */
#define BMI088_ACC_SOFTRESET           0x7EU   /* 软复位寄存器 */

/* --- 加速度计量程定义 --- */
#define BMI088_ACC_RANGE_3G            0x00U   /* ±3g  (仅BMI088) */
#define BMI088_ACC_RANGE_6G            0x01U   /* ±6g  (仅BMI088) */
#define BMI088_ACC_RANGE_12G           0x02U   /* ±12g (仅BMI088) */
#define BMI088_ACC_RANGE_24G           0x03U   /* ±24g (仅BMI088) */

/* --- 加速度计ODR定义 --- */
#define BMI088_ACC_ODR_12_5HZ          0x05U   /* 12.5 Hz */
#define BMI088_ACC_ODR_25HZ            0x06U   /* 25 Hz */
#define BMI088_ACC_ODR_50HZ            0x07U   /* 50 Hz */
#define BMI088_ACC_ODR_100HZ           0x08U   /* 100 Hz */
#define BMI088_ACC_ODR_200HZ           0x09U   /* 200 Hz */
#define BMI088_ACC_ODR_400HZ           0x0AU   /* 400 Hz */
#define BMI088_ACC_ODR_800HZ           0x0BU   /* 800 Hz */
#define BMI088_ACC_ODR_1600HZ          0x0CU   /* 1600 Hz */

/* --- 加速度计电源模式 --- */
#define BMI088_ACC_PWR_SUSPEND         0x03U   /* 挂起模式 */
#define BMI088_ACC_PWR_ACTIVE          0x00U   /* 活跃模式 */
#define BMI088_ACC_ENABLE              0x04U   /* 使能加速度计 */

/* 加速度计量程对应的灵敏度 (LSB/g) (根据实际选定量程) */
/* 注意: BMI088 量程/灵敏度不同于BMI088 (旧版BMI088为±2/4/8/16g, 新版为±3/6/12/24g) */
/* 此处按最新版BMI088 (ACC range ±3/6/12/24g) 编写 */
/* 灵敏度: ±3g -> 10920 LSB/g, ±6g -> 5460 LSB/g, ±12g -> 2730 LSB/g, ±24g -> 1365 LSB/g */

/* ========================== BMI088 陀螺仪 (GYRO) 寄存器 ========================== */
#define BMI088_GYRO_CHIP_ID            0x00U   /* Chip ID, 应返回 0x0F */
#define BMI088_GYRO_RATE_X_LSB         0x02U   /* 陀螺仪X轴数据低字节 */
#define BMI088_GYRO_RATE_X_MSB         0x03U   /* 陀螺仪X轴数据高字节 */
#define BMI088_GYRO_RATE_Y_LSB         0x04U   /* 陀螺仪Y轴数据低字节 */
#define BMI088_GYRO_RATE_Y_MSB         0x05U   /* 陀螺仪Y轴数据高字节 */
#define BMI088_GYRO_RATE_Z_LSB         0x06U   /* 陀螺仪Z轴数据低字节 */
#define BMI088_GYRO_RATE_Z_MSB         0x07U   /* 陀螺仪Z轴数据高字节 */

#define BMI088_GYRO_DATA_START         0x02U   /* 陀螺仪数据起始地址 (6字节) */

/* 陀螺仪中断状态寄存器 */
#define BMI088_GYRO_INT_STAT_1         0x0AU

/* 陀螺仪配置寄存器 */
#define BMI088_GYRO_RANGE              0x0FU   /* 量程选择 */
#define BMI088_GYRO_BANDWIDTH          0x10U   /* ODR和带宽 */
#define BMI088_GYRO_PWR_MODE           0x11U   /* 电源模式 */
#define BMI088_GYRO_SOFTRESET          0x14U   /* 软复位 */

/* --- 陀螺仪量程定义 --- */
#define BMI088_GYRO_RANGE_2000DPS      0x00U   /* ±2000 °/s */
#define BMI088_GYRO_RANGE_1000DPS      0x01U   /* ±1000 °/s */
#define BMI088_GYRO_RANGE_500DPS       0x02U   /* ±500  °/s */
#define BMI088_GYRO_RANGE_250DPS       0x03U   /* ±250  °/s */
#define BMI088_GYRO_RANGE_125DPS       0x04U   /* ±125  °/s */

/* --- 陀螺仪ODR定义 --- */
#define BMI088_GYRO_ODR_2000HZ         0x07U   /* 2000 Hz */
#define BMI088_GYRO_ODR_1000HZ         0x06U   /* 1000 Hz */
#define BMI088_GYRO_ODR_400HZ          0x05U   /* 400 Hz */
#define BMI088_GYRO_ODR_200HZ          0x04U   /* 200 Hz */
#define BMI088_GYRO_ODR_100HZ          0x03U   /* 100 Hz */
#define BMI088_GYRO_ODR_50HZ           0x02U   /* 50 Hz */
#define BMI088_GYRO_ODR_25HZ           0x01U   /* 25 Hz */

/* --- 陀螺仪电源模式 --- */
#define BMI088_GYRO_PWR_NORMAL         0x00U   /* 正常模式 */
#define BMI088_GYRO_PWR_SUSPEND        0x80U   /* 挂起模式 */
#define BMI088_GYRO_PWR_DEEP_SUSPEND   0x20U   /* 深度挂起模式 */

/* ========================== 软复位命令 ========================== */
#define BMI088_SOFTRESET_CMD           0xB6U   /* 软复位命令字节 */

/* ========================== SPI读写标志位 ========================== */
#define BMI088_SPI_READ                0x80U   /* 读操作标志 (地址最高位置1) */
#define BMI088_SPI_WRITE               0x7FU   /* 写操作掩码 (地址最高位清零) */

/* ========================== 状态/错误码 ========================== */
#define BMI088_OK                      0       /* 操作成功 */
#define BMI088_ERROR                   -1      /* 操作失败 */
#define BMI088_CHIP_ID_ERR             -2      /* Chip ID不匹配 */

/* ========================== 数据结构体 ========================== */

/**
  * @brief BMI088 陀螺仪原始数据 (int16)
  */
typedef struct {
    int16_t x;  /**< X轴角速度原始值 */
    int16_t y;  /**< Y轴角速度原始值 */
    int16_t z;  /**< Z轴角速度原始值 */
} BMI088_GyroRaw_t;

/**
  * @brief BMI088 加速度计原始数据 (int16)
  */
typedef struct {
    int16_t x;  /**< X轴加速度原始值 */
    int16_t y;  /**< Y轴加速度原始值 */
    int16_t z;  /**< Z轴加速度原始值 */
} BMI088_AccelRaw_t;

/**
  * @brief BMI088 陀螺仪数据 (物理量: °/s)
  */
typedef struct {
    float x;    /**< X轴角速度 (°/s) */
    float y;    /**< Y轴角速度 (°/s) */
    float z;    /**< Z轴角速度 (°/s) */
} BMI088_GyroData_t;

/**
  * @brief BMI088 加速度计数据 (物理量: m/s²)
  */
typedef struct {
    float x;    /**< X轴加速度 (m/s²) */
    float y;    /**< Y轴加速度 (m/s²) */
    float z;    /**< Z轴加速度 (m/s²) */
} BMI088_AccelData_t;

/* ========================== 函数声明 ========================== */

/**
  * @brief  BMI088 初始化 (加速度计 + 陀螺仪)
  * @note   执行设备ID校验、软复位、配置默认参数
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Init(void);

/**
  * @brief  BMI088 加速度计初始化
  * @param  range  量程 (BMI088_ACC_RANGE_3G / 6G / 12G / 24G)
  * @param  odr    输出数据速率 (BMI088_ACC_ODR_xxx)
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Accel_Init(uint8_t range, uint8_t odr);

/**
  * @brief  BMI088 陀螺仪初始化
  * @param  range  量程 (BMI088_GYRO_RANGE_xxxDPS)
  * @param  odr    输出数据速率 (BMI088_GYRO_ODR_xxxHZ)
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Gyro_Init(uint8_t range, uint8_t odr);

/**
  * @brief  读取陀螺仪原始数据 (int16)
  * @param  raw  存放原始数据的结构体指针
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Read_Gyro_Raw(BMI088_GyroRaw_t *raw);

/**
  * @brief  读取陀螺仪数据 (物理量 °/s)
  * @param  data  存放物理量数据的结构体指针
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Read_Gyro(BMI088_GyroData_t *data);

/**
  * @brief  读取加速度计原始数据 (int16)
  * @param  raw  存放原始数据的结构体指针
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Read_Accel_Raw(BMI088_AccelRaw_t *raw);

/**
  * @brief  读取加速度计数据 (物理量 m/s²)
  * @param  data  存放物理量数据的结构体指针
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Read_Accel(BMI088_AccelData_t *data);

/**
  * @brief  读取加速度计温度
  * @param  temp  温度值指针 (单位: °C)
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Read_Temperature(float *temp);

/**
  * @brief  批量读取加速度计和陀螺仪数据
  * @param  accel  加速度计数据指针 (可为NULL)
  * @param  gyro   陀螺仪数据指针 (可为NULL)
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Read_All(BMI088_AccelData_t *accel, BMI088_GyroData_t *gyro);

/**
  * @brief  加速度计自检
  * @retval BMI088_OK 通过, 其他 失败
  */
int BMI088_Accel_SelfTest(void);

/**
  * @brief  陀螺仪自检
  * @retval BMI088_OK 通过, 其他 失败
  */
int BMI088_Gyro_SelfTest(void);

/* ========================== 底层SPI操作函数 ========================== */

/**
  * @brief  向加速度计写一个字节
  * @param  reg   寄存器地址
  * @param  data  写入的数据
  */
void BMI088_ACC_Write_Reg(uint8_t reg, uint8_t data);

/**
  * @brief  从加速度计读一个字节
  * @param  reg  寄存器地址
  * @retval 读取到的数据
  */
uint8_t BMI088_ACC_Read_Reg(uint8_t reg);

/**
  * @brief  从加速度计连续读取多字节
  * @param  reg   起始寄存器地址
  * @param  buf   数据缓冲区
  * @param  len   读取长度
  */
void BMI088_ACC_Read_Multi(uint8_t reg, uint8_t *buf, uint16_t len);

/**
  * @brief  向陀螺仪写一个字节
  * @param  reg   寄存器地址
  * @param  data  写入的数据
  */
void BMI088_GYRO_Write_Reg(uint8_t reg, uint8_t data);

/**
  * @brief  从陀螺仪读一个字节
  * @param  reg  寄存器地址
  * @retval 读取到的数据
  */
uint8_t BMI088_GYRO_Read_Reg(uint8_t reg);

/**
  * @brief  从陀螺仪连续读取多字节
  * @param  reg   起始寄存器地址
  * @param  buf   数据缓冲区
  * @param  len   读取长度
  */
void BMI088_GYRO_Read_Multi(uint8_t reg, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __BMI088_H__ */
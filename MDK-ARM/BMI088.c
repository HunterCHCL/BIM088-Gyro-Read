#include "BMI088.h"
#include <string.h>   /* for memset */

static uint8_t  g_accel_range = BMI088_ACC_RANGE_3G;
static uint8_t  g_gyro_range  = BMI088_GYRO_RANGE_2000DPS;
//片选控制函数
static inline void BMI088_ACC_CS_Low(void)
{
    HAL_GPIO_WritePin(BMI088_CS_ACCEL_PORT, BMI088_CS_ACCEL_PIN, GPIO_PIN_RESET);
}
static inline void BMI088_ACC_CS_High(void)
{
    HAL_GPIO_WritePin(BMI088_CS_ACCEL_PORT, BMI088_CS_ACCEL_PIN, GPIO_PIN_SET);
}
static inline void BMI088_GYRO_CS_Low(void)
{
    HAL_GPIO_WritePin(BMI088_CS_GYRO_PORT, BMI088_CS_GYRO_PIN, GPIO_PIN_RESET);
}
static inline void BMI088_GYRO_CS_High(void)
{
    HAL_GPIO_WritePin(BMI088_CS_GYRO_PORT, BMI088_CS_GYRO_PIN, GPIO_PIN_SET);
}
//延时函数
static inline void BMI088_Delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}
static inline void BMI088_TransmitRecieve(uint8_t *txData, uint8_t *rxData, uint16_t size)
{
    HAL_SPI_TransmitReceive(&BMI088_SPI_PORT, txData, rxData, size, HAL_MAX_DELAY);
}
void BMI088_ACC_Write_Reg(uint8_t addr, uint8_t data)
{
    uint8_t tx[2] = {addr & 0x7F, data};
    uint8_t rx[2] = {0};
    BMI088_ACC_CS_Low();
    BMI088_TransmitRecieve(tx,rx,2);
    BMI088_ACC_CS_High();
}
void BMI088_ACC_Read_Reg(uint8_t addr, uint8_t *data)
{
    uint8_t tx[2] = {addr | 0x80, 0x00};
    BMI088_ACC_CS_Low();
    BMI088_TransmitRecieve(tx, data, 2);
    BMI088_ACC_CS_High();
}
void BMI088_ACC_Read_Multi_Reg(uint8_t addr, uint8_t *data, uint16_t len)
{
    uint8_t tx[len + 1];
    uint8_t rx[len + 1];
    memset(tx, 0x00, sizeof(tx));
    tx[0] = addr | 0x80;
    BMI088_ACC_CS_Low();
    BMI088_TransmitRecieve(tx, rx, len + 1);
    BMI088_ACC_CS_High();
    memcpy(data, rx + 1, len);
}
void BMI088_GYRO_Write_Reg(uint8_t addr, uint8_t data)
{
    uint8_t tx[2] = {addr & 0x7F, data};
    uint8_t rx[2] = {0};
    BMI088_GYRO_CS_Low();
    BMI088_TransmitRecieve(tx, rx, 2);
    BMI088_GYRO_CS_High();
}
void BMI088_GYRO_Read_Reg(uint8_t addr, uint8_t *data)
{
    uint8_t tx[2] = {addr | 0x80, 0x00};
    BMI088_GYRO_CS_Low();
    BMI088_TransmitRecieve(tx, data, 2);
    BMI088_GYRO_CS_High();
}
void BMI088_GYRO_Read_Multi_Reg(uint8_t addr, uint8_t *data, uint16_t len)
{
    uint8_t tx[len + 1];
    uint8_t rx[len + 1];
    memset(tx, 0x00, sizeof(tx));
    tx[0] = addr | 0x80;
    BMI088_GYRO_CS_Low();
    BMI088_TransmitRecieve(tx, rx, len + 1);
    BMI088_GYRO_CS_High();
    memcpy(data, rx + 1, len);
}
void BMI088_Init(void)
{
    //加速度计初始化
    BMI088_ACC_Write_Reg(BMI088_ACC_PWR_CTRL, 0x00); //正常模式
    BMI088_Delay_ms(10);
    BMI088_ACC_Write_Reg(BMI088_ACC_PWR_CTRL, 0x04); //开启加速度计
    BMI088_Delay_ms(10);
    BMI088_ACC_Write_Reg(BMI088_ACC_RANGE, g_accel_range); //设置加速度计量程
    BMI088_Delay_ms(10);
    //陀螺仪初始化
    BMI088_GYRO_Write_Reg(BMI088_GYRO_LPM1, 0x00); //正常模式
    BMI088_Delay_ms(10);
    BMI088_GYRO_Write_Reg(BMI088_GYRO_RANGE, g_gyro_range); //设置陀螺仪量程
    BMI088_Delay_ms(10);
}
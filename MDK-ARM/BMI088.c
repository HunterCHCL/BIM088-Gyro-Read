#include "BMI088.h"
#include <string.h>

static uint8_t  g_accel_range = BMI088_ACC_RANGE_3G;
static uint8_t  g_gyro_range  = BMI088_GYRO_RANGE_2000DPS;

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
void BMI088_Read_Accel(BMI088_Data_t *data)
{
    uint8_t raw_data[6];
    BMI088_ACC_Read_Multi_Reg(BMI088_ACC_X_LSB, raw_data, 6);
    data.ax = BMI088_Accel_Sensitivity() * (float)(int16_t)((raw_data[1] << 8) | raw_data[0]);
    data.ay = BMI088_Accel_Sensitivity() * (float)(int16_t)((raw_data[3] << 8) | raw_data[2]);
    data.az = BMI088_Accel_Sensitivity() * (float)(int16_t)((raw_data[5] << 8) | raw_data[4]);
}
void BMI088_Read_Gyro(BMI088_Data_t *data)
{
    uint8_t raw_data[6];
    BMI088_GYRO_Read_Multi_Reg(BMI088_GYRO_X_LSB, raw_data, 6);
    data.gx = BMI088_Gyro_Sensitivity() * (float)(int16_t)((raw_data[1] << 8) | raw_data[0]);
    data.gy = BMI088_Gyro_Sensitivity() * (float)(int16_t)((raw_data[3] << 8) | raw_data[2]);
    data.gz = BMI088_Gyro_Sensitivity() * (float)(int16_t)((raw_data[5] << 8) | raw_data[4]);
}
void BMI088_Set_Accel_Range(uint8_t range)
{
    g_accel_range = range;
    BMI088_ACC_Write_Reg(BMI088_ACC_RANGE, g_accel_range);
}
void BMI088_Set_Gyro_Range(uint8_t range)
{
    g_gyro_range = range;
    BMI088_GYRO_Write_Reg(BMI088_GYRO_RANGE, g_gyro_range);
}
static float BMI088_Accel_Sensitivity()
{
    switch (g_accel_range)
    {
        case BMI088_ACC_RANGE_3G:
            return BMI088_ACC_SENS_3G;
        case BMI088_ACC_RANGE_6G:
            return BMI088_ACC_SENS_6G;
        case BMI088_ACC_RANGE_12G:
            return BMI088_ACC_SENS_12G;
        case BMI088_ACC_RANGE_24G:
            return BMI088_ACC_SENS_24G;
        default:
            return BMI088_ACC_SENS_3G;
    }
}
static float BMI088_Gyro_Sensitivity()
{
    switch (g_gyro_range)
    {
        case BMI088_GYRO_RANGE_2000DPS:
            return BMI088_GYRO_SENS_2000DPS;
        case BMI088_GYRO_RANGE_1000DPS:
            return BMI088_GYRO_SENS_1000DPS;
        case BMI088_GYRO_RANGE_500DPS:
            return BMI088_GYRO_SENS_500DPS;
        case BMI088_GYRO_RANGE_250DPS:
            return BMI088_GYRO_SENS_250DPS;
        case BMI088_GYRO_RANGE_125DPS:
            return BMI088_GYRO_SENS_125DPS;
        default:
            return BMI088_GYRO_SENS_2000DPS;
    }
}

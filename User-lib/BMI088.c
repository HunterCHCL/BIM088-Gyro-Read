#include "BMI088.h"
#include <string.h>
#include <math.h>

static uint8_t  g_accel_range = BMI088_ACC_RANGE_3G;
static uint8_t  g_gyro_range  = BMI088_GYRO_RANGE_250DPS;

volatile BMI088_Data_t BMI088_Data = {0};

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
static void BMI088_ACC_Write_Reg(uint8_t addr, uint8_t data)
{
    uint8_t tx[2] = {addr & 0x7F, data};
    uint8_t rx[2] = {0};
    BMI088_ACC_CS_Low();
    BMI088_TransmitRecieve(tx,rx,2);
    BMI088_ACC_CS_High();
}
static void BMI088_ACC_Read_Reg(uint8_t addr, uint8_t *data)
{
    uint8_t tx[3] = {addr | 0x80, 0x00, 0x00};
    uint8_t rx[3] = {0};
    BMI088_ACC_CS_Low();
    BMI088_TransmitRecieve(tx, rx, 3);
    BMI088_ACC_CS_High();
    *data = rx[2];
}
static void BMI088_ACC_Read_Multi_Reg(uint8_t addr, uint8_t *data, uint16_t len)
{
    uint8_t tx[len + 2];
    uint8_t rx[len + 2];
    memset(tx, 0x00, sizeof(tx));
    memset(rx, 0x00, sizeof(rx));
    tx[0] = addr | 0x80;
    BMI088_ACC_CS_Low();
    BMI088_TransmitRecieve(tx, rx, len + 2);
    BMI088_ACC_CS_High();
    memcpy(data, rx + 2, len);
}
static void BMI088_GYRO_Write_Reg(uint8_t addr, uint8_t data)
{
    uint8_t tx[2] = {addr & 0x7F, data};
    uint8_t rx[2] = {0};
    BMI088_GYRO_CS_Low();
    BMI088_TransmitRecieve(tx, rx, 2);
    BMI088_GYRO_CS_High();
}
static void BMI088_GYRO_Read_Reg(uint8_t addr, uint8_t *data)
{
    uint8_t tx[2] = {addr | 0x80, 0x00};
    uint8_t rx[2] = {0};
    BMI088_GYRO_CS_Low();
    BMI088_TransmitRecieve(tx, rx, 2);
    BMI088_GYRO_CS_High();
    *data = rx[1];
}
static void BMI088_GYRO_Read_Multi_Reg(uint8_t addr, uint8_t *data, uint16_t len)
{
    uint8_t tx[len + 1];
    uint8_t rx[len + 1];
    memset(tx, 0x00, sizeof(tx));
    memset(rx, 0x00, sizeof(rx));
    tx[0] = addr | 0x80;
    BMI088_GYRO_CS_Low();
    BMI088_TransmitRecieve(tx, rx, len + 1);
    BMI088_GYRO_CS_High();
    memcpy(data, rx + 1, len);
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
void BMI088_Init(void)
{
    uint8_t dummy = 0;
    BMI088_ACC_Read_Reg(0x00, &dummy);
    BMI088_Delay_ms(2);

    BMI088_ACC_Write_Reg(BMI088_REG_ACC_SOFTRESET, 0xB6);
    BMI088_Delay_ms(50);
    
    BMI088_ACC_Read_Reg(0x00, &dummy);
    BMI088_Delay_ms(2);


    BMI088_ACC_Write_Reg(BMI088_REG_ACC_PWR_CTRL, 0x04);
    BMI088_Delay_ms(50);
    BMI088_ACC_Write_Reg(BMI088_REG_ACC_PWR_CONF, 0x00);
    BMI088_Delay_ms(10);
    BMI088_ACC_Write_Reg(BMI088_REG_ACC_RANGE, g_accel_range);
    BMI088_Delay_ms(10);
    BMI088_ACC_Write_Reg(BMI088_REG_ACC_CONF, 0x88);
    BMI088_Delay_ms(10);

    BMI088_GYRO_Write_Reg(BMI088_REG_GYRO_SOFTRESET, 0xB6);
    BMI088_Delay_ms(50);    


    BMI088_GYRO_Read_Reg(0x00, &dummy);
    BMI088_Delay_ms(1);
    BMI088_GYRO_Write_Reg(BMI088_REG_GYRO_LPM1, 0x00);
    BMI088_Delay_ms(50);
    BMI088_GYRO_Write_Reg(BMI088_REG_GYRO_BANDWIDTH, 0x80 | BMI088_GYRO_ODR_100HZ_BW_32HZ);
    BMI088_Delay_ms(10);
    BMI088_GYRO_Write_Reg(BMI088_REG_GYRO_RANGE, g_gyro_range);
    BMI088_Delay_ms(10);
}
void BMI088_Read_Accel(volatile BMI088_Data_t *data)
{
    uint8_t raw_data[6];
    BMI088_ACC_Read_Multi_Reg(BMI088_REG_ACC_X_L, raw_data, 6);
    data->ax = BMI088_Accel_Sensitivity() * (float)(int16_t)((raw_data[1] << 8) | raw_data[0]) - data->ax_offset;
    data->ay = BMI088_Accel_Sensitivity() * (float)(int16_t)((raw_data[3] << 8) | raw_data[2]) - data->ay_offset;
    data->az = BMI088_Accel_Sensitivity() * (float)(int16_t)((raw_data[5] << 8) | raw_data[4]) - data->az_offset;
}
void BMI088_Read_Gyro(volatile BMI088_Data_t *data)
{
    uint8_t raw_data[6];
    BMI088_GYRO_Read_Multi_Reg(BMI088_REG_GYRO_X_L, raw_data, 6);
    data->gx = BMI088_Gyro_Sensitivity() * (float)(int16_t)((raw_data[1] << 8) | raw_data[0]) - data->gx_offset;
    data->gy = BMI088_Gyro_Sensitivity() * (float)(int16_t)((raw_data[3] << 8) | raw_data[2]) - data->gy_offset;
    data->gz = BMI088_Gyro_Sensitivity() * (float)(int16_t)((raw_data[5] << 8) | raw_data[4]) - data->gz_offset;
}
void BMI088_Set_Accel_Range(uint8_t range)
{
    g_accel_range = range;
    BMI088_ACC_Write_Reg(BMI088_REG_ACC_RANGE, g_accel_range);
}
void BMI088_Set_Gyro_Range(uint8_t range)
{
    g_gyro_range = range;
    BMI088_GYRO_Write_Reg(BMI088_REG_GYRO_RANGE, g_gyro_range);
}

uint8_t BMI088_Get_Accel_ID(void)
{
    uint8_t id = 0;
    BMI088_ACC_Read_Reg(0x00, &id);
    return id;
}

uint8_t BMI088_Get_Gyro_ID(void)
{
    uint8_t id = 0;
    BMI088_GYRO_Read_Reg(0x00, &id);
    return id;
}

void BMI088_Calib_Init(void)
{
    float sum_gx = 0, sum_gy = 0, sum_gz = 0;

    BMI088_Data.gx_offset = 0;
    BMI088_Data.gy_offset = 0;
    BMI088_Data.gz_offset = 0;

    for (int i = 0; i < BMI088_CALIB_SAMPLES; i++)
    {
        BMI088_Read_Gyro(&BMI088_Data);
        sum_gx += BMI088_Data.gx;
        sum_gy += BMI088_Data.gy;
        sum_gz += BMI088_Data.gz;
        BMI088_Delay_ms(5);
    }

    BMI088_Data.gx_offset = sum_gx / BMI088_CALIB_SAMPLES;
    BMI088_Data.gy_offset = sum_gy / BMI088_CALIB_SAMPLES;
    BMI088_Data.gz_offset = sum_gz / BMI088_CALIB_SAMPLES;
}
// void BMI088_Update_Angle(float dt)
// {
//     BMI088_Read_Gyro(&BMI088_Data);
//     BMI088_Read_Accel(&BMI088_Data);
//     BMI088_Data.angle.roll += BMI088_Data.gx * dt;
//     BMI088_Data.angle.pitch += BMI088_Data.gy * dt;
//     BMI088_Data.angle.yaw += BMI088_Data.gz * dt;
// }


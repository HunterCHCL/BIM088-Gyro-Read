#include "IST8310.h"

#define IST8310_INIT_WRITE_REG_NUM 4

IST8310_Data_t IST8310_Data={0};

static void IST8310_Read_Reg(uint8_t addr, uint8_t *data)
{
    HAL_I2C_Mem_Read(&IST8310_I2C_PORT, IST8310_I2C_ADDR << 1, addr, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}
static void IST8310_Write_Reg(uint8_t addr, uint8_t data)
{
    HAL_I2C_Mem_Write(&IST8310_I2C_PORT, IST8310_I2C_ADDR << 1, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}
static void IST8310_Read_Multi_Reg(uint8_t addr, uint8_t *data, uint16_t len)
{
    HAL_I2C_Mem_Read(&IST8310_I2C_PORT, IST8310_I2C_ADDR << 1, addr, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}
static void IST8310_Write_Multi_Reg(uint8_t addr, uint8_t *data, uint16_t len)
{
    HAL_I2C_Mem_Write(&IST8310_I2C_PORT, IST8310_I2C_ADDR << 1, addr, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}
static inline void IST8310_Delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}
static void IST8310_Reset(void)
{
    HAL_GPIO_WritePin(IST8310_RSTN_GPIO_Port, IST8310_RSTN_Pin, GPIO_PIN_RESET);
    IST8310_Delay_ms(10);
    HAL_GPIO_WritePin(IST8310_RSTN_GPIO_Port, IST8310_RSTN_Pin, GPIO_PIN_SET);
    IST8310_Delay_ms(10);
}

void IST8310_Who_Am_I(uint8_t *id)
{
    IST8310_Read_Reg(IST8310_REG_WHO_AM_I, id);
}

IST8310_Error_t IST8310_Init(void)
{
    uint8_t ist8310_init_write_reg[IST8310_INIT_WRITE_REG_NUM][3] = {
        {IST8310_REG_CNTL2, 0x08, IST8310_WRITE_CNTL2_ERR},//开启中断，中断为低电平
        {IST8310_REG_AVGCNTL, 0x09, IST8310_WRITE_AVGCNTL_ERR},//平均2次采样
        {IST8310_REG_PDCNTL, 0xC0, IST8310_WRITE_PDCNTL_ERR},
        {IST8310_REG_CNTL1, 0x0B, IST8310_WRITE_CNTL1_ERR},//连续测量模式，200Hz
    };
    uint8_t id = 0;
    IST8310_Reset();
    IST8310_Who_Am_I(&id);
    if (id != IST8310_VAL_WHO_AM_I)
        return IST8310_ID_ERR;
    for (uint8_t i = 0; i < IST8310_INIT_WRITE_REG_NUM; i++)
    {
        IST8310_Write_Reg(ist8310_init_write_reg[i][0], ist8310_init_write_reg[i][1]);
        IST8310_Delay_ms(10);
        uint8_t res = 0;
        IST8310_Read_Reg(ist8310_init_write_reg[i][0], &res);
        if (res != ist8310_init_write_reg[i][1])
            return ist8310_init_write_reg[i][2];
    }

    return IST8310_NO_ERR;
}

void IST8310_Read_Data(IST8310_Data_t *data)
{
    uint8_t raw_data[7];
    IST8310_Read_Multi_Reg(IST8310_REG_STAT1, raw_data, 7);
    data->status = raw_data[0];
    data->mag_x = IST8310_MAG_SEN * (float)(int16_t)((raw_data[2] << 8) | raw_data[1]);
    data->mag_y = IST8310_MAG_SEN * (float)(int16_t)((raw_data[4] << 8) | raw_data[3]);
    data->mag_z = IST8310_MAG_SEN * (float)(int16_t)((raw_data[6] << 8) | raw_data[5]);
}

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
/**
  * @brief  向加速度计写一个字节
  * @param  reg   寄存器地址
  * @param  data  写入的数据
  * @note   BMI088 ACC SPI写: 发送地址(最高位为0), 再发送数据
  */
void BMI088_ACC_Write_Reg(uint8_t reg, uint8_t data)
{
    uint8_t tx_data[2];
    tx_data[0] = reg & BMI088_SPI_WRITE;
    tx_data[1] = data;
    BMI088_ACC_CS_Low();
    HAL_SPI_Transmit(BMI088_SPI_PORT, tx_data, 2, HAL_MAX_DELAY);
    BMI088_ACC_CS_High();
}

/**
  * @brief  从加速度计读取一个字节
  * @param  reg  寄存器地址
  * @retval 读取到的数据
  * @note   BMI088 ACC SPI读: 发送地址(最高位为1), 发送1字节dummy, 再读取数据
  */
uint8_t BMI088_ACC_Read_Reg(uint8_t reg)
{
    uint8_t tx_data[2];
    uint8_t rx_data[2] = {0};

    /* 读操作: 地址最高位为1 */
    tx_data[0] = reg | BMI088_SPI_READ;
    tx_data[1] = 0x00;  /* dummy byte */

    BMI088_ACC_CS_Low();
    HAL_SPI_TransmitReceive(BMI088_SPI_PORT, tx_data, rx_data, 2, HAL_MAX_DELAY);
    BMI088_ACC_CS_High();

    /* rx_data[1] 为读取到的数据 (rx_data[0]为第一个dummy结果丢弃) */
    return rx_data[1];
}

/**
  * @brief  从加速度计连续读取多字节
  * @param  reg   起始寄存器地址
  * @param  buf   数据缓冲区
  * @param  len   读取长度
  * @note   先发送地址+dummy, 再连续读取len字节
  */
void BMI088_ACC_Read_Multi(uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint8_t tx_data[2];
    uint8_t rx_dummy[2];

    tx_data[0] = reg | BMI088_SPI_READ;
    tx_data[1] = 0x00;  /* dummy byte */

    BMI088_ACC_CS_Low();
    HAL_SPI_TransmitReceive(BMI088_SPI_PORT, tx_data, rx_dummy, 2, HAL_MAX_DELAY);
    HAL_SPI_Receive(BMI088_SPI_PORT, buf, len, HAL_MAX_DELAY);
    BMI088_ACC_CS_High();
}
/**
  * @brief  向陀螺仪写一个字节
  * @param  reg   寄存器地址
  * @param  data  写入的数据
  * @note   BMI088 GYRO SPI写: 发送地址(最高位为0), 再发送数据
  */
void BMI088_GYRO_Write_Reg(uint8_t reg, uint8_t data)
{
    uint8_t tx_data[2];

    tx_data[0] = reg & BMI088_SPI_WRITE;
    tx_data[1] = data;

    BMI088_GYRO_CS_Low();
    HAL_SPI_Transmit(BMI088_SPI_PORT, tx_data, 2, HAL_MAX_DELAY);
    BMI088_GYRO_CS_High();
}
/**
  * @brief  从陀螺仪读取一个字节
  * @param  reg  寄存器地址
  * @retval 读取到的数据
  * @note   BMI088 GYRO SPI读: 发送地址(最高位为1), 直接读取数据 (无需dummy)
  */
uint8_t BMI088_GYRO_Read_Reg(uint8_t reg)
{
    uint8_t tx_data[1];
    uint8_t rx_data[1] = {0};

    tx_data[0] = reg | BMI088_SPI_READ;

    BMI088_GYRO_CS_Low();
    HAL_SPI_TransmitReceive(BMI088_SPI_PORT, tx_data, rx_data, 1, HAL_MAX_DELAY);
    BMI088_GYRO_CS_High();

    return rx_data[0];
}
/**
  * @brief  从陀螺仪连续读取多字节
  * @param  reg   起始寄存器地址
  * @param  buf   数据缓冲区
  * @param  len   读取长度
  * @note   先发送地址, 再连续读取len字节 (无需dummy)
  */
void BMI088_GYRO_Read_Multi(uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint8_t tx_addr[1];

    if (buf == NULL || len == 0) return;

    tx_addr[0] = reg | BMI088_SPI_READ;

    BMI088_GYRO_CS_Low();
    HAL_SPI_Transmit(BMI088_SPI_PORT, tx_addr, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(BMI088_SPI_PORT, buf, len, HAL_MAX_DELAY);
    BMI088_GYRO_CS_High();
}
/**
  * @brief  BMI088 加速度计初始化
  * @param  range  量程 (BMI088_ACC_RANGE_3G/6G/12G/24G)
  * @param  odr    输出数据速率 (BMI088_ACC_ODR_xxx)
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Accel_Init(uint8_t range, uint8_t odr)
{
    uint8_t chip_id;
    uint8_t status;
    int retry = 5;

    /* ---- 1. 软复位 ---- */
    BMI088_ACC_Write_Reg(BMI088_ACC_SOFTRESET, BMI088_SOFTRESET_CMD);
    BMI088_Delay_ms(50);  /* 等待复位完成 */

    /* ---- 2. 检查Chip ID ---- */
    for (retry = 5; retry > 0; retry--)
    {
        chip_id = BMI088_ACC_Read_Reg(BMI088_ACC_CHIP_ID);
        if (chip_id == 0x1EU)
        {
            BMI088_Printf("BMI088 ACC: Chip ID = 0x%02X OK\r\n", chip_id);
            break;
        }
        BMI088_Printf("BMI088 ACC: Chip ID = 0x%02X (expected 0x1E), retry...\r\n", chip_id);
        BMI088_Delay_ms(10);
    }
    if (retry == 0)
    {
        BMI088_Printf("BMI088 ACC: Chip ID check FAILED!\r\n");
        return BMI088_CHIP_ID_ERR;
    }

    /* ---- 3. 使能加速度计 ---- */
    BMI088_ACC_Write_Reg(BMI088_ACC_PWR_CTRL, BMI088_ACC_ENABLE);
    BMI088_Delay_ms(5);

    /* ---- 4. 设置电源模式为活跃模式 ---- */
    BMI088_ACC_Write_Reg(BMI088_ACC_PWR_CONF, BMI088_ACC_PWR_ACTIVE);
    BMI088_Delay_ms(5);

    /* ---- 5. 配置ODR ---- */
    BMI088_ACC_Write_Reg(BMI088_ACC_CONF, odr);
    BMI088_Delay_ms(1);

    /* ---- 6. 配置量程 ---- */
    BMI088_ACC_Write_Reg(BMI088_ACC_RANGE, range);
    BMI088_Delay_ms(1);

    /* ---- 7. 验证配置 ---- */
    status = BMI088_ACC_Read_Reg(BMI088_ACC_CONF);
    BMI088_Printf("BMI088 ACC: ODR reg = 0x%02X\r\n", status);
    status = BMI088_ACC_Read_Reg(BMI088_ACC_RANGE);
    BMI088_Printf("BMI088 ACC: Range reg = 0x%02X\r\n", status);

    /* 保存当前量程 */
    g_accel_range = range;

    BMI088_Printf("BMI088 ACC: Init OK\r\n");
    return BMI088_OK;
}
/**
  * @brief  BMI088 陀螺仪初始化
  * @param  range  量程 (BMI088_GYRO_RANGE_xxxDPS)
  * @param  odr    输出数据速率 (BMI088_GYRO_ODR_xxxHZ)
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Gyro_Init(uint8_t range, uint8_t odr)
{
    uint8_t chip_id;
    int retry = 5;

    /* ---- 1. 软复位 ---- */
    BMI088_GYRO_Write_Reg(BMI088_GYRO_SOFTRESET, BMI088_SOFTRESET_CMD);
    BMI088_Delay_ms(50);  /* 等待复位完成 */

    /* ---- 2. 检查Chip ID ---- */
    for (retry = 5; retry > 0; retry--)
    {
        chip_id = BMI088_GYRO_Read_Reg(BMI088_GYRO_CHIP_ID);
        if (chip_id == 0x0FU)
        {
            BMI088_Printf("BMI088 GYRO: Chip ID = 0x%02X OK\r\n", chip_id);
            break;
        }
        BMI088_Printf("BMI088 GYRO: Chip ID = 0x%02X (expected 0x0F), retry...\r\n", chip_id);
        BMI088_Delay_ms(10);
    }
    if (retry == 0)
    {
        BMI088_Printf("BMI088 GYRO: Chip ID check FAILED!\r\n");
        return BMI088_CHIP_ID_ERR;
    }

    /* ---- 3. 设置陀螺仪为正常模式 ---- */
    BMI088_GYRO_Write_Reg(BMI088_GYRO_PWR_MODE, BMI088_GYRO_PWR_NORMAL);
    BMI088_Delay_ms(30);  /* 等待启动稳定 */

    /* ---- 4. 配置ODR ---- */
    BMI088_GYRO_Write_Reg(BMI088_GYRO_BANDWIDTH, odr);
    BMI088_Delay_ms(1);

    /* ---- 5. 配置量程 ---- */
    BMI088_GYRO_Write_Reg(BMI088_GYRO_RANGE, range);
    BMI088_Delay_ms(1);

    /* 保存当前量程 */
    g_gyro_range = range;

    BMI088_Printf("BMI088 GYRO: Init OK\r\n");
    return BMI088_OK;
}
/**
  * @brief  BMI088 总初始化 (加速度计+陀螺仪) 使用默认参数
  * @note   加速度计: ±3g, 200Hz ODR; 陀螺仪: ±2000°/s, 200Hz ODR
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Init(void)
{
    int ret;

    /* 初始化加速度计 (默认: ±3g, 200Hz) */
    ret = BMI088_Accel_Init(BMI088_ACC_RANGE_3G, BMI088_ACC_ODR_200HZ);
    if (ret != BMI088_OK)
    {
        BMI088_Printf("BMI088: Accel init failed! ret = %d\r\n", ret);
        return ret;
    }

    /* 初始化陀螺仪 (默认: ±2000°/s, 200Hz) */
    ret = BMI088_Gyro_Init(BMI088_GYRO_RANGE_2000DPS, BMI088_GYRO_ODR_200HZ);
    if (ret != BMI088_OK)
    {
        BMI088_Printf("BMI088: Gyro init failed! ret = %d\r\n", ret);
        return ret;
    }

    BMI088_Printf("BMI088: Both ACC and GYRO initialized successfully!\r\n");
    return BMI088_OK;
}
/**
  * @brief  根据当前陀螺仪量程获取灵敏度 (LSB / (°/s))
  * @param  range  量程值
  * @retval 灵敏度值
  */
static float BMI088_Get_Gyro_Sensitivity(uint8_t range)
{
    switch (range)
    {
        case BMI088_GYRO_RANGE_2000DPS:
            return 16.384f;   /* 32768 / 2000 = 16.384 LSB/°/s */
        case BMI088_GYRO_RANGE_1000DPS:
            return 32.768f;   /* 32768 / 1000 = 32.768 LSB/°/s */
        case BMI088_GYRO_RANGE_500DPS:
            return 65.536f;   /* 32768 / 500  = 65.536 LSB/°/s */
        case BMI088_GYRO_RANGE_250DPS:
            return 131.072f;  /* 32768 / 250  = 131.072 LSB/°/s */
        case BMI088_GYRO_RANGE_125DPS:
            return 262.144f;  /* 32768 / 125  = 262.144 LSB/°/s */
        default:
            return 16.384f;
    }
}
/**
  * @brief  根据当前加速度计量程获取灵敏度 (LSB/g)
  * @param  range  量程值
  * @retval 灵敏度值
  */
static float BMI088_Get_Accel_Sensitivity(uint8_t range)
{
    switch (range)
    {
        case BMI088_ACC_RANGE_3G:
            return 10920.0f;   /* ±3g  -> 10920 LSB/g (from datasheet) */
        case BMI088_ACC_RANGE_6G:
            return 5460.0f;    /* ±6g  -> 5460 LSB/g */
        case BMI088_ACC_RANGE_12G:
            return 2730.0f;    /* ±12g -> 2730 LSB/g */
        case BMI088_ACC_RANGE_24G:
            return 1365.0f;    /* ±24g -> 1365 LSB/g */
        default:
            return 10920.0f;
    }
}
/**
  * @brief  读取陀螺仪原始数据 (int16, 含符号)
  * @param  raw  存放原始数据的结构体指针
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Read_Gyro_Raw(BMI088_GyroRaw_t *raw)
{
    uint8_t buf[6] = {0};

    if (raw == NULL) return BMI088_ERROR;

    /* 连续读取6字节 (从地址0x02开始, 顺序: X_LSB, X_MSB, Y_LSB, Y_MSB, Z_LSB, Z_MSB) */
    BMI088_GYRO_Read_Multi(BMI088_GYRO_DATA_START, buf, 6);

    /* 组合原始数据 (小端格式: LSB在前, MSB在后) */
    raw->x = (int16_t)((uint16_t)buf[1] << 8) | (uint16_t)buf[0];
    raw->y = (int16_t)((uint16_t)buf[3] << 8) | (uint16_t)buf[2];
    raw->z = (int16_t)((uint16_t)buf[5] << 8) | (uint16_t)buf[4];

    return BMI088_OK;
}

/**
  * @brief  读取陀螺仪数据 (物理量 °/s)
  * @param  data  存放物理量数据的结构体指针
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Read_Gyro(BMI088_GyroData_t *data)
{
    BMI088_GyroRaw_t raw;
    float sensitivity;

    if (data == NULL) return BMI088_ERROR;

    /* 读取原始数据 */
    if (BMI088_Read_Gyro_Raw(&raw) != BMI088_OK)
    {
        return BMI088_ERROR;
    }

    /* 获取当前量程对应的灵敏度 */
    sensitivity = BMI088_Get_Gyro_Sensitivity(g_gyro_range);

    /* 转换为物理量: (°/s) = 原始值 / 灵敏度 */
    data->x = (float)raw.x / sensitivity;
    data->y = (float)raw.y / sensitivity;
    data->z = (float)raw.z / sensitivity;

    return BMI088_OK;
}
/**
  * @brief  读取加速度计原始数据 (int16, 含符号)
  * @param  raw  存放原始数据的结构体指针
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Read_Accel_Raw(BMI088_AccelRaw_t *raw)
{
    uint8_t buf[6] = {0};

    if (raw == NULL) return BMI088_ERROR;

    /* 连续读取6字节 (从地址0x12开始, 顺序: X_LSB, X_MSB, Y_LSB, Y_MSB, Z_LSB, Z_MSB) */
    BMI088_ACC_Read_Multi(BMI088_ACC_DATA_START, buf, 6);

    /* 组合原始数据 (小端格式: LSB在前, MSB在后) */
    raw->x = (int16_t)((uint16_t)buf[1] << 8) | (uint16_t)buf[0];
    raw->y = (int16_t)((uint16_t)buf[3] << 8) | (uint16_t)buf[2];
    raw->z = (int16_t)((uint16_t)buf[5] << 8) | (uint16_t)buf[4];

    return BMI088_OK;
}

/**
  * @brief  读取加速度计数据 (物理量 m/s²)
  * @param  data  存放物理量数据的结构体指针
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Read_Accel(BMI088_AccelData_t *data)
{
    BMI088_AccelRaw_t raw;
    float sensitivity;  /* LSB/g */

    if (data == NULL) return BMI088_ERROR;

    /* 读取原始数据 */
    if (BMI088_Read_Accel_Raw(&raw) != BMI088_OK)
    {
        return BMI088_ERROR;
    }

    /* 获取当前量程对应的灵敏度 (LSB/g) */
    sensitivity = BMI088_Get_Accel_Sensitivity(g_accel_range);

    /* 转换为物理量: 先转成 g, 再乘以 9.8 得到 m/s² */
    data->x = ((float)raw.x / sensitivity) * 9.8f;
    data->y = ((float)raw.y / sensitivity) * 9.8f;
    data->z = ((float)raw.z / sensitivity) * 9.8f;

    return BMI088_OK;
}
/**
  * @brief  读取加速度计内部温度
  * @param  temp  温度值指针 (单位: °C)
  * @retval BMI088_OK 成功, 其他 失败
  * @note   温度计算公式: Temp = (temp_msb * 8 + temp_lsb/32) * 0.125 + 23
  *         简化: Temp = ((int16_t)raw * 0.125) + 23
  */
int BMI088_Read_Temperature(float *temp)
{
    uint8_t buf[2];

    if (temp == NULL) return BMI088_ERROR;

    /* 读取温度寄存器 (0x22 MSB, 0x23 LSB) */
    buf[0] = BMI088_ACC_Read_Reg(BMI088_ACC_TEMP_MSB);
    buf[1] = BMI088_ACC_Read_Reg(BMI088_ACC_TEMP_LSB);

    /* 组合原始值: MSB[7:0], LSB[7:5] (高3位) */
    int16_t raw_temp = (int16_t)((uint16_t)buf[0] << 3) | ((uint16_t)buf[1] >> 5);

    /* 符号扩展 (12位有符号数扩展为16位) */
    if (raw_temp & 0x0800)
    {
        raw_temp |= 0xF000;
    }

    /* 温度转换: T = raw * 0.125 + 23 (°C) */
    *temp = (float)raw_temp * 0.125f + 23.0f;

    return BMI088_OK;
}
/**
  * @brief  批量读取加速度计和陀螺仪数据
  * @param  accel  加速度计数据指针 (可为NULL, 跳过加速度计读取)
  * @param  gyro   陀螺仪数据指针 (可为NULL, 跳过陀螺仪读取)
  * @retval BMI088_OK 成功, 其他 失败
  */
int BMI088_Read_All(BMI088_AccelData_t *accel, BMI088_GyroData_t *gyro)
{
    int ret_acc = BMI088_OK;
    int ret_gyro = BMI088_OK;

    if (accel != NULL)
    {
        ret_acc = BMI088_Read_Accel(accel);
    }

    if (gyro != NULL)
    {
        ret_gyro = BMI088_Read_Gyro(gyro);
    }

    if (ret_acc != BMI088_OK || ret_gyro != BMI088_OK)
    {
        return BMI088_ERROR;
    }

    return BMI088_OK;
}
/**
  * @brief  加速度计自检
  * @note   启用自检模式, 检查加速度计输出是否在正常范围内
  * @retval BMI088_OK 通过, 其他 失败
  */
int BMI088_Accel_SelfTest(void)
{
    uint8_t range_backup;
    uint8_t conf_backup;
    BMI088_AccelRaw_t raw_before, raw_after;
    int16_t diff_x, diff_y, diff_z;
    int result = BMI088_OK;

    /* 备份当前配置 */
    conf_backup  = BMI088_ACC_Read_Reg(BMI088_ACC_CONF);
    range_backup = BMI088_ACC_Read_Reg(BMI088_ACC_RANGE);

    /* 设置自检配置: ±24g, 1600Hz */
    BMI088_ACC_Write_Reg(BMI088_ACC_RANGE, BMI088_ACC_RANGE_24G);
    BMI088_ACC_Write_Reg(BMI088_ACC_CONF, BMI088_ACC_ODR_1600HZ);
    BMI088_Delay_ms(10);

    /* 读取自检前数据 */
    BMI088_Read_Accel_Raw(&raw_before);

    /* 启用自检 (ACC自检寄存器配置 - 此处为简化实现, 通过写入特定值触发自检) */
    /* 注意: BMI088 ACC自检通过写0x7D (PWR_CTRL) 相关位实现, 具体参考datasheet */
    /* 这里作为框架展示, 实际自检需要根据datasheet完整实现 */
    BMI088_ACC_Write_Reg(BMI088_ACC_PWR_CTRL, 0x15);  /* 位[4:2]=101 使能自检 */
    BMI088_Delay_ms(50);

    /* 读取自检后数据 */
    BMI088_Read_Accel_Raw(&raw_after);

    /* 计算差值 */
    diff_x = raw_after.x - raw_before.x;
    diff_y = raw_after.y - raw_before.y;
    diff_z = raw_after.z - raw_before.z;

    BMI088_Printf("BMI088 ACC SelfTest: diff X=%d, Y=%d, Z=%d\r\n", diff_x, diff_y, diff_z);

    /* 验证差值是否在预期范围内 (BMI088自检最小偏移量约为1g对应的LSB值) */
    float sens = BMI088_Get_Accel_Sensitivity(BMI088_ACC_RANGE_24G);
    int16_t min_diff = (int16_t)(sens * 0.5f);   /* 至少0.5g的偏移 */

    if (abs(diff_x) < min_diff || abs(diff_y) < min_diff || abs(diff_z) < min_diff)
    {
        BMI088_Printf("BMI088 ACC SelfTest: FAILED (diff too small)\r\n");
        result = BMI088_ERROR;
    }

    /* 恢复配置 */
    BMI088_ACC_Write_Reg(BMI088_ACC_RANGE, range_backup);
    BMI088_ACC_Write_Reg(BMI088_ACC_CONF, conf_backup);
    BMI088_ACC_Write_Reg(BMI088_ACC_PWR_CTRL, BMI088_ACC_ENABLE);
    BMI088_Delay_ms(10);

    return result;
}

/**
  * @brief  陀螺仪自检
  * @note   启用自检模式, 检查陀螺仪输出是否在正常范围内
  * @retval BMI088_OK 通过, 其他 失败
  */
int BMI088_Gyro_SelfTest(void)
{
    uint8_t range_backup;
    uint8_t bw_backup;
    BMI088_GyroRaw_t raw_before, raw_after;
    int16_t diff_x, diff_y, diff_z;
    int result = BMI088_OK;

    /* 备份当前配置 */
    range_backup = BMI088_GYRO_Read_Reg(BMI088_GYRO_RANGE);
    bw_backup    = BMI088_GYRO_Read_Reg(BMI088_GYRO_BANDWIDTH);

    /* 设置自检配置: ±2000dps, 200Hz */
    BMI088_GYRO_Write_Reg(BMI088_GYRO_RANGE, BMI088_GYRO_RANGE_2000DPS);
    BMI088_GYRO_Write_Reg(BMI088_GYRO_BANDWIDTH, BMI088_GYRO_ODR_200HZ);
    BMI088_Delay_ms(10);

    /* 读取自检前数据 */
    BMI088_Read_Gyro_Raw(&raw_before);

    /* 启用陀螺仪自检 */
    BMI088_GYRO_Write_Reg(0x3CU, 0x02);  /* GYRO SELF TEST REG, 使能自检 */
    BMI088_Delay_ms(100);

    /* 读取自检后数据 */
    BMI088_Read_Gyro_Raw(&raw_after);

    /* 计算差值 */
    diff_x = raw_after.x - raw_before.x;
    diff_y = raw_after.y - raw_before.y;
    diff_z = raw_after.z - raw_before.z;

    BMI088_Printf("BMI088 GYRO SelfTest: diff X=%d, Y=%d, Z=%d\r\n", diff_x, diff_y, diff_z);

    /* 验证差值是否在预期范围内 (BMI088陀螺仪自检最小偏移量) */
    /* 对于±2000dps量程, 自检偏移量通常 > 0.1 * 16.384 = ~1.64 LSB */
    float sens = BMI088_Get_Gyro_Sensitivity(BMI088_GYRO_RANGE_2000DPS);
    int16_t min_diff = (int16_t)(sens * 10.0f);  /* 至少10°/s的偏移 */

    if (abs(diff_x) < min_diff || abs(diff_y) < min_diff || abs(diff_z) < min_diff)
    {
        BMI088_Printf("BMI088 GYRO SelfTest: FAILED (diff too small)\r\n");
        result = BMI088_ERROR;
    }

    /* 恢复配置 */
    BMI088_GYRO_Write_Reg(BMI088_GYRO_RANGE, range_backup);
    BMI088_GYRO_Write_Reg(BMI088_GYRO_BANDWIDTH, bw_backup);
    BMI088_GYRO_Write_Reg(0x3CU, 0x00);  /* 关闭自检 */
    BMI088_Delay_ms(10);

    return result;
}

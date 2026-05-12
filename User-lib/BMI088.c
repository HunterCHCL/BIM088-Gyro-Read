#include "BMI088.h"
#include <string.h>
#include <math.h>

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
    uint8_t tx[3] = {addr | 0x80, 0x00, 0x00};
    uint8_t rx[3] = {0};
    BMI088_ACC_CS_Low();
    BMI088_TransmitRecieve(tx, rx, 3);
    BMI088_ACC_CS_High();
    *data = rx[2];
}
void BMI088_ACC_Read_Multi_Reg(uint8_t addr, uint8_t *data, uint16_t len)
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
    uint8_t rx[2] = {0};
    BMI088_GYRO_CS_Low();
    BMI088_TransmitRecieve(tx, rx, 2);
    BMI088_GYRO_CS_High();
    *data = rx[1];
}
void BMI088_GYRO_Read_Multi_Reg(uint8_t addr, uint8_t *data, uint16_t len)
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
    //产生SPI上升沿，将加速度计切换到SPI模式
    BMI088_ACC_Read_Reg(0x00, &dummy);
    BMI088_Delay_ms(2);
    
    //加速度计软复位
    BMI088_ACC_Write_Reg(BMI088_REG_ACC_SOFTRESET, 0xB6);
    BMI088_Delay_ms(50);
    
    //产生SPI上升沿，将加速度计切换到SPI模式
    BMI088_ACC_Read_Reg(0x00, &dummy);
    BMI088_Delay_ms(2);

    //加速度计开启工作
    BMI088_ACC_Write_Reg(BMI088_REG_ACC_PWR_CTRL, 0x04); //开启加速度计: ACC_ON
    BMI088_Delay_ms(50); //等待加速度计完成启动
    //加速度计设置为Active模式 (要在打开加速度计之后)
    BMI088_ACC_Write_Reg(BMI088_REG_ACC_PWR_CONF, 0x00); 
    BMI088_Delay_ms(10);
    BMI088_ACC_Write_Reg(BMI088_REG_ACC_RANGE, g_accel_range); //设置加速度计量程
    BMI088_Delay_ms(10);
    
    //陀螺仪软复位
    BMI088_GYRO_Write_Reg(BMI088_REG_GYRO_SOFTRESET, 0xB6);
    BMI088_Delay_ms(50);    

    //陀螺仪初始化
    BMI088_GYRO_Read_Reg(0x00, &dummy); //陀螺仪软复位可能会用到
    BMI088_Delay_ms(1);
    BMI088_GYRO_Write_Reg(BMI088_REG_GYRO_LPM1, 0x00); //正常模式
    BMI088_Delay_ms(50);
    BMI088_GYRO_Write_Reg(BMI088_REG_GYRO_RANGE, g_gyro_range); //设置陀螺仪量程
    BMI088_Delay_ms(10);
}
void BMI088_Read_Accel(BMI088_Data_t *data)
{
    uint8_t raw_data[6];
    BMI088_ACC_Read_Multi_Reg(BMI088_REG_ACC_X_L, raw_data, 6);
    data->ax = BMI088_Accel_Sensitivity() * (float)(int16_t)((raw_data[1] << 8) | raw_data[0]);
    data->ay = BMI088_Accel_Sensitivity() * (float)(int16_t)((raw_data[3] << 8) | raw_data[2]);
    data->az = BMI088_Accel_Sensitivity() * (float)(int16_t)((raw_data[5] << 8) | raw_data[4]);
}
void BMI088_Read_Gyro(BMI088_Data_t *data)
{
    uint8_t raw_data[6];
    BMI088_GYRO_Read_Multi_Reg(BMI088_REG_GYRO_X_L, raw_data, 6);
    data->gx = BMI088_Gyro_Sensitivity() * (float)(int16_t)((raw_data[1] << 8) | raw_data[0]);
    data->gy = BMI088_Gyro_Sensitivity() * (float)(int16_t)((raw_data[3] << 8) | raw_data[2]);
    data->gz = BMI088_Gyro_Sensitivity() * (float)(int16_t)((raw_data[5] << 8) | raw_data[4]);
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

/* 角度解算全局变量 */
BMI088_Angle_t BMI088_Angle = {0};
static BMI088_Kalman_t s_kf_roll;
static BMI088_Kalman_t s_kf_pitch;
static BMI088_GyroCalib_t s_gyro_calib = {0};

#define BMI088_CALIB_SAMPLES 200
#define BMI088_RAD_TO_DEG   57.29578f

/**
  * @brief  启动校准：采集静止状态下陀螺仪各轴零偏
  * @note   校准时传感器应保持静止，函数会阻塞约1秒
  */
void BMI088_Calib_Init(void)
{
    float sum_gx = 0, sum_gy = 0, sum_gz = 0;
    BMI088_Data_t data;

    s_gyro_calib.calibrated = 0;

    for (int i = 0; i < BMI088_CALIB_SAMPLES; i++)
    {
        BMI088_Read_Gyro(&data);
        sum_gx += data.gx;
        sum_gy += data.gy;
        sum_gz += data.gz;
        BMI088_Delay_ms(5);
    }

    s_gyro_calib.gx_offset = sum_gx / BMI088_CALIB_SAMPLES;
    s_gyro_calib.gy_offset = sum_gy / BMI088_CALIB_SAMPLES;
    s_gyro_calib.gz_offset = sum_gz / BMI088_CALIB_SAMPLES;
    s_gyro_calib.calibrated = 1;
}

/**
  * @brief  查询陀螺仪零偏校准是否完成
  */
uint8_t BMI088_Is_Calibrated(void)
{
    return s_gyro_calib.calibrated;
}

/**
  * @brief  初始化单轴卡尔曼滤波器
  * @param  kf       : 卡尔曼滤波器实例指针
  * @param  Q_angle  : 角度过程噪声协方差 (越小越信任陀螺仪积分)
  * @param  Q_gyro   : 角速度过程噪声协方差 (越小越信任陀螺仪零偏估计)
  * @param  R_angle  : 角度测量噪声协方差 (越大越信任陀螺仪、越小越信任加速度计)
  */
void BMI088_Kalman_Init(BMI088_Kalman_t *kf, float Q_angle, float Q_gyro, float R_angle)
{
    kf->Q_angle = Q_angle;
    kf->Q_gyro  = Q_gyro;
    kf->R_angle = R_angle;
    kf->angle   = 0;
    kf->bias    = 0;
    kf->P[0][0] = 0;
    kf->P[0][1] = 0;
    kf->P[1][0] = 0;
    kf->P[1][1] = 0;
}

/**
  * @brief  单轴卡尔曼滤波器更新 (预测+测量更新)
  * @param  kf         : 卡尔曼滤波器实例指针
  * @param  gyro_rate  : 该轴陀螺仪角速度 (°/s)
  * @param  acc_angle  : 该轴加速度计参考角度 (°)
  * @param  dt         : 上次更新以来的时间间隔 (s)
  * @retval 滤波后的角度估计值 (°)
  */
float BMI088_Kalman_Update(BMI088_Kalman_t *kf, float gyro_rate, float acc_angle, float dt)
{
    float S, K0, K1;

    /* 预测：陀螺仪积分 (状态: angle, bias) */
    kf->angle += dt * (gyro_rate - kf->bias);
    kf->P[0][0] += dt * (dt * kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + kf->Q_angle);
    kf->P[0][1] -= dt * kf->P[1][1];
    kf->P[1][0] -= dt * kf->P[1][1];
    kf->P[1][1] += kf->Q_gyro * dt;

    /* 更新：加速度计观测修正 */
    S  = kf->P[0][0] + kf->R_angle;
    K0 = kf->P[0][0] / S;
    K1 = kf->P[1][0] / S;

    kf->angle += K0 * (acc_angle - kf->angle);
    kf->bias  += K1 * (acc_angle - kf->angle);

    kf->P[0][0] -= K0 * kf->P[0][0];
    kf->P[0][1] -= K0 * kf->P[0][1];
    kf->P[1][0] -= K1 * kf->P[0][0];
    kf->P[1][1] -= K1 * kf->P[0][1];

    return kf->angle;
}

/**
  * @brief  更新角度解算 (需以固定频率调用)
  * @param  dt  上次调用以来的时间间隔 (s)
  * @note  内部读取加速度计和陀螺仪，输出角度到 BMI088_Angle
  *        首次调用前需完成 BMI088_Calib_Init()
  */
void BMI088_Update_Angle(float dt)
{
    float acc_roll, acc_pitch;
    BMI088_Data_t data;

    BMI088_Read_Gyro(&data);
    BMI088_Read_Accel(&data);

    /* 陀螺仪去零偏 */
    float gx = data.gx - s_gyro_calib.gx_offset;
    float gy = data.gy - s_gyro_calib.gy_offset;
    float gz = data.gz - s_gyro_calib.gz_offset;

    /* 由加速度计计算参考角度 */
    acc_roll  = atan2f(data.ay, data.az) * BMI088_RAD_TO_DEG;
    acc_pitch = atan2f(-data.ax, sqrtf(data.ay * data.ay + data.az * data.az)) * BMI088_RAD_TO_DEG;

    /* 卡尔曼滤波融合 */
    BMI088_Kalman_Update(&s_kf_roll,  gx, acc_roll,  dt);
    BMI088_Kalman_Update(&s_kf_pitch, gy, acc_pitch, dt);

    BMI088_Angle.roll  = s_kf_roll.angle;
    BMI088_Angle.pitch = s_kf_pitch.angle;
    BMI088_Angle.yaw  += gz * dt;
}


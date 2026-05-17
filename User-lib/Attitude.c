#include "Attitude.h"

extern volatile float q0, q1, q2, q3;

void Attitude_Update(void)
{
    BMI088_Read_Gyro(&BMI088_Data);
    BMI088_Read_Accel(&BMI088_Data);
    IST8310_Read_Data(&IST8310_Data);
    MahonyAHRSupdate(BMI088_Data.gx*BMI088_DEG_TO_RAD, BMI088_Data.gy*BMI088_DEG_TO_RAD, BMI088_Data.gz*BMI088_DEG_TO_RAD, BMI088_Data.ax, BMI088_Data.ay, BMI088_Data.az, IST8310_Data.mag_x, IST8310_Data.mag_y, IST8310_Data.mag_z);
    BMI088_Data.angle.roll = atan2f(2.0f * (q0*q1 + q2*q3), 1.0f - 2.0f * (q1*q1 + q2*q2)) * 57.29578f;
    BMI088_Data.angle.pitch = asinf(2.0f * (q0*q2 - q3*q1)) * 57.29578f;
    BMI088_Data.angle.yaw = atan2f(2.0f * (q0*q3 + q1*q2), 1.0f - 2.0f * (q2*q2 + q3*q3)) * 57.29578f;
}
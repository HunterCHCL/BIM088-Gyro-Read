#ifndef __ATTITUDE_H__
#define __ATTITUDE_H__

#include "IST8310.h"
#include "BMI088.h"
#include "MahonyAHRS.h"
#include <math.h>


extern volatile BMI088_Data_t BMI088_Data;
extern volatile IST8310_Data_t IST8310_Data;

void Attitude_Update(void);

#endif /* __ATTITUDE_H__ */
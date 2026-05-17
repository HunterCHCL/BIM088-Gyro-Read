#ifndef __IST8310_H__
#define __IST8310_H__

#include "i2c.h"

#define IST8310_I2C_PORT hi2c3

#define IST8310_I2C_ADDR 0x0E

#define IST8310_REG_WHO_AM_I 0x00
#define IST8310_VAL_WHO_AM_I 0x10

#define IST8310_REG_STAT1 0x02
#define IST8310_REG_DATAXL 0x03
#define IST8310_REG_DATAXH 0x04
#define IST8310_REG_DATAYL 0x05
#define IST8310_REG_DATAYH 0x06
#define IST8310_REG_DATAZL 0x07
#define IST8310_REG_DATAZH 0x08
#define IST8310_REG_STAT2 0x09
#define IST8310_REG_CNTL1 0x0A
#define IST8310_REG_CNTL2 0x0B
#define IST8310_REG_CNTL3 0x0C
#define IST8310_REG_TEMPL 0x0D
#define IST8310_REG_TEMPH 0x0E
#define IST8310_REG_AVGCNTL 0x41
#define IST8310_REG_PDCNTL 0x42

#define IST8310_MAG_SEN 0.3f // IST8310的磁力计灵敏度为0.3uT/LSB

typedef enum{
    IST8310_NO_ERR = 0,
    IST8310_ID_ERR = 1,
    IST8310_WRITE_CNTL2_ERR = 2,
    IST8310_WRITE_AVGCNTL_ERR = 3,
    IST8310_WRITE_PDCNTL_ERR = 4,
    IST8310_WRITE_CNTL1_ERR = 5,
} IST8310_Error_t;

typedef struct{
    uint8_t status;
    float mag_x;
    float mag_y;
    float mag_z;
} IST8310_Data_t;

extern IST8310_Data_t IST8310_Data;

IST8310_Error_t IST8310_Init(void);
void IST8310_Read_Data(IST8310_Data_t *data);

#endif /* __IST8310_H__ */
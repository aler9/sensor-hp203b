
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include "hp203b.h"

#define ADDRESS 0x76
#define ADC_CVT 0x40
#define CHANNEL_PRESSURE_AND_TEMP 0
#define READ_PRESSURE 0x30
#define READ_ALTITUDE 0x31
#define READ_TEMP 0x32

typedef struct {
    int i2c_fd;
    hp203b_dsr dsr;
} _objt;

error* hp203b_init(hp203bt** pobj, int i2c_fd, hp203b_dsr dsr) {
    _objt* _obj = malloc(sizeof(_objt));
    _obj->i2c_fd = i2c_fd;
    _obj->dsr = dsr;

    *pobj = _obj;
    return NULL;
}

void hp203b_destroy(hp203bt* obj) {
    free(obj);
}

error* hp203b_do_adc(hp203bt* obj, int wait) {
    _objt* _obj = (_objt*)obj;

    int res = ioctl(_obj->i2c_fd, I2C_SLAVE, ADDRESS);
    if(res != 0) {
        return "ioctl() failed";
    }

    // perform ADC with given dsr/osr and channels
    // must be called before sensor_get()
    uint8_t adc_cmd = ADC_CVT | _obj->dsr | CHANNEL_PRESSURE_AND_TEMP;
    res = write(_obj->i2c_fd, &adc_cmd, 1);
    if(res != 1) {
        return "unable to perform AD conversion";
    }

    // wait for ADC conversion, otherwise the sensor returns 0xFF
    if(wait) {
        usleep(hp203b_get_wait_usec(obj));
    }

    return NULL;
}

int hp203b_get_wait_usec(hp203bt* obj) {
    _objt* _obj = (_objt*)obj;
    switch(_obj->dsr) {
    case DSR_4096: return 131100;
    case DSR_2048: return 65600;
    case DSR_1024: return 32800;
    case DSR_512: return 16400;
    case DSR_256: return 8200;
    case DSR_128: return 4100;
    }
    return 0;
}

static inline error* sensor_get(hp203bt* obj, uint8_t cmd, double* pout) {
    _objt* _obj = (_objt*)obj;

    int res = ioctl(_obj->i2c_fd, I2C_SLAVE, ADDRESS);
    if(res != 0) {
        return "ioctl() failed";
    }

    res = write(_obj->i2c_fd, &cmd, 1);
    if(res != 1) {
        return "write() failed";
    }

    uint8_t out_raw[3];
    res = read(_obj->i2c_fd, out_raw, 3);
    if(res != 3) {
        return "read() failed";
    }

    *pout = ((out_raw[0] & 0x0F) * 65536 + out_raw[1] * 256 + out_raw[2]) / 100.0;
    return NULL;
}

error* hp203b_get_pressure(hp203bt* obj, double* pout) {
    return sensor_get(obj, READ_PRESSURE, pout);
}

error* hp203b_get_altitude(hp203bt* obj, double* pout) {
    return sensor_get(obj, READ_ALTITUDE, pout);
}

error* hp203b_get_temperature(hp203bt* obj, double* pout) {
    return sensor_get(obj, READ_TEMP, pout);
}

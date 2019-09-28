
#pragma once

typedef const char error;

// DSR = down sampling rate
// the higher the DSR, the higher the precision and sampling time
// 4096 = 131ms
// 128 = 4.1ms
typedef enum {
    DSR_4096 = (0 << 2),
    DSR_2048 = (1 << 2),
    DSR_1024 = (2 << 2),
    DSR_512 = (3 << 2),
    DSR_256 = (4 << 2),
    DSR_128 = (5 << 2),
} hp203b_dsr;

typedef void hp203bt;

error* hp203b_init(hp203bt** pobj, int i2c_fd, hp203b_dsr dsr);
void hp203b_destroy(hp203bt* obj);
error* hp203b_do_adc(hp203bt* obj, int wait);
int hp203b_get_wait_usec(hp203bt* obj);
error* hp203b_get_pressure(hp203bt* obj, double* target);
error* hp203b_get_altitude(hp203bt* obj, double* target);
error* hp203b_get_temperature(hp203bt* obj, double* target);

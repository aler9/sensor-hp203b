# sensor-hp203b

C library for interacting with the HP203B barometer sensor.

Features:
* works with the Raspberry Pi and probably with almost every single-board computer equipped with I2C


## Installation

Copy all the files ending with `.c` and `.h` into your project folder.


## Usage

```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "hp203b.h"

int main() {
    int i2c_fd = open("/dev/i2c-1", O_RDWR);
    if(i2c_fd < 0) {
        return -1;
    }

    void* hp203b;
    error* err = hp203b_init(&hp203b, i2c_fd, DSR_4096);
    if(err != NULL) {
        return -1;
    }

    err = hp203b_do_adc(hp203b, 1);
    if(err != NULL) {
        return -1;
    }

    double p;
    err = hp203b_get_pressure(hp203b, &p);
    if(err != NULL) {
        return -1;
    }

    double a;
    err = hp203b_get_altitude(hp203b, &a);
    if(err != NULL) {
        return -1;
    }

    double t;
    err = hp203b_get_temperature(hp203b, &t);
    if(err != NULL) {
        return -1;
    }

    printf("%f %f %f\n", p, t, a);
    return 0;
}
```


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>

#include "error.h"
#include "hp203b.h"

static uint32_t clock_usec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

static error* run() {
    // disable stdout buffering
    setbuf(stdout, NULL);

    int i2c_fd = open("/dev/i2c-1", O_RDWR);
    if(i2c_fd < 0) {
        return "unable to open /dev/i2c-1";
    }

    void* hp203b;
    error* err = hp203b_init(&hp203b, i2c_fd, DSR_512);
    if(err != NULL) {
        close(i2c_fd);
        return err;
    }

    int a_samples = 0;
    double a_weighted = 0;
    const double alpha = 0.02;

    uint32_t last_report = clock_usec();
    uint32_t read_count = 0;

    while(1) {
        err = hp203b_do_adc(hp203b, 1);
        if(err != NULL) {
            return err;
        }
        read_count++;

        double p;
        err = hp203b_get_pressure(hp203b, &p);
        if(err != NULL) {
            return err;
        }

        double a;
        err = hp203b_get_altitude(hp203b, &a);
        if(err != NULL) {
            return err;
        }

        double t;
        err = hp203b_get_temperature(hp203b, &t);
        if(err != NULL) {
            return err;
        }

        uint32_t now = clock_usec();

        if(a_samples < 5) {
            a_samples++;
            a_weighted += a/5.0;
        } else {
            a_weighted = alpha*a + (1 - alpha)*a_weighted;
            printf("%f %f %f %f\n", p, t, a, a_weighted);
        }

        if((now - last_report) >= 1000000) {
            printf("read per sec: %d\n", read_count);
            last_report = now;
            read_count = 0;
        }
    }
}

int main() {
    error* err = run();
    if(err != NULL) {
        printf("ERR: %s\n", err);
        exit(1);
    }
    return 0;
}

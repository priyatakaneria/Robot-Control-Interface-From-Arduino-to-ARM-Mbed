/*
Breadboard recorder program

Copyright 2018 Mollusc Micros Ltd.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include "mbed.h"
#include "SDBlockDevice.h"
//#include "LittleFileSystem.h"
#include "FATFileSystem.h"

#define pin_mosi PTE3
#define pin_miso PTE1
#define pin_clk PTE2
#define pin_cs PTE4

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

SDBlockDevice bd(pin_mosi, pin_miso, pin_clk, pin_cs);

//LittleFileSystem fs("fs");
FATFileSystem fs("fs");

int main() {
    fflush(stdout);
    led1 = false;
    led2 = true;
    led3 = false;
    led4 = false;

    int err = fs.mount(&bd);
    if (err) {
        printf("Failed to mount filesystem (%d, %s), formatting...\n", err, strerror(-err));
        fflush(stdout);
        err = fs.reformat(&bd);
        printf("err = %d, %s\n", err, strerror(-err));
        fflush(stdout);
        if (err > 0) {
            return 1;
        }
    }
    led1 = true;
    printf("Mounted filesystem\n");
    fflush(stdout);

    FILE *f = fopen("/fs/test.txt", "w");
    if (f) {
        err = fprintf(f, "Hello, SD card!\n");
        if (err > 0) {
            led3 = true;
            printf("Wrote %d bytes\n", err);
        } else {
            printf("Error writing = %d (%s)\n", err, strerror(-err));
            fflush(stdout);
        }
    }
    err = fclose(f);
    printf("Error closing = %d (%s)\n", err, strerror(err));
    fflush(stdout);

    err = fs.unmount();
    printf("Error unmounting = %d (%s)\n", err, strerror(err));
    fflush(stdout);
    led4 = true;

    while (true) {
        led1 = !led1;
        wait_us(1000000);
    }
}
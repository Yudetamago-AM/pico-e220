#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "../../src/pico-e220.h"
#include "./motor_encoder/src/motor.h"

#define TX_PIN  0 // rx_pin = 1
#define M0_PIN  11
#define M1_PIN  10
#define AUX_PIN 6
#define MOTOR_LEFT_A_PIN  18
#define MOTOR_RIGHT_A_PIN 22

e220_pin_t lora_pin = {TX_PIN, M0_PIN, M1_PIN, AUX_PIN};

e220_config_t lora_cfg = {
    0xffff, // addr: all(broadcast)
    96,     // baudrate: 9600 bps
    50005,  // air data rate: bw: 500, sf: 5
    32,     // subpacket length: 32 byte
    false,  // rssi: off
    13,     // transmitting power: 13
    0,      // channel: 0
    false,  // rssi output: off
    true,   // mode: transparent
    20,     // wor cycle: 2 sec
    0x0000  // encryption key: 0x0000
};

int main(void) {
    stdio_init_all();
    sleep_ms(2000);
    printf("hello, this is child.\nrc car\n");

    uint slice_l = motor_init(MOTOR_LEFT_A_PIN);
    uint slice_r = motor_init(MOTOR_RIGHT_A_PIN);

    //printf("motor test:");
    motor_rotate(slice_r, 1023);
    motor_rotate(slice_l, -1023);
    sleep_ms(1000);
    motor_rotate(slice_r, 0);
    motor_rotate(slice_l, 0);
    sleep_ms(1000);
    motor_rotate(slice_r, -1023);
    motor_rotate(slice_l, 1023);
    sleep_ms(1000);

    lora_init(uart0, &lora_pin);
    lora_config(uart0, &lora_cfg, &lora_pin);

    uint8_t rsv[50];
    int16_t cnt = 0;
    while (1) {
        lora_receive(uart0, rsv);
        if (cnt > 50) {
            motor_rotate(slice_r, 0);
            motor_rotate(slice_l, 0);
            cnt = 0;
        }

        switch (rsv[0])
        {
        case 'a':
            motor_rotate(slice_l, -1023);
            break;

        case 'd':
            motor_rotate(slice_r, 1023);
            break;

        case 'w':
            motor_rotate(slice_r, 1023);
            motor_rotate(slice_l, -1023);
            break;

        case 'v':
            motor_rotate(slice_r, 800);
            motor_rotate(slice_l, -800);
            break;

        case 'b':
            motor_rotate(slice_r, -800);
            motor_rotate(slice_l, 800);
            break;

        case 'l':
            motor_rotate(slice_l, -800);
            break;

        case 'r':
            motor_rotate(slice_r, 800);
            break;

        case 'x':
            motor_rotate(slice_r, 600);
            break;

        case 'y':
            motor_rotate(slice_r, 600);
            break;

        default:
            cnt++;
            break;
        }

        rsv[0] = 's';
        sleep_ms(10);
    }
    return 0;
}
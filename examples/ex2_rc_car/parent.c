#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "../../src\pico-e220.h"

#define DELAY   1
#define TX_PIN  0 // rx_pin = 1
#define M0_PIN  11
#define M1_PIN  10
#define AUX_PIN 6

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
    printf("hello, this is parent.\nrc car controller\n");
    lora_init(uart0, &lora_pin);
    lora_config(uart0, &lora_cfg, &lora_pin);

    uint8_t buf[4] = {'t'};
    uint8_t rsv[50];
    uint8_t ch;
    while (1) {
        ch = getchar_timeout_us(10 * 1000);
        //printf("ch: %c\n", ch);

        switch (ch)
        {
        case 'a':
            buf[0] = 'a';
            lora_send(uart0, buf, 1);
            break;
        case 'd':
            buf[0] = 'd';
            lora_send(uart0, buf, 1);
            break;
        case 'w':
            buf[0] = 'w';
            lora_send(uart0, buf, 1);
            break;
        case 'l':
            buf[0] = 'l';
            lora_send(uart0, buf, 1);
            break;
        case 'r':
            buf[0] = 'r';
            lora_send(uart0, buf, 1);
            break;
        case 'x':
            buf[0] = 'x';
            lora_send(uart0, buf, 1);
            break;
        case 'y':
            buf[0] = 'y';
            lora_send(uart0, buf, 1);
            break;
        case 'v':
            buf[0] = 'v';
            lora_send(uart0, buf, 1);
            break;
        case 'b':
            buf[0] = 'b';
            lora_send(uart0, buf, 1);
            break;
        
        default:
            break;
        }
        
        sleep_ms(200);
    }
    return 0;
}
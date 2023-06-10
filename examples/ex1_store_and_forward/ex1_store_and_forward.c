#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "../../src/pico-e220.h"

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
    uint8_t rsv[100] = {0};
    uint8_t sbuf[32] = {0};
    uint8_t timebuf[100] = {0};
    // Start on Friday 5th of June 2020 15:45:00
    datetime_t time = {
            .year  = 2020,
            .month = 06,
            .day   = 05,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 15,
            .min   = 45,
            .sec   = 00
    };

    stdio_init_all();
    sleep_ms(2000);
    printf("hello, this is store-and-forward\n");
    
    rtc_init();
    rtc_set_datetime(&time);
    sleep_us(64);

    lora_init(uart0, &lora_pin);
    lora_config(uart0, &lora_cfg, &lora_pin);

    while (1) {
        for (int i = 0; i < 100; i++) {
            rsv[i] = '0';
            timebuf[i] = '0';
        }
        rtc_get_datetime(&time);
        datetime_to_str(timebuf, 100, &time);

        for (int i = 0; i < 32; i++) sbuf[i] = timebuf[i];
        lora_send(uart0, sbuf, 32);
        printf("send: ");
        for (int i = 0; i < 32; i++) printf("%c", sbuf[i]);
        printf("\n");
        sleep_ms(500);

        lora_receive(uart0, rsv);
        printf("receive: ");
        for (int i = 0; i < 32; i++) printf("%c", rsv[i]);
        printf("\n");
        sleep_ms(500);
    }

}
#pragma once
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

typedef struct e220_config {
    uint16_t addr;            // module address to be delivered to, 0xffff for broadcast
    uint16_t baudrate_div100; // serial baudrate(bps) / 100; 12, 24, 48, 96, 192, 384, 576, 1152
    uint16_t air_data_rate;   // bw, sf. (ex) bw: 125, sf: 5 -> 12505
    uint8_t  subpacket_len;   // 200, 128, 64, 32 (byte)
    bool     rssi;            // enable rssi feature if true
    uint8_t  tx_power;        // 13, 7, 0 (dBm)
    uint8_t  channel;         // 0-37 if bw=125kHz, 0-36 if bw=250kHz, 0-30 if bw=500kHz
    bool     rssi_byte;       // output rssi (dBm) if true
    bool     tx_mode_transp;  // transparent mode if true, fixed mode if false
    uint8_t  wor_cycle_div100;// wake on receive. desired delay(ms) / 100
    uint16_t enckey;          // encryption key
} e220_config_t;

typedef struct e220_pin {
    uint8_t txpin;
    uint8_t m0pin;
    uint8_t m1pin;
    uint8_t auxpin;
} e220_pin_t;

void lora_init(uart_inst_t *uart, e220_pin_t *pin);
bool lora_mode(e220_pin_t *pin, int8_t mode);
void lora_config(uart_inst_t *uart, e220_config_t *config, e220_pin_t *pin);
void lora_send(uart_inst_t *uart, uint8_t *src, int8_t len);
uint16_t lora_receive(uart_inst_t *uart, uint8_t *dst);
#include "pico-e220.h"

void lora_init(uart_inst_t *uart, e220_pin_t *pin) {
    gpio_init(pin->m0pin);
    gpio_set_dir(pin->m0pin, GPIO_OUT);

    gpio_init(pin->m1pin);
    gpio_set_dir(pin->m1pin, GPIO_OUT);

    gpio_init(pin->auxpin);
    gpio_set_dir(pin->auxpin, GPIO_IN);

    uart_init(uart, 9600);
    gpio_set_function(pin->txpin, GPIO_FUNC_UART);
    gpio_set_function((pin->txpin)+1, GPIO_FUNC_UART);

    uart_set_baudrate(uart, 9600);
    uart_set_hw_flow(uart, false, false);
    uart_set_format(uart, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(uart, true);

#if 0
    int UART_IRQ = uart == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, lora_on_uart_rx);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(uart, true, false);
#endif
}

#if 0
void lora_on_uart_rx() {
    while (uart_is_readable(uart0)) {
        printf("%c\n", uart_getc(uart0));
    }
}
#endif

bool lora_mode(e220_pin_t *pin, int8_t mode) {
    while (gpio_get(pin->auxpin) == 0) sleep_ms(1);
#ifdef NDEBUG
    printf("lora: mode %d\n", ((mode >= 0) && (mode <= 3)) ? mode : 999);
#endif
    switch (mode)
    {
    case 0: // normal mode
        gpio_put(pin->m0pin, 0);
        gpio_put(pin->m1pin, 0);
        break;
    case 1: // WOR send mode
        gpio_put(pin->m0pin, 1);
        gpio_put(pin->m1pin, 0);
        break;
    case 2:
        gpio_put(pin->m0pin, 0);
        gpio_put(pin->m1pin, 1);
        break;
    case 3: 
        gpio_put(pin->m0pin, 1);
        gpio_put(pin->m1pin, 1);
        break;
    default:
#ifdef NDEBUG
        printf("lora_mode: invaild mode\n");
#endif
        return false;
    }
    return true;
}

void lora_config(uart_inst_t *uart, e220_config_t *config, e220_pin_t *pin) {
    uint8_t buf[15] = {0};
    uint8_t* reg = &buf[5];

    // reg0: baud rate, air data rate
    switch (config->baudrate_div100) {
    case 12:
        reg[0] |= (0 << 5);
        break;
    case 24:
        reg[0] |= (1 << 5);
        break;
    case 48:
        reg[0] |= (2 << 5);
        break;
    case 96:
        reg[0] |= (3 << 5);
        break;
    case 192:
        reg[0] |= (4 << 5);
        break;
    case 384:
        reg[0] |= (5 << 5);
        break;
    case 576:
        reg[0] |= (6 << 5);
        break;
    case 1152:
        reg[0] |= (7 << 5);
        break;
    
    default:
        // 9600bps
        reg[0] |= (3 << 5);
        break;
    }

    switch (config->air_data_rate) {
    case 12505:
        reg[0] |= 0;
        break;
    case 12506:
        reg[0] |= 4;
        break;
    case 12507:
        reg[0] |= 8;
        break;
    case 12508:
        reg[0] |= 12;
        break;
    case 12509:
        reg[0] |= 16;
        break;
    case 25005:
        reg[0] |= 1;
        break;
    case 25006:
        reg[0] |= 5;
        break;
    case 25007:
        reg[0] |= 9;
        break;
    case 25008:
        reg[0] |= 13;
        break;
    case 25009:
        reg[0] |= 17;
        break;
    case 25010:
        reg[0] |= 21;
        break;
    case 50005:
        reg[0] |= 2;
        break;
    case 50006:
        reg[0] |= 6;
        break;
    case 50007:
        reg[0] |= 10;
        break;
    case 50008:
        reg[0] |= 14;
        break;
    case 50009:
        reg[0] |= 18;
        break;
    case 50010:
        reg[0] |= 22;
        break;
    case 50011:
        reg[0] |= 26;
        break;

    default:
        // 125, 7
        reg[0] |= 8;
        break;
    }

    // reg1: subpacket_len, rssi, tx_power
    switch (config->subpacket_len) {
    case 200:
        reg[1] |= (0 << 6);
        break;
    case 128:
        reg[1] |= (1 << 6);
        break;
    case 64:
        reg[1] |= (2 << 6);
        break;
    case 32:
        reg[1] |= (3 << 6);
    
    default:
        // 200
        reg[1] |= (0 << 6);
        break;
    }

    if (config->rssi) reg[1] |= (1 << 5);

    switch (config->tx_power) {
    case 13:
        reg[1] |= 1;
        break;
    case 7:
        reg[1] |= 2;
        break;
    case 0:
        reg[1] |= 3;
        break;
    
    default:
        // 13
        reg[1] |= 1;
        break;
    }

    // reg2: channel
    switch (config->air_data_rate / 100)
    {
    case 125:
        if (!((0 < config->channel) && (config->channel < 37))) reg[2] = 0;
        break;
    case 250:
        if (!((0 < config->channel) && (config->channel < 36))) reg[2] = 0;
        break;
    case 500:
        if (!((0 < config->channel) && (config->channel < 30))) reg[2] = 0;
        break;

    default:
        reg[2] = config->channel;
        break;
    }

    // reg3: rssi_byte, tx_mode_transparent, wor_cycle
    if (config->rssi_byte) reg[3] |= (1 << 7);
    if (!config->tx_mode_transp) reg[3] |= (1 << 6);

    switch (config->wor_cycle_div100)
    {
    case 5:
        reg[3] |= 0;
        break;
    case 10:
        reg[3] |= 1;
        break;
    case 15:
        reg[3] |= 2;
        break;
    case 20:
        reg[3] |= 3;
        break;
    case 25:
        reg[3] |= 4;
        break;
    case 30:
        reg[3] |= 5;
        break;
    case 35:
        reg[3] |= 6;
        break;
    case 40:
        reg[3] |= 7;
        break;
    
    default:
        // 20 means 2000
        reg[3] |= 3;
        break;
    }

    buf[0] = 0xc0; // set register
    buf[1] = 0x00;
    buf[2] = 8;
    buf[3] = (config->addr) >> 8;
    buf[4] = (config->addr) & 0xff;
    // reg[0] to reg[3]
    buf[9] = (config->enckey) >> 8;
    buf[10] = (config->enckey) & 0xff;
    lora_mode(pin, 3);
    sleep_ms(200);
    while (uart_is_readable(uart)) uart_getc(uart);
    while (gpio_get(pin->auxpin) == 0) sleep_ms(1);
    uart_write_blocking(uart, buf, 11);
#ifdef NDEBUG
    for (int8_t i = 0; i <= 11; i++) printf("%x ", buf[i]);
    printf("\n");
#endif
    sleep_ms(200);
    while (gpio_get(pin->auxpin) == 0) sleep_ms(1);
    while (uart_is_readable(uart)) {
        uint8_t c = uart_getc(uart);
#ifdef NDEBUG
        printf("%c", c);
#endif
    }
    lora_mode(pin, 0);

    //printf("lora: done config\n");
}  

void lora_send(uart_inst_t *uart, uint8_t *src, int8_t len) {
    // only transparent mode
#if 0
    uint16_t target = 0xffff;
    uint8_t chan = 0;
    uart_putc(uart, target >> 8);
    uart_putc(uart, target & 0xff);
    uart_putc(uart, chan);
#endif
    for (int8_t i = 0; i < len; i++) uart_puts(uart, &src[i]);
#ifdef NDEBUG
    printf("send:\n");
    for (int8_t i = 0; i < len; i++) printf("%c", src[i]);
    printf("\n");
#endif
}

uint16_t lora_receive(uart_inst_t *uart, uint8_t *dst) {
    //printf("recieve: \n");
    uint16_t i = 0;
    while (uart_is_readable(uart)) {
        //printf("receive!");
        //uart_read_blocking(uart, &dst[i], 1);
        dst[i] = uart_getc(uart);
#ifdef NDEBUG
        printf("%c", dst[i]);
#endif
        i++;
    }
    
    //for (int8_t j = 0; j < i; j++) printf("%c", dst[i]);
    //printf("\n");

    return i;
}
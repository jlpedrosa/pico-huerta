#include <RF24.h>
#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "radio.h"
#include "log.h"
#include "board.h"


const uint LED_PIN = 25;
const uint CE_PIN_GPIO2 = 2;
const uint CSN_PIN_GPIO = 3;
const uint SCK_PIN_GPIO = 10; 
const uint TX_PIN_GPIO = 11;
const uint RX_PIN_GPIO = 12;

RF24 *radio = NULL;



void description() {
    bi_decl(bi_program_description("This is the controller for the huerta"));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));
    bi_decl(bi_1pin_with_name(CE_PIN_GPIO2, "Connected to the radio's CE"));
    bi_decl(bi_1pin_with_name(CSN_PIN_GPIO, "Connected to the radio's CSN"));
    bi_decl(bi_1pin_with_name(SCK_PIN_GPIO, "Connected to the radio's CSK"));
    bi_decl(bi_1pin_with_name(TX_PIN_GPIO, "Connected to the radio's TX"));
    bi_decl(bi_1pin_with_name(RX_PIN_GPIO, "Connected to the radio's RX"));
}


int main() {   
    description();
    init_board();

    log("Initializing Pins\n");
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    RadioPinOut picoPinOut = RadioPinOut{
        .CE = CE_PIN_GPIO2,
        .CSN = CSN_PIN_GPIO,
        .SCK = SCK_PIN_GPIO,
        .TX = TX_PIN_GPIO,
        .RX = RX_PIN_GPIO,
    };
   
    radio = InitRadio(picoPinOut);
    if (radio == NULL) {
           return -1;
    }   
    
    adc_init();
 
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
   
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);
 
    while (1) {
        // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
        const float conversion_factor = 3.3f / (1 << 12);
        uint16_t result = adc_read();
        printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
        sleep_ms(1000);
    }
}
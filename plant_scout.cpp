#include <RF24.h>
#include <stdio.h>

#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "radio.h"
#include "log.h"
#include "board.h"
#include "adc_sensor.h"
#include "messages.h"


const uint LED_PIN = 25;
const uint CE_PIN_GPIO2 = 2;
const uint CSN_PIN_GPIO = 3;
const uint SCK_PIN_GPIO = 10; 
const uint TX_PIN_GPIO = 11;
const uint RX_PIN_GPIO = 12;




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
    
    RF24 *radio = NULL;
    description();
    
    InitializeACD();
    uint32_t boardID = InitBoard();


    log("Initializing Pins\n");
    
   // while (1) {log("I have changed"); sleep_ms(1000);}

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

    AcdSensor sensor(ACD_GPIO_26); 
    while (1) {
        auto voltageValue =  sensor.GetValue();

        printf("Voltage: %f V\n", voltageValue );
        SensorReading sensor = SensorReading{
            device_id: boardID,
            sensor_type: HUMIDITY, 
            value: voltageValue,
        };

        if (!Send(radio, (void*)&sensor, sizeof(SensorReading))) {
            printf("Unable to send sensor reading\n");
        } else {
            printf("Sent { device_id:%ju sensor_type:%u value:%f }\n", sensor.device_id, sensor.sensor_type, sensor.value);
        }        
        sleep_ms(2000);
    }
}
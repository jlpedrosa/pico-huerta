#include <RF24.h>
#include <stdio.h>

#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pico/sync.h"
#include "pico/unique_id.h"

#include "radio.h"
#include "log.h"
#include "wifi.h"
#include "http.h"
#include "board.h"
#include "messages.h"


const uint LED_PIN = 25;
const uint CE_PIN_GPIO2 = 2;
const uint CSN_PIN_GPIO = 3;
const uint SCK_PIN_GPIO = 10; 
const uint TX_PIN_GPIO = 11;
const uint RX_PIN_GPIO = 12;


static mutex_t my_mutex;
static SensorReading lastreading = SensorReading{0, TEMPERATURE, 0.0 };

int handleRequest(char *buffer) {

    mutex_enter_blocking(&my_mutex);     
    auto written = sprintf(buffer, " { \"device_id\": %ju  \"sensor_type\": %u \"value\": %f }\n", lastreading.device_id, lastreading.sensor_type, lastreading.value);   
    mutex_exit(&my_mutex);

    return written;
}

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

    InitBoard();    
      
    RadioPinOut picoPinOut = RadioPinOut{
        .CE = CE_PIN_GPIO2,
        .CSN = CSN_PIN_GPIO,
        .SCK = SCK_PIN_GPIO,
        .TX = TX_PIN_GPIO,
        .RX = RX_PIN_GPIO,
    };   
    

    RF24 *radio = NULL;
    radio = InitRadio(picoPinOut);
    if (radio == NULL) {
           return -1;
    }

    InitWifi();
    RunWebServer(handleRequest);
    
    mutex_init(&my_mutex);

    u_int8_t buffer[1024];
    u_int32_t totalRead = 0;

    while (1)
    {
        log("Waiting for messages....\n");
        auto read = ReceiveSync(radio, buffer, 1024);
        if (read <= 0) {
            log("error reading from buffer, resetting the whole thing\n");
            totalRead = 0;
            continue;
        }
        totalRead += read;

        if (totalRead == sizeof(SensorReading)) {
            SensorReading newReading = SensorReading{};
            memcpy(&newReading, buffer,  sizeof(SensorReading));
            log("new reading { device_id:%ju  sensor_type:%d value:%f }\n", newReading.device_id, newReading.sensor_type, newReading.value);
            
            mutex_enter_blocking(&my_mutex);
            lastreading.device_id = newReading.device_id;
            lastreading.sensor_type = newReading.sensor_type;
            lastreading.value = newReading.value;
            mutex_exit(&my_mutex);
            
            totalRead = 0;
            continue;
        }
        

       

       
        sleep_ms(500);        
    }
    
    return 1;
}
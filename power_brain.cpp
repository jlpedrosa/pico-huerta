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


const uint LED_PIN = 25;
const uint CE_PIN_GPIO2 = 2;
const uint CSN_PIN_GPIO = 3;
const uint SCK_PIN_GPIO = 10; 
const uint TX_PIN_GPIO = 11;
const uint RX_PIN_GPIO = 12;

RF24 *radio = NULL;

static mutex_t my_mutex;
static int counter = 1;

int handleRequest(char *buffer) {
    mutex_enter_blocking(&my_mutex);
    int localCounter = counter;
    mutex_exit(&my_mutex);
    
    return sprintf(buffer, "{ \"counter\": \"%d\" }", localCounter);
    return 19;
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
    stdio_init_all();
    log("\nStdio initalized\n");
    
   
    sleep_ms(1 * 1000);
    log("Waiting for serial\n");
    
    init_board();    
      
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

    InitWifi();
    RunWebServer(handleRequest);

    mutex_init(&my_mutex);

    while (1)
    {
        mutex_enter_blocking(&my_mutex);
        counter++;
        mutex_exit(&my_mutex);
        sleep_ms(500);        
    }
    
    return 1;

    //SendLoop(radio, GPIO_OUT);
    //ReceiveLoop(radio, LED_PIN);
}
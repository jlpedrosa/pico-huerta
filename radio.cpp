#include "radio.h"

#include <RF24.h>
#include <stdio.h>
#include "log.h"

RF24* InitRadio(RadioPinOut pinout) {
    log("Initializing Radio\n");

    RF24* radio = new RF24(pinout.CE, pinout.CSN, RF24_SPI_SPEED ); // pin numbers connected to the radio's CE and CSN pins (respectively)

    // again please review the GPIO pins' "Function Select Table" in the Pico SDK docs
    spi.begin(spi1, pinout.SCK, pinout.TX, pinout.RX); // spi1 bus, SCK, TX, RX
 
    if (!radio->begin(&spi)) {
        log("Radio hardware is not responding!\n");
        return NULL;
    }

    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to
    // each other.
    radio->setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

    if (!radio->isChipConnected()) {
        return NULL;
    }

    if (!radio->isValid()) {
        return NULL;
    }

    radio -> setChannel(110);

    printf("Radio intialized correctly\n");
    radio->printPrettyDetails();

    return radio;
}


void SendLoop(RF24* radio, uint8_t blinkingLed) {
    const uint8_t transmiterAddr[] =  "1Node";
    const uint8_t receiverAddr[] =  "2Node";

    u_int64_t payload = 0.0;

    // save on transmission time by setting the radio to only transmit the
    // number of bytes we need to transmit a float
    radio->setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

    // set the TX address of the RX node into the TX pipe
    radio->openWritingPipe(receiverAddr); // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio->openReadingPipe(1, transmiterAddr); // using pipe 1
    
    radio->stopListening(); // put radio in TX mode

    int sucess = 0;
    int failures = 0;
    

    while (true) {    
      //  uint64_t start_timer = to_us_since_boot(get_absolute_time()); // start the timer
        bool report = radio -> write(&payload, sizeof(payload));      // transmit & save the report
      //  uint64_t end_timer = to_us_since_boot(get_absolute_time());   // end the timer

        if (report) {
            payload += 1;
            sucess++;           
        }
        else {       
            failures++;
        }

        if ( (sucess + failures) % 80 == 0) {
            printf("Transmissions{ success: %d, failures: %d }\n",sucess, failures );

            sucess =0;
            failures = 0;

        }
    }
}

void ReceiveLoop(RF24* radio, uint8_t blinkingLed) {
    const uint8_t transmiterAddr[] =  "1Node";
    const uint8_t receiverAddr[] =  "2Node";   

    u_int64_t payload = 0;

    // save on transmission time by setting the radio to only transmit the
    // number of bytes we need to transmit a float
    radio-> setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

    // set the TX address of the RX node into the TX pipe
    radio->openWritingPipe(transmiterAddr); // always uses pipe 0


    // set the RX address of the TX node into a RX pipe
    radio->openReadingPipe(1, receiverAddr); // using pipe 1

    
    radio->startListening(); // put radio in TX mode

    while (true) {
        uint8_t pipe;
        
        if (radio->available(&pipe)) {     
            payload = 0;
            gpio_put(blinkingLed, 1);                  // is there a payload? get the pipe number that recieved it
            uint8_t bytes = radio->getPayloadSize();   // get the size of the payload
            radio->read(&payload, bytes);            // fetch payload from FIFO            

            // print the size of the payload, the pipe number, payload's value
            printf("\nReceived %d bytes on pipe %d: %ju\n", bytes, pipe, payload);
            gpio_put(blinkingLed, 0);
        } else {           
            sleep_ms(10);
            printf(".");
        }        
    }
}
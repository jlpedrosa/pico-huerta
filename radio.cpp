#include "radio.h"

#include <RF24.h>
#include <stdio.h>
#include "log.h"

const uint8_t power_brain_addr[] = "1Node";
const uint8_t plant_scout_addr[] = "2Node";

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

    radio -> setPayloadSize(16);

    printf("Radio intialized correctly\n");
    radio->printPrettyDetails();

    return radio;
}

bool Send(RF24 *radio, const void *payload, uint8_t length)
{
    // set the TX address of the RX node into the TX pipe
    radio->openWritingPipe(plant_scout_addr); // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio->openReadingPipe(1, power_brain_addr); // using pipe 1

    radio->stopListening(); // put radio in TX mode

    auto result = radio->write(&payload, length); // transmit & save the report
    return result;
}


int ReceiveSync(RF24 *radio, void *buffer, uint32_t buuferSize)
{
    // set the TX address of the RX node into the TX pipe
    radio->openWritingPipe(power_brain_addr); // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio->openReadingPipe(1, plant_scout_addr); // using pipe 1

    radio->startListening(); // put radio in TX mode

    radio->printPrettyDetails();

    
    bool hadData = false;
    int readBytes = 0;
    while (true) {    
        uint8_t pipe;    
        // is there a payload? get the pipe number that recieved it
        if (radio->available(&pipe)) {                                    
            uint8_t bytes = radio->getPayloadSize(); // get the size of the payload
            radio->read(&buffer, bytes);             // fetch payload from FIFO            
            log("\nReceived %d bytes on pipe %d\n", bytes, pipe);

            hadData = true;
            readBytes += bytes;
            buffer = buffer + bytes;
        } else {
            if (hadData) {
                return readBytes;
            }
            sleep_ms(20);            
        }
    }
}
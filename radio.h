#include <RF24.h>

struct RadioPinOut {   
  uint CE;           
  uint CSN;
  uint SCK;
  uint TX;
  uint RX;
};

RF24* InitRadio(RadioPinOut pinout);

void ReceiveLoop(RF24* radio, uint8_t blinkingLed);
void SendLoop(RF24* radio, uint8_t blinkingLed);



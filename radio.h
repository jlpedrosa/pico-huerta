#include <RF24.h>

struct RadioPinOut {   
  uint CE;           
  uint CSN;
  uint SCK;
  uint TX;
  uint RX;
};

RF24* InitRadio(RadioPinOut pinout);

bool Send(RF24 *radio,  const void *payload, uint8_t length);
int ReceiveSync(RF24 *radio, void *buffer, uint32_t buuferSize);



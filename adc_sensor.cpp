#include "adc_sensor.h"

#include "hardware/adc.h"

// 12-bit conversion, assume max value == ADC_VREF == 3.3 V
const float conversion_factor = 3.3f / (1 << 12);
    
void InitializeACD() {
      adc_init();
}


AcdSensor::AcdSensor(AcdPin pin) {
   // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(pin.GPIO_PIN);
   
    // Select ADC input 0 (GPIO26)
    adc_select_input(pin.ADC_NUM);
    this->pin = pin;
}

float AcdSensor::GetValue() {
    uint16_t result = adc_read();    
    return result * conversion_factor;
}
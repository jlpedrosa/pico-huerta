#include <stdio.h>

enum SensorTypes : u_int8_t {
    HUMIDITY = 0,
    TEMPERATURE = 2,
    LUMINOSITY = 3,
};

struct SensorReading {
    u_int64_t   device_id;
    SensorTypes sensor_type;
    float       value;
};
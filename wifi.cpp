#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "wifi.h"
#include "log.h"


int InitWifi() {
    log("Initializing Wifi module\n");
    if (cyw43_arch_init()) {
            printf("failed to initialise\n");
            return 1;
    }

    cyw43_arch_enable_sta_mode();


    printf("Connecting to AP with BSSID: %s\n", WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK,  30000)) {
        printf("Giving up on WiFi");
        return -1;
    }
    

    printf("Wifi initialized correctly\n");

    return 0;
}

    
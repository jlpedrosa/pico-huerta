#include "board.h"

#include "pico/stdlib.h"
#include "pico/unique_id.h"

#include "log.h"

const int BOARD_ID_MESSAGE_LEN = 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1;


int init_board() {
    stdio_init_all();
    log("\nStdio initalized\n");
    
   
    sleep_ms(1 * 1000);
    log("Waiting for serial\n");

    char boardIDDesc[BOARD_ID_MESSAGE_LEN];
    pico_get_unique_board_id_string (boardIDDesc, BOARD_ID_MESSAGE_LEN);
    log("Booting... board ID %s", boardIDDesc);
    return 0;
}

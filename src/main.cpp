#include <Arduino.h>
#include "board_config.h"

void setup() {
    Serial.begin(115200);
    Serial.println("Board: " BOARD_NAME);
}

void loop() {
}
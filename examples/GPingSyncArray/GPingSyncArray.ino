#include <GyverPing.h>

GPingSync arr[] = { GPingSync(3, 2), GPingSync(5, 4) };
GPingSyncArray pings(arr, 2);

void setup() {
    Serial.begin(115200);
}

void loop() {
    // открой плоттер для вывода графика
    if (pings.tick()) {
        for (uint8_t i = 0; i < pings.length(); i++) {
            Serial.print(pings[i].getFiltered());
            Serial.print(',');
        }
        Serial.println();
    }
}
#include <GyverPing.h>

// trig пины на 4 и 5
GPingISR arr[] = { GPingISR(4), GPingISR(5) };
GPingISRArray pings(arr, 2);

void setup() {
    Serial.begin(115200);

    // echo пины на 2 и 3 (arduino nano)
    attachInterrupt(0, []() { pings[0].pinChange(); }, CHANGE);
    attachInterrupt(1, []() { pings[1].pinChange(); }, CHANGE);
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
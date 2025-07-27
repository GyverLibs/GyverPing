#include <GyverPing.h>

#define HC_TRIG 3
#define HC_ECHO 2

GPingSync sonar(HC_TRIG, HC_ECHO);

void setup() {
    Serial.begin(115200);
}

void loop() {
    // открой плоттер для вывода графика
    sonar.ping();
    Serial.print(sonar.getRaw());
    Serial.print(',');
    Serial.print(sonar.getSmooth());
    Serial.print(',');
    Serial.println(sonar.getFiltered());
    delay(50);
}
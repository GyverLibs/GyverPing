#include <GyverPing.h>

#define HC_TRIG 3

GPingISR sonar(HC_TRIG);

void setup() {
    Serial.begin(115200);

    // подключаем пин echo на прерывания по CHANGE
    // в данном случае пин 2 (прерывание 0)
    attachInterrupt(0, []() { sonar.pinChange(); }, CHANGE);

    // начать измерение
    sonar.ping();
}

void loop() {
    // измерение завершено (в любом случае)
    if (sonar.tick()) {
        Serial.print(sonar.getRaw());
        Serial.print(',');
        Serial.print(sonar.getSmooth());
        Serial.print(',');
        Serial.println(sonar.getFiltered());

        // ждём. В реальной программе это должен быть асинхронный таймер
        delay(50);

        // запуск следующего измерения
        sonar.ping();
    }
}
[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverPing.svg?color=brightgreen)](https://github.com/GyverLibs/GyverPing/releases/latest/download/GyverPing.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/GyverPing.svg)](https://registry.platformio.org/libraries/gyverlibs/GyverPing)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverPing?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverPing
Библиотека для ультразвукового дальномера HC-SR04
- Два режима работы: синхронный на базе pulseIn и асинхронный на прерываниях + micros
- Коррекция по температуре воздуха
- Результат в миллиметрах, точность ~1 мм (с фильтром)
- Две ступени фильтрации: пропускающий пиковый фильтр + экспоненциальное среднее
- Целочисленные вычисления
- Отдельный инструмент для асинхронного опроса массива датчиков

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

## Содержание
- [Использование](#usage)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="usage"></a>

## Использование
### Особенности
- Некорректный результат (датчик не ответил) - `0`. Реальное нулевое расстояние с датчика получиться не может
- По умолчанию максимальная дальность - 4 метра, всё что больше будет считаться `0`
- Первый фильтр пропускает пики. Если значение изменилось сильнее, чем на заданный порог (умолч. 500 мм) и это продолжается дольше нескольких измерений подряд (умолч. 3) - значение будет считаться корректным
- Второй фильтр - экспоненциальное бегущее среднее, сглаживает каждое следующее измерение и при постоянном опросе позволяет достигать миллиметровой точности с датчика

### GPingSync
Синхронное измерение на базе `pulseIn`

```cpp
// указать пины
GPingSync(uint8_t trig, uint8_t echo);

// выполнить измерение. Вернёт true при успехе. Результат можно забрать через getXxx
bool ping();

// выполнить измерение и вернуть сырой результат [мм]
uint16_t pingRaw();

// выполнить измерение и вернуть медианный результат [мм]
uint16_t pingFiltered();

// выполнить измерение и вернуть фильтрованный результат [мм]
// k = 2, 3, 4... чем больше - тем плавнее фильтр
uint16_t pingSmooth(uint8_t k = 2);

// получить среднее из N измерений с delayMs между ними. Блокирующий вызов
uint16_t pingAverage(uint8_t N, uint16_t delayMs = 50);

// сбросить фильтры (применится на следующем измерении)
void reset();

// установить температуру воздуха [градусы Цельсия]
void setTemp(int8_t temp);

// получить сырое расстояние с последнего измерения [мм]
uint16_t getRaw();

// получить медианное расстояние с последнего измерения [мм]
uint16_t getFiltered();

// получить фильтрованное расстояние с последнего измерения [мм]
// каждый вызов сильнее фильтрует значение
// k = 2, 3, 4... чем больше - тем плавнее фильтр
uint16_t getSmooth(uint8_t k = 2);

// настроить фильтр пиков
// thrsh - порог, выше которого отклонение считается шумом (умолч. 500 мм)
// n - кол-во превышающих порог измерений подряд для перехода в новый диапазон (умолч. 3)
void configFilter(uint16_t thrsh, uint8_t n);
```

#### Пример
```cpp
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
```

### GPingSyncArray
Опрашиватель массива GPingSync. Опрашивает с заданным периодом асинхронно, сам опрос - синхронный

```cpp
GPingSyncArray(GPingSync* arr, uint8_t n, uint16_t prdMs = 50);

// тикер, вызывать в loop. Вернёт true после опроса всех датчиков
bool tick();

// начать измерения (запущен по умолчанию)
void start();

// остановить измерения
void stop();

// измерения производятся
bool running();

// количество датчиков
uint8_t length();

// доступ через []
GPingSync& operator[](uint8_t i);
```

#### Пример
```cpp
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
```

### GPingISR
Асинхронное измерение на базе micros

```cpp
// пин trig
GPingISR(uint8_t trig);

// запустить измерение
void ping();

// вызывать при изменении сигнала на пине TRIG, например в прерывании по CHANGE
void pinChange();

// ожидается ответ от датчика
bool waiting();

// прервать текущее измерение
void abort();

// тикер, вызывать в loop. Вернёт true при завершении измерения. Результат можно забрать через getXxx
bool tick();

// сбросить фильтры (применится на следующем измерении)
void reset();

// установить температуру воздуха [градусы Цельсия]
void setTemp(int8_t temp);

// получить сырое расстояние с последнего измерения [мм]
uint16_t getRaw();

// получить медианное расстояние с последнего измерения [мм]
uint16_t getFiltered();

// получить фильтрованное расстояние с последнего измерения [мм]
// каждый вызов сильнее фильтрует значение
// k = 2, 3, 4... чем больше - тем плавнее фильтр
uint16_t getSmooth(uint8_t k = 2);

// настроить фильтр пиков
// thrsh - порог, выше которого отклонение считается шумом (умолч. 500 мм)
// n - кол-во превышающих порог измерений подряд для перехода в новый диапазон (умолч. 3)
void configFilter(uint16_t thrsh, uint8_t n);
```

#### Пример
```cpp
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
```

### GPingISRArray
Опрашиватель массива GPingISR

```cpp
GPingISRArray(GPingISR* arr, uint8_t n, uint16_t prdMs = 50);

// тикер, вызывать в loop. Вернёт true после опроса всех датчиков
bool tick();

// начать измерения (запущен по умолчанию)
void start(); 

// остановить измерения
void stop();

// измерения производятся
bool running();

// количество датчиков
uint8_t length();

// доступ через []
GPingISR& operator[](uint8_t i);
```

#### Пример
```cpp
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
```

<a id="versions"></a>

## Версии
- v1.0

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **GyverPing** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/GyverPing/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="feedback"></a>

## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код
#pragma once
#include "GPingBase.h"

// синхронное измерение на базе pulseIn
class GPingSync : public GPingBase {
   public:
    // указать пины
    GPingSync(uint8_t trig, uint8_t echo) : _trig(trig), _echo(echo) {
        pinMode(_echo, INPUT);
        pinMode(_trig, OUTPUT);
        digitalWrite(_trig, LOW);
    }

    // выполнить измерение. Вернёт true при успехе. Результат можно забрать через getXxx
    bool ping() {
        digitalWrite(_trig, HIGH);
        delayMicroseconds(GP_TRIG_PULSE);
        digitalWrite(_trig, LOW);

        return GPingBase::compute(pulseIn(_echo, HIGH, GP_MAX_PULSE));
    }

    // выполнить измерение и вернуть сырой результат [мм]
    uint16_t pingRaw() {
        ping();
        return GPingBase::getRaw();
    }

    // выполнить измерение и вернуть медианный результат [мм]
    uint16_t pingFiltered() {
        ping();
        return GPingBase::getFiltered();
    }

    // выполнить измерение и вернуть фильтрованный результат [мм]
    // k = 2, 3, 4... чем больше - тем плавнее фильтр
    uint16_t pingSmooth(uint8_t k = 2) {
        ping();
        return GPingBase::getSmooth(k);
    }

    // получить среднее из N измерений с delayMs между ними. Блокирующий вызов
    uint16_t pingAverage(uint8_t N, uint16_t delayMs = 50) {
        uint32_t sum = 0;
        uint8_t i = N;
        while (i--) {
            sum += pingFiltered();
            delay(delayMs);
        }
        return sum / N;
    }

   private:
    uint8_t _trig;
    uint8_t _echo;
};

// опрашиватель массива GPingSync
class GPingSyncArray {
   public:
    GPingSyncArray(GPingSync* arr, uint8_t n, uint16_t prdMs = 50) : _arr(arr), _prd(prdMs), _n(n) {}

    // тикер, вызывать в loop. Вернёт true после опроса всех датчиков
    bool tick() {
        if (_tmr && uint16_t(uint16_t(millis()) - _tmr) >= _prd) {
            _arr[_i].ping();
            if (++_i >= _n) _i = 0;
            _tmr = millis();
            if (!_tmr) --_tmr;
            if (!_i) return true;
        }
        return false;
    }

    // начать измерения (запущен по умолчанию)
    void start() {
        _tmr = 1;
        _i = 0;
    }

    // остановить измерения
    void stop() {
        _tmr = 0;
    }

    // измерения производятся
    bool running() {
        return _tmr;
    }

    // количество датчиков
    uint8_t length() {
        return _n;
    }

    // доступ через []
    GPingSync& operator[](uint8_t i) {
        return _arr[i];
    }

   private:
    GPingSync* _arr;
    uint16_t _tmr = 1;
    uint16_t _prd;
    uint8_t _n, _i = 0;
};
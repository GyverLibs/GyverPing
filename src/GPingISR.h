#pragma once
#include "GPingBase.h"

// асинхронное измерение на базе micros
class GPingISR : public GPingBase {
    enum class State : uint8_t {
        Idle,
        WaitRise,
        WaitFall,
        Done,
    };

   public:
    // пин trig
    GPingISR(uint8_t trig) : _trig(trig) {
        pinMode(_trig, OUTPUT);
        digitalWrite(_trig, LOW);
    }

    // запустить измерение
    void ping() {
        digitalWrite(_trig, HIGH);
        delayMicroseconds(GP_TRIG_PULSE);
        digitalWrite(_trig, LOW);

        _us = micros();
        _state = State::WaitRise;
    }

    // вызывать при изменении сигнала на пине TRIG, например в прерывании по CHANGE
    void pinChange() {
        switch (_state) {
            case State::WaitRise:
                _us = micros();
                _state = State::WaitFall;
                break;

            case State::WaitFall:
                _us = micros() - _us;
                _state = State::Done;
                break;

            default: break;
        }
    }

    // ожидается ответ от датчика
    bool waiting() {
        return _state != State::Idle;
    }

    // прервать текущее измерение
    void abort() {
        _state = State::Idle;
    }

    // тикер, вызывать в loop. Вернёт true при завершении измерения. Результат можно забрать через getXxx
    bool tick() {
        if (_state == State::Idle) return false;

        noInterrupts();
        uint16_t us = _us;
        State s = _state;
        interrupts();

        switch (s) {
            case State::Idle: return false;

            case State::WaitRise:
                if (uint16_t(uint16_t(micros()) - us) < GP_TRIG_TOUT) return false;
                us = 0;
                break;

            case State::WaitFall:
                if (uint16_t(uint16_t(micros()) - us) < GP_MAX_PULSE) return false;
                us = 0;
                break;

            case State::Done:
                break;
        }

        GPingBase::compute(us);
        _state = State::Idle;
        return true;
    }

   private:
    volatile uint16_t _us;
    volatile State _state = State::Idle;
    uint8_t _trig;
};

// опрашиватель массива GPingISR
class GPingISRArray {
   public:
    GPingISRArray(GPingISR* arr, uint8_t n, uint16_t prdMs = 50) : _arr(arr), _prd(prdMs), _n(n) {}

    // тикер, вызывать в loop. Вернёт true после опроса всех датчиков
    bool tick() {
        if (_arr[_i].tick()) {
            if (++_i >= _n) _i = 0;
            _tmr = millis();
            if (!_tmr) --_tmr;
            if (!_i) return true;
        } else {
            if (_tmr && uint16_t(uint16_t(millis()) - _tmr) >= _prd) {
                _arr[_i].ping();
                _tmr = 0;
            }
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
        _arr[_i].abort();
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
    GPingISR& operator[](uint8_t i) {
        return _arr[i];
    }

   private:
    GPingISR* _arr;
    uint16_t _tmr = 1;
    uint16_t _prd;
    uint8_t _n, _i = 0;
};
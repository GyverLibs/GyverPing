#pragma once
#include <inttypes.h>

#define GP_MAX_DIST 400ul    // макс. измеряемое расстояние [см]
#define GP_TRIG_PULSE 10     // импульс триггера [мкс]
#define GP_TRIG_TOUT 1000ul  // таймаут триггера [мкс]

#define GP_MAX_PULSE (GP_MAX_DIST * 58)

// пиковый фильтр
class GPPeak {
   public:
    void init(uint16_t val) {
        _filt = _raw = val;
        _count = 0;
    }

    void config(uint16_t thrsh, uint8_t n) {
        _thrsh = thrsh;
        _n = n;
    }

    uint16_t add(uint16_t val) {
        _raw = val;

        if ((val > _filt && val - _filt > _thrsh) ||
            (_filt > val && _filt - val > _thrsh)) {
            if (++_count >= _n) {
                _filt = val;
                _count = 0;
            }
        } else {
            _filt = val;
            _count = 0;
        }

        return val;
    }

    uint16_t getFiltered() {
        return _filt;
    }

    uint16_t getRaw() {
        return _raw;
    }

   private:
    uint16_t _filt = 0;
    uint16_t _raw = 0;
    uint16_t _thrsh = 500;
    uint8_t _n = 3;
    uint8_t _count = 0;
};

// целочисленный экспоненциальный фильтр
class GPExp {
   public:
    void init(uint16_t val) {
        _filt = val;
        _err = 0;
    }

    uint16_t filter(uint16_t val, uint8_t k) {
        int16_t sum = (val - _filt) + _err;
        int16_t div = sum / k;
        _err = sum - div * k;
        return _filt += div;
    }

    uint16_t getFiltered() {
        return _filt;
    }

   private:
    int16_t _filt = 0, _err = 0;
};

// медианный фильтр 3-го порядка
class GPMedian3 {
   public:
    void init(uint16_t val) {
        _buf[0] = _buf[1] = _buf[2] = val;
    }

    uint16_t add(uint16_t val) {
        if (++_i >= 3) _i = 0;
        return _buf[_i] = val;
    }

    uint16_t getFiltered() {
        return (_buf[0] < _buf[1]) ? ((_buf[1] < _buf[2]) ? _buf[1] : ((_buf[2] < _buf[0]) ? _buf[0] : _buf[2]))
                                   : ((_buf[0] < _buf[2]) ? _buf[0] : ((_buf[2] < _buf[1]) ? _buf[1] : _buf[2]));
    }

    uint16_t getRaw() {
        return _buf[_i];
    }

   private:
    uint16_t _buf[3] = {};
    uint8_t _i = 0;
};
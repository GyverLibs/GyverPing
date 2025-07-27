#pragma once
#include <Arduino.h>

#include "GPutils.h"

// базовый измеряющий класс
class GPingBase {
   public:
    GPingBase() {
        setTemp(24);
    }

    // установить температуру воздуха [градусы Цельсия]
    void setTemp(int8_t temp) {
        _tempK = 331 + temp * 61 / 100;
    }

    // получить сырое расстояние с последнего измерения [мм]
    uint16_t getRaw() {
        return _peak.getRaw();
    }

    // получить медианное расстояние с последнего измерения [мм]
    uint16_t getFiltered() {
        return _peak.getFiltered();
    }

    // получить фильтрованное расстояние с последнего измерения [мм]
    // каждый вызов сильнее фильтрует значение
    // k = 2, 3, 4... чем больше - тем плавнее фильтр
    uint16_t getSmooth(uint8_t k = 2) {
        return _exp.filter(_peak.getFiltered(), k);
    }

    // настроить фильтр пиков
    // thrsh - порог, выше которого отклонение считается шумом (умолч. 500 мм)
    // n - кол-во превышающих порог измерений подряд для перехода в новый диапазон (умолч. 3)
    void configFilter(uint16_t thrsh, uint8_t n) {
        _peak.config(thrsh, n);
    }

   protected:
    uint16_t compute(uint32_t us) {
        if (us >= GP_MAX_PULSE) us = 0;
        us = us * _tempK / 2000;

        if (_first) {
            _first = false;
            _exp.init(us);
            _peak.init(us);
        }

        return _peak.add(us);
    }

   private:
    GPPeak _peak;
    GPExp _exp;
    uint16_t _tempK;
    bool _first = true;
};
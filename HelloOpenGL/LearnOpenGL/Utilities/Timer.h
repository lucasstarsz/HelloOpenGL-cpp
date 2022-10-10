#pragma once
#ifndef TIMER_H
#define TIMER_H
#include <functional>

namespace LearnOpenGL::Utilities
{
    struct Timer
    {
        static double getSystemTimeNanos();

        explicit Timer(std::function<double()> getTimeFunction = getSystemTimeNanos);

        [[nodiscard]] float getDeltaTime() const;
        [[nodiscard]] double getLastTimestamp() const;
        [[nodiscard]] double getCurrentTime() const;

        float evaluateDeltaTime();

    private:
        std::function<double()> _alternativeGetTime;
        double _lastTimestamp;
        float _deltaTime;
    };
}

#endif

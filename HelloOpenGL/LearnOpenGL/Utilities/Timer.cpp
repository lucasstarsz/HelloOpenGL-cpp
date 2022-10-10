#include "Timer.h"

#include <chrono>

namespace chrono = std::chrono;

namespace LearnOpenGL::Utilities
{
    Timer::Timer(std::function<double()> getTimeFunction)
        : _alternativeGetTime(std::move(getTimeFunction)), _lastTimestamp(0.0), _deltaTime(0.0f)
    {
    }

    double Timer::getCurrentTime() const
    {
        if (_alternativeGetTime)
        {
            return _alternativeGetTime();
        }

        return getSystemTimeNanos();
    }

    float Timer::evaluateDeltaTime()
    {
        const double time = getCurrentTime();
        _deltaTime = !(_lastTimestamp > 0 || _lastTimestamp < 0) ? 0.0f : static_cast<float>(time - _lastTimestamp);
        _lastTimestamp = time;

        return _deltaTime;
    }

    float Timer::getDeltaTime() const
    {
        return _deltaTime;
    }

    double Timer::getLastTimestamp() const
    {
        return _lastTimestamp;
    }

    double Timer::getSystemTimeNanos()
    {
        const auto now = chrono::system_clock::now();
        const auto timeNanos = chrono::time_point_cast<chrono::nanoseconds>(now).time_since_epoch().count();

        return static_cast<double>(timeNanos) / 1000000000.0;
    }
}

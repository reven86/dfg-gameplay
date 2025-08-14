#pragma once


class Throttle 
{
public:
    Throttle(float interval) : _interval(interval), _lastTime(0.0f) {}

    bool shouldExecute() 
    {
        float currentTime = gameplay::Game::getInstance()->getGameTime();
        if (currentTime - _lastTime >= _interval)
        {
            _lastTime = currentTime;
            return true;
        }
        return false;
    }

    void setInterval(float interval) { _interval = interval; }

private:
    float _interval;
    float _lastTime;
};
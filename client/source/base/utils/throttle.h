#pragma once


class Throttle 
{
public:
    float interval;

    Throttle(float _interval) : interval(_interval), _lastTime(0.0f) {}

    bool shouldExecute() 
    {
        float currentTime = gameplay::Game::getInstance()->getGameTime();
        if (currentTime - _lastTime >= interval)
        {
            _lastTime = currentTime;
            return true;
        }
        return false;
    }

private:
    float _lastTime;
};
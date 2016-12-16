#include "slide_menu.h"


inline const gameplay::Curve::InterpolationType& SlideMenu::getAnimationInterpolator() const
{
    return _animationInterpolator;
}

inline float SlideMenu::getAnimationDuration() const
{
    return _animationDuration;
}

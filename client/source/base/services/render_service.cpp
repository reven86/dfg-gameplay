#include "pch.h"
#include "render_service.h"
#include "service_manager.h"







RenderService::RenderService(const ServiceManager * manager)
    : Service(manager)
{
}

RenderService::~RenderService()
{
}

bool RenderService::onInit()
{
    return true;
}

bool RenderService::onShutdown()
{
    _renderSteps.clear();
    return true;
}

RenderStep * RenderService::createRenderStep(const char * name, RenderStep * insertAfter)
{
    RenderStepsType::iterator it = _renderSteps.begin(), end_it = _renderSteps.end();

    while (it != end_it && (*it) != insertAfter)
        it++;

    if (it != end_it)
        it++;

    RenderStep * newStep = new RenderStep(name);
    _renderSteps.insert(it, newStep);
    return newStep;
}

RenderStep * RenderService::findRenderStep(const char * name)
{
    for (RenderStepsType::iterator it = _renderSteps.begin(), end_it = _renderSteps.end(); it != end_it; it++)
        if (!strcmp((*it)->getName(), name))
            return *it;

    return NULL;
}

void RenderService::removeRenderStep(RenderStep * step)
{
    _renderSteps.remove(step);
}

void RenderService::renderFrame()
{
    for (RenderStepsType::const_iterator it = _renderSteps.begin(), end_it = _renderSteps.end(); it != end_it; it++)
        (*it)->render();
}



//
// RenderClick
//

RenderClick::RenderClick(const char * name, std::function<void()> fn)
    : _active(true)
    , _name(name)
    , _renderFn(fn)
{
}

RenderClick::~RenderClick()
{
}




//
// RenderStep
//

RenderStep::RenderStep(const char * name)
    : _name(name)
{
}

RenderStep::~RenderStep()
{
}

RenderClick * RenderStep::findRenderClick(const char * name)
{
    for (RenderClicksType::iterator it = _renderClicks.begin(), end_it = _renderClicks.end(); it != end_it; it++)
        if (!strcmp((*it)->getName(), name))
            return *it;

    return NULL;
}

void RenderStep::addRenderClick(RenderClick * click, RenderClick * insertAfter)
{
    RenderClicksType::iterator it = _renderClicks.begin(), end_it = _renderClicks.end();

    while (it != end_it && (*it) != insertAfter)
        it++;

    if (it != end_it)
        it++;

    _renderClicks.insert(it, click);
}

void RenderStep::render() const
{
    preRenderSignal(this);
    for (RenderClicksType::const_iterator it = _renderClicks.begin(), end_it = _renderClicks.end(); it != end_it; it++)
        if ((*it)->isActive())
            (*it)->render();
    postRenderSignal(this);
}
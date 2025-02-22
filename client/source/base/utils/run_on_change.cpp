#include "pch.h"
#include "run_on_change.h"
#include "main/settings.h"






RunOnChange::RunOnChange(const std::function<void()>& fn, const std::vector<const VariantType *>& dependencies)
    : _function(fn), _dependencies(dependencies)
{
    connectDependencies();
}

RunOnChange::RunOnChange(const std::function<void()>& fn, const std::vector<const char *>& dependencies)
    : _function(fn)
{
    for(const auto& dep : dependencies)
        _dependencies.push_back(&Settings::getInstance()->get<VariantType>(dep));

    connectDependencies();
}

RunOnChange::~RunOnChange()
{
    for (auto& con : _connections)
        con.disconnect();
}

void RunOnChange::connectDependencies()
{
    GP_ASSERT(bool(_function));

    for (auto& dep : _dependencies)
        _connections.emplace_back(dep->valueChangedSignal.connect(sigc::mem_fun(this, &RunOnChange::onDependencyChanged)));

    // run the callback for the first time
    _function();
}

void RunOnChange::onDependencyChanged(const VariantType&)
{
    _function();
}
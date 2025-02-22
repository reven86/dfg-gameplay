#pragma once

#include <functional>



/**
 * A wrapper around some function which is executed when any of the dependencies change.
 * Similar to how React.useEffect works.
 */
class RunOnChange
{
public:
    /**
     * Constructs a wrapper around function with list of dependencies.
     * Dependencies are VariantType.
     *
     * @param fn Function to wrap.
     * @param dependencies List of dependencies.
     */
    RunOnChange(const std::function<void()>& fn, const std::vector<const VariantType *>& dependencies);

    /**
     * Constructs a wrapper using settings names.
     *
     * @param fn Function to wrap.
     * @param dependencies List of settings names.
     */
    RunOnChange(const std::function<void()>& fn, const std::vector<const char *>& dependencies);

    ~RunOnChange();

private:
    std::function<void()> _function;
    std::vector<const VariantType *> _dependencies;
    std::vector<sigc::connection> _connections;

    void connectDependencies();
    void onDependencyChanged(const VariantType& value);
};



/**
 * Helper base class to manage all RunOnChange objects.
 */

class DependencyWatcher
{
public:
    DependencyWatcher() {};
    virtual ~DependencyWatcher() {};

    void runOnChange(const std::function<void()>&fn, const std::vector<const VariantType *>&dependencies)
    {
        _handlers.emplace_back(new RunOnChange(fn, dependencies));
    }

    void runOnChange(const std::function<void()>& fn, const std::vector<const char *>& dependencies)
    {
        _handlers.emplace_back(new RunOnChange(fn, dependencies));
    }

private:
    std::vector<std::unique_ptr<RunOnChange>> _handlers;
};
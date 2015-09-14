#ifndef __DFG_RENDER_SERVICE__
#define __DFG_RENDER_SERVICE__

#include "service.h"





/** @brief Render click class.
 *
 *	Render click is just a very small rendering step.
 *  Render clicks are gathered into RenderStep.
 */

class RenderClick : Noncopyable
{
public:
    RenderClick(const char * name, std::function<void()> fn);
    virtual ~RenderClick();

    const bool& isActive() const { return _active; };
    void setActive(bool active) { _active = active; };

    /**
     * Render click name.
     */
    const char * getName() const { return _name.c_str(); };

    /**
     * Rendering.
     */
    void render() const { _renderFn(); };

private:
    bool _active;
    std::function<void()> _renderFn;
    std::string _name;
};




/** @brief Named collection of render clicks.
 *
 *	RenderStep is a logical step of scene rendering. For example, typical
 *	scene will consist of scene graph, shadows, HUD rendering steps.
 *	Scene graph step by itself may consist of depth-only,
 *	color, reflection render clicks.
 */

class RenderStep : Noncopyable
{
    friend class RenderService;

public:
    /**
     * Signals is invoked when RenderStep is about to be drawn.
     */
    sigc::signal<void, const RenderStep *> preRenderSignal;

    /**
    * Signals is invoked when RenderStep has been drawn.
    */
    sigc::signal<void, const RenderStep *> postRenderSignal;



    virtual ~RenderStep();

    /**
     * Get render step's active flag. True by default.
     */
    const bool& isActive() const { return _active; };

    /**
     * Set render step's active flag.
     */
    void setActive(bool active) { _active = active; };

    /**
     * Render step name.
     */
    const char * getName() const { return _name.c_str(); };

    /**
     * Find RenderClick by its name.
     */
    RenderClick * findRenderClick(const char * name);

    /**
     * Add render click before specified one, or at the end.
     */
    void addRenderClick(RenderClick * click, RenderClick * insertAfter = NULL);

    /**
     * Render.
     */
    void render() const;

private:
    RenderStep(const char * name);

    typedef std::list<RenderClick *> RenderClicksType;
    RenderClicksType _renderClicks;
    std::string _name;
    bool _active;
};





/** @brief Service responsible for scene rendering.
 *
 *	@author Andrew "RevEn" Karpushin
 */

class RenderService : public Service
{
    friend class ServiceManager;

public:
    static const char * getTypeName() { return "RenderService"; }

    //
    // Render steps management.
    //

    /**
     * Create render step and insert it after specified render step (or at the end of list)
     */
    RenderStep * createRenderStep(const char * name, RenderStep * insertAfter = NULL);

    /**
     * Find render step by name.
     */
    RenderStep * findRenderStep(const char * name);

    /**
     * Remove render step from the rendering list.
     */
    void removeRenderStep(RenderStep * step);

    /**
     * Draw frame.
     */
    void renderFrame();

protected:
    RenderService(const ServiceManager * manager);
    virtual ~RenderService();

    virtual bool onInit();
    virtual bool onShutdown();
    virtual bool onTick() { return false; };

private:
    typedef std::list<RenderStep *> RenderStepsType;
    RenderStepsType	_renderSteps;
};





#endif
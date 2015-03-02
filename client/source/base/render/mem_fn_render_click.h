#ifndef __DFG_MEMFNRENDERCLICK__
#define __DFG_MEMFNRENDERCLICK__

#include "services/render_service.h"





//! Renders click which invokes class member function on render.
template< class _Class >
class MemFnRenderClick : public RenderClick
{
	const _Class * _instance;
	void (_Class::* _renderFn ) () const;
	std::string _name;

public:
	MemFnRenderClick( const _Class * instance, void (_Class::* renderFn) ( ) const, const char * name )
		: _renderFn( renderFn ), _instance( instance ), _name( name ) {};
	virtual ~MemFnRenderClick( ) {};

	//! Render click name.
	virtual const char * GetName( ) const { return _name.c_str( ); };

	//! Rendering.
	virtual void Render ( ) const { ( _instance->*_renderFn )( ); };
};




#endif
#ifndef __DFG_RENDER_SERVICE__
#define __DFG_RENDER_SERVICE__

#include "service.h"





/*! \brief Render click class.
 *
 *	Render click is just a simple rendering step. Render clicks
 *	are gathered into RenderStep.
 */

class RenderClick : public gameplay::Ref, Noncopyable
{
	bool _active;

public:
	RenderClick				( );
	virtual ~RenderClick	( );

	const bool& IsActive	( ) const { return _active; };
	void SetActive			( bool active ) { _active = active; };

	//! Render click name.
	virtual const char * GetName( ) const = 0;

	//! Rendering.
	virtual void Render ( ) const = 0;
};




/*! \brief Named collection of render clicks.
 *	
 *	RenderStep is a logical step of scene rendering. For example, typical 
 *	scene will consist of scene graph, shadows, HUD rendering steps.
 *	Scene graph step by itself may consist of depth-only,
 *	color, reflection render clicks.
 */

class RenderStep : public gameplay::Ref, Noncopyable
{
	friend class RenderService;

	typedef std::list< RefPtr< RenderClick > > RenderClicksType;
	RenderClicksType _renderClicks;
	std::string _name;

private:
	RenderStep			( const char * name );

public:
	virtual ~RenderStep	( );

	//! Render step name.
	const char * GetName( ) const { return _name.c_str( ); };

	//! Find RenderClick by its name.
	RenderClick * FindRenderClick( const char * name );

	//! Add render click (takes ownership) before specified one, or at the end.
	void AddRenderClick	( RenderClick * click, RenderClick * insert_after = NULL );

	//! Render.
	void Render ( ) const;
};





/*! \brief Service responsible for scene rendering.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class RenderService : public Service
{
	typedef std::list< RefPtr< RenderStep > > RenderStepsType;
	RenderStepsType	_renderSteps;

	typedef std::unordered_map< gameplay::Ref *, gameplay::SpriteBatch * > SpriteBatchRegistryType;
	SpriteBatchRegistryType _spriteBatchRegistry;
	gameplay::SpriteBatch * _activeSpriteBatch;

    typedef std::tuple< gameplay::VertexFormat, gameplay::Mesh::PrimitiveType, gameplay::Material *, bool > MeshBatchItem;
    typedef std::list< std::pair< MeshBatchItem, gameplay::MeshBatch * > > MeshBatchRegistryType;
    MeshBatchRegistryType _meshBatchRegistry;
    gameplay::MeshBatch * _activeMeshBatch;

public:
	RenderService			( const ServiceManager * manager );
	virtual ~RenderService	( );

	static const char * GetName( ) { return "RenderService"; }

	virtual bool OnInit( );
	virtual bool OnShutdown( );
    virtual bool OnTick( ) { return false; };

	//
	// Drawing helper functions.
	//

	/*! Creates sprite batch instance for texture, 
     *  assuming sprite is rendered by standard VSH and PSH programs
     *  and use alpha blending with premultiplied alpha.
     *
     *  Takes ownership of texture.
     */
	gameplay::SpriteBatch * RegisterSpriteTexture ( gameplay::Texture * texture );

	/*! \brief Creates sprite batch instance for material.
     *
     *   Takes ownership of material.
     */
    gameplay::SpriteBatch * RegisterSpriteMaterial ( gameplay::Material * material );

	/*! \brief Creates or returns mesh batch instance by its vertex format, primitive type and indexed flag.
     *
     *   Takes ownership of material.
     */
    gameplay::MeshBatch * RegisterMeshBatch ( const gameplay::VertexFormat& vertexFormat, const gameplay::Mesh::PrimitiveType& primitiveType, gameplay::Material * material, bool indexed );

	//! Begin sprite rendering.
	void BeginSpritesRendering ( gameplay::SpriteBatch * batch );

	//! Begin mesh rendering.
    void BeginMeshesRendering ( gameplay::MeshBatch * batch );

	//! Flush sprites and meshes.
	void FlushAll ( );

	//
	// Render steps management.
	//

	//! Create render step and insert it after specified render step (or at the end of list)
	RenderStep * CreateRenderStep	( const char * name, RenderStep * insertAfter = NULL );

	//! Find render step by name.
	RenderStep * FindRenderStep		( const char * name );

	//! Draw frame.
	void RenderFrame ( );
};





#endif
#ifndef __DFG_ASSET__
#define __DFG_ASSET__




/*! \brief Base resource class.
 *
 *	Base asset object that can be loaded from stream.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class Asset : public gameplay::Ref
{
    std::string _name;
    std::string _filename;

public:
    //! Constructs an empty resource by name.
    Asset ( const char * name ) : _name( name ) {};

    //! Destructs resource.
    virtual ~Asset ( ) { };

    //! Get resource name.
    const char * GetName ( ) const { return _name.c_str( ); };

    //! Get resource file name.
    const char * GetFilename ( ) const { return _filename.c_str( ); };

    //! Load resource from file.
    virtual bool LoadFromFile ( const char * filename );

    //! Load resource from stream.
    virtual bool LoadFromStream ( gameplay::Stream * /*stream*/ ) { return false; };

    //! String representation.
    std::string ToString ( ) const { return "<" + _name + " from '" + ( _filename.empty( ) ? "memory" : _filename ) + "'>"; };

protected:
    //! Get/Set resource file name.
    void SetFilename ( const char * filename ) { _filename = filename; };
};






#endif // __DFG_ASSET__
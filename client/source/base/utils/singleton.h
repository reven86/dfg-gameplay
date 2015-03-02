#ifndef __DFG_SINGLETON__
#define __DFG_SINGLETON__





/*! \brief %Singleton base class.
 *
 *	\author Andrew "RevEn" Karpushin
 */

template< class T >
class Singleton : public Noncopyable
{
protected:
	Singleton					( ) { };
	virtual ~Singleton			( ) { };

public:
	//! Get an instance.
	static T& Instance			( )
	{
		static T instance;

		return instance;
	};
};




#endif // __DFG_SINGLETON__
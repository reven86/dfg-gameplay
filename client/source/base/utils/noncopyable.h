#ifndef __DFG_NONCOPYABLE__
#define __DFG_NONCOPYABLE__





/** @class Noncopyable noncopyable.h "utils/noncopyable.h"
 *	@brief %Noncopyable base class.
 *
 *	@author Andrew "RevEn" Karpushin
 */
 
class Noncopyable
{
protected:
	Noncopyable() {}
	~Noncopyable() {}
private:  // emphasize the following members are private
	Noncopyable( const Noncopyable& ) = delete;
	const Noncopyable& operator=( const Noncopyable& ) = delete;
};




#endif // __DFG_NONCOPYABLE__
#ifndef __DFG_SINGLETON__
#define __DFG_SINGLETON__





/** @brief %Singleton base class.
 *
 *  Classes derived from singleton MUST have trivial default constructor and destructor.
 *
 *	@author Andrew "RevEn" Karpushin
 */

template< class T >
class Singleton : public Noncopyable
{
public:
	/**
     * Get an instance.
     */
	static T * getInstance		()
	{
		static T instance;
		return &instance;
	};

protected:
    Singleton() {};
    virtual ~Singleton() {};

private:
    Singleton(const Singleton&) = delete;
    Singleton& operator = (const Singleton&) = delete;
};




#endif // __DFG_SINGLETON__
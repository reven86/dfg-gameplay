#ifndef __DFG_CURVE__
#define __DFG_CURVE__

#include "utils.h"





/**
 * %Curve class is a container that assign custom values with
 * decimal keys. %Curve provides methods to get a value between keys
 * using custom interpolator.
 *
 * Template class is specialezed by value type T, key type _KT and
 * interpolator functor _Interpolator which by default is LinearInterpolator< T >
 *
 * @author Andrew "RevEn" Karpushin
 */

template<class T, class _KT = uint8_t, class _Interpolator = Utils::LinearInterpolator<T> >
class Curve
{
public:
    typedef std::pair<_KT, T> KeyType;
    typedef std::vector<KeyType> KeysType;

    Curve() {};
    ~Curve() {};

    /** @brief Add new key and value associated with this key.
     *
     *	@note Keys should be added in ascending order.
     *
     *	@param[in] t		Key.
     *	@param[in] key		Value.
     */
    void addKey(const _KT& t, const T& key)
    {
        GP_ASSERT(_keys.empty() || (t > _keys.back().first));
        if (_keys.empty() || (t > _keys.back().first))
            _keys.push_back(KeyType(t, key));
    };

    /**
     * Gets a value, associated with key.
     */
    T key(const _KT& t) const
    {
        if (_keys.empty())
            return _emptyKey;

        struct key_comp : public std::binary_function < bool, KeyType, KeyType >
        {
            bool operator( ) (const KeyType& a, const KeyType& b) const
            {
                return a.first < b.first;
            };
        };

        typename KeysType::const_iterator it2 = std::lower_bound(
            _keys.begin(),
            _keys.end(),
            KeyType(t, _emptyKey),
            key_comp()
        );

        if (it2 != _keys.end())
        {
            if (it2 == _keys.begin())
                return (*it2).second;

            typename KeysType::const_iterator it = it2;
            it--;

            float lerp_frac = static_cast<float>(t - (*it).first) / ((*it2).first - (*it).first);

            return _Interpolator().interpolate((*it).second, (*it2).second, lerp_frac);
        }

        return _keys.back().second;
    };

    /**
     * Makes a curve empty.
     */
    void clear() { KeysType().swap(_keys); };

    /**
     * Whether Curve is empty, e.g. contains no keys.
     */
    bool empty() const { return _keys.empty(); };

    /**
     * Initialize Curve from Properties.
     */
    inline bool initialize(gameplay::Properties * properties);

    /**
     * Access to keys.
     */
    const KeysType& keys() const { return _keys; };

private:
    KeysType _keys;
    T _emptyKey;
};




template<>
inline bool Curve<float>::initialize(gameplay::Properties * properties)
{
    clear();
    const char* name;
    while ((name = properties->getNextProperty()) != 0)
        addKey(static_cast<unsigned char>(atoi(name)), properties->getFloat());

    return true;
};



template<>
inline bool Curve<gameplay::Vector4>::initialize(gameplay::Properties * properties)
{
    clear();
    const char* name;
    while ((name = properties->getNextProperty()) != 0)
    {
        char * tmp;

        // assume colors are stored as hex values
        addKey(static_cast<unsigned char>(atoi(name)), gameplay::Vector4::fromColor(strtoul(properties->getString(), &tmp, 16)));
    }

    return true;
};



#endif // __DFG_CURVE__
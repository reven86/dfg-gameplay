#ifndef __DFG_GAMEPLAY_ASSETS__
#define __DFG_GAMEPLAY_ASSETS__





/** Asset wrapper for gameplay::Properties.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class PropertiesAsset : public Asset
{
    gameplay::Properties * _properties;

    static Cache< PropertiesAsset > _cache;

protected:
    PropertiesAsset();

public:
    virtual ~PropertiesAsset();

    static Cache< PropertiesAsset >& getCache() { return _cache; };

    /** Load Properties from url.
     */
    static PropertiesAsset * create(const char * url);

    //! Get resource name.
    virtual const char * getTypeName() const { return "PropertiesAsset"; };

    /** Reload asset.
     *
     * \return true when asset was reloaded successfully.
     */
    virtual bool reload();

    /// Return underlying asset.
    gameplay::Properties * get () const { return _properties; };
};




/** Asset wrapper for gameplay::SpriteBatch.
 *
 *  URL for SpriteBatch creation is URL to material file.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class SpriteBatchAsset : public Asset
{
    gameplay::SpriteBatch * _spriteBatch;

    static Cache< SpriteBatchAsset > _cache;

protected:
    SpriteBatchAsset();

public:
    virtual ~SpriteBatchAsset();

    static Cache<SpriteBatchAsset>& getCache() { return _cache; };

    /** Load SpriteBatch from url.
    */
    static SpriteBatchAsset * create(const char * materialURL);

    //! Get resource name.
    virtual const char * getTypeName() const { return "SpriteBatchAsset"; };

    /** Reload asset.
    *
    * \return true when asset was reloaded successfully.
    */
    virtual bool reload();

    /// Return underlying asset.
    gameplay::SpriteBatch * get () const { return _spriteBatch; };
};





/** Asset wrapper for Ref-based gameplay assets (Font, AudioSource, etc).
 *
 *  URL for Asset creation is URL to asset's file.
 *
 *	\author Andrew "RevEn" Karpushin
 */

template<class _Type>
class GameplayRefAsset : public Asset
{
    RefPtr< _Type > _asset;

    static Cache< GameplayRefAsset< _Type > > _cache;

protected:
    GameplayRefAsset() {};

public:
    virtual ~GameplayRefAsset() {};

    static Cache< GameplayRefAsset< _Type > >& getCache() { return _cache; };

    /** Load FontAsset from url.
    */
    static GameplayRefAsset< _Type > * create(const char * url)
    {
        _Type * asset = _Type::create(url);
        if (!asset)
            return NULL;

        GameplayRefAsset< _Type > * res = new GameplayRefAsset< _Type >();
        res->_asset.reset(asset);
        res->setURL(url);

        return res;
    }

    //! Get resource name.
    virtual const char * getTypeName() const { return "GameplayRefAsset"; };

    /** Reload asset.
    *
    * \return true when asset was reloaded successfully.
    */
    virtual bool reload()
    {
        _asset.reset(_Type::create(getURL()));
        return _asset.get() != NULL;
    }

    /// Return underlying asset and increment ref counter.
    _Type * share() const { const_cast<_Type *>(_asset.get())->addRef(); return const_cast<_Type *>(_asset.get()); };

    /// Return underlying asset.
    _Type * get() const { return const_cast<_Type *>(_asset.get()); };
};





#endif // __DFG_GAMEPLAY_ASSETS__
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

    operator gameplay::Properties * () const { return _properties; };
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

    operator gameplay::SpriteBatch * () const { return _spriteBatch; };
};





/** Asset wrapper for gameplay::Font.
 *
 *  URL for Font creation is URL to font file.
 *
 *	\author Andrew "RevEn" Karpushin
 */

class FontAsset : public Asset
{
    RefPtr< gameplay::Font > _font;

    static Cache< FontAsset > _cache;

protected:
    FontAsset();

public:
    virtual ~FontAsset();

    static Cache<FontAsset>& getCache() { return _cache; };

    /** Load FontAsset from url.
    */
    static FontAsset * create(const char * url);

    //! Get resource name.
    virtual const char * getTypeName() const { return "FontAsset"; };

    /** Reload asset.
    *
    * \return true when asset was reloaded successfully.
    */
    virtual bool reload();

    operator gameplay::Font * () const { return const_cast<gameplay::Font *>(_font.get()); };
};






#endif // __DFG_GAMEPLAY_ASSETS__
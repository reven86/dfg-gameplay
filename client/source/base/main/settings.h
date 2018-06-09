#pragma once


#ifndef __DFG_SETTINGS_H__
#define __DFG_SETTINGS_H__

#include "archive.h"




/**
 * Class that holds all app's settings.
 * Basically an archive with predefined key-value pairs and some helper methods.
 *
 * Every component of the app can put its own properties here to make them serializable.
 */

class Settings : public Archive, public Singleton<Settings>
{
    friend class Singleton<Settings>;

public:
    /**
     * Helper function to allow connect specialized slots to general VariantType's signals.
     */
    template<typename _Type, typename _Fn> inline sigc::connection connect(const char * key, const _Fn& fn) const;

    /**
     * Helper function to allow connect specialized validators to general VariantType's signals.
     */
    template<typename _Type, typename _Fn> inline sigc::connection connectValidator(const char * key, const _Fn& fn) const;


protected:
    Settings();

private:
    template<typename _Type, typename _Fn> inline void slotFunctor(const VariantType& value, const _Fn& fn) const;
    template<typename _Type, typename _Fn> inline bool validatorFunctor(const VariantType& oldValue, VariantType& newValue, const _Fn& fn) const;
};



#include "settings.inl"


#endif // __DFG_SETTINGS_H__
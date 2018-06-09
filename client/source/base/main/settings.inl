#include "settings.h"





template<typename _Type, typename _Fn> inline sigc::connection Settings::connect(const char * key, const _Fn& fn) const
{
    const VariantType& value = get<VariantType>(key);
    GP_ASSERT(!value.isEmpty());
    return value.valueChangedSignal.connect(sigc::bind(sigc::mem_fun(this, &Settings::slotFunctor<_Type, _Fn>), fn));
}

template<typename _Type, typename _Fn> inline sigc::connection Settings::connectValidator(const char * key, const _Fn& fn) const
{
    const VariantType& value = get<VariantType>(key);
    GP_ASSERT(!value.isEmpty());
    return value.valueValidatorSignal.connect(sigc::bind(sigc::mem_fun(this, &Settings::validatorFunctor<_Type, _Fn>), fn));
}

template<typename _Type, typename _Fn> void Settings::slotFunctor(const VariantType& value, const _Fn& fn) const
{
    fn(value.get<_Type>());
}

template<typename _Type, typename _Fn> bool Settings::validatorFunctor(const VariantType& oldValue, VariantType& newValue, const _Fn& fn) const
{
    _Type newVal = newValue.get<_Type>();
    if (!fn(oldValue.get<_Type>(), newVal))
        return false;
    if (newVal != newValue.get<_Type>())
        newValue.set(newVal);
    return true;
}
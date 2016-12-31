#include "intrusive_list.h"




template<class _Type>
IntrusiveList<_Type>::IntrusiveList()
{
    GP_ASSERT(__first && __last || !__first && !__last);

    if (__last)
    {
        __last->_next = this;
        this->_prev = __last;
        this->_next = nullptr;
        __last = this;
    }
    else
    {
        __first = __last = this;
        this->_prev = this->_next = nullptr;
    }
}

template<class _Type>
IntrusiveList<_Type>::IntrusiveList(const IntrusiveList& other)
{
    GP_ASSERT(__first && __last || !__first && !__last);

    if (__last)
    {
        __last->_next = this;
        this->_prev = __last;
        this->_next = nullptr;
        __last = this;
    }
    else
    {
        __first = __last = this;
        this->_prev = this->_next = nullptr;
    }
}

template<class _Type>
IntrusiveList<_Type>::~IntrusiveList()
{
    GP_ASSERT(__first && __last);

    if (this->_prev)
        this->_prev->_next = this->_next;
    else
        __first = this->_next;

    if (this->_next)
        this->_next->_prev = this->_prev;
    else
        __last = this->_prev;
}

template<class _Type>
inline _Type * IntrusiveList<_Type>::getFirstInList()
{
    return static_cast<_Type *>(__first);
}

template<class _Type>
inline _Type * IntrusiveList<_Type>::getLastInList()
{
    return static_cast<_Type *>(__last);
}

template<class _Type>
inline _Type * IntrusiveList<_Type>::getNextInList()
{
    return static_cast<_Type *>(_next);
}

template<class _Type>
inline const _Type * IntrusiveList<_Type>::getNextInList() const
{
    return static_cast<const _Type *>(_next);
}

template<class _Type>
inline _Type * IntrusiveList<_Type>::getPrevInList()
{
    return static_cast<_Type *>(_prev);
}

template<class _Type>
inline const _Type * IntrusiveList<_Type>::getPrevInList() const
{
    return static_cast<const _Type *>(_prev);
}
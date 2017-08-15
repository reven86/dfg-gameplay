#pragma once


#ifndef __INTRUSIVE_LIST_H__
#define __INTRUSIVE_LIST_H__



/**
 * Template class of intrusive lists implementation.
 * Derive from this class if you need to traverse list of one kind of objects
 * and you don't want to store then in any container.
 *
 * Any new object constructed is added as a last element in the list.
 */

template<class _Type>
class IntrusiveList
{
public:
    /**
     * Default constructor adds an instance as a last in the list.
     */
    IntrusiveList();

    /**
     * Copy constructor adds an instance as a last in the list.
     */
    IntrusiveList(const IntrusiveList& other);

    /**
     * Destructor removes the instance from the list.
     */
    virtual ~IntrusiveList();

    /**
     * Get the first item in the list.
     */
    static inline _Type * getFirstInList();

    /**
     * Get the list item in the list.
     */
    static inline _Type * getLastInList();

    /**
     * Get next item in the list.
     */
    inline _Type * getNextInList();

    /**
     * Get next item in the list (const).
     */
    inline const _Type * getNextInList() const;

    /**
     * Get previous item in the list.
     */
    inline _Type * getPrevInList();

    /**
     * Get previous item in the list (const).
     */
    inline const _Type * getPrevInList() const;

private:
    static IntrusiveList<_Type> * __first;
    static IntrusiveList<_Type> * __last;

    IntrusiveList<_Type> * _next;
    IntrusiveList<_Type> * _prev;
};


template <typename _Type>
IntrusiveList< _Type > * IntrusiveList< _Type >::__first = nullptr;
template <typename _Type>
IntrusiveList< _Type > * IntrusiveList< _Type >::__last = nullptr;


#include "intrusive_list.inl"


#endif // __INTRUSIVE_LIST_H__
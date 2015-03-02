#ifndef __DFG_PRIORITYSIGNAL__
#define __DFG_PRIORITYSIGNAL__




/*! \brief sigc::signal wrapper that stores slots 
 *  as ordered collection
 */
template< class _Signal >
class priority_signal
{
public:
    _Signal signal;

    typedef typename _Signal::emitter_type           emitter_type;
    typedef typename _Signal::emitter_type::result_type         result_type;
    typedef typename _Signal::slot_type              slot_type;
    typedef typename _Signal::slot_list_type         slot_list_type;
    typedef typename _Signal::iterator               iterator;
    typedef typename _Signal::const_iterator         const_iterator;
    typedef typename _Signal::reverse_iterator       reverse_iterator;
    typedef typename _Signal::const_reverse_iterator const_reverse_iterator;

private:
    struct slot_data
    {
        int priority;
        iterator slot_it;

        slot_data( int p, iterator it ) : priority( p ), slot_it( it ) {};
    };

    struct slot_data_cmp : public std::binary_function< bool, int, slot_data >
    {
        bool operator( )    ( const int& a, const slot_data& b ) const
        {
            return a < b.priority;
        };
    };

    typedef std::list< slot_data >  slot_data_list_type;
    slot_data_list_type _slotData;

public:

    /** Add a slot to the list of slots.
    * Any functor or slot may be passed into connect().
    * It will be converted into a slot implicitely.
    * The returned iterator may be stored for disconnection
    * of the slot at some later point. It stays valid until
    * the slot is removed from the list of slots. The iterator
    * can also be implicitely converted into a sigc::connection object
    * that may be used safely beyond the life time of the slot.
    * @param slot_ The slot to add to the list of slots.
    * @return An iterator pointing to the new slot in the list.
    */
    iterator connect(int priority, const slot_type& slot_)
    {
        typename slot_data_list_type::iterator it = std::upper_bound( _slotData.begin( ), _slotData.end( ), priority, slot_data_cmp( ) );
        iterator res = signal.slots( ).insert( it == _slotData.end( ) ? signal.slots( ).end( ) : ( *it ).slot_it, slot_);
        _slotData.insert( it, slot_data( priority, res ) );

        return res; 
    }
};




#endif
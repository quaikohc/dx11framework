#pragma once

#ifndef CB_HASHTABLE_H_INCLUDED
#define CB_HASHTABLE_H_INCLUDED

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
///////////  hash table implementation by Charles Bloom - not altered ///////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/***********
This license applies to all software available on cbloom.com unless otherwise stated.

Copyright (c) 1998-2012, Charles Bloom

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.

---------------------------

cbhashtable.h

Single file self-contained hash table implementation.

This header defined a class name CB_HASHTABLE in namespace CB_HASHTABLE_NAMESPACE.
If you don't #define those macros, they will default to "hashtable" and "", respectively.

cbhashtable.h was written by Charles Bloom.
Public domain.
Don't be a jerk.

---------------------------------------------------------

Your own cbhashfunction or cbhashtable_ops must be in the CB_HASHTABLE_NAMESPACE

This makes use anonymous CB_HASHTABLE_NAMESPACE awkward

---------------------------------------------------------

hashtable is always "multi"
    eg. you can add many entries with the same key/hash if you like
    eg. the "multi" is not baked into the type, but rather in how you use it

This is a basic Knuth reprobe flat hash table

similar to Rad StringHash
    but I removed the "count" thing here for simplicity

hashtable is built on cb::vector  so it uses StlAlloc

begin() & end() let you walk an iterator over all contents
    if you insert during a head/tail walk the iterator is meaningless, but won't crash
    
entry_type structs are returned to the user
    user can erase() with this handle
    user can stuff anything he wants in ->data
    User can badly fuck up the index by changing "hash" or "key" in entry_type !!
    because of this I always give back entry_type as const, but you can const_cast the data

find() is templated on any type of key, not necessarily just the type stored in the hash
    so for example you could store a hash of String but search using "char *"
    BUT if you do this you must ensure that the different types are hashed exactly the same way !
    key_equal() must support the cross-compares
    and cbhashfunction() of the two types must be identical

---------------------------------------------------------

done : could pretty easily do a prime-mod variant as well
    prime-mod is a bit slower to look up cuz of the mod
    but safer cuz it automatically fixes bad quality hashes
    and can fit the desired fill much closer because there are more primes than powers of 2
    -> a lot slower
    
using references in find() and insert() and such causes some problems
 it means you can't pass in temporaries, and there are const annoyances
 but I don't want to unnecessarily copy nontrivial objects either
 
 interesting -
    hash insert should take the parameter by value and then *swap* them in
    vector too
    avoid copying big objects

-> maybe pile all the stuff you need to define into a helper struct ?
    did this, not sure I love it
    

*************/

#include <vector>
// options :

// class name :
#ifndef CB_HASHTABLE
#define CB_HASHTABLE	hashtable
#endif

#ifndef CB_HASHTABLE_VECTOR
#define CB_HASHTABLE_VECTOR	std::vector
#endif

#ifndef CB_HASHTABLE_U32
#define CB_HASHTABLE_U32 unsigned int
#endif

/*
#ifndef CB_HASHTABLE_U64
#if defined(__GCC__)
#define CB_HASHTABLE_U64 unsigned long long
#else
#define CB_HASHTABLE_U64 unsigned __int64
#endif
#endif
*/

// assert :
#ifndef CB_ASSERT
#define CB_ASSERT(exp)
#endif

#ifndef CB_DURING_ASSERT
#define CB_DURING_ASSERT(exp)
#endif

// compiler assert :
#ifndef CB_COMPILER_ASSERT
#define CB_COMPILER_ASSERT(exp)
#endif

// min :
#ifndef CB_MIN
#define CB_MIN(a,b) ( (a) < (b) ? (a) : (b) )
#endif

// max :
#ifndef CB_MAX
#define CB_MAX(a,b) ( (a) > (b) ? (a) : (b) )
#endif

// option : use swap instead of copy when extending vector
//#define CB_HASHTABLE_USE_SWAP

#ifndef CB_HASHTABLE_DEFAULT_FILL_RATIO
#define CB_HASHTABLE_DEFAULT_FILL_RATIO	(0.707f) // sqrt(2)/2
#endif

// namespace :
//	blank namespace ensures that includes with different options don't conflict in the linker
// define CB_HASHTABLE_NO_NAMESPACE if you really want none (not recommended)
#ifndef CB_HASHTABLE_NO_NAMESPACE
#ifndef CB_HASHTABLE_NAMESPACE
#define CB_HASHTABLE_NAMESPACE		// blank
#endif
#endif

#ifdef _MSC_VER
#define CB_HASHTABLE_FORCEINLINE __forceinline
#elif defined(__GCC__)
#define CB_HASHTABLE_FORCEINLINE inline __attribute((always_inline))
#else
#define CB_HASHTABLE_FORCEINLINE
#endif

//=======================================================

#ifdef CB_HASHTABLE_NAMESPACE
namespace CB_HASHTABLE_NAMESPACE
{
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4345) // an object of POD type constructed with an initializer of the form () will be default-initialized
#pragma warning(disable : 4100) // unreferenced formal parameter
#pragma warning(disable : 4127) // conditional on constant
#pragma warning(disable : 4328) // indirection alignment of formal parameter parameter_number (alignment_value_for_parameter) is greater than the actual argument
#endif

//=======================================================================

#ifndef CB_HASHTABLE_INTLOG2_ROUND

CB_HASHTABLE_FORCEINLINE int cbintlog2round_f(const float f)
{
    return (((reinterpret_cast<const CB_HASHTABLE_U32 &>(f)) + 0x257D86) >> 23) - 127;
}

#define CB_HASHTABLE_INTLOG2_ROUND	cbintlog2round_f
#endif

#ifndef CB_HASHTABLE_INTLOG2_CEIL

CB_HASHTABLE_FORCEINLINE int cbintlog2ceil_i(const int i)
{
    const float f = (float)i;
    // note : BitScanReverse is faster
    return (((reinterpret_cast<const CB_HASHTABLE_U32 &>(f)) + 0x7FFFFF) >> 23) - 127;
}

#define CB_HASHTABLE_INTLOG2_CEIL	cbintlog2ceil_i
#endif

//=======================================================================

typedef CB_HASHTABLE_U32 cbhashtype;

/**

generic cbhashfunction<> :
implement your own


template <typename t_key>
cbhashtype cbhashfunction(const t_key & t);

**/

// Bob's non-multiplying version :
CB_HASHTABLE_FORCEINLINE CB_HASHTABLE_U32 cbHash32( CB_HASHTABLE_U32 a)
{
    a -= (a<<6);
    a ^= (a>>17);
    a -= (a<<9);
    a ^= (a<<4);
    a -= (a<<3);
    a ^= (a<<10);
    a ^= (a>>15);
    return a;
}

/*
CB_HASHTABLE_FORCEINLINE CB_HASHTABLE_U32 cbHash64(CB_HASHTABLE_U64 key)
{
    key = (~key) + (key << 18); // key = (key << 18) - key - 1;
    key = key ^ (key >> 31);
    key = key * 21; // key = (key + (key << 2)) + (key << 4);
    key = key ^ (key >> 11);
    key = key + (key << 6);
    key = key ^ (key >> 22);
    return (CB_HASHTABLE_U32) key;
}
*/

template <typename t_key>
inline cbhashtype cbhashfunction(const t_key & t)
{
    // works for any type that implicitly converts to CB_HASHTABLE_U32 :
    return cbHash32( t );
}

//*****************************************************************


//=========================================================================

/**
    {hash,key} pairs must have two special values
    
    EMPTY and DELETED
    
    note that these are not necessarilly just key values,
        though that is often the case
        
    for pointers 0 and 1 are good choices
    
    for ints, maybe 0x7FFFFFFF
    
**/

template <typename t_key>
struct cbhashtable_key_equal
{
    // to check key equality, I call key_equal
    //	by default it uses operator ==
    //	but you can override it to other things
    //	(using a helper function like this is handy when key is a basic type like char *)
    bool key_equal(const t_key & lhs,const t_key & rhs)
    {
        return lhs == rhs;
    }
    
    inline cbhashtype hash_key(const t_key & t)
    {
        return cbhashfunction<t_key>( t );
    }
};

template <typename t_key,t_key empty_val,t_key deleted_val>
struct cbhashtable_ops : public cbhashtable_key_equal<t_key>
{

    void make_empty(cbhashtype & hash,t_key & key)
    {
        key = empty_val;
    }

    bool is_empty(const cbhashtype & hash,const t_key & key)
    {
        return ( key == empty_val );
    }

    void make_deleted(cbhashtype & hash,t_key & key)
    {
        key = deleted_val;
    }

    bool is_deleted(const cbhashtype & hash,const t_key & key)
    {
        return ( key == deleted_val );
    }

};

//=======================================================================
// cbhashtableentry_hkd :
//	entry with hash, key & data stored

template <typename t_key,typename t_data,typename t_ops>
struct cbhashtableentry_hkd
{
    typedef t_key	key_type;
    typedef t_data	data_type;
    typedef t_ops	ops;
    
    t_data		m_data;
    
    // change_data lets you change data on entry_type ; to protect hash & key
    void change_data(const t_data & d) const { const_cast<t_data &>(m_data) = d; }
    t_data & mutable_data() const { return const_cast<t_data &>(m_data); }
    
    const data_type & data() const { return m_data; }
    
    //-----------------------------------------
    
    cbhashtype	m_hash;
    t_key		m_key;
    
    bool is_empty() const { return ops().is_empty( m_hash, m_key ); }
    bool is_deleted() const { return ops().is_deleted( m_hash, m_key ); }
    
    void make_empty() { ops().make_empty( m_hash, m_key ); }
    void make_deleted() { ops().make_deleted( m_hash, m_key ); }
        
    const key_type  & key()  const { return m_key; }
    cbhashtype hash() const { return m_hash; }
    
    bool check_hash( const cbhashtype h ) const { return m_hash == h; }
    
    // entry_type constructs to an is_empty() state
    cbhashtableentry_hkd() : m_data() , m_hash() , m_key() { make_empty(); }
    
    void set( cbhashtype h, const key_type & k, const data_type & d )
    {
        m_hash = h;
        m_key = k;
        m_data = d;
    }
};

//-----------------------------------------------------------------
// cbhashtableentry_hd :
//	entry with hash & data stored
//	key is the same as data

template <typename t_data,typename t_ops>
struct cbhashtableentry_hd
{
    typedef t_data	key_type;
    typedef t_data	data_type;
    typedef t_ops	ops;
    
    t_data		m_data;
    
    // change_data lets you change data on entry_type ; to protect hash & key
    void change_data(const t_data & d) const { const_cast<t_data &>(m_data) = d; }
    t_data & mutable_data() const { return const_cast<t_data &>(m_data); }
    
    const data_type & data() const { return m_data; }
    
    //-----------------------------------------
    
    cbhashtype	m_hash;
    
    bool is_empty() const { return ops().is_empty( m_hash, m_data ); }
    bool is_deleted() const { return ops().is_deleted( m_hash, m_data ); }
    
    void make_empty() { ops().make_empty( m_hash, m_data ); }
    void make_deleted() { ops().make_deleted( m_hash, m_data ); }
        
    const key_type  & key()  const { return m_data; }
    cbhashtype hash() const { return m_hash; }
    
    bool check_hash( const cbhashtype h ) const { return m_hash == h; }
    
    // entry_type constructs to an is_empty() state
    cbhashtableentry_hd() : m_data() , m_hash() { make_empty(); }
    
    void set( cbhashtype h, const key_type & k, const data_type & d )
    {
        m_hash = h;
        CB_ASSERT( k == d );
        m_data = d;
    }
};

//-----------------------------------------------------------------
// cbhashtableentry_d :
//	entry with data stored
//	key is data
//	hash is computed
//	make_empty/make_deleted must work only on key

template <typename t_data,typename t_ops>
struct cbhashtableentry_d
{
    typedef t_data	key_type;
    typedef t_data	data_type;
    typedef t_ops	ops;
    
    t_data		m_data;
    
    // change_data lets you change data on entry_type ; to protect hash & key
    void change_data(const t_data & d) const { const_cast<t_data &>(m_data) = d; }
    t_data & mutable_data() const { return const_cast<t_data &>(m_data); }
    
    const data_type & data() const { return m_data; }
    
    //-----------------------------------------
    
    bool is_empty() const   { return ops().is_empty( 0, m_data ); }
    bool is_deleted() const { return ops().is_deleted( 0, m_data ); }
    
    void make_empty()   
    {
        cbhashtype h; 
        ops().make_empty( h, m_data );
        CB_ASSERT( is_empty() ); 
        CB_ASSERT( ! is_deleted() );
    }
    
    void make_deleted()
    {
        cbhashtype h;
        ops().make_deleted( h, m_data );
        CB_ASSERT( is_deleted() );
        CB_ASSERT( ! is_empty() );
    }
        
    const key_type  & key()  const { return m_data; }
    cbhashtype hash() const { return ops().hash_key(m_data); }
    
    bool check_hash( const cbhashtype h ) const { return true; }
    
    // entry_type constructs to an is_empty() state
    cbhashtableentry_d() : m_data() { make_empty(); }
    
    void set( cbhashtype h, const key_type & k, const data_type & d )
    {
        CB_ASSERT( k == d );
        m_data = d;
        CB_ASSERT( h == hash() );
    }
};

//=======================================================================

// could do a default template arg like this :
// = hashtable2_ops<t_key,t_key(0),t_key(1)>

template <typename t_entry> class CB_HASHTABLE
{
public:
    
    typedef t_entry								entry_type;
    typedef t_entry	*							entry_ptr;
    typedef t_entry	const *						entry_ptrc;
    typedef CB_HASHTABLE<t_entry>					this_type;
    
    typedef typename t_entry::ops		ops;
    typedef typename t_entry::key_type			key_type;
    typedef typename t_entry::data_type			data_type;
        
    //-------------------------------------------

    // constructor does no allocations, makes an empty table
    CB_HASHTABLE() : m_hashBits(0), m_hashMask(0), m_numInserts(0), m_numDeleted(0), m_numInsertsRebuild(0), m_hashFillRatio(CB_HASHTABLE_DEFAULT_FILL_RATIO)
     { }
    ~CB_HASHTABLE() 
     { }

    //-------------------------------------------
    
    // "capacity" is the size of the table
    int capacity() const;
    // "reserve" resizes the table for an occupancy of "count" (allocated table size will be larger)
    void reserve(int count);
    // "reserve_initial_size" can only be used for the first reserve ;
    //	 it takes an explicit table size to allocate, not a number of items to reserve space for
    void reserve_initial_size(int table_size);
    
    // clear : make the table empty, but don't resize
    void clear();
    // release : free the table; restores to just-constructed state
    void release();
    // tighten : resize to minimum size (good after many erases)
    void tighten();
    
    // size : # of occupants (not empty or deleted)
    int size() const;
    // bool empty = no valid occupants
    bool empty() const;

    // set_fill_ratio does not cause a resize, but you can call tighten() after if you want
    void  set_fill_ratio(float f);
    float get_fill_ratio() const;
    
    //-------------------------------------------
    // entry_type pointers returned are TEMP
    //	they are invalidated by any further action on the table, DO NOT STORE
    
    // find returns NULL if not found
    template <typename t_findkey>
    const entry_type * find(const cbhashtype & hash,const t_findkey & key) const;
    
    // insert returns the one just made
    const entry_type * insert(const cbhashtype & hash,const key_type & key,const data_type & data);

    // insert if not found :
    //	data is used only for insert
    const entry_type * find_or_insert(const cbhashtype & hash,const key_type & key,const data_type & data,bool * pWasFound = NULL);

    // sedata_type ; add_or_replace ; insert_unique
    const entry_type * insert_or_replace(const cbhashtype & hash,const key_type & key,const data_type & data);
    
    void erase(const entry_type * entry);

    // aliases :
    
    template <typename t_findkey>
    const entry_type * find(const t_findkey & key) const;
    
    const entry_type * insert(const key_type & key,const data_type & data);
    const entry_type * insert(const entry_type & entry);
        
    const entry_type * insert_or_replace(const key_type & key,const data_type & data);
    
    // mainly for use when you have a no-data entry type
    const entry_type * insert(const key_type & key);
    const entry_type * insert_or_replace(const key_type & key);
    
    //-------------------------------------------
    // funny "end"
    //  this is an easy way to make an end() that compares to either a find_iterator or walk_iterator

    //#define INT_MAX 2147483647 
    typedef enum { eHash_End = 2147483647 } t_end;
    
    t_end end() const { return eHash_End; }
    
    //-------------------------------------------
    // multi finder :
    //  get a find_iterator from find_first
    //		then use ++ on it to step
    
    class find_iterator
    {
    public:
        find_iterator() : owner(NULL), index(0), probeCount(0), hash(), key() { }
        find_iterator(const this_type * _owner,int _index,int _probeCount, cbhashtype _hash, key_type _key) : 
            owner(_owner), index(_index), probeCount(_probeCount), hash(_hash), key(_key) { }
        ~find_iterator() { }	
    
        const entry_type * operator -> () const { CB_ASSERT( owner != NULL); return   owner->iterator_at(index); }
        const entry_type & operator *  () const { CB_ASSERT( owner != NULL); return * owner->iterator_at(index); }
    
        // a few possibilities for how to expose the access
        //operator bool () const { return owner != NULL; }
        //const entry_type * ptr() const { return owner->iterator_at(index); }
        
        operator const entry_type * () const { return owner ? owner->iterator_at(index) : NULL; }
        
        // pre-increment :
        find_iterator & operator ++ () 
        {
            CB_ASSERT( owner != NULL && *this != owner->end() ); 
            owner->find_next(this); 
            return *this; 
        }
        
        bool operator == (const t_end & e) const { return owner == NULL; }
        bool operator != (const t_end & e) const { return owner != NULL; }
        
    private:
    
        void set_null() { owner = NULL; }
        void update(int _index,int _probeCount) { index = _index; probeCount = _probeCount; }
        
        friend class CB_HASHTABLE<t_entry>;
        const this_type * owner;
        int	index;
        int probeCount;
        
        // find_iterator could store the hash & key as well since you must find_next with the same vals
        //	but better not to in case key is expensive to copy (?)
        cbhashtype hash;
        key_type	key;
    };	
    
    // call find_first , the check find_iterator != NULL and keep calling find_next
    const find_iterator find_first(const cbhashtype & hash,const key_type & key) const;
    //const find_iterator find_next(const find_iterator & finder,const cbhashtype & hash,const key_type & key) const;
    bool find_next(find_iterator * finder) const;
    
    //-------------------------------------------
    // iterator for walking over all members in the hash :
    //	skips empty & deleted entries
        
    class walk_iterator
    {
    public:
        walk_iterator() : owner(NULL), index(0) { }
        walk_iterator(const this_type * _owner,int _index) : owner(_owner), index(_index) { }
        ~walk_iterator() { }
        // default copy-construct and such is good
        
        // pre-increment :
        walk_iterator & operator ++ ()
        {
            CB_ASSERT( owner != NULL && *this != owner->end() ); 
            index = owner->iterator_next(index);
            return *this;
        }
        /*
        // post-increment :
        walk_iterator & operator ++ (int)
        {
            walk_iterator save(*this);
            index = owner->iterator_next(index);
            return save;
        }
        */
    
        const entry_type * operator -> () const { CB_ASSERT( owner != NULL); return owner->iterator_at(index); }
        const entry_type & operator * () const  { CB_ASSERT( owner != NULL); return * owner->iterator_at(index); }
        
        bool operator == (const walk_iterator & rhs) const { return owner == rhs.owner && index == rhs.index; }
        bool operator != (const walk_iterator & rhs) const { return ! (*this == rhs); }
    
        bool operator == (const t_end & e) const { return index == eHash_End; }
        bool operator != (const t_end & e) const { return index != eHash_End; }
        
    private:
        friend class CB_HASHTABLE<t_entry>;
        const this_type *	owner;
        int			index;
    };
    
    // head/tail like begin/end :
    //	for walking over all members
    walk_iterator begin() const;
    //walk_iterator end() const;
    
    const entry_type * iterator_at(const int it) const;
    int iterator_next(const int it) const;
    
    //-------------------------------------------
    
private:

    void rebuild_table(int newSize);

    int table_index(const cbhashtype hash) const;
    int table_reindex(const int index,const cbhashtype hash,const int probeCount) const;

    CB_HASHTABLE_VECTOR<entry_type>	m_table;
    CB_HASHTABLE_U32		m_hashBits;
    CB_HASHTABLE_U32		m_hashMask; // (1<<hashBits)-1
    CB_HASHTABLE_U32		m_numInserts;		// occupancy is the number of *inserts* (not decremented when you remove members)
    CB_HASHTABLE_U32		m_numDeleted;		// actual number of items = occupancy - numDeleted
    CB_HASHTABLE_U32		m_numInsertsRebuild; //,occupancyDownSize;
    
    float		m_hashFillRatio;
};

//=========================================================================================

#define T_PRE1	template <typename t_entry>
#define T_PRE1_FINDKEY	template <typename t_entry> template <typename t_findkey>
#define T_PRE2	CB_HASHTABLE<t_entry>
#define T_TABLEENTRY	t_entry
#define T_KEY		typename CB_HASHTABLE<t_entry>::key_type
#define T_DATA		typename CB_HASHTABLE<t_entry>::data_type
#define T_ITERATOR	typename CB_HASHTABLE<t_entry>::walk_iterator
#define T_FINDER	typename CB_HASHTABLE<t_entry>::find_iterator

T_PRE1 int T_PRE2::size() const
{
    return m_numInserts - m_numDeleted;
}

T_PRE1 int T_PRE2::capacity() const
{
    return (int) m_table.size();
}

T_PRE1 bool T_PRE2::empty() const
{
    return size() == 0;
}

T_PRE1 void T_PRE2::reserve(int count)
{
    if ( count <= capacity() )
        return;
    
    rebuild_table(count);
}

T_PRE1 void T_PRE2::clear()
{
    // clear out table but don't free :
    m_numInserts = 0;
    m_numDeleted = 0;
    for(int i=0;i<m_table.size();i++)
    {
        ops().make_empty( m_table[i].hash, m_table[i].key );
        m_table[i].data = data_type();
    }
}	

T_PRE1 void T_PRE2::release()
{
    m_table.release();
    m_hashBits = 0;
    m_hashMask = 0;
    m_numInserts = 0;
    m_numDeleted = 0;
    m_numInsertsRebuild = 0;
}

T_PRE1 void T_PRE2::tighten()
{
    if ( m_numDeleted == 0 )
        return;
    rebuild_table(0);
}

T_PRE1 void T_PRE2::set_fill_ratio(float f)
{
    m_hashFillRatio = f;
    m_numInsertsRebuild = ftoi( (m_hashMask+1) * m_hashFillRatio );
}

T_PRE1 float T_PRE2::get_fill_ratio() const 
{
    return m_hashFillRatio; 
}
    
T_PRE1 int T_PRE2::table_reindex(const int index,const cbhashtype hash,const int probeCount) const
{
    // triangular step :
    return (int) ( ( index + probeCount ) & (m_hashMask) );
}

T_PRE1 int T_PRE2::table_index(const cbhashtype hash) const
{
    // xor fold :
    return (int) ( ( (hash>>16) ^ hash ) & (m_hashMask) );
}
    
T_PRE1 void T_PRE2::rebuild_table(int newOccupancy)
{
    if ( m_table.empty() )
    {
        // first time :
        const int c_minHashCount = 16; // this will turn into 64 after the rounding up below ...
        newOccupancy = CB_MAX(newOccupancy, c_minHashCount );

        // newSize = ( newOccupancy / m_hashFillRatio ) would put us right at the desired fill
        //	so bias up a little bit (assume more inserts will follow)
        
        float newSize = 1.5f * newOccupancy / m_hashFillRatio;
        m_hashBits = CB_HASHTABLE_INTLOG2_ROUND(newSize);
                
        int hashSize = 1<<(m_hashBits);

        // because we did intlog2round we may have dropped the size below acceptable
        if ( (hashSize * m_hashFillRatio) < newOccupancy * 1.1f )
        {
            m_hashBits ++;
            hashSize *= 2;
        }

        m_hashMask = hashSize-1;
        
        entry_type zero; // = { 0 };
        m_table.resize(hashSize,zero);
        
        m_numInserts = 0;
        m_numDeleted = 0;
        m_numInsertsRebuild = (int)( hashSize * m_hashFillRatio );
        
        // make sure that just reinserting the existing items won't cause a rebuild :
        CB_ASSERT( m_numInsertsRebuild > (CB_HASHTABLE_U32)newOccupancy );
    }
    else
    {
        // rebuild :
        
        CB_HASHTABLE_VECTOR<entry_type>	old_table;
        m_table.swap(old_table);
        CB_ASSERT( m_table.empty() );
        
        int occupancy = m_numInserts - m_numDeleted + 16;
        newOccupancy = CB_MAX(occupancy,newOccupancy);
        // call self : should get the other branch
        rebuild_table(newOccupancy);
        
        // not true if I'm rebuilding because of deletes
        //rrassert( sh->hashMask > newSize );
        //rrassert( sh->occupancyUpSize > newSize );
        
        for(int i=0;i<old_table.size();i++)
        {
            if ( old_table[i].is_empty() ||
                old_table[i].is_deleted() )
            {
                continue;
            }

            insert(old_table[i]);
        }
        
        // old_table descructs here
    }
}

T_PRE1 void T_PRE2::reserve_initial_size(int size )
{
    CB_ASSERT( m_table.empty() );

    // first time :
    const int c_minHashCount = 16; // this will turn into 64 after the rounding up below ...
    size = CB_MAX(size, c_minHashCount );

    m_hashBits = CB_HASHTABLE_INTLOG2_CEIL(size);
                
    int hashSize = 1<<(m_hashBits);

    CB_ASSERT( hashSize >= size );

    m_hashMask = hashSize-1;
        
    entry_type zero; // = { 0 };
    m_table.resize(hashSize,zero);
        
    m_numInserts = 0;
    m_numDeleted = 0;
    m_numInsertsRebuild = ftoi( hashSize * m_hashFillRatio );		
}

T_PRE1_FINDKEY const T_TABLEENTRY * T_PRE2::find(const cbhashtype & hash,const t_findkey & key) const
{
    // don't ever try to find the magic values :
    CB_ASSERT( ! ops().is_deleted(hash,key) && ! ops().is_empty(hash,key) );
    
    if ( m_hashMask == 0 )
        return NULL;
    
    int index = table_index(hash);
    int probeCount = 1;
    
    const entry_type * table = &(*m_table.begin());
        
    while ( ! table[index].is_empty() )
    {
        if ( table[index].check_hash(hash) && 
            ! table[index].is_deleted() &&
            ops().key_equal(table[index].key(),key) )
        {
            // found it
            return &table[index];
        }
    
        index = table_reindex(index,hash,probeCount);
        probeCount++;
    }
    
    return NULL;
}

T_PRE1 const T_TABLEENTRY * T_PRE2::insert(const cbhashtype & hash,const key_type & key,const data_type & data)
{
    // don't ever try to insert the magic values :
    CB_DURING_ASSERT( entry_type test_entry; test_entry.set(hash,key,data); );
    CB_ASSERT( ! test_entry.is_deleted() && ! test_entry.is_empty() );
    
    // this triggers the first table build when m_numInserts == m_numInsertsRebuild == 0
    if ( m_numInserts >= m_numInsertsRebuild )
    {
        rebuild_table(0);
    }
    m_numInserts ++;

    int index = table_index(hash);
    int probeCount = 1;
        
    entry_type * table = &(*m_table.begin());
    
    while ( ! table[index].is_empty() &&
            ! table[index].is_deleted() )
    {
        // @@ I could check to see if this exact same object is in here already and just not add it
        //	return pointer to the previous
        // currently StringHash is a "multi" hash - you can add the same key many times with different data
        //	and get them out with Find_Start / Find_Next
    
        //s_collisions++;
        index = table_reindex(index,hash,probeCount);
        probeCount++;
    }
        
    table[index].set( hash, key, data );
    
    return &table[index];
}

T_PRE1 const T_TABLEENTRY * T_PRE2::find_or_insert(const cbhashtype & hash,const key_type & key,const data_type & data,bool * pWasFound)
{
    // don't ever try to insert the magic values :
    CB_DURING_ASSERT( t_entry test_entry; test_entry.set(hash,key,data); );
    CB_ASSERT( ! test_entry.is_deleted() && ! test_entry.is_empty() );
    
    if ( m_hashMask == 0 )
    {
        rebuild_table(0);
        if ( pWasFound ) *pWasFound = false;
        return insert(hash,key,data);
    }
    
    int index = table_index(hash);
    int probeCount = 1;
    int deleted_index = -1;
    
    entry_type * table = m_table.begin();
        
    while ( ! table[index].is_empty() )
    {
        if ( table[index].is_deleted() )
        {
            // save the first deleted index in case we insert
            if ( deleted_index == -1 )
                deleted_index = index;
        }
        else if ( table[index].check_hash(hash) &&
            ops().key_equal(table[index].key(),key) )
        {
            // found it
            if ( pWasFound ) *pWasFound = true;
            return &table[index];
        }
    
        index = table_reindex(index,hash,probeCount);
        probeCount++;
    }
    
    // not found, insert :
    
    if ( pWasFound ) *pWasFound = false;
            
    // this triggers the first table build when m_numInserts == m_numInsertsRebuild == 0
    if ( m_numInserts >= m_numInsertsRebuild )
    {
        rebuild_table(0);
        // have to rescan because table changed :
        return insert(hash,key,data);
    }
    m_numInserts ++;
    
    if ( deleted_index >= 0 )
    {
        index = deleted_index;
    }
    
    CB_ASSERT( table[index].is_empty() || table[index].is_deleted() );
    
    table[index].set( hash, key, data );
    
    return &table[index];
}

T_PRE1 const T_TABLEENTRY * T_PRE2::insert_or_replace(const cbhashtype & hash,const key_type & key,const data_type & data)
{
    const entry_type * te = find_or_insert(hash,key,data);
    CB_ASSERT( te );
    // @@ if not inserted
    te->change_data( data );
    return te;
}

T_PRE1 void T_PRE2::erase(const entry_type * entry)
{
    CB_ASSERT( entry >= m_table.begin() && entry < m_table.end() );
    entry_type * el = const_cast< entry_type *>(entry);

    el->make_deleted();
    //ops().make_deleted(el->hash,el->key);	
    //WARNING : leave el->hs.hash the same for count !!
    
    m_numDeleted++;
    
    // do NOT decrease occupancy
}

// aliases :

T_PRE1_FINDKEY const T_TABLEENTRY * T_PRE2::find(const t_findkey & key) const
{
    return find(ops().hash_key(key),key);
}

T_PRE1 const T_TABLEENTRY * T_PRE2::insert(const key_type & key,const data_type & data)
{
    return insert(ops().hash_key(key),key,data);
}

T_PRE1 const T_TABLEENTRY * T_PRE2::insert_or_replace(const key_type & key,const data_type & data)
{
    return insert_or_replace(ops().hash_key(key),key,data);
}

T_PRE1 const T_TABLEENTRY * T_PRE2::insert(const entry_type & entry)
{
    return insert(entry.hash(),entry.key(),entry.data());
}

T_PRE1 const T_TABLEENTRY * T_PRE2::insert(const key_type & key)
{
    return insert(ops().hash_key(key),key,key);
}

T_PRE1 const T_TABLEENTRY * T_PRE2::insert_or_replace(const key_type & key)
{
    return insert_or_replace(ops().hash_key(key),key,key);
}
    
T_PRE1 T_ITERATOR T_PRE2::begin() const
{
    walk_iterator it(this,-1);
    ++it;
    return it;
}

/*
// tail is *past* the last valid one
T_PRE1 T_ITERATOR T_PRE2::tail() const
{
    iterator it(this,m_table.size());
    return it;
}
*/
    
T_PRE1 const T_TABLEENTRY * T_PRE2::iterator_at(const int it) const
{
    if ( it < 0 || it >= m_table.size32() ) return NULL;
    const entry_type * E = & m_table[it];
    CB_ASSERT( ! E->is_deleted() && ! E->is_empty() );
    return E;
}

T_PRE1 int T_PRE2::iterator_next(int index) const
{
    // step past last :
    ++index;
    
    // don't go past tail
    int size = m_table.size32();
    if ( index >= size ) return eHash_End; // return tail;
    
    // look for a non-empty one :
    while ( m_table[index].is_empty() ||
            m_table[index].is_deleted() )
    {
        ++index;
        if ( index >= size ) return eHash_End; // return tail;
    }
    return index;
}

// WARNING : find_first : big code dupe with ::find - keep in sync !!
T_PRE1 const T_FINDER T_PRE2::find_first(const cbhashtype & hash,const key_type & key) const
{
    // don't ever try to find the magic values :
    CB_ASSERT( ! ops().is_deleted(hash,key) && ! ops().is_empty(hash,key) );
    
    if ( m_hashMask == 0 )
        return find_iterator(); // NULL
        
    int index = table_index(hash);
    int probeCount = 1;
    
    const entry_type * table = m_table.begin();
        
    while ( ! table[index].is_empty() )
    {
        if ( table[index].check_hash(hash) &&
            ! table[index].is_deleted() &&
            ops().key_equal(table[index].key(),key) )
        {
            // found it
            return find_iterator(this,index,probeCount,hash,key);
        }
    
        index = table_reindex(index,hash,probeCount);
        probeCount++;
    }
    
    return find_iterator(); // NULL
    //return find_iterator(NULL,0,probeCount); // NULL
}

// WARNING : find_next : big code dupe with ::find - keep in sync !!
//T_PRE1 const T_FINDER T_PRE2::find_next(const find_iterator & finder,const cbhashtype & hash,const key_type & key) const
T_PRE1 bool T_PRE2::find_next(find_iterator * finder) const
{
    CB_ASSERT( finder->owner == this );
    CB_ASSERT( m_hashMask != 0 );
    
    int index = finder->index;
    int probeCount = finder->probeCount;
    cbhashtype hash = finder->hash;
    
    index = table_reindex(index,hash,probeCount);
    probeCount++;
        
    const entry_type * table = m_table.begin();
        
    while ( ! table[index].is_empty() )
    {
        if ( table[index].check_hash(hash) &&
            ! table[index].is_deleted() &&
            ops().key_equal(table[index].key(),finder->key) )
        {
            // found it
            finder->update(index,probeCount);
            return true;
        }
    
        index = table_reindex(index,hash,probeCount);
        probeCount++;
    }
    
    finder->set_null();
    return false;
}
    
    
#undef T_PRE1
#undef T_PRE2
#undef T_TABLEENTRY
#undef T_ITERATOR
#undef T_FINDER

/*
//-------------------
// old type :

template <typename t_key,typename t_data,typename t_ops> class hash_table :
    public CB_HASHTABLE< cbhashtableentry_hkd<t_key,t_data,t_ops> >
{
public:

    hash_table() { }
    ~hash_table() { }
};
*/

//=========================================================================================


//=========================================================================

#if 0

// for example :

// hash for char_ptr :



// ** warning "char *" and "const char *" are different types



template <>
inline cbhashtype cbhashfunction<intptr_t>(const intptr_t & t)
{
    #ifdef CB_64
    return Hash64( (uint64) t );
    #else
    return cbHash32( (CB_HASHTABLE_U32) t );
    #endif
}

template <>
inline cbhashtype cbhashfunction<CB_HASHTABLE_U64>(const CB_HASHTABLE_U64 & t)
{
    return Hash64( t );
}



struct cbhashtable_ops_intptr_t : public cbhashtable_ops<intptr_t,intptr_t(0),intptr_t(1)>
{
};


#endif

typedef const char * char_ptr;
typedef const wchar * wchar_ptr;

//////////////// char
CB_HASHTABLE_FORCEINLINE CB_HASHTABLE_U32 FNVHashStr(const wchar* str)
{
    CB_HASHTABLE_U32 hash = (CB_HASHTABLE_U32)(2166136261);
    while (*str)
    {
        hash = ((CB_HASHTABLE_U32)(16777619) * hash) ^ (*str++);
    }
    return hash;
}

template <>
inline cbhashtype cbhashfunction<wchar_ptr>(const wchar_ptr & t)
{
    return FNVHashStr(t);
}

struct cbhashtable_ops_wcharptr : public cbhashtable_ops<wchar_ptr, wchar_ptr(0), wchar_ptr(1)>
{
    bool key_equal(const wchar_ptr & lhs,const wchar_ptr & rhs)
    {
        return wcscmp(lhs,rhs) == 0;
    }
};
////////////////////////////////////////////////////////////////////////////////////////////////


//////////////// char
CB_HASHTABLE_FORCEINLINE CB_HASHTABLE_U32 FNVHashStr(const char * str)
{
    CB_HASHTABLE_U32 hash = (CB_HASHTABLE_U32)(2166136261);
    while (*str)
    {
        hash = ((CB_HASHTABLE_U32)(16777619) * hash) ^ (*str++);
    }
    return hash;
}

template <>
inline cbhashtype cbhashfunction<char_ptr>(const char_ptr & t)
{
    return FNVHashStr(t);
}

struct cbhashtable_ops_charptr : public cbhashtable_ops<char_ptr,char_ptr(0),char_ptr(1)>
{
    bool key_equal(const char_ptr & lhs,const char_ptr & rhs)
    {
        return strcmp(lhs,rhs) == 0;
    }
};
////////////////////////////////////////////////////////////////////////////////////////////////

struct cbhashtable_ops_int : public cbhashtable_ops<int,int(0x80000000),int(0x80000001)>
{
};


//}{=======================================================================================

#ifdef _MSC_VER
//#pragma pack(pop)
#pragma warning(pop)
#endif


#ifdef CB_HASHTABLE_NAMESPACE
}; // namespace
#endif

//}{=======================================================================================

#endif // CB_HASHTABLE_H_INCLUDED

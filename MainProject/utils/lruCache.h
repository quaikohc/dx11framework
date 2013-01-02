#pragma once


// TODO: implement this using hashtable
//       implement hits/misses info
//       change interface to get rid of returning std::pair<>
//       add reverse iterator

template<class T_KEY, class T_VALUE>
class CLRUCache
{
public:
    template<class T_KEY>
    struct SCacheKey
    {
        SCacheKey(const T_KEY& key): key(key), prev(nullptr), next(nullptr) {}
        bool operator<(const SCacheKey& k) const { return key < k.key; }

        T_KEY key;
        mutable SCacheKey* prev;
        mutable SCacheKey* next;
    };

    template<class T_KEY>
    struct HCacheKey
    {
        HCacheKey(const SCacheKey<T_KEY>* key): key(key) {}
        bool operator<(const HCacheKey<T_KEY>& k) const {return key->key < k.key->key;}

        const SCacheKey<T_KEY>* key;
    };


    template<class T_KEY>
    struct Iterator
    {
    public:
        Iterator(): m_element(nullptr) {}
        explicit Iterator(SCacheKey<T_KEY>* element): m_element(element) {}

        bool operator==(const Iterator& it) const { return m_element == it.element; }
        bool operator!=(const Iterator& it) const { return m_element != it.element; }

        Iterator& operator++() { m_element = m_element->next; return *this; }
        Iterator& operator--() { m_element = m_element->prev; return *this; }

        Iterator operator++(int) const { SCacheKey<T_KEY>* tmp = m_element; m_element = m_element->next; return Iterator(tmp); }
        Iterator operator--(int) const { SCacheKey<T_KEY>* tmp = m_element; m_element = m_element->prev; return Iterator(tmp); }

        const T_KEY& GetKey() const { return element->key; }

    private:
        SCacheKey<T_KEY>* m_element;
    };

    CLRUCache(): m_head(nullptr), m_tail(nullptr), m_elemsCount(0) 
    {
    }

    ~CLRUCache()                
    { 
        EraseAll(); 
    }

    uint                          GetCacheSize() const { return m_elemsCount; }

    std::pair<bool, T_VALUE>      Insert(const T_KEY& key, const T_VALUE& value);
    // find and return element with repositioning
    std::pair<bool, T_VALUE>      Get(const T_KEY& key);
    // find and return element without repositioning
    std::pair<bool, T_VALUE>      Find(const T_KEY& key);
    std::pair<bool, T_VALUE>      Erase(const T_KEY& key);

    void                          EraseAll();

    T_KEY                         GetTailKey();
    T_VALUE                       GetTailValue();

    typename std::map<HCacheKey<T_KEY>, T_VALUE>::const_iterator Begin() const { return m_elems.begin(); }
    typename std::map<HCacheKey<T_KEY>, T_VALUE>::const_iterator End() const   { return m_elems.end(); }

    typename std::map<HCacheKey<T_KEY>, T_VALUE>::iterator Begin()  { return m_elems.begin(); }
    typename std::map<HCacheKey<T_KEY>, T_VALUE>::iterator End()    { return m_elems.end(); }

    Iterator<T_KEY>               BeginLRU() { return Iterator<T_KEY>(m_head); }
    Iterator<T_KEY>               EndLRU()   { return Iterator<T_KEY>(nullptr); }


private:
    void                          AddToList(const SCacheKey<T_KEY>* key);
    void                          RemFromList(const SCacheKey<T_KEY>* key);


    std::map<HCacheKey<T_KEY>, T_VALUE>     m_elems;
    uint                                    m_elemsCount;

    SCacheKey<T_KEY>*                       m_head;
    SCacheKey<T_KEY>*                       m_tail;

};

template<class T_KEY, class T_VALUE>
void CLRUCache<T_KEY, T_VALUE>::AddToList(const SCacheKey<T_KEY>* ck)
{
    if (m_head == nullptr)
    {
        ck->prev = ck->next = nullptr;
        m_head = const_cast<SCacheKey<T_KEY>*>(ck);
        m_tail = const_cast<SCacheKey<T_KEY>*>(ck);
    }
    else
    {
        ck->prev = nullptr;
        ck->next = m_head;
        m_head->prev = const_cast<SCacheKey<T_KEY>*>(ck);
        m_head       = const_cast<SCacheKey<T_KEY>*>(ck);
    }
}

template<class T_KEY, class T_VALUE>
void CLRUCache<T_KEY, T_VALUE>::RemFromList(const SCacheKey<T_KEY>* pi)
{
    if (pi->prev)
        pi->prev->next = pi->next;
    if (pi->next)
        pi->next->prev = pi->prev;

    if (m_head == pi)
        m_head = m_head->next;
    if (m_tail == pi)
        m_tail = m_tail->prev;

    pi->prev = nullptr;
    pi->next = nullptr;
}

// Returns either (true, 'value') or (false, old value) if there is already a value with a given 'key'.
template<class T_KEY, class T_VALUE>
std::pair<bool, T_VALUE> CLRUCache<T_KEY, T_VALUE>::Insert(const T_KEY& key, const T_VALUE& value)
{
    SCacheKey<T_KEY>* ck = new SCacheKey<T_KEY>(key);
    auto inserted = m_elems.insert(std::make_pair(ck, value));

    if (inserted.second)
    {
        ++m_elemsCount;
        AddToList(ck);
        return std::make_pair(true, value);
    }
    else
    {
        delete ck;
        return std::make_pair(false, inserted.first->second);
    }
}

// Returns either (true, found value) or (false, VALUE()) if there is no value with given 'key'.
template<class T_KEY, class T_VALUE>
std::pair<bool, T_VALUE> CLRUCache<T_KEY, T_VALUE>::Get(const T_KEY& key)
{
    SCacheKey<T_KEY> ck(key);
    auto it =  m_elems.find(HCacheKey<T_KEY>(&ck));

    if (it == m_elems.end())
    {
        return std::make_pair(false, T_VALUE());
    }
    else
    {
        const SCacheKey<T_KEY>* ck = it->first.key;
        RemFromList(ck);
        AddToList(ck);
        return std::make_pair(true, it->second);
    }
}

// Returns either (true, found value) or (false, VALUE()) if there is no value with given 'key'.
template<class T_KEY, class T_VALUE >
std::pair<bool, T_VALUE> CLRUCache<T_KEY, T_VALUE>::Find(const T_KEY& key)
{
    SCacheKey<T_KEY> ck(key);
    auto it =  m_elems.find(HCacheKey<T_KEY>(&ck));

    if (it == m_elems.end())
    {
        return std::make_pair(false, T_VALUE());
    }
    else
    {
        return std::make_pair(true, it->second);
    }
}

// Erases element with given 'key'.
// Returns either (true, erased value) or (false, VALUE()) if there is no value with given 'key'.
template<class T_KEY, class T_VALUE>
std::pair<bool, T_VALUE> CLRUCache<T_KEY, T_VALUE>::Erase(const T_KEY& key)
{
    SCacheKey<T_KEY> ck(key);
    auto it =  m_elems.find(HCacheKey<T_KEY>(&ck));

    if (it != m_elems.end())
    {
        std::pair<bool, T_VALUE> rslt(true, it->second);
        auto pKey = it->first.key;
        RemFromList(it->first.key);
        m_elems.erase(it);
        delete pKey;
        --m_cStoredElems;
        return rslt;
    }
    else
    {
        return std::make_pair(false, T_VALUE());
    }
}

template<class T_KEY, class T_VALUE>
void CLRUCache<T_KEY, T_VALUE>::EraseAll()
{
    m_elems.clear();

    SCacheKey<T_KEY>* curr = m_head;
    while (curr)
    {
        SCacheKey<T_KEY>* next = curr->next;
        delete curr;
        curr = next;
    }

    m_head    = nullptr;
    m_tail    = nullptr;
    m_elemsCount   = 0;
}

template<class T_KEY, class T_VALUE>
T_KEY CLRUCache<T_KEY, T_VALUE>::GetTailKey()
{
    return m_tail->key;
}

template<class T_KEY, class T_VALUE>
T_VALUE CLRUCache<T_KEY, T_VALUE>::GetTailValue()
{
    auto it = m_elems.find(m_tail);
    return it->second;
}

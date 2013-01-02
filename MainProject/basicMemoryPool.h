static const int g_defaultGrowSize = 50;

// very simple automatic (data structre only need to inherit from it) object pool, only for pointers
// goal is only to avoid allocations, memory is still fragmented
// usage:
//
//  struct STestStruct: public CPointerObjectPool<STestStruct>
//  {
//  };
//
//  STestStruct::Initialize(100);
//  ..
//  STestStruct::Destory();


template <class T> class CPointerObjectPool
{
    static std::vector<T*>& GetList()
    {
        static std::vector<T*> m_unusedObjects;
        return m_unusedObjects;
    }

public:
    CPointerObjectPool()          {}
    virtual ~CPointerObjectPool() {}
    
    static void Initialize(int objectsCount)
    {
        for (int i = 0; i < objectsCount; ++i)
        {
            T* p = ::new T;
            T::GetList().push_back(p);
        }
    }
  
    static void Destroy()
    {
        std::vector<T*>::iterator first = T::GetList().begin();
        std::vector<T*>::iterator last  = T::GetList().end();
        while (first != last)
        {
            T* p = *first; ++first;
            ::delete p;
        }
        T::GetList().erase(T::GetList().begin(), T::GetList().end());
    }

    inline void *operator new(size_t stAllocateBlock)
    {
        if (T::GetList().size() <= 0)
        	T::Initialize(g_defaultGrowSize);

        T* p = T::GetList().back();
        T::GetList().pop_back();
        
        return p;
    }
    
    inline void operator delete(void *p)
    {
        T::GetList().push_back((T*)p);
    }
};
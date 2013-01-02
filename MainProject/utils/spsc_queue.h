#ifndef SPSC_QUEUE_H
#define SPSC_QUEUE_H


// very basic single producer/single consumer, bounded FIFO queue using MemoryBarrier() by Maciej Sinilo
template<typename T, size_t TCapacity>
class SPSCQueue
{
    typedef char ERR_CapacityNotPowerOfTwo[((TCapacity & (TCapacity - 1)) == 0 ? 1 : -1)];    // TCapacity must be power-of-two

public:
    SPSCQueue() : m_pushIndex(0), m_popIndex(0)
    {
        m_buffer = new T[TCapacity];
        pushesCount = 0;
        popsCount = 0;
    }
    ~SPSCQueue()
    {
        delete[] m_buffer;
    }

    uint Size() const
    {
        const uint pushed = m_pushIndex;
        const uint popped = m_popIndex;
        return pushed - popped;
    }
    bool IsFull() const
    {
        const uint s = Size();
        // It's OK on overflow -> it's not full in such case (more pushed than popped if it overflows).
        return s >= TCapacity;
    }

    void Push(const T& t)
    {
        const uint pushIndex = m_pushIndex;
        const uint index = pushIndex & (TCapacity - 1);

        pushesCount++;

        m_buffer[index] = t;
        MemoryBarrier();

        // Publish data in buffer before increasing index.
        Write32(&m_pushIndex, pushIndex + 1);
    }

    void Pop(T& t)
    {
        const uint popIndex = m_popIndex;
        const uint index = popIndex & (TCapacity - 1);
        popsCount++;
        t = m_buffer[index];
        MemoryBarrier();
        // Make sure data is full copied from buffer before publishing pop.
        Write32(&m_popIndex, popIndex + 1);
    }


private:
    SPSCQueue(const SPSCQueue&);
    SPSCQueue& operator=(const SPSCQueue&);

    static void Write32(volatile uint* ptr, uint v)
    {
        *ptr = v;
    }

    uint            m_pushIndex;
    uint            m_popIndex;

    T*              m_buffer;

    uint            pushesCount;
    uint            popsCount;
};



#endif
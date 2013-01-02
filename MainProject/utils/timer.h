#pragma once



class CTimer
{
private:
    LARGE_INTEGER       m_frequency;
    LARGE_INTEGER       m_currentTime;
    LARGE_INTEGER       m_startTime;
    LARGE_INTEGER       m_lastTime;
    float               m_total;
    float               m_delta;

public:
    CTimer()
    {
        if (!QueryPerformanceFrequency(&m_frequency))
            assert(false);
        Reset();
    }

    void Reset()
    {
        Update();

        m_startTime = m_currentTime;
        m_total     = 0.0f;
        m_delta     = 1.0f / 60.0f;
    }

    void Update()
    {
        if (!QueryPerformanceCounter(&m_currentTime))
            assert(false);

        m_total = static_cast<float>(static_cast<double>(m_currentTime.QuadPart-m_startTime.QuadPart) / static_cast<double>(m_frequency.QuadPart));

        if (m_lastTime.QuadPart == m_startTime.QuadPart)  // If the timer was just reset
            m_delta = 1.0f / 60.0f;
        else
            m_delta = static_cast<float>(static_cast<double>(m_currentTime.QuadPart-m_lastTime.QuadPart) / static_cast<double>(m_frequency.QuadPart));

        m_lastTime = m_currentTime;
    }

    float GetTotal() { return m_total; }
    float GetDelta() { return m_delta; }
};

//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "BasicTimer.h"

BasicTimer::BasicTimer()
{
    if (!QueryPerformanceFrequency(&m_frequency))
    {
        throw ref new Platform::FailureException();
    }
    Reset();
}

void BasicTimer::Reset()
{
    Update();
    m_startTime = m_currentTime;
    m_total = 0.0f;
    m_delta = 1.0f / 60.0f;
}

void BasicTimer::Update()
{
    if (!QueryPerformanceCounter(&m_currentTime))
    {
        throw ref new Platform::FailureException();
    }

    m_total = static_cast<float>(
        static_cast<double>(m_currentTime.QuadPart-m_startTime.QuadPart) /
        static_cast<double>(m_frequency.QuadPart)
        );

    if (m_lastTime.QuadPart == m_startTime.QuadPart)
    {
        // If the timer was just reset, report a time delta equivalent to 60Hz frame time.
        m_delta = 1.0f / 60.0f;
    }
    else
    {
        m_delta = static_cast<float>(
            static_cast<double>(m_currentTime.QuadPart-m_lastTime.QuadPart) /
            static_cast<double>(m_frequency.QuadPart)
            );
    }

    m_lastTime = m_currentTime;
}

float BasicTimer::Total::get()
{
    return m_total;
}

float BasicTimer::Delta::get()
{
    return m_delta;
}

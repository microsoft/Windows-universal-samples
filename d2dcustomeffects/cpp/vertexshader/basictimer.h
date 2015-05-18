//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// A simple timer class used to drive the render loop of the actively-animated
// DirectX SDK samples. Uses the QueryPerformanceCounter function to retrieve
// high-resolution timing information.
ref class BasicTimer
{
private:
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_currentTime;
    LARGE_INTEGER m_startTime;
    LARGE_INTEGER m_lastTime;
    float m_total;
    float m_delta;

internal:
    BasicTimer();
    void Reset();
    void Update();
    property float Total
    {
        float get();
    }
    property float Delta
    {
        float get();
    }
};

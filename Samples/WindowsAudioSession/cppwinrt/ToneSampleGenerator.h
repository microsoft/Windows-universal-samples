//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <limits.h>

namespace winrt::SDKTemplate
{
    class ToneSampleGenerator
    {
    public:
        bool IsEOF() noexcept { return m_sampleQueue.empty(); };
        uint32_t GetBufferLength() noexcept { return m_sampleQueue.empty() ? 0 : m_sampleQueue.front().BufferSize; };
        void Flush() { m_sampleQueue.clear(); }

        void GenerateSampleBuffer(double Frequency, UINT32 FramesPerPeriod, WAVEFORMATEX* wfx);
        void FillSampleBuffer(array_view<uint8_t> buffer);

    private:
        struct SineSampleInfo
        {
            double Frequency;
            WORD ChannelCount;
            DWORD SamplesPerSecond;
            double Amplitude;
            double Theta = 0.0;
            size_t SampleSize;
            void (*StoreSample)(uint8_t* buffer, double value);

            void GenerateSamples(array_view<uint8_t> buffer) noexcept;
        };

    private:
        std::forward_list<RenderBuffer> m_sampleQueue;
    };
}

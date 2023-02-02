//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "ToneSampleGenerator.h"

namespace winrt::SDKTemplate
{
    static constexpr int TONE_DURATION_SEC = 30;
    static constexpr double TONE_AMPLITUDE = 0.5;     // Scalar value, should be between 0.0 - 1.0

    //
    //  GenerateSampleBuffer()
    //
    //  Create a linked list of sample buffers
    //
    void ToneSampleGenerator::GenerateSampleBuffer(double Frequency, UINT32 FramesPerPeriod, WAVEFORMATEX* wfx)
    {
        uint32_t renderBufferSizeInBytes = FramesPerPeriod * wfx->nBlockAlign;
        uint64_t renderDataLength = (wfx->nSamplesPerSec * TONE_DURATION_SEC * wfx->nBlockAlign) + (renderBufferSizeInBytes - 1);
        uint64_t renderBufferCount = renderDataLength / renderBufferSizeInBytes;

        SineSampleInfo info;
        info.Frequency = Frequency;
        info.ChannelCount = wfx->nChannels;
        info.SamplesPerSecond = wfx->nSamplesPerSec;
        info.Amplitude = TONE_AMPLITUDE;
        info.Theta = 0; // Updated by GenerateSamples to remember where to resume

        switch (GetRenderSampleType(wfx))
        {
        case RenderSampleType::Pcm16Bit:
            info.SampleSize = sizeof(int16_t);
            info.StoreSample = [](uint8_t* buffer, double value) { *reinterpret_cast<int16_t*>(buffer) = static_cast<int16_t>(value * (std::numeric_limits<int16_t>::max)()); };
            break;

        case RenderSampleType::Pcm24In32Bit:
            info.SampleSize = sizeof(int32_t);
            // Scale to a 24-bit integer, stored in the upper 24 bits of the 32-bit sample.
            info.StoreSample = [](uint8_t* buffer, double value) { *reinterpret_cast<int32_t*>(buffer) = static_cast<int32_t>(value * ((1 << 23) - 1)) << 8; };
            break;

        case RenderSampleType::Float:
            info.SampleSize = sizeof(float);
            info.StoreSample = [](uint8_t* buffer, double value) { *reinterpret_cast<float*>(buffer) = static_cast<float>(value); };
            break;

        default:
            throw hresult_error(E_UNEXPECTED);
        }

        std::forward_list<RenderBuffer>::iterator sampleQueueTail = m_sampleQueue.before_begin();
        for (uint64_t i = 0; i < renderBufferCount; i++)
        {
            sampleQueueTail = m_sampleQueue.emplace_after(sampleQueueTail, renderBufferSizeInBytes);
            RenderBuffer& sampleBuffer = *sampleQueueTail;
            info.GenerateSamples({ sampleBuffer.Buffer.get(), sampleBuffer.BufferSize });
        }
    }

    //
    // GenerateSamples()
    //
    //  Generate samples which represent a sine wave that fits into the specified buffer.
    //  The wave begins at the Theta value stored in the SineSampleInfo, and on exit the
    //  value is updated to the first Theta value for the next buffer.
    //
    //  Buffer - Buffer to hold the samples
    //  BufferLength - Length of the buffer.

    void ToneSampleGenerator::SineSampleInfo::GenerateSamples(array_view<uint8_t> buffer) noexcept
    {
        // Each sample represents (Frequency * 2 * M_PI / SamplesPerSecond) radians.
        // To avoid accumulated rounding errors, we track the cumulative Frequency.
        // To avoid loss of significance, we take the value mod SamplesPerSecond.
        size_t frames = buffer.size() / SampleSize / ChannelCount;
        uint8_t* next = buffer.data();
        for (size_t frame = 0; frame < frames; frame++)
        {
            double sinValue = Amplitude * sin(Theta * M_PI * 2 / SamplesPerSecond);
            for (size_t channel = 0; channel < ChannelCount; channel++)
            {
                StoreSample(next, sinValue);
                next += SampleSize;
            }
            Theta += Frequency;
            if (Theta > SamplesPerSecond)
            {
                Theta = Theta - SamplesPerSecond;
            }
        }
    }

    //
    //  FillSampleBuffer()
    //
    //  File the buffer of size bytesToRead with the first item in the queue.  Caller is responsible for allocating and freeing the buffer.
    //
    void ToneSampleGenerator::FillSampleBuffer(array_view<uint8_t> buffer)
    {
        RenderBuffer& sampleBuffer = m_sampleQueue.front();

        if (buffer.size() > sampleBuffer.BufferSize)
        {
            throw hresult_invalid_argument();
        }

        CopyMemory(buffer.data(), sampleBuffer.Buffer.get(), buffer.size());

        m_sampleQueue.pop_front();
    }
}

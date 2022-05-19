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

namespace winrt::SDKTemplate
{
    // RAII helper class to ensure something happens even in case of exception.
    template<typename Lambda>
    struct scope_exit
    {
        scope_exit(Lambda&& lambda) : m_lambda(std::move(lambda)) {}
        ~scope_exit() { m_lambda(); }

        scope_exit(const scope_exit&) = delete;
        void operator=(const scope_exit&) = delete;

        Lambda m_lambda;
    };

    // RAII class to initialize Media Foundation.
    struct MediaFoundationInitializer
    {
        MediaFoundationInitializer()
        {
            // Initialize MF
            check_hresult(MFStartup(MF_VERSION, MFSTARTUP_LITE));
        }

        ~MediaFoundationInitializer()
        {
            MFShutdown();
        }
    };

    // RAII class to lock/unlock a shared work queue.
    struct unique_shared_work_queue
    {
        unique_shared_work_queue(PCWSTR className)
        {
            DWORD taskId = 0; // 0 means "create a new task group"
            check_hresult(MFLockSharedWorkQueue(className, 0, &taskId, &m_queueId));
        }

        ~unique_shared_work_queue()
        {
            MFUnlockWorkQueue(m_queueId);
        }

        unique_shared_work_queue(unique_shared_work_queue const&) = delete;
        void operator=(unique_shared_work_queue const&) = delete;

        DWORD get() { return m_queueId; }
    private:
        DWORD m_queueId = 0;
    };

    // RAII class analogous to std::unique_ptr, but calls CoTaskMemFree instead of delete.
    template<typename T>
    struct unique_cotaskmem_ptr
    {
        unique_cotaskmem_ptr(T* p = nullptr) : m_p(p) {}
        ~unique_cotaskmem_ptr() { CoTaskMemFree(m_p); }

        unique_cotaskmem_ptr(unique_cotaskmem_ptr const&) = delete;
        unique_cotaskmem_ptr(unique_cotaskmem_ptr&& other) : m_p(std::exchange(other.m_p, nullptr)) {}
        unique_cotaskmem_ptr& operator=(unique_cotaskmem_ptr const& other)
        {
            CoTaskMemFree(std::exchange(m_p, std::exchange(other.m_p, nullptr)));
            return *this;
        }

        T* operator->() { return m_p; }
        T* get() { return m_p; }
        T** put() { return &m_p; }
        T* m_p;
    };

    // Helper class for allowing a class to implement multiple versions of
    // IMFAsyncCallback.
    template<auto Callback>
    struct EmbeddedMFAsyncCallback : ::IMFAsyncCallback
    {
        template<typename Parent> static Parent* parent_finder(HRESULT(Parent::*)(IMFAsyncResult*)) { return nullptr; }
        using ParentPtr = decltype(parent_finder(Callback));

        ParentPtr m_parent;
        DWORD m_queueId = 0;

        EmbeddedMFAsyncCallback(ParentPtr parent) : m_parent(parent) {}

        STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) final
        {
            if (is_guid_of<::IMFAsyncCallback, ::IUnknown>(riid))
            {
                (*ppvObject) = this;
                AddRef();
                return S_OK;
            }
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        STDMETHOD_(ULONG, AddRef)() final { return m_parent->AddRef(); }
        STDMETHOD_(ULONG, Release)() final { return m_parent->Release(); }

        STDMETHOD(GetParameters)(DWORD* flags, DWORD* queueId) final
        {
            *flags = 0;
            *queueId = m_queueId;
            return S_OK;
        }

        STDMETHOD(Invoke)(IMFAsyncResult* result) final
        {
            return (m_parent->*Callback)(result);
        }

        void SetQueueID(DWORD queueId) { m_queueId = queueId; }
    };

    struct RenderBuffer
    {
        uint32_t BufferSize;
        uint32_t BytesFilled;
        std::unique_ptr<BYTE[]> Buffer;

        RenderBuffer(uint32_t size) :
            BufferSize(size),
            BytesFilled(size),
            Buffer(new BYTE[size])
        {
        }
    };

    enum class RenderSampleType
    {
        Unknown,
        Float,
        Pcm16Bit,
        Pcm24In32Bit,
    };

    //
    //  GetRenderSampleType()
    //
    //  Determine the sample format based on media type
    //
    inline RenderSampleType GetRenderSampleType(WAVEFORMATEX const* wfx)
    {
        WAVEFORMATEXTENSIBLE const* wfext = reinterpret_cast<WAVEFORMATEXTENSIBLE const*>(wfx);

        if ((wfx->wFormatTag == WAVE_FORMAT_PCM) ||
            ((wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) &&
                (wfext->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)))
        {
            if (wfx->wBitsPerSample == 16)
            {
                return RenderSampleType::Pcm16Bit;
            }
            else if (wfx->wBitsPerSample == 32)
            {
                if ((wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) &&
                    (wfext->Samples.wValidBitsPerSample == 24))
                {
                    return RenderSampleType::Pcm24In32Bit;
                }
            }
        }
        else if ((wfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) ||
            ((wfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) &&
                (wfext->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)))
        {
            return RenderSampleType::Float;
        }

        return RenderSampleType::Unknown;
    }
}

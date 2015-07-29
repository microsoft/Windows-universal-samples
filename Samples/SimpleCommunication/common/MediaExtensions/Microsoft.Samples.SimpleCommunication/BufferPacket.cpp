//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "BufferPacket.h"

using namespace Microsoft::Samples::SimpleCommunication::Network;

HRESULT Microsoft::Samples::SimpleCommunication::Network::CreateBufferPacketFromMFSample(_In_ IMFSample *pSample, _Outptr_ IBufferPacket **ppBufferPacket)
{
    return CBufferPacket::FromMFSample(pSample, ppBufferPacket);
}

HRESULT Microsoft::Samples::SimpleCommunication::Network::CreateBufferPacket(_Outptr_ IBufferPacket **ppBufferPacket)
{
    return CBufferPacket::CreateInstance(ppBufferPacket);
}

CBufferPacket::CBufferPacket(void)
    : _cRef(1)
{
}


CBufferPacket::~CBufferPacket(void)
{
}

// IUnknown methods

IFACEMETHODIMP CBufferPacket::QueryInterface(REFIID riid, void **ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }
    (*ppv) = nullptr;

    HRESULT hr = S_OK;
    if (riid == IID_IUnknown || riid == __uuidof(IBufferPacket))
    {
        (*ppv) = static_cast<IBufferPacket*>(this);
        AddRef();
        hr = S_OK;
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}

IFACEMETHODIMP_(ULONG) CBufferPacket::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CBufferPacket::Release()
{
    long cRef = InterlockedDecrement(&_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

HRESULT CBufferPacket::FromMFSample(IMFSample *pSample, IBufferPacket **ppBufferPacket)
{
    if (pSample == nullptr || ppBufferPacket == nullptr)
    {
        return E_INVALIDARG;
    }

    ComPtr<IBufferPacket> spPacket;
    DWORD cBuffers = 0;
    HRESULT hr = CBufferPacket::CreateInstance(&spPacket);

    if (SUCCEEDED(hr))
    {
        hr = pSample->GetBufferCount(&cBuffers);
    }

    if (SUCCEEDED(hr))
    {
        for (DWORD nIndex = 0; nIndex < cBuffers; ++nIndex)
        {
            ComPtr<IMFMediaBuffer> spMediaBuffer;
            ComPtr<IMediaBufferWrapper> spBufferWrapper;
            
            // Get buffer from the sample
            hr = pSample->GetBufferByIndex(nIndex, &spMediaBuffer);
            if (FAILED(hr))
            {
                break;
            }

            // Create buffer wrapper
            hr = CreateMediaBufferWrapper(spMediaBuffer.Get(), &spBufferWrapper);
            if (FAILED(hr))
            {
                break;
            }

            // Add buffer to the package
            hr = spPacket->AddBuffer(spBufferWrapper.Get());
            if (FAILED(hr))
            {
                break;
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        *ppBufferPacket = spPacket.Detach();
    }

    TRACEHR_RET(hr);
}

HRESULT CBufferPacket::CreateInstance(IBufferPacket **ppBufferPacket)
{
    if (ppBufferPacket == nullptr)
    {
        return E_INVALIDARG;
    }

    ComPtr<IBufferPacket> spPacket;
    HRESULT hr = S_OK;

    spPacket.Attach(new CBufferPacket());

    if (!spPacket)
    {
        hr = E_OUTOFMEMORY;
    }

    if (SUCCEEDED(hr))
    {
        *ppBufferPacket = spPacket.Detach();
    }

    TRACEHR_RET(hr);
}

// IBufferPacket methods
IFACEMETHODIMP CBufferPacket::AddBuffer(IMediaBufferWrapper *pBuffer)
{
    _buffers.push_back(pBuffer);

    return S_OK;
}

IFACEMETHODIMP CBufferPacket::InsertBuffer(unsigned int nIndex, IMediaBufferWrapper *pBuffer)
{
    if (nIndex > _buffers.size() || pBuffer == nullptr)
    {
        return E_INVALIDARG;
    }

    Iterator it = _buffers.begin();
    Iterator itEnd = _buffers.end();

    for (DWORD nCurrent = 1; nCurrent < nIndex && it != itEnd; ++nCurrent, ++it);

    _buffers.insert(it, pBuffer);

    return S_OK;
}

IFACEMETHODIMP CBufferPacket::RemoveBuffer(unsigned int nIndex, IMediaBufferWrapper **ppBuffer)
{
    if (nIndex > _buffers.size() || ppBuffer == nullptr)
    {
        return E_INVALIDARG;
    }

    Iterator it = _buffers.begin();
    Iterator itEnd = _buffers.end();

    for (DWORD nCurrent = 1; nCurrent < nIndex && it != itEnd; ++nCurrent, ++it);

    *ppBuffer = (*it).Get();
    (*ppBuffer)->AddRef();

    _buffers.erase(it);

    return S_OK;
}

IFACEMETHODIMP CBufferPacket::Clear()
{
    _buffers.clear();
    return S_OK;
}

IFACEMETHODIMP_(size_t) CBufferPacket::GetBufferCount() const
{
    return _buffers.size();
}

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
IFACEMETHODIMP CBufferPacket::FillWSABUF(WSABUF *pBuffers, size_t cBuffers)
{
    if (pBuffers == nullptr)
    {
        return E_INVALIDARG;
    }

    if (cBuffers < GetBufferCount())
    {
        return MF_E_INSUFFICIENT_BUFFER;
    }

    HRESULT hr = S_OK;
    Iterator it = _buffers.begin();
    Iterator itEnd = _buffers.end();
    size_t pos = 0;

    for (;it != itEnd; ++it, ++pos)
    {
        DWORD cbLen;
        hr = (*it)->GetCurrentLength(&cbLen);
        if (FAILED(hr))
        {
            break;
        }
        pBuffers[pos].buf = reinterpret_cast<CHAR *>((*it)->GetBuffer());
        pBuffers[pos].len = cbLen;
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CBufferPacket::SaveToFile(LPCWSTR pszFileName)
{
    HANDLE hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    HRESULT hr = S_OK;
    if (hFile == INVALID_HANDLE_VALUE)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    if (SUCCEEDED(hr))
    {
        Container::iterator it = _buffers.begin();
        Container::iterator endIt = _buffers.end();

        for(;it != endIt; ++it)
        {
            IMediaBufferWrapper *pBuffer = (*it);
            DWORD cbSize;

            hr = pBuffer->GetCurrentLength(&cbSize);
                        
            if (SUCCEEDED(hr) && !WriteFile(hFile, pBuffer->GetBuffer(), cbSize, &cbSize, nullptr))
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
        }
    }

    CloseHandle(hFile);

    return hr;
}
#endif

IFACEMETHODIMP CBufferPacket::GetTotalLength(_Out_ DWORD *pcbTotalLength)
{
    if (pcbTotalLength == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    Iterator it = _buffers.begin();
    Iterator itEnd = _buffers.end();
    DWORD cbSize = 0;

    for (;it != itEnd; ++it)
    {
        DWORD cbLen;
        hr = (*it)->GetCurrentLength(&cbLen);
        if (FAILED(hr))
        {
            break;
        }
        cbSize += cbLen;
    }

    *pcbTotalLength = cbSize;

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CBufferPacket::CopyTo(DWORD nOffset, DWORD cbSize, _In_reads_bytes_(cbSize) void *pDest, _Out_ DWORD *pcbCopied)
{
    if (pDest == nullptr || pcbCopied == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    Iterator it = _buffers.begin();
    Iterator itEnd = _buffers.end();
    DWORD cbSkipped = 0;
    DWORD cbCopied = 0;
    
    // Skip to the offset
    for (;cbSkipped < nOffset && it != itEnd; ++it)
    {
        DWORD cbLen;
        DWORD nStart = 0;
        DWORD cbCopy = 0;
        hr = (*it)->GetCurrentLength(&cbLen);
        if (FAILED(hr))
        {
            break;
        }
        if (cbSkipped + cbLen <= nOffset)
        {
            cbSkipped += cbLen;
            if (cbSkipped == nOffset)
            {
                break;
            }
            continue;
        }
        else
        {
            nStart = nOffset - cbSkipped;
            cbCopy = min(cbSize, cbLen - nStart);
        }

       CopyMemory(pDest, (*it)->GetBuffer() + nStart, cbCopy);

       cbCopied = cbCopy;
       cbSkipped += nStart;
    }

    if (SUCCEEDED(hr))
    {
        for (;it != itEnd && cbCopied < cbSize; ++it)
        {
            DWORD cbLen;
            hr = (*it)->GetCurrentLength(&cbLen);
            if (FAILED(hr))
            {
                break;
            }
            DWORD cbCopy = min(cbLen, cbSize - cbCopied);

            CopyMemory(static_cast<BYTE *>(pDest) + cbCopied, (*it)->GetBuffer(), cbCopy);

           cbCopied += cbCopy;
        }
    }

    *pcbCopied = cbCopied;

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CBufferPacket::MoveLeft(DWORD cbSize, _Out_writes_bytes_(cbSize) void *pDest)
{
    DWORD cbCopied = 0;
    HRESULT hr = CopyTo(0, cbSize, pDest, &cbCopied);

    if (SUCCEEDED(hr) && cbCopied != cbSize)
    {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        hr = TrimLeft(cbSize);
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CBufferPacket::TrimLeft(DWORD cbSize)
{
    DWORD cbTotalLength = 0;
    HRESULT hr = GetTotalLength(&cbTotalLength);

    if (SUCCEEDED(hr) && cbSize > cbTotalLength)
    {
        return E_INVALIDARG;
    }

    Iterator it = _buffers.begin();
    Iterator itEnd = _buffers.end();
    DWORD cbSkipped = 0;

    for (;cbSkipped < cbSize; it = _buffers.begin())
    {
        DWORD cbLen;
        DWORD nStart = 0;
        DWORD cbCopy = 0;
        hr = (*it)->GetCurrentLength(&cbLen);
        if (FAILED(hr))
        {
            break;
        }
        if (cbSkipped + cbLen <= cbSize)
        {
            _buffers.pop_front();
            cbSkipped += cbLen;
        }
        else
        {
            // skip the rest
            hr = (*it)->TrimLeft(cbSize-cbSkipped);
            break;
        }
    }

    TRACEHR_RET(hr);
}

// Convert buffer packet to IMFSample
IFACEMETHODIMP CBufferPacket::ToMFSample(IMFSample **ppSample)
{
    ComPtr<IMFSample> spSample;

    HRESULT hr = MFCreateSample(&spSample);

    if (SUCCEEDED(hr))
    {
        Container::iterator it = _buffers.begin();
        Container::iterator endIt = _buffers.end();

        for(;it != endIt; ++it)
        {
            IMediaBufferWrapper *pBuffer = (*it).Get();
            ComPtr<IMFMediaBuffer> spMediaBuffer;

            if (pBuffer->GetOffset() == 0)
            {
                // If offset is zero we can just use embedded media buffer
                spMediaBuffer = pBuffer->GetMediaBuffer();
            }
            else
            {
                // We have to create MF media buffer wrapper to include offset
                DWORD cbLen;
                hr = pBuffer->GetCurrentLength(&cbLen);
                if (FAILED(hr))
                {
                    break;
                }
                hr = MFCreateMediaBufferWrapper(pBuffer->GetMediaBuffer(), pBuffer->GetOffset(), cbLen, &spMediaBuffer);
                if (FAILED(hr))
                {
                    break;
                }
            }

            // Add media buffer to the sample
            hr = spSample->AddBuffer(spMediaBuffer.Get());
            if (FAILED(hr))
            {
                break;
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        *ppSample = spSample.Detach();
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CBufferPacket::GetEnumerator (_Out_ IBufferEnumerator **ppEnumerator)
{
    return CBufferEnumerator::CreateInstance(_buffers, this, ppEnumerator);
}

HRESULT CBufferEnumerator::CreateInstance(CBufferPacket::Container &container, IUnknown *pParent, IBufferEnumerator **ppEnumerator)
{
    ComPtr<IBufferEnumerator> spResult;

    if (ppEnumerator == nullptr)
    {
        return E_INVALIDARG;
    }

    spResult.Attach(new CBufferEnumerator(container, pParent));
   
    if (!spResult)
    {
        return E_OUTOFMEMORY;
    }
    else
    {
        *ppEnumerator = spResult.Detach();
    }

    return S_OK;
}

CBufferEnumerator::CBufferEnumerator(CBufferPacket::Container &container, IUnknown *pParent)
    : _cRef(1)
    , _buffers(container)
    , _spParent(pParent)
    , _currentIt(container.begin())
{
}

CBufferEnumerator::~CBufferEnumerator()
{
}

// IUnknown methods
IFACEMETHODIMP CBufferEnumerator::QueryInterface(REFIID riid, void **ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }
    (*ppv) = nullptr;

    HRESULT hr = S_OK;
    if (riid == IID_IUnknown || riid == __uuidof(IBufferEnumerator))
    {
        (*ppv) = static_cast<IBufferEnumerator*>(this);
        AddRef();
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}

IFACEMETHODIMP_(ULONG) CBufferEnumerator::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CBufferEnumerator::Release()
{
    long cRef = InterlockedDecrement(&_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

// IBufferEnumerator methods
IFACEMETHODIMP_ (bool) CBufferEnumerator::IsValid ()
{
    return _currentIt != _buffers.end();
}

IFACEMETHODIMP CBufferEnumerator::GetCurrent (_Out_ IMediaBufferWrapper **ppBuffer)
{
    if (ppBuffer == nullptr)
    {
        return E_INVALIDARG;
    }

    if (!IsValid())
    {
        return MF_E_OUT_OF_RANGE;
    }

    *ppBuffer = (*_currentIt).Get();
    (*ppBuffer)->AddRef();

    return S_OK;
}

IFACEMETHODIMP CBufferEnumerator::MoveNext ()
{
    if (!IsValid())
    {
        return MF_E_OUT_OF_RANGE;
    }

    ++_currentIt;

    if (!IsValid())
    {
        return MF_E_OUT_OF_RANGE;
    }

    return S_OK;
}

IFACEMETHODIMP CBufferEnumerator::Reset ()
{
    _currentIt = _buffers.begin();

    if (!IsValid())
    {
        return MF_E_OUT_OF_RANGE;
    }

    return S_OK;
}

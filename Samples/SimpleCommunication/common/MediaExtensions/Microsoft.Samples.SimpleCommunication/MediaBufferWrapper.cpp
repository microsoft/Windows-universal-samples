//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "MediaBufferWrapper.h"

using namespace Microsoft::Samples::SimpleCommunication::Network;

HRESULT Microsoft::Samples::SimpleCommunication::Network::CreateMediaBufferWrapper(DWORD dwMaxLength, _Outptr_ IMediaBufferWrapper **ppMediaBufferWrapper)
{
    return CMediaBufferWrapper::CreateInstance(dwMaxLength, ppMediaBufferWrapper);
}

HRESULT Microsoft::Samples::SimpleCommunication::Network::CreateMediaBufferWrapper(_In_ IMFMediaBuffer *pMediaBuffer, _Outptr_ IMediaBufferWrapper **ppMediaBufferWrapper)
{
    return CMediaBufferWrapper::CreateInstance(pMediaBuffer, ppMediaBufferWrapper);
}

CMediaBufferWrapper::CMediaBufferWrapper(void)
    : _pBuffer(nullptr)
    , _nOffset(0)
{
    TRACE(TRACE_LEVEL_LOW, L"Creating CMediaBufferWrapper %p\n", this);
}

CMediaBufferWrapper::~CMediaBufferWrapper(void)
{
    if (_spMediaBuffer)
    {
        if (_sp2DBuffer)
        {
            _sp2DBuffer->Unlock2D();
        }
        else
        {
            _spMediaBuffer->Unlock();
        }
    }
    TRACE(TRACE_LEVEL_LOW, L"Deleting CMediaBufferWrapper %p\n", this);
}

HRESULT CMediaBufferWrapper::CreateInstance(DWORD cbMaxLength, _Outptr_ IMediaBufferWrapper **ppMediaBufferWrapper)
{
    ComPtr<IMFMediaBuffer> spMediaBuffer;
    HRESULT hr = MFCreateMemoryBuffer(cbMaxLength, &spMediaBuffer);

    if (SUCCEEDED(hr))
    {
        hr = CMediaBufferWrapper::CreateInstance(spMediaBuffer.Get(), ppMediaBufferWrapper);
    }

    TRACEHR_RET(hr);
}

HRESULT CMediaBufferWrapper::CreateInstance(_In_ IMFMediaBuffer *pMediaBuffer, _Outptr_ IMediaBufferWrapper **ppMediaBufferWrapper)
{
    HRESULT hr = S_OK;
    ComPtr<CMediaBufferWrapper> spResult;

    if (pMediaBuffer == nullptr || ppMediaBufferWrapper == nullptr)
    {
        return E_INVALIDARG;
    }

    hr = MakeAndInitialize<CMediaBufferWrapper>(&spResult, pMediaBuffer);

    if (!spResult)
    {
        hr = E_OUTOFMEMORY;
    }

    if (SUCCEEDED(hr))
    {
        *ppMediaBufferWrapper = spResult.Detach();
    }

    TRACEHR_RET(hr);
}

HRESULT CMediaBufferWrapper::RuntimeClassInitialize(IMFMediaBuffer *pMediaBuffer)
{
    assert(pMediaBuffer != nullptr);

    DWORD cbMaxLength;
    DWORD cbCurrentLength;
    HRESULT hr = S_OK;
    if (SUCCEEDED(pMediaBuffer->QueryInterface(IID_PPV_ARGS(&_sp2DBuffer))))
    {
        LONG lPitch;
        hr = _sp2DBuffer->Lock2D(&_pBuffer, &lPitch);
        TRACE(TRACE_LEVEL_LOW, L"D3D CMediaBufferWrapper %p\n", this);
    }
    else
    {
        hr = pMediaBuffer->Lock(&_pBuffer, &cbMaxLength, &cbCurrentLength);
    }

    if (SUCCEEDED(hr))
    {
        _spMediaBuffer = pMediaBuffer;
    }
    else
    {
        __debugbreak();
    }

    TRACEHR_RET(hr);
}

// IBuffer methods
IFACEMETHODIMP CMediaBufferWrapper::get_Capacity(UINT *pcbCapacity)
{
    DWORD cbCapacity = 0;
    HRESULT hr = _spMediaBuffer->GetMaxLength(&cbCapacity);

    *pcbCapacity = cbCapacity;

    return hr;
}

IFACEMETHODIMP CMediaBufferWrapper::get_Length(UINT *pcbLength)
{
    DWORD cbLength = 0;
    HRESULT hr = GetCurrentLength(&cbLength);

    *pcbLength = cbLength;

    return hr;
}

IFACEMETHODIMP CMediaBufferWrapper::put_Length(UINT cbLength)
{
    return SetCurrentLength(cbLength);
}

// IBufferByteAccess methods
IFACEMETHODIMP CMediaBufferWrapper::Buffer(_Out_ BYTE **ppBuffer)
{
    *ppBuffer = GetBuffer();
    return S_OK;
}

// IMarshal methods
IFACEMETHODIMP CMediaBufferWrapper::GetUnmarshalClass(REFIID riid, _In_opt_ void *pv, DWORD dwDestContext, 
                                                      _Reserved_ void *pvDestContext, DWORD mshlflags, _Out_ CLSID *pclsid)
{
    HRESULT hr = CheckMarshal();

    if (SUCCEEDED(hr))
    {
        hr = _spBufferMarshal->GetUnmarshalClass(riid, pv, dwDestContext, pvDestContext, mshlflags, pclsid);
    }

    return hr;
}

IFACEMETHODIMP CMediaBufferWrapper::GetMarshalSizeMax(REFIID riid, _In_opt_ void *pv, DWORD dwDestContext,
                                                      _Reserved_ void *pvDestContext, DWORD mshlflags, _Out_ DWORD *pcbSize)
{
    HRESULT hr = CheckMarshal();

    if (SUCCEEDED(hr))
    {
        hr = _spBufferMarshal->GetMarshalSizeMax(riid, pv, dwDestContext, pvDestContext, mshlflags, pcbSize);
    }

    return hr;
}

IFACEMETHODIMP CMediaBufferWrapper::MarshalInterface(_In_ IStream *pStm, REFIID riid, _In_opt_ void *pv, DWORD dwDestContext,
                                                     _Reserved_ void *pvDestContext, DWORD mshlflags)
{
    HRESULT hr = CheckMarshal();

    if (SUCCEEDED(hr))
    {
        hr = _spBufferMarshal->MarshalInterface(pStm,riid, pv, dwDestContext, pvDestContext, mshlflags);
    }

    return hr;
}

IFACEMETHODIMP CMediaBufferWrapper::UnmarshalInterface(_In_ IStream *, _In_ REFIID, _Outptr_ void **)
{ 
    return E_NOTIMPL; 
}

IFACEMETHODIMP CMediaBufferWrapper::ReleaseMarshalData(_In_ IStream *)
{ 
    return E_NOTIMPL; 
}

IFACEMETHODIMP CMediaBufferWrapper::DisconnectObject(DWORD)
{ 
    return E_NOTIMPL; 
}

// IMediaBufferWrapper methods
IFACEMETHODIMP CMediaBufferWrapper::SetOffset(DWORD nOffset) 
{
    DWORD cbCurrentLength;

    HRESULT hr = _spMediaBuffer->GetCurrentLength(&cbCurrentLength);

    if (SUCCEEDED(hr))
    {
        if (cbCurrentLength < nOffset)
        {
            hr = E_INVALIDARG;
        }
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaBufferWrapper::GetCurrentLength(DWORD *pcbCurrentLength)
{
    if (pcbCurrentLength == nullptr)
    {
        return E_INVALIDARG;
    }
    DWORD cbCurrentLength;

    HRESULT hr = _spMediaBuffer->GetCurrentLength(&cbCurrentLength);

    if (SUCCEEDED(hr))
    {
        if (cbCurrentLength <= _nOffset)
        {
            cbCurrentLength = 0;
        }
        else
        {
            cbCurrentLength -= _nOffset;
        }

        *pcbCurrentLength = cbCurrentLength;
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaBufferWrapper::SetCurrentLength(DWORD cbCurrentLength)
{
    DWORD cbMaxLen = 0;
    HRESULT hr = _spMediaBuffer->GetMaxLength(&cbMaxLen);

    if (SUCCEEDED(hr))
    {
        if (_nOffset > cbMaxLen)
        {
            _nOffset = cbMaxLen;
        }

        if (cbCurrentLength > (cbMaxLen - _nOffset))
        {
            hr = E_INVALIDARG;
        }
        else
        {
            hr = _spMediaBuffer->SetCurrentLength(cbCurrentLength+_nOffset);
        }
    }
    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaBufferWrapper::TrimLeft(DWORD cbSize)
{
    DWORD cbCurrentLen;
    HRESULT hr = GetCurrentLength(&cbCurrentLen);

    if (SUCCEEDED(hr))
    {
        if (cbCurrentLen >= cbSize)
        {
            _nOffset += cbSize;
        }
        else
        {
            hr = E_INVALIDARG;
        }
    }

    TRACEHR_RET(hr);
}

// Trims cbSize bytes from the right end and returns them as a new buffer
IFACEMETHODIMP CMediaBufferWrapper::TrimRight(DWORD cbSize, _Out_ IMediaBufferWrapper **pWrapper)
{
    if (pWrapper == nullptr)
    {
        return E_INVALIDARG;
    }

    ComPtr<IMediaBufferWrapper> spResult;
    DWORD cbCurrentLen;
    HRESULT hr = this->GetCurrentLength(&cbCurrentLen);

    // Offset cannot be larger than current length.
    if (SUCCEEDED(hr) && cbCurrentLen <= cbSize)
    {
        return E_INVALIDARG;
    }

    // Create result buffer representing at the end of the current one
    if (SUCCEEDED(hr))
    {
        ComPtr<IMFMediaBuffer> spMediaBuffer;
        hr = MFCreateMediaBufferWrapper(GetMediaBuffer(), GetOffset() + cbCurrentLen - cbSize, cbSize, &spMediaBuffer);

        if (SUCCEEDED(hr))
        {
            hr = CMediaBufferWrapper::CreateInstance(spMediaBuffer.Get(), &spResult);
        }
    }

    // Change the size of the length of the buffer
    if (SUCCEEDED(hr))
    {
        hr = SetCurrentLength(cbCurrentLen - cbSize);
    }

    if (SUCCEEDED(hr))
    {
        *pWrapper = spResult.Detach();
    }

    return hr;
}

IFACEMETHODIMP CMediaBufferWrapper::Reset()
{
    _nOffset = 0;
    return _spMediaBuffer->SetCurrentLength(0);
}

HRESULT CMediaBufferWrapper::CheckMarshal()
{
    HRESULT hr = S_OK;
    IMarshal **ppMarshal = _spBufferMarshal.GetAddressOf();
    if (*ppMarshal == nullptr)
    {
        IMarshal *pNewMarshal = nullptr;
        hr = RoGetBufferMarshaler(&pNewMarshal);

        if (SUCCEEDED(hr) &&
            nullptr != InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(ppMarshal), pNewMarshal, nullptr))
        {
            pNewMarshal->Release();
        }
    }

    return hr;
}

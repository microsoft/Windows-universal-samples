//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include <InitGuid.h>
#include "StspSchemeHandler.h"
#include "StspMediaSource.h"
#include <wrl/module.h>

using namespace Microsoft::Samples::SimpleCommunication;
using namespace Microsoft::Samples::SimpleCommunication::Network;

ActivatableClass(CSchemeHandler);

CSchemeHandler::CSchemeHandler(void)
{
}

CSchemeHandler::~CSchemeHandler(void)
{
}

// IMediaExtension methods
IFACEMETHODIMP CSchemeHandler::SetProperties (ABI::Windows::Foundation::Collections::IPropertySet *pConfiguration)
{
    return S_OK;
}

// IMFSchemeHandler methods
IFACEMETHODIMP CSchemeHandler::BeginCreateObject( 
        _In_ LPCWSTR pwszURL,
        _In_ DWORD dwFlags,
        _In_ IPropertyStore *pProps,
        _COM_Outptr_opt_  IUnknown **ppIUnknownCancelCookie,
        _In_ IMFAsyncCallback *pCallback,
        _In_ IUnknown *punkState)
{
    HRESULT hr = S_OK;
    ComPtr<CMediaSource> spSource;

    try
    {
        if (ppIUnknownCancelCookie != nullptr)
        {
            *ppIUnknownCancelCookie = nullptr;
        }

        if (pwszURL == nullptr || pCallback == nullptr)
        {
            Throw(E_INVALIDARG);
        }

        if ((dwFlags & MF_RESOLUTION_MEDIASOURCE) == 0)
        {
            Throw(E_INVALIDARG);
        }

        ComPtr<IMFAsyncResult> spResult;
        ThrowIfError(CMediaSource::CreateInstance(&spSource));

        ComPtr<IUnknown> spSourceUnk;
        ThrowIfError(spSource.As(&spSourceUnk));
        ThrowIfError(MFCreateAsyncResult(spSourceUnk.Get(), pCallback, punkState, &spResult));

        ComPtr<CSchemeHandler> spThis = this;
        spSource->OpenAsync(pwszURL).then([this, spThis, spResult, spSource](concurrency::task<void>& openTask)
        {
            try
            {
                if (spResult == nullptr)
                {
                    ThrowIfError(MF_E_UNEXPECTED);
                }

                openTask.get();
            }
            catch(Exception ^exc)
            {
                if (spResult != nullptr)
                {
                    spResult->SetStatus(exc->HResult);
                }
            }

            if (spResult != nullptr)
            {
                MFInvokeCallback(spResult.Get());
            }
        });
    }
    catch(Exception ^exc)
    {
        if (spSource != nullptr)
        {
            spSource->Shutdown();
        }
        hr = exc->HResult;
    }

    TRACEHR_RET(hr);
}
        
IFACEMETHODIMP CSchemeHandler::EndCreateObject( 
        _In_ IMFAsyncResult *pResult,
        _Out_  MF_OBJECT_TYPE *pObjectType,
        _Out_  IUnknown **ppObject)
{
    if (pResult == nullptr || pObjectType == nullptr || ppObject == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = pResult->GetStatus();
    *pObjectType = MF_OBJECT_INVALID;
    *ppObject = nullptr;

    if (SUCCEEDED(hr))
    {
        ComPtr<IUnknown> punkSource;
        hr = pResult->GetObject(&punkSource);
        if (SUCCEEDED(hr))
        {
            *pObjectType = MF_OBJECT_MEDIASOURCE;
            *ppObject = punkSource.Detach();
        }
    }

    TRACEHR_RET(hr);
}
        
IFACEMETHODIMP CSchemeHandler::CancelObjectCreation( 
            _In_ IUnknown *pIUnknownCancelCookie)
{
    return E_NOTIMPL;
}

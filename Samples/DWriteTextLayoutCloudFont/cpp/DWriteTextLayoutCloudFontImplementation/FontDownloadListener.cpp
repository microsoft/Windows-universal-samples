//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "FontDownloadListener.h"
#include "DirectXHelper.h"

using namespace DWriteTextLayoutCloudFontImplementation;


// Only one instance of FontDownloadListener will be needed for the app, so
// declaring this as a global.
FontDownloadListener^ g_listener;


// DownloadListener implementation:

HRESULT STDMETHODCALLTYPE Listener::QueryInterface(
    REFIID riid,
    _COM_Outptr_ void **obj
    )
{
    *obj = nullptr;
    if (riid == __uuidof(IDWriteFontDownloadListener) || riid == __uuidof(IUnknown))
    {
        AddRef();
        *obj = this;
        return S_OK;
    }
    return E_NOINTERFACE;
}


ULONG STDMETHODCALLTYPE Listener::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

ULONG STDMETHODCALLTYPE Listener::Release()
{
    uint32 newCount = InterlockedDecrement(&m_refCount);
    if (newCount == 0)
        delete this;
    return newCount;
}


void STDMETHODCALLTYPE Listener::DownloadCompleted(
    _In_ IDWriteFontDownloadQueue* downloadQueue,
    _In_opt_ IUnknown* context,
    HRESULT downloadResult
    )
{
    // Since this is a platform callback that doesn't expect exceptions,
    // we shouldn't leave any exceptions unhandled.
    try
    {
        if (SUCCEEDED(downloadResult))
        {
            // If we had passed a context in the BeginDownload call, we could test
            // for that context now. 
            //
            // Note, however: relying on a context isn't useful if the font download 
            // queue is shared with other things, such as the XAML framework, that 
            // may also be calling BeginDownload. That's because you may do something,
            // such as creating or modifying a text layout, that results in requests 
            // being added to the download queue, but XAML may detect that the queue 
            // is not empty and call BeginDownload before you do, and it won't pass in
            // the context you'd be expecting.

            FontDownloadListener::RaiseDownloadCompleted();
        }
    }
    catch (...)
    {
        return;
    }
}



// FontDownloadListener implementation:

FontDownloadListener::FontDownloadListener()
{
    // Get the system's font download queue.

    Microsoft::WRL::ComPtr<IDWriteFactory3> dwriteFactory;

    DX::ThrowIfFailed(
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), &dwriteFactory)
        );

    DX::ThrowIfFailed(
        dwriteFactory->GetFontDownloadQueue(&m_downloadQueue)
        );

    // We will need an instance of our IDWriteFontDownloadListener class.
    m_registeredListener = new Listener();

    // To get notified when downloads complete, we can either register this 
    // listener now by calling m_downloadQueue->AddListener, or we can pass the 
    // listener when BeginDownload is called. If we only pass the listener when 
    // calling BeginDownload, then the effect of that would be that this listener 
    // will receive the DownloadCompleted callback only as a result of those 
    // BeginDownload calls that we make. But if something else calls BeginDownload 
    // on the queue, then this listener won't get called.
    //
    // Note that our app is hosted within XAML, and the XAML framework shares and 
    // also interacts with the same font download queue. We should assume that, 
    // as the app runs, the other object sharing the queue might also make calls 
    // to BeginDownload. 
    //
    // For example, suppose we create a text layout that uses a downloadable font
    // and then call BeginDownload. If we then call SetMaxWidth on that text layout 
    // before the download completes, another request for the same font data will 
    // get added to the queue. (This won't result in the data being downloaded
    // multiple times, however.) XAML checks the queue regularly, will see that 
    // it's not empty, and will call BeginDownload.
    //
    // In this sample, our code that created a text layout and called BeginDownload
    // is running in the UI thread along with XAML. In this situation, then, it 
    // would work okay to pass our listener when calling BeginDownload: there is no 
    // possibility that XAML would call BeginDownload between the point we created
    // the layout and when we checked the queue. But in other apps, things may not
    // be so predictable. For that reason, this sample demonstrates use of a global
    // listener rather than passing a listener when BeginDownload is called.
    //
    // In general, if your app has a download queue that's shared between 
    // different objects that can independently call BeginDownload, you should 
    // always register a listener so that any of your objects that may need to
    // respond will see all DownloadCompleted events. 
    //
    // This does create the possibility that an object could get multiple 
    // DownloadCompleted calls while it only called BeginDownload once, or that 
    // it gets DownloadCompleted calls resulting from download requests that 
    // another object generated. There are mitigations that can be put in place 
    // in the UI layer to avoid doing redundant work. See Scenario_Document1.cpp
    // for an example of this.

    DX::ThrowIfFailed(
        m_downloadQueue->AddListener(m_registeredListener.Get(), &m_token)
        );
}


FontDownloadListener::~FontDownloadListener()
{
    if (m_registeredListener != nullptr)
    {
        m_downloadQueue->RemoveListener(m_token);
    }
}


void FontDownloadListener::Initialize()
{
    if (g_listener == nullptr)
    {
        g_listener = ref new FontDownloadListener();
    }
}


void FontDownloadListener::BeginDownload()
{
    if (g_listener != nullptr)
    {
        // Don't request a download if the queue is empty.
        if (!g_listener->m_downloadQueue->IsEmpty())
        {
            g_listener->m_downloadQueue->BeginDownload();
        }
    }
}


void FontDownloadListener::RaiseDownloadCompleted()
{
    DownloadCompleted();
}

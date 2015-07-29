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

#pragma once

#include "TextLayout.h"

namespace DWriteTextLayoutCloudFontImplementation
{
    public delegate void FontDownloadCompletedHandler();


    // To use downloadable fonts, the app must implement
    // IDWriteFontDownloadListener.
    class Listener : public IDWriteFontDownloadListener
    {
    public:
        void STDMETHODCALLTYPE DownloadCompleted(
            _In_ IDWriteFontDownloadQueue* downloadQueue,
            _In_opt_ IUnknown* context,
            HRESULT downloadResult
            );

        // IDWriteFontDownloadListener inherits from IUnknown
        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid,
            _COM_Outptr_ void **obj
            ) override;

        ULONG STDMETHODCALLTYPE AddRef() override;

        ULONG STDMETHODCALLTYPE Release() override;

    private:
        uint32 m_refCount = 0;
    };


    // This class wraps around the IDWriteFontDownloadListener 
    // implementation to provide WinRT interop.
    public ref class FontDownloadListener sealed
    {
    public:
        FontDownloadListener();
        virtual ~FontDownloadListener();

        static event FontDownloadCompletedHandler^ DownloadCompleted;

        static void Initialize();
        static void BeginDownload();

    internal:
        static void RaiseDownloadCompleted();

    private:
        Microsoft::WRL::ComPtr<Listener>                    m_registeredListener; // native Listener class
        Microsoft::WRL::ComPtr<IDWriteFontDownloadQueue>    m_downloadQueue;
        uint32                                              m_token = 0;
    };

}
//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once
#include "StspNetwork.h"
#include "StspDefs.h"
#include <queue>

namespace Microsoft { namespace Samples { namespace SimpleCommunication {
    ref class StspMediaSinkProxy;
    
    public ref class IncomingConnectionEventArgs sealed
    {
    public:
        property String ^RemoteUrl
        {
            String ^get() { return _remoteUrl; }
        }

        void Accept();
        void Refuse();

    internal:
        IncomingConnectionEventArgs(IStspSinkInternal *pSink, DWORD connectionId, String ^remoteUrl);

    private:
        String ^_remoteUrl;
        DWORD _connectionId;
        ComPtr<IStspSinkInternal> _spSink;
    };

    interface class ISinkCallback
    {
        void FireIncomingConnection(IncomingConnectionEventArgs ^args);
        void OnShutdown();
    };
    
    public ref class StspMediaSinkProxy sealed
    {
    public:
        StspMediaSinkProxy();
        virtual ~StspMediaSinkProxy();

        Windows::Media::IMediaExtension ^GetMFExtensions();

        Windows::Foundation::IAsyncOperation<Windows::Media::IMediaExtension^>^ InitializeAsync(
            Windows::Media::MediaProperties::IMediaEncodingProperties ^videoEncodingProperties,
            Windows::Media::MediaProperties::IMediaEncodingProperties ^audioEncodingProperties
            );

        event Windows::Foundation::EventHandler<Object^>^ IncomingConnectionEvent;

    internal:

        void SetMediaStreamProperties(
            Windows::Media::Capture::MediaStreamType MediaStreamType,
            _In_opt_ Windows::Media::MediaProperties::IMediaEncodingProperties ^mediaEncodingProperties
            );

    private:
        void FireIncomingConnection(IncomingConnectionEventArgs ^args);
        void OnShutdown();

        ref class StspSinkCallback sealed: ISinkCallback
        {
        public:
            virtual void FireIncomingConnection(IncomingConnectionEventArgs ^args)
            {
                _parent->FireIncomingConnection(args);
            }

            virtual void OnShutdown()
            {
                _parent->OnShutdown();
            }

        internal:
            StspSinkCallback(StspMediaSinkProxy ^parent)
                : _parent(parent)
            {
            }

        private:
            StspMediaSinkProxy ^_parent;
        };

        void CheckShutdown()
        {
            if (_fShutdown)
            {
                Throw(MF_E_SHUTDOWN);
            }
        }

    private:
        CritSec _critSec;
        ComPtr<IMFMediaSink> _spMediaSink;
        bool _fShutdown;
    };
} } }

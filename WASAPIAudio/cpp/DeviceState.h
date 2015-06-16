//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// devicestate.h
//

#include "MainPage.xaml.h"

#pragma once

using namespace Windows::Storage::Streams;

namespace SDKSample
{
    namespace WASAPIAudio
    {
        // NB: All states >= DeviceStateInitialized will allow some methods
        // to be called successfully on the Audio Client
        public enum class DeviceState
        {
            DeviceStateUnInitialized,
            DeviceStateInError,
            DeviceStateDiscontinuity,
            DeviceStateFlushing,
            DeviceStateActivated,
            DeviceStateInitialized,
            DeviceStateStarting,
            DeviceStatePlaying,
            DeviceStateCapturing,
            DeviceStatePausing,
            DeviceStatePaused,
            DeviceStateStopping,
            DeviceStateStopped
        };

        // Class for DeviceStateChanged events
        public ref class DeviceStateChangedEventArgs sealed
        {
        internal:
            DeviceStateChangedEventArgs( DeviceState newState, HRESULT hr) :
                m_DeviceState( newState ),
                m_hr( hr )
            {};

            property DeviceState State
            {
                DeviceState get() { return m_DeviceState; }
            };

            property int hr
            {
                int get() { return m_hr; }
            }

        private:
            DeviceState      m_DeviceState;
            HRESULT          m_hr;
        };

        // DeviceStateChanged delegate
        public delegate void DeviceStateChangedHandler( Platform::Object^ sender, DeviceStateChangedEventArgs^ e );

        // DeviceStateChanged Event
        public ref class DeviceStateChangedEvent sealed
        {
        public:
            DeviceStateChangedEvent() :
                m_DeviceState( DeviceState::DeviceStateUnInitialized )
            {};

            DeviceState GetState() { return m_DeviceState; };

        internal:
            void SetState( DeviceState newState, HRESULT hr, Platform::Boolean FireEvent ) {
                if (m_DeviceState != newState)
                {
                    m_DeviceState = newState;

                    if (FireEvent)
                    {
                        DeviceStateChangedEventArgs^ e = ref new DeviceStateChangedEventArgs( m_DeviceState, hr );
                        StateChangedEvent( this, e );
                    }
                }
            };

        public:
            static event DeviceStateChangedHandler^    StateChangedEvent;

        private:
            DeviceState     m_DeviceState;
        };
    }
}

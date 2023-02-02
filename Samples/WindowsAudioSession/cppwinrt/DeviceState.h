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
#include "DeviceStateChangedEventArgs.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct DeviceStateChangedEventArgs : DeviceStateChangedEventArgsT<DeviceStateChangedEventArgs>
    {
        DeviceStateChangedEventArgs(enum DeviceState deviceState, hresult error) : m_deviceState(deviceState), m_error(error) {}

        enum DeviceState DeviceState() { return m_deviceState; }
        hresult ExtendedError() { return m_error; }

    private:
        enum DeviceState const m_deviceState;
        hresult const m_error;
    };

    template<typename D>
    struct DeviceStateSourceT
    {
        DeviceState DeviceState() { return m_deviceState; }

        event_token DeviceStateChanged(Windows::Foundation::TypedEventHandler<IDeviceStateSource, SDKTemplate::DeviceStateChangedEventArgs> const& handler)
        {
            return m_deviceStateChanged.add(handler);
        }
        void DeviceStateChanged(event_token token)
        {
            return m_deviceStateChanged.remove(token);
        }

    protected:
        enum DeviceState m_deviceState = DeviceState::Uninitialized;
        event<Windows::Foundation::TypedEventHandler<IDeviceStateSource, SDKTemplate::DeviceStateChangedEventArgs>> m_deviceStateChanged;

        void SetState(enum DeviceState state, HRESULT error = S_OK)
        {
            if (m_deviceState != state)
            {
                m_deviceState = state;
                m_deviceStateChanged(*static_cast<D*>(this), make<implementation::DeviceStateChangedEventArgs>(state, error));
            }
        }

        void SetStateNoNotify(enum DeviceState state)
        {
            m_deviceState = state;
        }
    };
}


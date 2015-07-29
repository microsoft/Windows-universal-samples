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
// Scenario1.xaml.h
// Declaration of the Scenario1 class
//

#pragma once

#include "pch.h"
#include "Scenario1.g.h"
#include "MainPage.xaml.h"

using namespace Windows::Devices::Enumeration;

namespace SDKSample
{
    namespace WASAPIAudio
    {
        // Custom properties defined in mmdeviceapi.h in the format "{GUID} PID"
        static Platform::String^ PKEY_AudioEndpoint_Supports_EventDriven_Mode = "{1da5d803-d492-4edd-8c23-e0c0ffee7f0e} 7";
    
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario1 sealed
        {
        public:
            Scenario1();
    
        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    
        private:
            void ShowStatusMessage( Platform::String^ str, NotifyType messageType );
            void Enumerate_Click( Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e );
            void EnumerateAudioDevicesAsync();
    
        private:
            MainPage^                               rootPage;
            Windows::UI::Xaml::Controls::ListBox^   m_DevicesList;
        };
    }
}

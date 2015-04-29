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
// Scenario4.xaml.h
// Declaration of the Scenario4 class
//

#pragma once

#include "pch.h"
#include "Scenario4.g.h"
#include "MainPage.xaml.h"
#include "WASAPICapture.h"

#define OSC_START_X  100
#define OSC_START_Y  100
#define OSC_X_LENGTH 700
#define OSC_TOTAL_HEIGHT 200

namespace SDKSample
{
    namespace WASAPIAudio
    {
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario4 sealed
        {

        public:
            Scenario4();

        protected:
            // Template Support
            virtual void OnNavigatedTo( Windows::UI::Xaml::Navigation::NavigationEventArgs^ e ) override;
            virtual void OnNavigatedFrom( Windows::UI::Xaml::Navigation::NavigationEventArgs^ e ) override;

        private:
            ~Scenario4();

            // UI Events
            void btnStartCapture_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void btnStopCapture_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        
            // UI Helpers
            void ShowStatusMessage( Platform::String^ str, NotifyType messageType );
            void UpdateMediaControlUI( DeviceState deviceState );

            // Handlers
            void OnDeviceStateChange( Object^ sender, DeviceStateChangedEventArgs^ e );
            void OnPlotDataReady( Object^ sender, PlotDataReadyEventArgs^ e );

            void InitializeCapture( Object^ sender, Object^ e );
            void StopCapture( Object^ sender, Object^ e );

        private:
            MainPage^                                       rootPage;
            Windows::UI::Core::CoreDispatcher^              m_CoreDispatcher;
            Windows::UI::Xaml::Shapes::Polyline^            m_Oscilloscope;

            Windows::Foundation::EventRegistrationToken     m_deviceStateChangeToken;
            Windows::Foundation::EventRegistrationToken     m_plotDataReadyToken;

            int                         m_DiscontinuityCount;
            Platform::Boolean           m_IsMFLoaded;
            Platform::Boolean           m_IsLowLatency;
            DeviceStateChangedEvent^    m_StateChangedEvent;
            ComPtr<WASAPICapture>       m_spCapture;
        };
    }
}

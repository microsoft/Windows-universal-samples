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

//
// Scenario2.xaml.h
// Declaration of the Scenario2 class
//

#pragma once

#include "pch.h"
#include "Scenario2.g.h"
#include "MainPage.xaml.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web;

namespace SDKTemplate
{
    namespace WebSocket
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
    	[Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario2 sealed
        {
        public:
            Scenario2();
    
        protected:
            virtual void OnNavigatedTo(NavigationEventArgs^ e) override;
        private:
            ~Scenario2();
    
    		MainPage^ rootPage;
            StreamWebSocket^ streamWebSocket;
            IBuffer^ readBuffer;
            IBuffer^ sendBuffer;
            int bytesReceived;
            int dataSent;
            CRITICAL_SECTION criticalSection;
    
            void Start_Click(Object^ sender, RoutedEventArgs^ e);
            void Stop_Click(Object^ sender, RoutedEventArgs^ e);
            void Scenario2ReceiveData();
            void Scenario2SendData();
            void Scenario2Closed(IWebSocket^ sender, WebSocketClosedEventArgs^ args);
            void HandleException(Exception^ exception);
            void MarshalText(TextBox^ output, String^ value);
            void MarshalText(TextBox^ output, String^ value, bool append);
        };
    }
}

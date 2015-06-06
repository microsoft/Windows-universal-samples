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
// Scenario1.xaml.h
// Declaration of the Scenario1 class
//

#pragma once

#include "pch.h"
#include "Scenario1.g.h"
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
        public ref class Scenario1 sealed
        {
        public:
            Scenario1();
    
        protected:
            virtual void OnNavigatedTo(NavigationEventArgs^ e) override;
        private:
            ~Scenario1();
    		
    		MainPage^ rootPage;
            MessageWebSocket^ messageWebSocket;
            DataWriter^ messageWriter;
            CRITICAL_SECTION criticalSection;
    
            void Start_Click(Object^ sender, RoutedEventArgs^ e);
            void Close_Click(Object^ sender, RoutedEventArgs^ e);
            void SendMessage();
            void MessageReceived(MessageWebSocket^ sender, MessageWebSocketMessageReceivedEventArgs^ args);
            void Closed(IWebSocket^ sender, WebSocketClosedEventArgs^ args);
            void MarshalText(TextBox^ output, String^ value);
            void MarshalText(TextBox^ output, String^ value, bool append);
            void HandleException(Exception^ exception);
        };
    }
}

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

namespace SDKTemplate
{
    namespace StreamSocketSample
    {
        [Windows::Foundation::Metadata::WebHostHidden]
        // In c++, adding this attribute to ref classes enables data binding.
        // For more info search for 'Bindable' on the page http://go.microsoft.com/fwlink/?LinkId=254639 
        [Windows::UI::Xaml::Data::Bindable]
        public ref class LocalHostItem sealed
        {
        public:
            LocalHostItem(Windows::Networking::HostName^ localHostName);
            property Platform::String^ DisplayString { Platform::String^ get() { return displayString; } }
            property Windows::Networking::HostName^ LocalHost { Windows::Networking::HostName^ get() { return localHost; } }

        private:
            Platform::String^ displayString;
            Windows::Networking::HostName^ localHost;
        };

        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario1 sealed
        {
        public:
            Scenario1();

        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        private:
            MainPage^ rootPage;

            // Vector containing all available local HostName endpoints.
            Platform::Collections::Vector<LocalHostItem^>^ localHostItems;
            void StartListener_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void BindToAny_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void BindToAny_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void PopulateAdapterList();
        };

        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class ListenerContext sealed
        {
        public:
            ListenerContext(MainPage^ rootPage, Windows::Networking::Sockets::StreamSocketListener^ listener);
            void OnConnection(
                Windows::Networking::Sockets::StreamSocketListener^ listener,
                Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs^ object);

        private:
            MainPage^ rootPage;
            Windows::Networking::Sockets::StreamSocketListener^ listener;

            ~ListenerContext();
            void ReceiveStringLoop(
                Windows::Storage::Streams::DataReader^ reader,
                Windows::Networking::Sockets::StreamSocket^ socket);
            void NotifyUserFromAsyncThread(Platform::String^ message, NotifyType type);
        };
    }
}

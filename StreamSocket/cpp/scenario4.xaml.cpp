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
// Scenario4.xaml.cpp
// Implementation of the Scenario4 class
//

#include "pch.h"
#include "Scenario4.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::StreamSocketSample;

using namespace Concurrency;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario4::Scenario4()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario4::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void Scenario4::CloseSockets_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (CoreApplication::Properties->HasKey("clientDataWriter"))
    {
        // Remove the listener from the list of application properties as we are about to close it.
        DataWriter^ dataWriter = dynamic_cast<DataWriter^>(CoreApplication::Properties->Lookup("clientDataWriter"));
        CoreApplication::Properties->Remove("clientDataWriter");

        // Close the data writer.
        delete dataWriter;
        dataWriter = nullptr;
    }

    if (CoreApplication::Properties->HasKey("clientSocket"))
    {
        // Remove the socket from the list of application properties as we are about to close it.
        StreamSocket^ socket = dynamic_cast<StreamSocket^>(CoreApplication::Properties->Lookup("clientSocket"));
        CoreApplication::Properties->Remove("clientSocket");

        // Close the socket.
        delete socket;
        socket = nullptr;
    }

    if (CoreApplication::Properties->HasKey("listener"))
    {
        // This is the last reference to the context, so once removed the object will be deleted.
        CoreApplication::Properties->Remove("listener");
    }

    if (CoreApplication::Properties->HasKey("connected"))
    {
        CoreApplication::Properties->Remove("connected");
    }

    if (CoreApplication::Properties->HasKey("serverAddress"))
    {
        CoreApplication::Properties->Remove("serverAddress");
    }
    
    if (CoreApplication::Properties->HasKey("adapter"))
    {
        CoreApplication::Properties->Remove("adapter");
    }

    rootPage->NotifyUser("Socket and listener closed", NotifyType::StatusMessage);
}

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
// Scenario3.xaml.cpp
// Implementation of the Scenario3 class
//

#include "pch.h"
#include "Scenario2.xaml.h"
#include "Scenario3.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::DatagramSocketSample;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario3::Scenario3()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}


void Scenario3::SendHello_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (!CoreApplication::Properties->HasKey("clientSocket"))
    {
        rootPage->NotifyUser("Please run previous steps before doing this one.", NotifyType::ErrorMessage);
        return;
    }

    SocketContext^ socketContext = dynamic_cast<SocketContext^>(CoreApplication::Properties->Lookup("clientSocket"));
    if (!socketContext->IsConnected())
    {
        rootPage->NotifyUser("The socket is not yet connected. Please wait.", NotifyType::ErrorMessage);
        return;
    }

    String^ stringToSend("Hello");
    try
    {
        socketContext->GetWriter()->WriteString(stringToSend);
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser("Send failed with error: " + ex->Message, NotifyType::ErrorMessage);
    }

    // Write the locally buffered data to the network. Please note that write operation will succeed
    // even if the server is not listening.
    create_task(socketContext->GetWriter()->StoreAsync()).then(
        [this, stringToSend] (task<unsigned int> writeTask)
    {
        try
        {
            // Try getting an excpetion.
            writeTask.get();
            SendOutput->Text = "\"" + stringToSend + "\" sent successfully";
        }
        catch (Exception^ exception)
        {
            rootPage->NotifyUser("Send failed with error: " + exception->Message, NotifyType::ErrorMessage);
        }
    });
}

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
#include "Scenario3.xaml.h"

using namespace SDKTemplate::StreamSocketSample;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
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
    if (!CoreApplication::Properties->HasKey("connected"))
    {
        rootPage->NotifyUser("Please run previous steps before doing this one.", NotifyType::ErrorMessage);
        return;
    }

    StreamSocket^ socket = dynamic_cast<StreamSocket^>(CoreApplication::Properties->Lookup("clientSocket"));
    DataWriter^ writer;

    // If possible, use the DataWriter we created previously. If not, then create new one.
    if (!CoreApplication::Properties->HasKey("clientDataWriter"))
    {
        writer = ref new DataWriter(socket->OutputStream);
        CoreApplication::Properties->Insert("clientDataWriter", writer);
    }
    else
    {
        writer = dynamic_cast<DataWriter^>(CoreApplication::Properties->Lookup("clientDataWriter"));
    }

    // Write first the length of the string a UINT32 value followed up by the string. The operation will just store 
    // the data locally.
    String^ stringToSend("Hello");
    writer->WriteUInt32(writer->MeasureString(stringToSend));
    writer->WriteString(stringToSend);

    // Write the locally buffered data to the network.
    create_task(writer->StoreAsync()).then([this, socket, stringToSend] (task<unsigned int> writeTask)
    {
        try
        {
            // Try getting an exception.
            writeTask.get();
            SendOutput->Text = "\"" + stringToSend + "\" sent successfully";
        }
        catch (Exception^ exception)
        {
            rootPage->NotifyUser("Send failed with error: " + exception->Message, NotifyType::ErrorMessage);
        }
    });
}

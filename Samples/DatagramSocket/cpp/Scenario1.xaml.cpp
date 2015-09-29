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
// Scenario1.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include "Scenario1.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::DatagramSocketSample;

using namespace Platform;
using namespace Concurrency;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Networking;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario1::Scenario1()
{
    InitializeComponent();
    this->localHostItems = ref new Vector<LocalHostItem^>();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
    BindToAny->IsChecked = true;
    PopulateAdapterList();
}

void Scenario1::BindToAny_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    AdapterList->IsEnabled = false;
}

void Scenario1::BindToAny_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    AdapterList->IsEnabled = true;
}

void Scenario1::StartListener_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (CoreApplication::Properties->HasKey("listener"))
    {
        rootPage->NotifyUser(
            "This step has already been executed. Please move to the next one.", 
            NotifyType::ErrorMessage);
        return;
    }

    if (ServiceNameForListener->Text == nullptr)
    {
        rootPage->NotifyUser("Please provide a service name.", NotifyType::ErrorMessage);
        return;
    }

    DatagramSocket^ listener = ref new DatagramSocket();

    if (!InboundBufferSize->Text->IsEmpty())
    {
        int inboundBufferSize = 0;
        inboundBufferSize = _wtoi(InboundBufferSize->Text->Data());
        if (inboundBufferSize <= 0)
        {
            rootPage->NotifyUser("Please provide a positive numeric Inbound buffer size.", NotifyType::ErrorMessage);
            return;
        }

        try
        {
            // Running both client and server on localhost will most likely not reach the buffer limit.
            // Refer to the DatagramSocketControl class' MSDN documentation for the full list of control options.
            listener->Control->InboundBufferSizeInBytes = inboundBufferSize;
        }
        catch (InvalidArgumentException^)
        {
            rootPage->NotifyUser("Please provide a valid Inbound buffer size.", NotifyType::ErrorMessage);
            return;
        }
    }

    LocalHostItem^ selectedLocalHost = nullptr;
    if (BindToAddress->IsChecked->Value || BindToAdapter->IsChecked->Value)
    {
        selectedLocalHost = static_cast<LocalHostItem^>(AdapterList->SelectedItem);
        if (selectedLocalHost == nullptr)
        {
            rootPage->NotifyUser("Please select an address / adapter.", NotifyType::ErrorMessage);
            return;
        }

        // The user selected an address. For demo purposes, we ensure that connect will be using the same address.
        CoreApplication::Properties->Insert("serverAddress", selectedLocalHost->LocalHost->CanonicalName);
    }
    else
    {
        // Clear up any previous serverAddress or adapter used by the client part of the demo.
        CoreApplication::Properties->Insert("serverAddress", nullptr);
    }
    
    ListenerContext^ listenerContext = ref new ListenerContext(rootPage, listener);
    listener->MessageReceived += ref new TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>(
        listenerContext, 
        &ListenerContext::OnMessage);

    // Events cannot be hooked up directly to the ScenarioInput1 object, as the object can fall out-of-scope and be 
    // deleted. This would render any event hooked up to the object ineffective. The ListenerContext guarantees that 
    // both the socket and object that serves its events have the same lifetime.
    CoreApplication::Properties->Insert("listener", listenerContext);

    if (BindToAny->IsChecked->Value)
    {
        // Don't limit traffic to an address or an adapter.
        create_task(listener->BindServiceNameAsync(ServiceNameForListener->Text)).then([this] (task<void> previousTask)
        {
            try
            {
                // Try getting an exception.
                previousTask.get();
                rootPage->NotifyUser("Listening", NotifyType::StatusMessage);
            }
            catch (Exception^ exception)
            {
                CoreApplication::Properties->Remove("listener");
                rootPage->NotifyUser(
                    "Start listening failed with error: " + exception->Message, 
                    NotifyType::ErrorMessage);
            }
        });
    }
    else if (BindToAddress->IsChecked->Value)
    {
        // Try to bind to a specific address.
        create_task(listener->BindEndpointAsync(selectedLocalHost->LocalHost, ServiceNameForListener->Text)).then(
            [this, selectedLocalHost] (task<void> previousTask)
        {
            try
            {
                // Try getting an exception.
                previousTask.get();
                rootPage->NotifyUser(
                    "Listening on address " + selectedLocalHost->LocalHost->CanonicalName, 
                    NotifyType::StatusMessage);
            }
            catch (Exception^ exception)
            {
                CoreApplication::Properties->Remove("listener");
                rootPage->NotifyUser(
                    "Start listening failed with error: " + exception->Message, 
                    NotifyType::ErrorMessage);
            }
        });
    }
    else if (BindToAdapter->IsChecked->Value)
    {
        // Try to limit traffic to the selected adapter. 
        // This option will be overridden by interfaces with weak-host or forwarding modes enabled.
        NetworkAdapter^ selectedAdapter = selectedLocalHost->LocalHost->IPInformation->NetworkAdapter;

        create_task(listener->BindServiceNameAsync(
            ServiceNameForListener->Text,
            selectedAdapter)).then([this, selectedAdapter] (task<void> previousTask)
        {
            try
            {
                // Try getting an exception.
                previousTask.get();
                rootPage->NotifyUser(
                    "Listening on adapter" + selectedAdapter->NetworkAdapterId,
                    NotifyType::StatusMessage);
            }
            catch (Exception^ exception)
            {
                CoreApplication::Properties->Remove("listener");
                rootPage->NotifyUser(
                    "Start listening failed with error: " + exception->Message, 
                    NotifyType::ErrorMessage);
            }
        });
    }
}

void Scenario1::PopulateAdapterList()
{
    localHostItems->Clear();
    AdapterList->ItemsSource = localHostItems;
    AdapterList->DisplayMemberPath = "DisplayString";

    for (HostName^ localHostInfo : NetworkInformation::GetHostNames())
    {
        if (localHostInfo->IPInformation != nullptr)
        {
            LocalHostItem^ adapterItem = ref new LocalHostItem(localHostInfo);
            localHostItems->Append(adapterItem);
        }
    }
}

void ListenerContext::OnMessage(DatagramSocket^ socket, DatagramSocketMessageReceivedEventArgs^ eventArguments)
{
    if (outputStream != nullptr)
    {
        EchoMessage(eventArguments);
        return;
    }

    // We do not have an output stream yet so create one.
    create_task(socket->GetOutputStreamAsync(eventArguments->RemoteAddress, eventArguments->RemotePort)).then(
        [this, socket, eventArguments] (IOutputStream^ stream)
    {
        // It might happen that the OnMessage was invoked more than once before the GetOutputStreamAsync call
        // completed. In this case we will end up with multiple streams - just keep one of them.
        EnterCriticalSection(&lock);

        if (outputStream == nullptr)
        {
            outputStream = stream;
            hostName = eventArguments->RemoteAddress;
            port = eventArguments->RemotePort;
        }

        LeaveCriticalSection(&lock);

        EchoMessage(eventArguments);
    }).then([this, socket, eventArguments] (task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();
        }
        catch (Exception^ exception)
        {
            NotifyUserFromAsyncThread(
                "Getting an output stream failed with error: " + exception->Message, 
                NotifyType::ErrorMessage);
        }
    });
}

ListenerContext::ListenerContext(MainPage^ rootPage, DatagramSocket^ listener)
{
    this->rootPage = rootPage;
    this->listener = listener;
    InitializeCriticalSectionEx(&lock, 0, 0);
}

ListenerContext::~ListenerContext()
{
    // A DatagramSocket can be closed in two ways:
    //  - explicitly: using the 'delete' keyword (listener is closed even if there are outstanding references to it).
    //  - implicitly: removing the last reference to it (i.e., falling out-of-scope).
    //
    // When a DatagramSocket is closed implicitly, it can take several seconds for the local UDP port being used
    // by it to be freed/reclaimed by the lower networking layers. During that time, other UDP sockets on the machine
    // will not be able to use the port. Thus, it is strongly recommended that DatagramSocket instances be explicitly
    // closed before they go out of scope(e.g., before application exit). The call below explicitly closes the socket.
    delete listener;
    listener = nullptr;

    DeleteCriticalSection(&lock);
}

void ListenerContext::EchoMessage(DatagramSocketMessageReceivedEventArgs^ eventArguments)
{
    if (!IsMatching(eventArguments->RemoteAddress, eventArguments->RemotePort))
    {
        // In the sample we are communicating with just one peer. To communicate with multiple peers, an application
        // should cache output streams (e.g., by using a hash map), because creating an output stream for each
        // received datagram is costly. Keep in mind though, that every cache requires logic to remove old
        // or unused elements; otherwise, the cache will turn into a memory leaking structure.
        NotifyUserFromAsyncThread("Got datagram from " + eventArguments->RemoteAddress->DisplayName + ":" +
            eventArguments->RemotePort + ", but already 'connected' to " + hostName->DisplayName + ":" +
            port, NotifyType::ErrorMessage);
    }

    create_task(outputStream->WriteAsync(eventArguments->GetDataReader()->DetachBuffer())).then(
        [this] (task<unsigned int> writeTask)
    {
        try
        {
            // Try getting an exception.
            writeTask.get();
        }
        catch (Exception^ exception)
        {
            NotifyUserFromAsyncThread(
                "Echoing a message failed with error: " + exception->Message, 
                NotifyType::ErrorMessage);
        }
    });
}

bool ListenerContext::IsMatching(HostName^ hostName, String^ port)
{
    return (this->hostName == hostName && this->port == port);
}

void ListenerContext::NotifyUserFromAsyncThread(String^ message, NotifyType type)
{
    rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, message, type] ()
    {
        rootPage->NotifyUser(message, type);
    }));
}

LocalHostItem::LocalHostItem(HostName^ localHostName)
{
    if (localHostName == nullptr)
    {
        throw ref new InvalidArgumentException("localHostName cannot be null");
    }

    if (localHostName->IPInformation == nullptr)
    {
        throw ref new InvalidArgumentException("Adapter information not found");
    }

    this->localHost = localHostName;
    this->displayString = "Address: " + localHostName->DisplayName +
        " Adapter: " + localHostName->IPInformation->NetworkAdapter->NetworkAdapterId;
}

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
using namespace SDKTemplate::StreamSocketSample;

using namespace Concurrency;
using namespace Platform;
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
    // Overriding the listener here is safe as it will be deleted once all references to it are gone. However,
    // in many cases this is a dangerous pattern to override data semi-randomly (each time user clicked the button)
    // so we block it here.
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
        CoreApplication::Properties->Insert("adapter", nullptr);
    }

    StreamSocketListener^ listener = ref new StreamSocketListener();
    ListenerContext^ listenerContext = ref new ListenerContext(rootPage, listener);
    listener->ConnectionReceived += 
        ref new TypedEventHandler<StreamSocketListener^, StreamSocketListenerConnectionReceivedEventArgs^>(
            listenerContext, 
            &ListenerContext::OnConnection);

    // If necessary, tweak the listener's control options before carrying out the bind operation.
    // These options will be automatically applied to the connected StreamSockets resulting from
    // incoming connections (i.e., those passed as arguments to the ConnectionReceived event handler).
    // Refer to the StreamSocketListenerControl class' MSDN documentation for the full list of control options.
    listener->Control->KeepAlive = false;

    // Events cannot be hooked up directly to the ScenarioInput1 object, as the object can fall out-of-scope and be
    // deleted. This would render any event hooked up to the object ineffective. The ListenerContext guarantees that
    // both the listener and object that serves its events have the same lifetime.
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

        // For demo purposes, ensure that we use the same adapter in the client connect scenario.
        CoreApplication::Properties->Insert("adapter", selectedAdapter);

        create_task(listener->BindServiceNameAsync(
            ServiceNameForListener->Text,
            SocketProtectionLevel::PlainSocket,
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

ListenerContext::ListenerContext(MainPage^ rootPage, StreamSocketListener^ listener)
{
    this->rootPage = rootPage;
    this->listener = listener;
}

ListenerContext::~ListenerContext()
{
    // The listener can be closed in two ways:
    //  - explicit: by using delete operator (the listener is closed even if there are outstanding references to it).
    //  - implicit: by removing last reference to it (i.e. falling out-of-scope).
    // In this case this is the last reference to the listener so both will yield the same result.
    delete listener;
    listener = nullptr;
}

void ListenerContext::OnConnection(
    StreamSocketListener^ listener, 
    StreamSocketListenerConnectionReceivedEventArgs^ object)
{
    DataReader^ reader = ref new DataReader(object->Socket->InputStream);

    // Start a receive loop.
    ReceiveStringLoop(reader, object->Socket);
}

void ListenerContext::ReceiveStringLoop(DataReader^ reader, StreamSocket^ socket)
{
    // Read first 4 bytes (length of the subsequent string).
    create_task(reader->LoadAsync(sizeof(UINT32))).then([this, reader, socket] (unsigned int size)
    {
        if (size < sizeof(UINT32))
        {
            // The underlying socket was closed before we were able to read the whole data.
            cancel_current_task();
        }

        unsigned int stringLength = reader->ReadUInt32();
        return create_task(reader->LoadAsync(stringLength)).then(
            [this, reader, stringLength] (unsigned int actualStringLength)
        {
            if (actualStringLength != stringLength)
            {
                // The underlying socket was closed before we were able to read the whole data.
                cancel_current_task();
            }

            // Display the string on the screen. This thread is invoked on non-UI thread, so we need to marshal the 
            // call back to the UI thread.
            NotifyUserFromAsyncThread(
                "Received data: \"" + reader->ReadString(actualStringLength) + "\"", 
                NotifyType::StatusMessage);
        });
    }).then([this, reader, socket] (task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();

            // Everything went ok, so try to receive another string. The receive will continue until the stream is
            // broken (i.e. peer closed the socket).
            ReceiveStringLoop(reader, socket);
        }
        catch (Exception^ exception)
        {
            NotifyUserFromAsyncThread(
                "Read stream failed with error: " + exception->Message, 
                NotifyType::ErrorMessage);

            // Explicitly close the socket.
            delete socket;
        }
        catch (task_canceled&)
        {
            // Do not print anything here - this will usually happen because user closed the client socket.

            // Explicitly close the socket.
            delete socket;
        }
    });
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

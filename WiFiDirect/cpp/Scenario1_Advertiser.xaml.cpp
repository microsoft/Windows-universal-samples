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
#include "pch.h"
#include "Scenario1_Advertiser.xaml.h"

using namespace SDKTemplate;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::WiFiDirect;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Popups;
using namespace Windows::Networking::Sockets;
using namespace Platform::Collections;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario1_Advertiser::Scenario1_Advertiser() : rootPage(MainPage::Current)
{
    InitializeComponent();

    _connectedDevices = ref new Vector<ConnectedDevice^>();
    lvConnectedDevices->ItemsSource = _connectedDevices;
    lvConnectedDevices->SelectionMode = ListViewSelectionMode::Single;
}

void SDKTemplate::Scenario1_Advertiser::btnStartAdvertisement_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        if (!_publisher)
        {
            _publisher = ref new WiFiDirectAdvertisementPublisher();
        }

        if (chkListener->IsChecked)
        {
            if (!_listener)
            {
                _listener = ref new WiFiDirectConnectionListener();
            }

            _connectionRequestedToken = _listener->ConnectionRequested += ref new TypedEventHandler<WiFiDirectConnectionListener^,
                WiFiDirectConnectionRequestedEventArgs^>(this, &Scenario1_Advertiser::OnConnectionRequested, CallbackContext::Same);
        }

        _publisher->Advertisement->IsAutonomousGroupOwnerEnabled = (chkPreferGroupOwnerMode->IsChecked->Value == true);

        if (cmbListenState->SelectionBoxItem->ToString()->Equals("Normal"))
        {
            _publisher->Advertisement->ListenStateDiscoverability = WiFiDirectAdvertisementListenStateDiscoverability::Normal;
        }
        else if (cmbListenState->SelectionBoxItem->ToString()->Equals("Intensive"))
        {
            _publisher->Advertisement->ListenStateDiscoverability = WiFiDirectAdvertisementListenStateDiscoverability::Intensive;
        }
        else if (cmbListenState->SelectionBoxItem->ToString()->Equals("None"))
        {
            _publisher->Advertisement->ListenStateDiscoverability = WiFiDirectAdvertisementListenStateDiscoverability::None;
        }

        _statusChangedToken = _publisher->StatusChanged += ref new TypedEventHandler<WiFiDirectAdvertisementPublisher^,
            WiFiDirectAdvertisementPublisherStatusChangedEventArgs^>(this, &Scenario1_Advertiser::OnStatusChanged, CallbackContext::Same);

        _publisher->Start();

        rootPage->NotifyUser("Advertisement started, waiting for StatusChanged callback...", NotifyType::StatusMessage);
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser("Error starting Advertisement: " + ex->ToString(), NotifyType::ErrorMessage);
    }
}

void SDKTemplate::Scenario1_Advertiser::btnAddIe_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (!_publisher)
    {
        _publisher = ref new WiFiDirectAdvertisementPublisher();
    }

    if (txtInformationElement->Text == "")
    {
        rootPage->NotifyUser("Please specifiy an IE", NotifyType::ErrorMessage);
        return;
    }

    WiFiDirectInformationElement^ IE = ref new WiFiDirectInformationElement();

    // IE blob
    DataWriter^ dataWriter = ref new DataWriter();
    dataWriter->UnicodeEncoding = UnicodeEncoding::Utf8;
    dataWriter->ByteOrder = ByteOrder::LittleEndian;
    dataWriter->WriteUInt32(dataWriter->MeasureString(txtInformationElement->Text));
    dataWriter->WriteString(txtInformationElement->Text);
    IE->Value = dataWriter->DetachBuffer();

    // OUI
    DataWriter^ dataWriterOUI = ref new DataWriter();
    dataWriterOUI->WriteBytes(CustomOui);
    IE->Oui = dataWriterOUI->DetachBuffer();

    // OUI Type
    IE->OuiType = CustomOuiType;

    _publisher->Advertisement->InformationElements->Append(IE);
    txtInformationElement->Text = "";

    rootPage->NotifyUser("IE added successfully", NotifyType::StatusMessage);
}

void SDKTemplate::Scenario1_Advertiser::btnStopAdvertisement_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        if (_publisher)
        {
            _publisher->Stop();
            _publisher->StatusChanged -= _statusChangedToken;
            rootPage->NotifyUser("Advertisement stopped successfully", NotifyType::StatusMessage);
        }

        if (_listener)
        {
            _listener->ConnectionRequested -= _connectionRequestedToken;
        }
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser("Error stopping Advertisement: " + ex->ToString(), NotifyType::ErrorMessage);
    }
}

void SDKTemplate::Scenario1_Advertiser::OnConnectionRequested(WiFiDirectConnectionListener^ sender, WiFiDirectConnectionRequestedEventArgs^ connectionEventArgs)
{
    try
    {
        WiFiDirectConnectionRequest^ connectionRequest = connectionEventArgs->GetConnectionRequest();

        auto messageDialog = ref new MessageDialog("Connection request received from " + connectionRequest->DeviceInformation->Name, "Connection Request");

        messageDialog->Commands->Append(ref new UICommand(
            "Accept",
            ref new UICommandInvokedHandler([this](Windows::UI::Popups::IUICommand^ command)
        {
        })));

        messageDialog->Commands->Append(ref new UICommand(
            "Decline",
            ref new UICommandInvokedHandler([](Windows::UI::Popups::IUICommand^ command)
        {
        })));

        messageDialog->DefaultCommandIndex = 1;
        messageDialog->CancelCommandIndex = 1;

        concurrency::task<IUICommand^> promptTask(messageDialog->ShowAsync());
        promptTask.then([this, connectionRequest](concurrency::task<IUICommand^> resultTask)
        {
            IUICommand^ command = resultTask.get();
            if (command->Label == "Accept")
            {
                rootPage->NotifyUser("Connecting to " + connectionRequest->DeviceInformation->Name + "...", NotifyType::StatusMessage);

                WiFiDirectConnectionParameters^ connectionParams = ref new WiFiDirectConnectionParameters();
                connectionParams->GroupOwnerIntent = (short)(wcstoul(txtGOIntent->Text->Data(), nullptr, 10));

                try
                {
                    concurrency::task<WiFiDirectDevice^> fromIdTask(WiFiDirectDevice::FromIdAsync(connectionRequest->DeviceInformation->Id, connectionParams));
                    fromIdTask.then([this](concurrency::task<WiFiDirectDevice^> fromIdResultTask)
                    {
                        WiFiDirectDevice^ wfdDevice = fromIdResultTask.get();

                        // Register for the ConnectionStatusChanged event handler
                        wfdDevice->ConnectionStatusChanged += ref new TypedEventHandler<WiFiDirectDevice^, Platform::Object^>(this, &Scenario1_Advertiser::OnConnectionStatusChanged, CallbackContext::Same);

                        ConnectedDevice^ connectedDevice = ref new ConnectedDevice("Waiting for client to connect...", wfdDevice, nullptr);
                        _connectedDevices->Append(connectedDevice);

                        auto endpointPairs = wfdDevice->GetConnectionEndpointPairs();

                        _listenerSocket = nullptr;
                        _listenerSocket = ref new StreamSocketListener();
                        _listenerSocket->ConnectionReceived += ref new TypedEventHandler<StreamSocketListener^, StreamSocketListenerConnectionReceivedEventArgs^>
                            (this, &Scenario1_Advertiser::OnSocketConnectionReceived, CallbackContext::Same);

                        concurrency::task<void> listenerTask(_listenerSocket->BindEndpointAsync(endpointPairs->GetAt(0)->LocalHostName, strServerPort));
                        listenerTask.then([this, endpointPairs](concurrency::task<void>)
                        {
                            rootPage->NotifyUserFromBackground("Devices connected on L2, listening on IP Address: " +
                                                                endpointPairs->GetAt(0)->LocalHostName->ToString() +
                                                                " Port: " + strServerPort, NotifyType::StatusMessage);
                        });
                    });
                }
                catch (Exception^ ex)
                {
                    rootPage->NotifyUserFromBackground("Connect operation threw an exception: " + ex->Message, NotifyType::ErrorMessage);
                }
            }
            else
            {
                // Decline the connection request
                rootPage->NotifyUserFromBackground("Connection request from " + connectionRequest->DeviceInformation->Name + " was declined", NotifyType::ErrorMessage);

                delete connectionRequest;
            }
        });
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUserFromBackground("Connect operation threw an exception: " + ex->Message, NotifyType::ErrorMessage);
    }
}

void SDKTemplate::Scenario1_Advertiser::OnStatusChanged(WiFiDirectAdvertisementPublisher^ sender, WiFiDirectAdvertisementPublisherStatusChangedEventArgs^ statusEventArgs)
{
    rootPage->NotifyUserFromBackground("Advertisement: Status: " + statusEventArgs->Status.ToString() + " Error: " + statusEventArgs->Error.ToString(), NotifyType::StatusMessage);
    return;
}

void SDKTemplate::Scenario1_Advertiser::OnSocketConnectionReceived(StreamSocketListener^ sender, StreamSocketListenerConnectionReceivedEventArgs^ connectionReceivedEventArgs)
{
    rootPage->NotifyUserFromBackground("Connecting to remote side on L4 layer...", NotifyType::StatusMessage);
    StreamSocket^ serverSocket = connectionReceivedEventArgs->Socket;

    try
    {
        SocketReaderWriter^ socketRW = ref new SocketReaderWriter(serverSocket, rootPage);
        socketRW->ReadMessage();

        while (true)
        {
            String^ sessionId = socketRW->GetCurrentMessage();
            if (sessionId)
            {
                rootPage->NotifyUserFromBackground("Connected with remote side on L4 layer", NotifyType::StatusMessage);

                for (UINT idx = 0; idx < _connectedDevices->Size; idx++)
                {
                    ConnectedDevice^ connectedDevice = _connectedDevices->GetAt(idx);
                    if (connectedDevice->DisplayName->Equals("Waiting for client to connect..."))
                    {
                        _connectedDevices->RemoveAt(idx);

                        _connectedDevices->SetAt(idx, ref new ConnectedDevice(sessionId, connectedDevice->WfdDevice, connectedDevice->SocketRW));
                        _connectedDevices->Append(connectedDevice);
                        break;
                    }
                }
                break;
            }

            Sleep(100);
        }
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser("Connection failed: " + ex->Message, NotifyType::ErrorMessage);
    }
}


void SDKTemplate::Scenario1_Advertiser::OnConnectionStatusChanged(WiFiDirectDevice^ sender, Object^ arg)
{
    rootPage->NotifyUserFromBackground("Connection status changed: " + sender->ConnectionStatus.ToString(), NotifyType::StatusMessage);
}

void SDKTemplate::Scenario1_Advertiser::btnSendMessage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (lvConnectedDevices->SelectedItems->Size == 0)
    {
        rootPage->NotifyUser("Please select a Session to send data", NotifyType::ErrorMessage);
        return;
    }

    if (txtSendMessage->Text == "")
    {
        rootPage->NotifyUser("Please type a message to send", NotifyType::ErrorMessage);
        return;
    }

    try
    {
        for (UINT idx = 0; idx < lvConnectedDevices->SelectedItems->Size; idx++)
        {
            ConnectedDevice^ connectedDevice = (ConnectedDevice^)(lvConnectedDevices->SelectedItems->GetAt(idx));

            connectedDevice->SocketRW->WriteMessage(txtSendMessage->Text);
        }
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser("Send threw an exception: " + ex->Message, NotifyType::ErrorMessage);
    }

}

void SDKTemplate::Scenario1_Advertiser::btnClose_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (lvConnectedDevices->SelectedItems->Size == 0)
    {
        rootPage->NotifyUser("Please select a device to close", NotifyType::ErrorMessage);
        return;
    }

    try
    {
        for (UINT idx = 0; idx < lvConnectedDevices->SelectedItems->Size; idx++)
        {
            ConnectedDevice^ connectedDevice = (ConnectedDevice^)(lvConnectedDevices->SelectedItems->GetAt(idx));

            // Close socket
            connectedDevice->SocketRW->Close();

            // Close WiFiDirectDevice object
            delete connectedDevice->WfdDevice;

            UINT index = 0;
            if (_connectedDevices->IndexOf(connectedDevice, &index))
            {
                _connectedDevices->RemoveAt(index);
            }

            rootPage->NotifyUser(connectedDevice->DisplayName + " closed successfully", NotifyType::StatusMessage);
        }
    }
    catch (Exception^ ex)
    {
        rootPage->NotifyUser("Close threw an exception: " + ex->Message, NotifyType::ErrorMessage);
    }
}
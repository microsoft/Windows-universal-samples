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
#include "Scenario2_Connector.xaml.h"

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
using namespace Windows::Devices::Enumeration;
using namespace Platform::Collections;
using namespace Windows::UI::Core;
using namespace Windows::Storage::Streams;
using namespace Windows::Networking::Sockets;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::ApplicationModel::Resources::Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario2_Connector::Scenario2_Connector() : rootPage(MainPage::Current)
{
    InitializeComponent();

    _discoveredDevices = ref new Vector<DiscoveredDevice^>();
    lvDiscoveredDevices->ItemsSource = _discoveredDevices;
    lvDiscoveredDevices->SelectionMode = ListViewSelectionMode::Single;

    _connectedDevices = ref new Vector<ConnectedDevice^>();
    lvConnectedDevices->ItemsSource = _connectedDevices;
    lvConnectedDevices->SelectionMode = ListViewSelectionMode::Single;
}

void SDKTemplate::Scenario2_Connector::btnWatcher_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_fWatcherStarted == false)
    {
        btnWatcher->Content = "Stop Watcher";
        _fWatcherStarted = true;

        _discoveredDevices->Clear();
        rootPage->NotifyUser("Finding Devices...", NotifyType::StatusMessage);

        _deviceWatcher = nullptr;
        String^ deviceSelector = WiFiDirectDevice::GetDeviceSelector((cmbDeviceSelector->SelectedItem->ToString()->Equals("Device Interface") == true) ?
            WiFiDirectDeviceSelectorType::DeviceInterface : WiFiDirectDeviceSelectorType::AssociationEndpoint);

        Vector<String^>^ requestedProperties = ref new Vector<String^>();
        requestedProperties->Append("System.Devices.WiFiDirect.InformationElements");

        _deviceWatcher = DeviceInformation::CreateWatcher(deviceSelector, requestedProperties);

        _deviceAddedToken = _deviceWatcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(this,
            &Scenario2_Connector::OnDeviceAdded, CallbackContext::Same);

        _deviceRemovedToken = _deviceWatcher->Removed += ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(this,
            &Scenario2_Connector::OnDeviceRemoved, CallbackContext::Same);

        _deviceUpdatedToken = _deviceWatcher->Updated += ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(this,
            &Scenario2_Connector::OnDeviceUpdated, CallbackContext::Same);

        _deviceWatcherEnumerationCompletedToken = _deviceWatcher->EnumerationCompleted += ref new TypedEventHandler<DeviceWatcher^, Object^>(this,
            &Scenario2_Connector::OnEnumerationCompleted, CallbackContext::Same);

        _deviceWatcherStoppedToken = _deviceWatcher->Stopped += ref new TypedEventHandler<DeviceWatcher^, Object^>(this,
            &Scenario2_Connector::OnStopped, CallbackContext::Same);

        _deviceWatcher->Start();
    }
    else
    {
        btnWatcher->Content = "Start Watcher";
        _fWatcherStarted = false;

        _deviceWatcher->Added -= _deviceAddedToken;
        _deviceWatcher->Removed -= _deviceRemovedToken;
        _deviceWatcher->Updated -= _deviceUpdatedToken;
        _deviceWatcher->EnumerationCompleted -= _deviceWatcherEnumerationCompletedToken;
        _deviceWatcher->Stopped -= _deviceWatcherStoppedToken;

        _deviceWatcher->Stop();
    }
}

void SDKTemplate::Scenario2_Connector::OnDeviceAdded(DeviceWatcher^ deviceWatcher, DeviceInformation^ deviceInfo)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, deviceInfo]()
    {
        std::lock_guard<std::mutex> lock(_discoveryMutex);

        _discoveredDevices->Append(ref new DiscoveredDevice(deviceInfo));
    }));
}

void SDKTemplate::Scenario2_Connector::OnDeviceRemoved(DeviceWatcher^ deviceWatcher, DeviceInformationUpdate^ deviceInfoUpdate)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, deviceInfoUpdate]()
    {
        std::lock_guard<std::mutex> lock(_discoveryMutex);

        for (UINT idx = 0; idx < _discoveredDevices->Size; idx++)
        {
            DiscoveredDevice^ discoveredDevice = (DiscoveredDevice^)(_discoveredDevices->GetAt(idx));

            if (discoveredDevice->DeviceInfo->Id->ToString()->Equals(deviceInfoUpdate->Id))
            {
                _discoveredDevices->RemoveAt(idx);
                break;
            }
        }
    }));
}

void SDKTemplate::Scenario2_Connector::OnDeviceUpdated(DeviceWatcher^ deviceWatcher, DeviceInformationUpdate^ deviceInfoUpdate)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, deviceInfoUpdate]()
    {
        std::lock_guard<std::mutex> lock(_discoveryMutex);

        for (UINT idx = 0; idx < _discoveredDevices->Size; idx++)
        {
            DiscoveredDevice^ discoveredDevice = (DiscoveredDevice^)(_discoveredDevices->GetAt(idx));

            if (discoveredDevice->DeviceInfo->Id->ToString()->Equals(deviceInfoUpdate->Id))
            {
                discoveredDevice->DeviceInfo->Update(deviceInfoUpdate);
                _discoveredDevices->SetAt(idx, discoveredDevice);
                break;
            }
        }
    }));
}

void SDKTemplate::Scenario2_Connector::OnEnumerationCompleted(DeviceWatcher^ deviceWatcher, Object^ o)
{
    rootPage->NotifyUserFromBackground("DeviceWatcher enumeration completed", NotifyType::StatusMessage);
}

void SDKTemplate::Scenario2_Connector::OnStopped(DeviceWatcher^ deviceWatcher, Object^ o)
{
    rootPage->NotifyUserFromBackground("DeviceWatcher stopped", NotifyType::StatusMessage);
}

void SDKTemplate::Scenario2_Connector::btnIe_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (lvDiscoveredDevices->SelectedItems->Size == 0)
    {
        rootPage->NotifyUser("No device selected, please select one", NotifyType::ErrorMessage);
        return;
    }

    auto selectedDevices = lvDiscoveredDevices->SelectedItems;

    for (UINT idx = 0; idx < selectedDevices->Size; idx++)
    {
        try
        {
            ListViewItem^ lviDiscoveredDevice = (ListViewItem^)(selectedDevices->GetAt(idx));
            auto iECollection = WiFiDirectInformationElement::CreateFromDeviceInformation((DeviceInformation^)lviDiscoveredDevice->Tag);

            if (iECollection->Size == 0)
            {
                rootPage->NotifyUser("No Information Elements found", NotifyType::StatusMessage);
                return;
            }

            String^ strIeOutput = "";
            String^ strIe = "N/A";

            for (UINT idx = 0; idx < iECollection->Size; idx++)
            {
                WiFiDirectInformationElement^ ie = iECollection->GetAt(idx);

                if (ie->Oui->Equals(MsftOui))
                {
                    strIeOutput->Concat(strIeOutput, "Microsoft IE: ");
                }
                else if (ie->Oui->Equals(WfaOui))
                {
                    strIeOutput->Concat(strIeOutput, "WFA IE: ");
                }
                else if (ie->Oui->Equals(CustomOui))
                {
                    strIeOutput->Concat(strIeOutput, "Custom IE: ");

                    DataReader^ dataReader = DataReader::FromBuffer(ie->Value);
                    dataReader->UnicodeEncoding = UnicodeEncoding::Utf8;
                    dataReader->ByteOrder = ByteOrder::LittleEndian;

                    strIe = dataReader->ReadString(dataReader->ReadUInt32());
                }
                else
                {
                    strIeOutput->Concat(strIeOutput, "Unknown IE: ");
                }

                strIeOutput += "OUI Type: " + ie->OuiType.ToString() + " OUI: " + ie->Oui->ToString() + " IE Data: " + strIe;
            }

            rootPage->NotifyUser(strIeOutput, NotifyType::StatusMessage);
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser("No Information element found: " + ex->Message, NotifyType::ErrorMessage);
        }
    }
}


void SDKTemplate::Scenario2_Connector::btnFromId_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (lvDiscoveredDevices->SelectedItems->Size == 0)
    {
        rootPage->NotifyUser("No device selected, please select atleast one.", NotifyType::ErrorMessage);
        return;
    }

    auto selectedDevices = lvDiscoveredDevices->SelectedItems;

    for (UINT idx = 0; idx < selectedDevices->Size; idx++)
    {
        DiscoveredDevice^ discoveredDevice = (DiscoveredDevice^)selectedDevices->GetAt(idx);
        rootPage->NotifyUser("Connecting to " + discoveredDevice->DisplayName + "...", NotifyType::StatusMessage);

        try
        {
            WiFiDirectConnectionParameters^ connectionParams = ref new WiFiDirectConnectionParameters();
            connectionParams->GroupOwnerIntent = (short)(wcstoul(txtGOIntent->Text->Data(), nullptr, 10));

            // IMPORTANT: FromIdAsync needs to be called from the UI thread
            concurrency::task<WiFiDirectDevice^> fromIdTask(WiFiDirectDevice::FromIdAsync(discoveredDevice->DeviceInfo->Id, connectionParams));
            fromIdTask.then([this](concurrency::task<WiFiDirectDevice^> fromIdResultTask)
            {
                try
                {
                    WiFiDirectDevice^ wfdDevice = fromIdResultTask.get();

                    // Register for the ConnectionStatusChanged event handler
                    wfdDevice->ConnectionStatusChanged += ref new TypedEventHandler<WiFiDirectDevice^, Platform::Object^>(this, &Scenario2_Connector::OnConnectionStatusChanged, CallbackContext::Same);

                    auto endpointPairs = wfdDevice->GetConnectionEndpointPairs();

                    rootPage->NotifyUser("Devices connected on L2 layer, connecting to IP Address: " + endpointPairs->GetAt(0)->RemoteHostName +
                        " Port: " + strServerPort, NotifyType::StatusMessage);

                    // Wait for server to start listening on a socket
                    Sleep(5000);

                    // Connect to Advertiser on L4 layer
                    StreamSocket^ clientSocket = ref new StreamSocket();

                    concurrency::task<void> listenerTask(clientSocket->ConnectAsync(endpointPairs->GetAt(0)->RemoteHostName, strServerPort));
                    listenerTask.then([this, endpointPairs, wfdDevice, clientSocket](concurrency::task<void>)
                    {
                        SocketReaderWriter^ socketRW = ref new SocketReaderWriter(clientSocket, rootPage);

                        String^ sessionId = "Session: " + rand().ToString();

                        _connectedDevices->Append(ref new ConnectedDevice(sessionId, wfdDevice, socketRW));

                        socketRW->ReadMessage();
                        socketRW->WriteMessage(sessionId);

                        rootPage->NotifyUserFromBackground("Connected with remote side on L4 layer", NotifyType::StatusMessage);
                    });
                }
                catch (Exception^ ex)
                {
                    rootPage->NotifyUserFromBackground("FromId operation threw an exception: " + ex->Message, NotifyType::ErrorMessage);
                }
            });
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUserFromBackground("Connect operation threw an exception: " + ex->Message, NotifyType::ErrorMessage);
        }
    }
}

void SDKTemplate::Scenario2_Connector::OnConnectionStatusChanged(WiFiDirectDevice^ sender, Object^ arg)
{
    rootPage->NotifyUserFromBackground("Connection status changed: " + sender->ConnectionStatus.ToString(), NotifyType::StatusMessage);
}

void SDKTemplate::Scenario2_Connector::btnSendMessage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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

void SDKTemplate::Scenario2_Connector::btnClose_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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
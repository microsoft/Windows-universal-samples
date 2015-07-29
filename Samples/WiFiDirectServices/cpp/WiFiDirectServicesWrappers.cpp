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
#include "WiFiDirectServicesWrappers.h"
#include <iomanip>

using namespace SDKTemplate::WiFiDirectServices;
using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::WiFiDirect::Services;
using namespace Windows::Foundation;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::Security::Cryptography;
using namespace Windows::UI::Core;
using namespace std;

int AdvertisementWrapper::_nextId = 1;

AdvertisementWrapper::AdvertisementWrapper(
    WiFiDirectServiceAdvertiser ^ advertiser,
    WiFiDirectServiceManager ^ manager,
    String ^ pin
    ) : _advertiser(advertiser),
        _manager(manager),
        _pin(pin),
        _uniqueInternalId(_nextId++)
{
    _advertisementStatusChangedToken = _advertiser->AdvertisementStatusChanged += ref new TypedEventHandler<WiFiDirectServiceAdvertiser^, Object^>(
        this,
        &AdvertisementWrapper::OnAdvertisementStatusChanged
        );
    _autoAcceptSessionConnectedToken = _advertiser->AutoAcceptSessionConnected += ref new TypedEventHandler<WiFiDirectServiceAdvertiser^, WiFiDirectServiceAutoAcceptSessionConnectedEventArgs^>(
        this,
        &AdvertisementWrapper::OnAutoAcceptSessionConnected
        );
    _sessionRequestedToken = _advertiser->SessionRequested += ref new TypedEventHandler<WiFiDirectServiceAdvertiser^, WiFiDirectServiceSessionRequestedEventArgs^>(
        this,
        &AdvertisementWrapper::OnSessionRequested
        );
}

AdvertisementWrapper::~AdvertisementWrapper()
{
    // Stop advertising
    // If for some reason the advertiser is stopping between the time that we check
    // the AdvertisementStatus and when we call Stop, this will throw
    try
    {
        if (_advertiser->AdvertisementStatus == WiFiDirectServiceAdvertisementStatus::Started)
        {
            _advertiser->Stop();
        }
    }
    catch (Exception^)
    {
        // Stop can throw if it is already stopped or stopping, ignore
    }

    // Remove event handlers
    _advertiser->AdvertisementStatusChanged -= _advertisementStatusChangedToken;
    _advertiser->AutoAcceptSessionConnected -= _autoAcceptSessionConnectedToken;
    _advertiser->SessionRequested -= _sessionRequestedToken;

    _requestList->Clear();
}

void AdvertisementWrapper::AcceptSessionRequest(Platform::String ^ id, Platform::String ^ pin)
{
    WiFiDirectServiceSessionRequest^ request = nullptr;
    bool found = false;
    unsigned int index = 0;

    {
        lock_guard<mutex> lock(_advertiserMutex);
        for (auto&& r : _requestList)
        {
            if (r->DeviceInformation->Id == id)
            {
                found = true;
                request = r;
                break;
            }
            ++index;
        }

        if (!found)
        {
            throw ref new OutOfBoundsException("Attempted to accept session not found in list");
        }

        _requestList->RemoveAt(index);
    }
    _manager->RemoveSessionRequest(request, this);

    task<WiFiDirectServiceSession^> sessionTask;
    String^ pinForConnect = pin;

    // Determine if a PIN is needed
    if (request->ProvisioningInfo->IsGroupFormationNeeded &&
        request->ProvisioningInfo->SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod::PinDisplay)
    {
        // This means the advertiser should display a PIN and the seeker will look at the display and enter the same PIN
        // The PIN may be randomly generated now, but for this sample, we configure a PIN when starting the advertiser
        pinForConnect = _pin;
    }
    else if (request->ProvisioningInfo->IsGroupFormationNeeded &&
        request->ProvisioningInfo->SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod::PinEntry)
    {
        // This means that the seeker should display a PIN and the advertiser will need to look at that display and enter the same PIN
        if (pin->Length() == 0)
        {
            throw ref new InvalidArgumentException("Expected PIN for connection, not provided");
        }
        pinForConnect = pin;
    }
    else
    {
        // PIN not needed, do not pass into connect
        pinForConnect = "";
    }

    _manager->NotifyUser("Accepting session request...", NotifyType::StatusMessage);

    if (pinForConnect->Length() > 0)
    {
        // NOTE: ConnectAsync MUST be called from the UI thread
        sessionTask = create_task(_advertiser->ConnectAsync(request->DeviceInformation, pinForConnect), task_continuation_context::use_current());
    }
    else
    {
        // NOTE: ConnectAsync MUST be called from the UI thread
        sessionTask = create_task(_advertiser->ConnectAsync(request->DeviceInformation), task_continuation_context::use_current());
    }

    sessionTask.then( [this](WiFiDirectServiceSession^ session)
    {
        _manager->NotifyUser("Session request accepted", NotifyType::StatusMessage);

        SessionWrapper^ sessionWrapper = ref new SessionWrapper(session, _manager);
        _manager->AddSession(sessionWrapper);
    }, task_continuation_context::use_current()).then( [this](task<void> finalTask)
    {
        try
        {
            // Rethrow exceptions from async task in case ConnectAsync failed
            finalTask.get();
        }
        catch (Exception^ ex)
        {
            _manager->NotifyUser("ConnectAsync Failed: " + ex->Message, NotifyType::ErrorMessage);
        }
    });
}

void AdvertisementWrapper::DeclineSessionRequest(Platform::String ^ id)
{
    WiFiDirectServiceSessionRequest^ request = nullptr;
    bool found = false;
    unsigned int index = 0;

    {
        lock_guard<mutex> lock(_advertiserMutex);
        for (auto&& r : _requestList)
        {
            if (r->DeviceInformation->Id == id)
            {
                found = true;
                request = r;
                break;
            }
            ++index;
        }

        if (!found)
        {
            throw ref new OutOfBoundsException("Attempted to accept session not found in list");
        }

        _requestList->RemoveAt(index);
    }
    _manager->RemoveSessionRequest(request, this);

    // Decline incoming requests by deleting the request
    delete request;
    _manager->NotifyUser("Declined Request", NotifyType::StatusMessage);
}

void AdvertisementWrapper::OnSessionRequested(
    WiFiDirectServiceAdvertiser^ sender,
    WiFiDirectServiceSessionRequestedEventArgs^ args
    )
{
    try
    {
        _manager->NotifyUser("Received session request", NotifyType::StatusMessage);

        String^ sessionInfo = "";
        if (args->GetSessionRequest()->SessionInfo != nullptr && args->GetSessionRequest()->SessionInfo->Length > 0)
        {
            DataReader^ sessionInfoDataReader = DataReader::FromBuffer(args->GetSessionRequest()->SessionInfo);
            sessionInfo = sessionInfoDataReader->ReadString(args->GetSessionRequest()->SessionInfo->Length);
            _manager->NotifyUser("Received Session Info: " + sessionInfo, NotifyType::StatusMessage);
        }

        {
            lock_guard<mutex> lock(_advertiserMutex);
            _requestList->Append(args->GetSessionRequest());
        }

        // Now we update the UI and wait for action to take (accept or decline)
        _manager->AddSessionRequest(args->GetSessionRequest(), this);

        if (args->GetSessionRequest()->ProvisioningInfo->IsGroupFormationNeeded &&
            args->GetSessionRequest()->ProvisioningInfo->SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod::PinDisplay)
        {
            // This means the advertiser should display a PIN and the seeker will look at the display and enter the same PIN
            // The PIN may be randomly generated now, but for this sample, we configure a PIN when starting the advertiser
            _manager->NotifyUser("Remote side should enter PIN: " + _pin, NotifyType::StatusMessage);
        }
    }
    catch (Exception^ ex)
    {
        _manager->NotifyUser("OnSessionRequest Failed: " + ex->Message, NotifyType::ErrorMessage);
    }
}

void AdvertisementWrapper::OnAutoAcceptSessionConnected(
    WiFiDirectServiceAdvertiser^ sender,
    WiFiDirectServiceAutoAcceptSessionConnectedEventArgs^ args
    )
{
    try
    {
        String^ sessionInfo = "";
        if (args->SessionInfo != nullptr && args->SessionInfo->Length > 0)
        {
            DataReader^ sessionInfoDataReader = DataReader::FromBuffer(args->SessionInfo);
            sessionInfo = sessionInfoDataReader->ReadString(args->SessionInfo->Length);
            _manager->NotifyUser("Received Session Info: " + sessionInfo, NotifyType::StatusMessage);
        }

        _manager->NotifyUser("Auto-Accept Session Connected: sessionInfo=" + sessionInfo, NotifyType::StatusMessage);

        SessionWrapper^ sessionWrapper = ref new SessionWrapper(args->Session, _manager);
        _manager->AddSession(sessionWrapper);
    }
    catch (Exception^ ex)
    {
        _manager->NotifyUser("OnAutoAcceptSessionConnected Failed: " + ex->Message, NotifyType::ErrorMessage);
    }
}

void AdvertisementWrapper::OnAdvertisementStatusChanged(
    WiFiDirectServiceAdvertiser^ sender,
    Object^ args
    )
{
    try
    {
        WiFiDirectServiceAdvertisementStatus status = _advertiser->AdvertisementStatus;

        _manager->NotifyUser("Advertisement Status Changed to " + status.ToString(), NotifyType::StatusMessage);

        _manager->AdvertiserStatusChanged(this);
    }
    catch (Exception^ ex)
    {
        _manager->NotifyUser("OnAdvertisementStatusChanged Failed: " + ex->Message, NotifyType::ErrorMessage);
    }
}

DiscoveredDeviceWrapper::DiscoveredDeviceWrapper(
    DeviceInformation^ deviceInfo,
    WiFiDirectServiceManager^ manager
    ) : _deviceInfo(deviceInfo),
        _manager(manager)
{
    // Read properties from the DeviceInfo
    ParseProperties();
}

SDKTemplate::WiFiDirectServices::DiscoveredDeviceWrapper::~DiscoveredDeviceWrapper()
{
    if (_service != nullptr)
    {
        _service->SessionDeferred -= _sessionDeferredToken;
    }
}

task<void> DiscoveredDeviceWrapper::OpenSessionAsync()
{
    // This wrapper can only be used for FromIdAsync once
    if (_service != nullptr)
    {
        throw ref new ChangedStateException("Attempted to connect to discovered service a second time!");
    }

    _manager->NotifyUser("Open session...", NotifyType::StatusMessage);

    // NOTE: FromIdAsync MUST be called from the UI thread
    return create_task(WiFiDirectService::FromIdAsync(_deviceInfo->Id)).then( [this](WiFiDirectService^ service)
    {
        _service = service;
        _sessionDeferredToken = _service->SessionDeferred += ref new TypedEventHandler<WiFiDirectService^, WiFiDirectServiceSessionDeferredEventArgs^>(
            this,
            &DiscoveredDeviceWrapper::OnSessionDeferred
            );
    });
}

task<void> DiscoveredDeviceWrapper::SetServiceOptionsAsync(bool preferGO, Platform::String ^ sessionInfo)
{
    // Must call FromIdAsync first
    auto task = create_task([]() { return; });
    if (_service == nullptr)
    {
        task = OpenSessionAsync();
    }

    return task.then( [this, preferGO, sessionInfo]()
    {
        _service->PreferGroupOwnerMode = preferGO;

        if (sessionInfo != "")
        {
            auto sessionInfoDataWriter = ref new DataWriter(ref new InMemoryRandomAccessStream());
            sessionInfoDataWriter->WriteString(sessionInfo);
            _service->SessionInfo = sessionInfoDataWriter->DetachBuffer();
        }
        else
        {
            _service->SessionInfo = nullptr;
        }
    });
}

task<WiFiDirectServiceProvisioningInfo^> DiscoveredDeviceWrapper::GetProvisioningInfoAsync(WiFiDirectServiceConfigurationMethod configMethod)
{
    // Must call FromIdAsync first
    auto task = create_task([]() { return; });
    if (_service == nullptr)
    {
        task = OpenSessionAsync();
    }

    return task.then([this, configMethod]()
    {
        _manager->NotifyUser("Get provisioning info...", NotifyType::StatusMessage);

        return _service->GetProvisioningInfoAsync(configMethod);
    }).then( [this](WiFiDirectServiceProvisioningInfo^ provisioningInfo)
    {
        // Take some action based on provisioning info, such as displaying the PIN
        if (provisioningInfo->IsGroupFormationNeeded)
        {
            if (provisioningInfo->SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod::PinDisplay)
            {
                // UI should display the PIN and pass it into the call to Connect
                _manager->NotifyUser("Group Formation is needed, should DISPLAY PIN", NotifyType::StatusMessage);
            }
            else if (provisioningInfo->SelectedConfigurationMethod == WiFiDirectServiceConfigurationMethod::PinEntry)
            {
                // UI should take PIN as input and pass it into the call to Connect
                _manager->NotifyUser("Group Formation is needed, should ENTER PIN", NotifyType::StatusMessage);
            }
            else
            {
                _manager->NotifyUser("Group Formation is needed, no PIN required", NotifyType::StatusMessage);
            }
        }
        else
        {
            _manager->NotifyUser("Group Formation is not needed", NotifyType::StatusMessage);
        }
        return provisioningInfo;
    });
}

task<void> SDKTemplate::WiFiDirectServices::DiscoveredDeviceWrapper::ConnectAsync(Platform::String ^ pin)
{
    // Must call FromIdAsync first
    auto task = create_task([]() { return; });
    if (_service == nullptr)
    {
        task = OpenSessionAsync();
    }

    return task.then([this, pin]()
    {
        _manager->NotifyUser("Connecting...", NotifyType::StatusMessage);

        IAsyncOperation<WiFiDirectServiceSession^>^ connectOperation;
        if (pin->Length() > 0)
        {
            connectOperation = _service->ConnectAsync(pin);
        }
        else
        {
            connectOperation = _service->ConnectAsync();
        }

        return create_task(connectOperation).then([this](WiFiDirectServiceSession^ session)
        {
            _manager->NotifyUser("Done Connecting", NotifyType::StatusMessage);

            // Now we are done with this WiFiDirectService instance
            // Clear state so a new connection can be started
            _service->SessionDeferred -= _sessionDeferredToken;
            _service = nullptr;

            SessionWrapper^ sessionWrapper = ref new SessionWrapper(session, _manager);
            _manager->AddSession(sessionWrapper);
        });
    });
}

void DiscoveredDeviceWrapper::OnSessionDeferred(WiFiDirectService^ sender, WiFiDirectServiceSessionDeferredEventArgs^ args)
{
    String^ deferredSessionInfo = "";
    if (args->DeferredSessionInfo != nullptr && args->DeferredSessionInfo->Length > 0)
    {
        DataReader^ sessionInfoDataReader = DataReader::FromBuffer(args->DeferredSessionInfo);
        deferredSessionInfo = sessionInfoDataReader->ReadString(args->DeferredSessionInfo->Length);
    }

    _manager->NotifyUser("Session Connection was deferred... (" + deferredSessionInfo + ")", NotifyType::StatusMessage);
}

void DiscoveredDeviceWrapper::ParseProperties()
{
    if (_deviceInfo->Properties->HasKey("System.Devices.WiFiDirectServices.ServiceName"))
    {
        auto serviceName = _deviceInfo->Properties->Lookup("System.Devices.WiFiDirectServices.ServiceName");
        ServiceName = serviceName->ToString();
    }
    else
    {
        ServiceName = "";
    }

    if (_deviceInfo->Properties->HasKey("System.Devices.WiFiDirectServices.AdvertisementId"))
    {
        auto advertisementId = _deviceInfo->Properties->Lookup("System.Devices.WiFiDirectServices.AdvertisementId");
        AdvertisementId = static_cast<unsigned int>(advertisementId);
    }
    else
    {
        AdvertisementId = 0;
    }
    
    if (_deviceInfo->Properties->HasKey("System.Devices.WiFiDirectServices.ServiceConfigMethods"))
    {
        auto configMethods = _deviceInfo->Properties->Lookup("System.Devices.WiFiDirectServices.ServiceConfigMethods");
        ConfigMethods = static_cast<unsigned int>(configMethods);
    }
    else
    {
        ConfigMethods = 0;
    }
    
    if (_deviceInfo->Properties->HasKey("System.Devices.WiFiDirectServices.ServiceAddress"))
    {
        auto serviceAddress = _deviceInfo->Properties->Lookup("System.Devices.WiFiDirectServices.ServiceAddress");
        auto serviceAddressProp = dynamic_cast<IPropertyValue^>(serviceAddress);

        Array<unsigned char>^ macBuffer;
        serviceAddressProp->GetUInt8Array(&macBuffer);

        // Convert from array to string of format xx:xx:xx:xx:xx:xx
        wostringstream stream;
        wstring strAddress;
        ServiceAddress = "";
        for (unsigned char b : macBuffer)
        {
            stream << hex << setfill(L'0') << setw(2) << b;
            stream << ":";
        }
        strAddress = stream.str().c_str();
        strAddress.pop_back();
        ServiceAddress = ref new String(strAddress.c_str());
    }
    else
    {
        ServiceAddress = "XX:XX:XX:XX:XX:XX";
    }

    ServiceInfo = "";
    if (_deviceInfo->Properties->HasKey("System.Devices.WiFiDirectServices.ServiceInformation"))
    {
        auto serviceInfo = _deviceInfo->Properties->Lookup("System.Devices.WiFiDirectServices.ServiceInformation");
        auto serviceInfoProp = dynamic_cast<IPropertyValue^>(serviceInfo);

        if (serviceInfoProp != nullptr)
        {
            Array<unsigned char>^ serviceInfoBuffer;
            serviceInfoProp->GetUInt8Array(&serviceInfoBuffer);

            auto sessionInfoDataWriter = ref new DataWriter(ref new InMemoryRandomAccessStream());
            sessionInfoDataWriter->WriteBytes(serviceInfoBuffer);

            DataReader^ serviceInfoDataReader = DataReader::FromBuffer(sessionInfoDataWriter->DetachBuffer());
            ServiceInfo = serviceInfoDataReader->ReadString(serviceInfoBuffer->Length);
        }
    }
}

SessionWrapper::SessionWrapper(WiFiDirectServiceSession^ session, WiFiDirectServiceManager^ manager)
    : _session(session),
      _manager(manager)
{
    _remotePortAddedToken = _session->RemotePortAdded += ref new TypedEventHandler<WiFiDirectServiceSession^, WiFiDirectServiceRemotePortAddedEventArgs^>(
        this,
        &SessionWrapper::OnRemotePortAdded
        );
    _sessionStatusChangedToken = _session->SessionStatusChanged += ref new TypedEventHandler<WiFiDirectServiceSession^, Object^>(
        this,
        &SessionWrapper::OnSessionStatusChanged
        );
}

SessionWrapper::~SessionWrapper()
{
    if (_session != nullptr)
    {
        _session->RemotePortAdded -= _remotePortAddedToken;
        _session->SessionStatusChanged -= _sessionStatusChangedToken;
    }

    _streamSocketListeners->Clear();
    _socketList->Clear();
}

void SDKTemplate::WiFiDirectServices::SessionWrapper::Close()
{
    if (_session != nullptr)
    {
        _session->RemotePortAdded -= _remotePortAddedToken;
        _session->SessionStatusChanged -= _sessionStatusChangedToken;

        _session = nullptr;
    }
}

Concurrency::task<void> SessionWrapper::AddStreamSocketListenerAsync(uint16 port)
{
    _manager->NotifyUser("Adding stream socket listener for TCP port " + port.ToString(), NotifyType::StatusMessage);

    auto endpointPairCollection = _session->GetConnectionEndpointPairs();

    // Create listener for socket connection (and add to list to cleanup later)
    StreamSocketListener^ listenerSocket = ref new StreamSocketListener();
    StreamSocketListenerWrapper^ listenerWrapper = ref new StreamSocketListenerWrapper(
        _manager,
        listenerSocket,
        this
        );
    _streamSocketListeners->Append(listenerWrapper);

    _manager->NotifyUser("BindEndpointAsync...", NotifyType::StatusMessage);
    
    return create_task(listenerSocket->BindEndpointAsync(endpointPairCollection->GetAt(0)->LocalHostName, port.ToString()))
    .then([this, listenerSocket]()
    {
        _manager->NotifyUser("BindEndpointAsync Done", NotifyType::StatusMessage);

        _manager->NotifyUser("AddStreamSocketListenerAsync...", NotifyType::StatusMessage);
        return create_task(_session->AddStreamSocketListenerAsync(listenerSocket))
        .then([this]()
        {
            _manager->NotifyUser("AddStreamSocketListenerAsync Done", NotifyType::StatusMessage);
        });
    });
}

Concurrency::task<void> SessionWrapper::AddDatagramSocketAsync(uint16 port)
{
    _manager->NotifyUser("Adding stream socket listener for UDP port " + port.ToString(), NotifyType::StatusMessage);

    auto endpointPairCollection = _session->GetConnectionEndpointPairs();

    DatagramSocket^ socket = ref new DatagramSocket();
    SocketWrapper^ socketWrapper = ref new SocketWrapper(_manager, nullptr, socket);

    _socketList->Append(socketWrapper);

    // Bind UDP socket for receiving messages (peer should call connect and send messages to this socket)
    _manager->NotifyUser("BindEndpointAsync...", NotifyType::StatusMessage);

    return create_task(socket->BindEndpointAsync(endpointPairCollection->GetAt(0)->LocalHostName, port.ToString()))
        .then([this, socket, socketWrapper]()
        {
            _manager->NotifyUser("BindEndpointAsync Done", NotifyType::StatusMessage);

            _manager->NotifyUser("AddDatagramSocketAsync...", NotifyType::StatusMessage);
            return create_task(_session->AddDatagramSocketAsync(socket))
                .then([this]()
                {
                    _manager->NotifyUser("AddDatagramSocketAsync Done", NotifyType::StatusMessage);
                }).then([this, socketWrapper]()
                {
                    // Update manager so UI can add to list
                    _manager->AddSocket(socketWrapper);
                });
        });
}

void SessionWrapper::AddStreamSocketInternal(StreamSocket ^ socket)
{
    SocketWrapper^ socketWrapper = ref new SocketWrapper(_manager, socket, nullptr);

    // Start receiving messages recursively
    socketWrapper->StartReceiveLoop();

    _socketList->Append(socketWrapper);
    // Update manager so UI can add to list
    _manager->AddSocket(socketWrapper);
}

void SessionWrapper::OnRemotePortAdded(WiFiDirectServiceSession^ sender, WiFiDirectServiceRemotePortAddedEventArgs^ args)
{
    auto endpointPairCollection = args->EndpointPairs;
    auto protocol = args->Protocol;
    if (endpointPairCollection->Size == 0)
    {
        _manager->NotifyUser("No endpoint pairs for remote port added event", NotifyType::ErrorMessage);
        return;
    }

    _manager->NotifyUser(
        (protocol == WiFiDirectServiceIPProtocol::Tcp) ? "TCP" : ((protocol == WiFiDirectServiceIPProtocol::Udp) ? "UDP" : "???") + 
        "Port " + endpointPairCollection->GetAt(0)->RemoteServiceName + "Added by remote peer",
        NotifyType::StatusMessage
        );

    task<void> socketTask;

    if (args->Protocol == WiFiDirectServiceIPProtocol::Tcp)
    {
        // Connect to the stream socket listener
        StreamSocket^ streamSocket = ref new StreamSocket();
        SocketWrapper^ socketWrapper = ref new SocketWrapper(_manager, streamSocket, nullptr);

        _manager->NotifyUser("Connecting to stream socket...", NotifyType::StatusMessage);

        socketTask = create_task(streamSocket->ConnectAsync(endpointPairCollection->GetAt(0)))
        .then([this, socketWrapper]()
        {
            // Start receiving messages recursively
            socketWrapper->StartReceiveLoop();

            _manager->NotifyUser("Stream socket connected", NotifyType::StatusMessage);

            _socketList->Append(socketWrapper);
            // Update manager so UI can add to list
            _manager->AddSocket(socketWrapper);
        });
    }
    else if (args->Protocol == WiFiDirectServiceIPProtocol::Udp)
    {
        // Connect a socket over UDP
        DatagramSocket^ datagramSocket = ref new DatagramSocket();
        SocketWrapper^ socketWrapper = ref new SocketWrapper(_manager, nullptr, datagramSocket);

        _manager->NotifyUser("Connecting to datagram socket...", NotifyType::StatusMessage);

        socketTask = create_task(datagramSocket->ConnectAsync(endpointPairCollection->GetAt(0)))
            .then([this, socketWrapper]()
            {
                _manager->NotifyUser("Datagram socket connected", NotifyType::StatusMessage);

                _socketList->Append(socketWrapper);
                // Update manager so UI can add to list
                _manager->AddSocket(socketWrapper);
            });
    }
    else
    {
        _manager->NotifyUser("Bad protocol for remote port added event", NotifyType::ErrorMessage);
        return;
    }

    socketTask.then([this](task<void> finalTask)
    {
        try
        {
            // Rethrow exceptions from async task in case ConnectAsync failed
            finalTask.get();
        }
        catch (Exception^ ex)
        {
            _manager->NotifyUser("ConnectAsync Failed: " + ex->Message, NotifyType::ErrorMessage);
        }
    });
}

void SessionWrapper::OnSessionStatusChanged(Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSession ^ sender, Platform::Object ^ args)
{
    _manager->NotifyUser("Session Status Changed to " + sender->Status.ToString(), NotifyType::StatusMessage);
}

StreamSocketListenerWrapper::StreamSocketListenerWrapper(
    WiFiDirectServiceManager^ manager,
    StreamSocketListener^ streamSocketListener,
    SessionWrapper^ session
    ) : _manager(manager),
        _streamSocketListener(streamSocketListener),
        _session(session)
{
    _connectionReceivedToken = _streamSocketListener->ConnectionReceived += ref new TypedEventHandler<StreamSocketListener^, StreamSocketListenerConnectionReceivedEventArgs^>(
        this,
        &StreamSocketListenerWrapper::OnConnectionReceived
        );
}

StreamSocketListenerWrapper::~StreamSocketListenerWrapper()
{
    _streamSocketListener->ConnectionReceived -= _connectionReceivedToken;
}

void StreamSocketListenerWrapper::OnConnectionReceived(Windows::Networking::Sockets::StreamSocketListener ^ sender, Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs ^ args)
{
    _manager->NotifyUser("Connection received for TCP Port " + sender->Information->LocalPort, NotifyType::StatusMessage);

    _session->AddStreamSocketInternal(args->Socket);
}

SocketWrapper::SocketWrapper(
    WiFiDirectServiceManager^ manager,
    StreamSocket^ streamSocket,
    DatagramSocket^ datagramSocket
    ) : _manager(manager),
        _streamSocket(streamSocket),
        _datagramSocket(datagramSocket)
{
    if (streamSocket == nullptr && datagramSocket == nullptr)
    {
        throw ref new InvalidArgumentException("Bad SocketWrapper parameters, must provide a TCP or UDP socket");
    }
    else if (streamSocket != nullptr && datagramSocket != nullptr)
    {
        throw ref new InvalidArgumentException("Bad SocketWrapper parameters, got both TCP and UDP socket, expected only one");
    }
    else if (streamSocket != nullptr)
    {
        _reader = ref new DataReader(streamSocket->InputStream);
        _writer = ref new DataWriter(streamSocket->OutputStream);
    }
    else
    {
        _datagramMessageReceivedToken = datagramSocket->MessageReceived += ref new TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>(
            this,
            &SocketWrapper::OnUDPMessageReceived
            );
        _writer = ref new DataWriter(datagramSocket->OutputStream);
    }

    if (_reader != nullptr)
    {
        _reader->UnicodeEncoding = UnicodeEncoding::Utf8;
        _reader->ByteOrder = ByteOrder::LittleEndian;
    }

    _writer->UnicodeEncoding = UnicodeEncoding::Utf8;
    _writer->ByteOrder = ByteOrder::LittleEndian;
}

SocketWrapper::~SocketWrapper()
{
    if (_datagramSocket != nullptr)
    {
        _datagramSocket->MessageReceived -= _datagramMessageReceivedToken;
    }
}

String^ SocketWrapper::Protocol::get()
{
    if (_streamSocket != nullptr)
    {
        return "TCP";
    }
    else if (_datagramSocket != nullptr)
    {
        return "UDP";
    }
    else
    {
        return "???";
    }
}

String^ SocketWrapper::Port::get()
{
    if (_streamSocket != nullptr)
    {
        return _streamSocket->Information->LocalPort;
    }
    else if (_datagramSocket != nullptr)
    {
        return _datagramSocket->Information->LocalPort;
    }
    else
    {
        return "???";
    }
}

task<void> SocketWrapper::SendMessageAsync(String ^ message)
{
    _writer->WriteUInt32(_writer->MeasureString(message));
    _writer->WriteString(message);

    return create_task(_writer->StoreAsync()).then( [this, message](unsigned int bytesSent)
    {
        _manager->NotifyUser(
            "Sent Message: \"" + message + "\", " + bytesSent +" bytes",
            NotifyType::StatusMessage
            );
    });
}

void SocketWrapper::StartReceiveLoop()
{
    create_task(_reader->LoadAsync(sizeof(UINT32))).then( [this](unsigned int size)
    {
        if (size < sizeof(UINT32))
        {
            // The underlying socket was closed before we were able to read the whole data. 
            cancel_current_task();
        }

        unsigned int messageLength = _reader->ReadUInt32();
        return create_task(_reader->LoadAsync(messageLength)).then( [this, messageLength](unsigned int actualStringLength)
        {
            if (actualStringLength != messageLength)
            {
                // The underlying socket was closed before we were able to read the whole data. 
                cancel_current_task();
            }

            // Just print a message for now
            String^ message = _reader->ReadString(messageLength);

            _manager->NotifyUser("Received Message: \"" +
                message
                + "\", " + messageLength + " bytes",
                NotifyType::StatusMessage
                );
        });
    }).then([this](task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point. 
            previousTask.get();

            // Everything went ok, so try to receive another string. The receive will continue until the stream is 
            // broken (i.e. peer closed the socket). 
            StartReceiveLoop();
        }
        catch (Exception^ ex)
        {
            _manager->NotifyUser("Message Receive Loop Failed: " + ex->Message, NotifyType::ErrorMessage);
        }
        catch (task_canceled&)
        {
            _manager->NotifyUser("Message Receive loop canceled", NotifyType::ErrorMessage);
        }
    });
}

void SocketWrapper::OnUDPMessageReceived(DatagramSocket^ sender, DatagramSocketMessageReceivedEventArgs^ args)
{
    try
    {
        DataReader^ datareader = args->GetDataReader();
        datareader->UnicodeEncoding = UnicodeEncoding::Utf8;
        datareader->ByteOrder = ByteOrder::LittleEndian;

        // Determine how long the string is.
        unsigned int messageLength = datareader->ReadUInt32();

        if (messageLength > 0)
        {
            // Decode the string.
            String^ message = datareader->ReadString(messageLength);

            // Just print a message for now
            _manager->NotifyUser(
                "Received Message: \"" +
                message
                + "\", " + messageLength + " bytes",
                NotifyType::StatusMessage
                );
        }
        else
        {
            _manager->NotifyUser("ReceiveMessage 0 bytes loaded for message content.", NotifyType::ErrorMessage);
        }
    }
    catch (Exception^ ex)
    {
        _manager->NotifyUser("HandleReceivedMessage Failed: " + ex->Message, NotifyType::ErrorMessage);
    }
}

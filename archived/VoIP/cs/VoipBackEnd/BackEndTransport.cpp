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
#include "BackEndTransport.h"
#include "BackEndNativeBuffer.h"
#include <ppltasks.h>

using namespace VoipBackEnd;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::System::Threading;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Details;
using namespace Windows::Networking::Sockets;
using namespace Windows::Networking;
using namespace Concurrency;

BackEndTransport::BackEndTransport() :
    isConnected(false),
    isListening(false),
    outputStream(nullptr),
    writer(nullptr),
    headerSize(sizeof(UINT32)*2 + sizeof(UINT64)*2),
    MaxPacketSize(50*1024)
{
    InitializeCriticalSectionEx(&lock, 0, 0);
    InitializeCriticalSectionEx(&readLock, 0, 0);
    Listen("12345");
}

void BackEndTransport::Listen(String^ localPort)
{
    OutputDebugString(L"+[BackEndTransport::Listen]\n");
    listener = ref new DatagramSocket();
    listener->MessageReceived += ref new TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>(this, &BackEndTransport::OnConnectionReceived);
    task<void>(listener->BindServiceNameAsync(localPort), this->cancelToken.get_token()).then([this] (task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();
            isListening = true;
            OutputDebugString(L"[BackEndTransport::Listen] Listening\n");
        }
        catch (Exception^ exception)
        {
            OutputDebugString(L"[BackEndTransport::Listen]");
            OutputDebugString(exception->ToString()->Data());

            SocketErrorStatus socketErrorStatus =  Windows::Networking::Sockets::SocketError::GetStatus(exception->HResult);
            switch(socketErrorStatus)
            {
            case SocketErrorStatus::AddressAlreadyInUse:
            case SocketErrorStatus::CannotAssignRequestedAddress:
                //DisconnectMessageReceived();
                break;
            default:
                throw exception;
            }
        }
    });
    OutputDebugString(L"-[BackEndTransport::Listen]\n");
}

void VoipBackEnd::BackEndTransport::Disconnect()
{
    delete listener;
    listener = nullptr;

    delete socket;
    socket = nullptr;

    isConnected = false;
}

void BackEndTransport::WriteAudio(BYTE* bytes, int byteCount)
{
    Write(bytes, byteCount, TransportMessageType::Audio, 0, 0);
}

void BackEndTransport::WriteVideo(BYTE* bytes, int byteCount, UINT64 hnsPresenationTime, UINT64 hnsSampleDuration)

{
    Write(bytes, byteCount, TransportMessageType::Video, hnsPresenationTime, hnsSampleDuration);
}

void BackEndTransport::Write(BYTE* bytes, unsigned int byteCount, TransportMessageType::Value dataType, UINT64 hnsPresentationTime, UINT64 hnsSampleDuration)
{
    
    unsigned int chunkSize = byteCount > MaxPacketSize - headerSize ? MaxPacketSize - headerSize: byteCount;
    
    if (!isConnected)
    {
        Connect("127.0.0.1", "12345");
    }

    if (isConnected)
    {
        DataWriter^ dWriter;
        
        try
        {
            EnterCriticalSection(&lock);
            dWriter = this->GetWriter();

            Array<BYTE>^ byteArray = ref new Array<BYTE>((BYTE*)bytes, chunkSize);
            
            dWriter->WriteUInt32(chunkSize);
            dWriter->WriteUInt32(dataType);
            dWriter->WriteUInt64(hnsPresentationTime);
            dWriter->WriteUInt64(hnsSampleDuration);
            dWriter->WriteBytes(byteArray);

            String^ debugStr = "[BackEndTransport::Write] Size:" + chunkSize.ToString() + 
                L" Type:" + ((UINT32)dataType).ToString() +
                " Presentation:" + hnsPresentationTime.ToString() + 
                " Duration:" + hnsSampleDuration + 
                " Unconsumed buffer:" + dWriter->UnstoredBufferLength +  "\n";

            OutputDebugString(debugStr->Data());
        }
        catch (Exception^ ex)
        {
            OutputDebugString(ex->Message->Data());
        }

        // Write the locally buffered data to the network. Please note that write operation will succeed
        // even if the server is not listening.
        task<unsigned int>(this->GetWriter()->StoreAsync()).then([this] (task<unsigned int> writeTask)
        {
            try
            {
                // Try getting an excpetion.
                writeTask.get();
                LeaveCriticalSection(&lock);
            }
            catch (Exception^ exception)
            {
            }
        });
    }
}

void BackEndTransport::OnConnectionReceived(DatagramSocket^ socket, DatagramSocketMessageReceivedEventArgs^ eventArguments)
{
    if (outputStream != nullptr)
    {
        try
        {
            EnterCriticalSection(&readLock);

            DataReader^ reader = eventArguments->GetDataReader();
            unsigned int dataSize = reader->ReadUInt32();
            if(dataSize > MaxPacketSize)
            {
                dataSize = MaxPacketSize;
            }

            unsigned int dataType = reader->ReadUInt32();
            UINT64 hnsPresentationTime = reader->ReadUInt64();
            UINT64 hnsSampleDuration = reader->ReadUInt64();
            IBuffer^ buffer = reader->ReadBuffer(dataSize);

            String^ debugStr = "[BackEndTransport::OnConnectionReceived] Size:" + dataSize.ToString() + 
                L" Type:" + dataType.ToString() + 
                " Presentation:" + hnsPresentationTime.ToString() + 
                " Duration:" + hnsSampleDuration + 
                " Unconsumed buffer:" + reader->UnconsumedBufferLength +
                " IBuffer:" + buffer->Length +  "\n";
                        
            LeaveCriticalSection(&readLock);
            OutputDebugString(debugStr->Data());

            BYTE* pBuffer = NULL;
            BYTE* pBufferCopy = NULL;
            ComPtr<NativeBuffer> spNativeBuffer = NULL;
            pBuffer = NativeBuffer::GetBytesFromIBuffer(buffer);
            pBufferCopy = new BYTE[dataSize];

            memcpy_s((void*) pBufferCopy, dataSize, (void*) pBuffer, dataSize);
            buffer = nullptr;
            pBuffer = NULL;
            // Now wrap this buffer with our NativeBuffer object
            if (FAILED(MakeAndInitialize<NativeBuffer>(&spNativeBuffer, pBufferCopy, dataSize, TRUE)))
            {
                return;
            }
            
            switch(dataType)
            {
                case TransportMessageType::Audio:
                    AudioMessageReceived(NativeBuffer::GetIBufferFromNativeBuffer(spNativeBuffer), hnsPresentationTime, hnsSampleDuration);
                    break;
                case TransportMessageType::Video:
                    VideoMessageReceived(NativeBuffer::GetIBufferFromNativeBuffer(spNativeBuffer), hnsPresentationTime, hnsSampleDuration);
                    break;
                default:
                    break;
            }
            
        }
        catch (Exception^ exception)
        {
            SocketErrorStatus socketError = SocketError::GetStatus(exception->HResult);
            if (socketError == SocketErrorStatus::ConnectionResetByPeer)
            {
                // This error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message.
            }
            else if (socketError != SocketErrorStatus::Unknown)
            {

            }
            else
            {
                throw;
            }
        }
        return;
    }

    // We do not have an output stream yet so create one.
    task<IOutputStream^>(socket->GetOutputStreamAsync(eventArguments->RemoteAddress, eventArguments->RemotePort)).then([this, socket, eventArguments] (IOutputStream^ stream)
    {
        // It might happen that the OnMessage was invoked more than once before the GetOutputStreamAsync completed.
        // In this case we will end up with multiple streams - make sure we have just one of it.
        EnterCriticalSection(&lock);

        if (outputStream == nullptr)
        {
            OutputDebugString(eventArguments->RemoteAddress->RawName->Data());
            outputStream = stream;
            hostName = eventArguments->RemoteAddress;
            port = eventArguments->RemotePort;
        }

        LeaveCriticalSection(&lock);
    }).then([this, socket, eventArguments] (task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();
        }
        catch (Exception^ exception)
        {
            OutputDebugString(exception->Message->Data());
        }
    });
    
}

void BackEndTransport::Connect(String^ hostNameStr, String^ port)
{
    if(!isConnected)
    {
        
        HostName^ hostName;
        try
        {
            hostName = ref new HostName("localhost");
        }
        catch (InvalidArgumentException^ e)
        {
            throw e;
        }

        socket = ref new DatagramSocket();
        socket->MessageReceived += ref new TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>(this, &BackEndTransport::OnMessage);
        task<void>(socket->ConnectAsync(hostName, port), this->cancelToken.get_token()).then([this] (task<void> previousTask)
        {
            try
            {
                // Try getting all exceptions from the continuation chain above this point.
                previousTask.get();
                // Mark the socket as connected.
                this->isConnected = true;
                OutputDebugString(L"[BackEndTransport::Connect] Socket is connected\n");
            }
            catch (Exception^ exception)
            {   
                OutputDebugString(L"[BackEndTransport::Connect ] Exception occured");
                
                OutputDebugString(exception->ToString()->Data());
                SocketErrorStatus socketErrorStatus =  Windows::Networking::Sockets::SocketError::GetStatus(exception->HResult);
                switch(socketErrorStatus)
                {
                    case SocketErrorStatus::HostIsDown:
                    case SocketErrorStatus::HostNotFound:
                    case SocketErrorStatus::ConnectionRefused:
                    case SocketErrorStatus::ConnectionTimedOut:
                    break;
                default:
                    throw exception;
                }
            }
            catch(task_canceled ex)
            {
            }
        });
    }
}

void BackEndTransport::OnMessage(DatagramSocket^ socket, DatagramSocketMessageReceivedEventArgs^ eventArguments)
{
    try
    {
        unsigned int stringLength = eventArguments->GetDataReader()->UnconsumedBufferLength;
        OutputDebugString(eventArguments->GetDataReader()->ReadString(stringLength)->Data());
        
    }
    catch (Exception^ exception)
    {
        SocketErrorStatus socketError = SocketError::GetStatus(exception->HResult);
        if (socketError == SocketErrorStatus::ConnectionResetByPeer)
        {
            // This error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message.
        }
        else if (socketError != SocketErrorStatus::Unknown)
        {

        }
        else
        {
            throw;
        }
    }
}

DataWriter^ BackEndTransport::GetWriter()
{
    if (writer == nullptr)
    {
        writer = ref new DataWriter(socket->OutputStream);
    }

    return writer;
}


BackEndTransport::~BackEndTransport()
{
    delete listener;
    listener = nullptr;
    
    delete socket;
    socket = nullptr;
    
    DeleteCriticalSection(&lock);
    DeleteCriticalSection(&readLock);
}

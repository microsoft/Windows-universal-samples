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

#pragma once

Platform::String^ GetCurrentTimeString();

LPBYTE PointerFromIBuffer(_In_ Windows::Storage::Streams::IBuffer^ buffer, _Out_opt_ DWORD* pcbLength);
Windows::Storage::Streams::IBuffer^ IBufferFromPointer(_In_ LPBYTE pbData, _In_ DWORD cbData);
Windows::Storage::Streams::IBuffer^ IBufferFromArray(_In_ Platform::Array<unsigned char>^ data);
std::wstring ByteArrayToString(_In_ Platform::Array<byte>^ byteArray);

HRESULT ParseApdu(_In_ BYTE* pbCommandApdu, _In_ DWORD cbCommandApdu, _Out_ USHORT* pusClaIns, _Out_ BYTE* pbP1, _Out_ BYTE* pbP2, _Out_ BYTE** ppbPayload, _Out_ DWORD* pcbPayload, _Out_ DWORD* pcbLE);

Concurrency::task<Platform::Array<byte>^> ReadAndUnprotectFileAsync(Platform::String^ filename);
Concurrency::task<Windows::Storage::Streams::IBuffer^> ReadFileToBufferAsync(Platform::String^ filename);

void AppendFile(Platform::String^ filename, Platform::String^ data);

void ChkHR(HRESULT hr);

#ifndef SAFE_DELETEARRAY
#define SAFE_DELETEARRAY(x) if ((x) != nullptr) { delete [] (x); (x) = nullptr; }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if ((x) != nullptr) { delete (x); (x) = nullptr; }
#endif

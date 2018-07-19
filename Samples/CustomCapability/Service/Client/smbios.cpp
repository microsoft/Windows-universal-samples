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
#include <intsafe.h>
#include <string>
#include "FirmwareAccess.h"

struct RawSMBIOSData
{
    BYTE    Used20CallingMethod;
    BYTE    SMBIOSMajorVersion;
    BYTE    SMBIOSMinorVersion;
    BYTE    DmiRevision;
    DWORD   Length;
    BYTE    SMBIOSTableData[ANYSIZE_ARRAY];
};

struct RawSMBIOSTable
{
    BYTE    Type;
    BYTE    Length;
    WORD    Handle;
};

struct RawSMBIOSSystemInfo : public RawSMBIOSTable
{
    BYTE    Manufacturer;
    BYTE    ProductName;
    BYTE    Version;
    BYTE    SerialNumber;
    // Ver 2.1 beyond here
    BYTE    UUID[16];
    BYTE    Wakeup_Type;
    // Ver 2.4 beyond here
    BYTE    SKUNumber;
    BYTE    Family;
};

DWORD
FindSmBiosTable(
    _In_ const RawSMBIOSData* SMBIOSData,
    _In_ BYTE Type,
    _Out_ RawSMBIOSTable** SMBIOSTable
    )
{
    DWORD error = S_OK;
    ULONG i = 0;
    RawSMBIOSTable* smbiosTable = NULL;
    bool properTermination = false;

    *SMBIOSTable = NULL;

    // Find SMBIOS Table 
    do
    {
        properTermination = false;

        // Check that the table header fits in the buffer.
        if (i + sizeof(RawSMBIOSTable) < SMBIOSData->Length)
        {

            if (SMBIOSData->SMBIOSTableData[i] == Type)
            {
                // Found table
                smbiosTable = (RawSMBIOSTable*)&SMBIOSData->SMBIOSTableData[i];
            }

            // Set i to the end of the formated section.
            i += SMBIOSData->SMBIOSTableData[i + 1];

            // Look for the end of the struct that must be terminated by \0\0
            while (i + 1 < SMBIOSData->Length)
            {
                if (0 == SMBIOSData->SMBIOSTableData[i] &&
                    0 == SMBIOSData->SMBIOSTableData[i + 1])
                {
                    properTermination = true;
                    i += 2;
                    break;
                }

                ++i;
            }
        }
    }
    while (properTermination && !smbiosTable);

    if (properTermination) {
        if (!smbiosTable) {
            // The table was not found.
            error = ERROR_FILE_NOT_FOUND;
        }
    }
    else
    {
        // A table was not double null terminated within the buffer.
        error = ERROR_INVALID_DATA;
    }

    if (ERROR_SUCCESS == error) {
        *SMBIOSTable = smbiosTable;
    }

    return error;
}

DWORD
GetSmBiosString(
    _In_ const RawSMBIOSTable* SmBiosTable,
    _In_ BYTE StringIndex,
    _Out_ PWSTR* ResultString
    )
{
    DWORD error = ERROR_SUCCESS;
    PSTR currentString = NULL;
    BYTE CurrentStringIndex = 1;
    ULONG stringLen = 0;
    ULONG bufferLen = 0;
    HRESULT hr = S_OK;

    *ResultString = NULL;

    // 0 index implies the empty string
    if (StringIndex <= 0)
    {
        goto exit;
    }

    currentString = (PSTR)(((BYTE*)SmBiosTable) + SmBiosTable->Length);

    // find the string in the multisz string
    while (*currentString)
    {
        if (CurrentStringIndex == StringIndex)
        {
            break;
        }

        ++currentString;

        if (!*currentString)
        {
            ++currentString;
            ++CurrentStringIndex;
        }
    }

    if (!*currentString)
    {
        // String was not found in the string table.
        error = ERROR_INVALID_DATA;
        goto exit;
    }

    // Convert the string to UNICODE
    stringLen = MultiByteToWideChar(
                    CP_ACP,
                    MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
                    currentString,
                    -1,
                    NULL,
                    0);

    if (0 == stringLen)
    {
        error = GetLastError();
        goto exit;
    }

    // SMBIOS strings are limited to 64 characters
    if (stringLen > 65)
    {
        error = ERROR_INVALID_DATA;
        goto exit;
    }

    hr = ULongMult(stringLen, sizeof(WCHAR), &bufferLen);
    if (hr != S_OK)
    {
        error = ERROR_INVALID_DATA;
        goto exit;
    }

    *ResultString = (PWSTR) HeapAlloc(GetProcessHeap(), 0, bufferLen);
    if (!*ResultString)
    {
        error = ERROR_OUTOFMEMORY;
        goto exit;
    }

    stringLen = MultiByteToWideChar(
                   CP_ACP,
                   MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
                   currentString,
                   -1,
                   *ResultString,
                   stringLen);
                  
    if (0 == stringLen)
    {
        error = GetLastError();
    }

exit:
    if (error != ERROR_SUCCESS)
    {
        HeapFree(GetProcessHeap(),  0, *ResultString);
        *ResultString = NULL;
    }

    return error;
}

DWORD
GetManufacturerNameFromSmbios(
    _Out_ wchar_t* ManufacturerName,
    _In_  DWORD    Size
    )
{
    DWORD error = ERROR_SUCCESS;
    DWORD smBiosDataSize = 0;
    RawSMBIOSData* smBiosData = NULL;
    DWORD bytesWritten = 0;
    RawSMBIOSSystemInfo* systemInfo = NULL;
    PWSTR manufacturerName = NULL;

    if (ManufacturerName == NULL)
    {
        return E_INVALIDARG;
    }

    //
    // Query size of SMBIOS data.
    //
    smBiosDataSize = GetSystemFirmwareTable('RSMB', 0, NULL, 0);
    if (smBiosDataSize <= 0)
    {
        error = GetLastError();
        goto exit;
    }

    //
    // Allocate memory for the smbios table
    //
    smBiosData = (RawSMBIOSData*) HeapAlloc(GetProcessHeap(), 0, smBiosDataSize);
    if (smBiosData == NULL)
    {
        error = ERROR_OUTOFMEMORY;
        goto exit;
    }

    bytesWritten = GetSystemFirmwareTable('RSMB', 0, smBiosData, smBiosDataSize);
    if (bytesWritten != smBiosDataSize)
    {
        error = ERROR_INVALID_DATA;
        goto exit;
    }

    error = FindSmBiosTable(smBiosData, 1, (RawSMBIOSTable**)&systemInfo);
    if (error != ERROR_SUCCESS)
    {
        goto exit;
    }

    error = GetSmBiosString(systemInfo, systemInfo->Manufacturer, &manufacturerName);
    if (error != ERROR_SUCCESS)
    {
        goto exit;
    }

    if (manufacturerName != NULL)
    {
        if (Size <  (DWORD) wcslen(manufacturerName))
        {
            error = ERROR_BUFFER_OVERFLOW;
            goto exit;
        }

        wcscpy_s(ManufacturerName, Size, manufacturerName);
        HeapFree(GetProcessHeap(), 0, manufacturerName);
    }
    else
    {
        error = E_UNEXPECTED;
    }

exit:
    if (smBiosData)
    {
        HeapFree(GetProcessHeap(), 0, smBiosData);
    }

    return error;
}
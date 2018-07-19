#pragma once

extern "C" {

#define MANUFACTURER_NAME_LENGTH_MAX 1024

__declspec(dllexport)
DWORD
GetManufacturerNameFromSmbios(
    _Out_ wchar_t* ManufacturerName,
    _In_  DWORD    Size
    );

__declspec(dllexport)
DWORD
GetSecureBootEnabledFromUefi(
    _Out_ bool* SecureBootEnabled
    );
}
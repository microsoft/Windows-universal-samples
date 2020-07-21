#pragma once
#include <windows.h>
#include <elscore.h>
#include <elssrvc.h>

#include <string>
#include <sstream>

namespace ELS {

namespace Private
{
    inline MAPPING_SERVICE_INFO* GetSingleELSServiceWithNoOptions(const GUID& serviceGUID)
    {
        MAPPING_ENUM_OPTIONS EnumOptions = {};
        EnumOptions.Size = sizeof(EnumOptions);
        EnumOptions.pGuid = const_cast<GUID*>(&serviceGUID);

        DWORD dwServicesCount = 0;
        MAPPING_SERVICE_INFO* pService;
        HRESULT hr = MappingGetServices(&EnumOptions, &pService, &dwServicesCount);
        if (FAILED(hr))
        {
            pService = nullptr;
        }

        return pService;
    }
} //namespace Private

inline std::wstring RecognizeTextLanguages(_In_ PCWSTR text)
{
    auto pService = Private::GetSingleELSServiceWithNoOptions(ELS_GUID_LANGUAGE_DETECTION);
    std::wstring ret;
    if (nullptr != pService)
    {
        MAPPING_PROPERTY_BAG bag = {};
        bag.Size = sizeof(bag);

        HRESULT hr = MappingRecognizeText(pService, text, static_cast<DWORD>(wcslen(text)), 0, nullptr, &bag);
        if (SUCCEEDED(hr))
        {
            for (auto pos = reinterpret_cast<wchar_t*>(bag.prgResultRanges[0].pData); *pos; pos += (wcslen(pos) + 1))
            {
                ret += pos;
                ret += L"\n";
            }
            MappingFreePropertyBag(&bag);
        }
        MappingFreeServices(pService);
    }

    return ret;
}

inline std::wstring RecognizeTextScripts(_In_ PCWSTR text)
{
    auto pService = Private::GetSingleELSServiceWithNoOptions(ELS_GUID_SCRIPT_DETECTION);
    std::wstring ret;
    if (nullptr != pService)
    {
        MAPPING_PROPERTY_BAG bag = {};
        bag.Size = sizeof(bag);

        HRESULT hr = MappingRecognizeText(pService, text, static_cast<DWORD>(wcslen(text)), 0, nullptr, &bag);
        if (SUCCEEDED(hr))
        {
            std::wstringstream output(std::wstringstream::out);
            for (DWORD rangeIndex = 0; rangeIndex < bag.dwRangesCount; ++rangeIndex)
            {
                output << L"Range from " << bag.prgResultRanges[rangeIndex].dwStartIndex << L" to " << bag.prgResultRanges[rangeIndex].dwEndIndex << L": ";
                output << reinterpret_cast<wchar_t*>(bag.prgResultRanges[rangeIndex].pData) << L" script\n";
            }

            ret = output.str();
            MappingFreePropertyBag(&bag);
        }
        MappingFreeServices(pService);
    }

    return ret;
}

inline std::wstring TransliterateFromCyrillicToLatin(_In_ PCWSTR text)
{
    auto pService = Private::GetSingleELSServiceWithNoOptions(ELS_GUID_TRANSLITERATION_CYRILLIC_TO_LATIN);
    std::wstring ret;
    if (nullptr != pService)
    {
        MAPPING_PROPERTY_BAG bag = {};
        bag.Size = sizeof(bag);
        HRESULT hr = MappingRecognizeText(pService, text, static_cast<DWORD>(wcslen(text) + 1), 0, nullptr, &bag); // wcslen(text) + 1 to account for a null terminator also
        if (SUCCEEDED(hr))
        {
            ret = reinterpret_cast<wchar_t*>(bag.prgResultRanges[0].pData);
            MappingFreePropertyBag(&bag);
        }
        MappingFreeServices(pService);
    }

    return ret;
}

} //namespace ELS

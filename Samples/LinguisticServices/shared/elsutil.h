#pragma once
#include <windows.h>
#include <elscore.h>
#include <elssrvc.h>

namespace ELS::Private
{
    struct unique_services
    {
        operator MAPPING_SERVICE_INFO* () { return services; }
        MAPPING_SERVICE_INFO** operator&() { return &services; }
        MAPPING_SERVICE_INFO* services = nullptr;
        unique_services() = default;
        unique_services(unique_services const&) = delete;
        ~unique_services() { if (services) MappingFreeServices(services); }
    };

    struct unique_bag : MAPPING_PROPERTY_BAG
    {
        unique_bag() : MAPPING_PROPERTY_BAG{} {}
        unique_bag(unique_bag const&) = delete;
        ~unique_bag() { MappingFreePropertyBag(this); }
    };

    inline void MappingRecognizeTextFromService(REFGUID service, wchar_t const* text, DWORD length, unique_bag& bag)
    {
        MAPPING_ENUM_OPTIONS EnumOptions = {};
        EnumOptions.Size = sizeof(EnumOptions);
        EnumOptions.pGuid = const_cast<GUID*>(&service);

        DWORD dwServicesCount = 0;
        unique_services services;
        winrt::check_hresult(MappingGetServices(&EnumOptions, &services, &dwServicesCount));

        bag.Size = sizeof(bag);
        winrt::check_hresult(MappingRecognizeText(services, text, length, 0, nullptr, &bag));
    }
}

namespace ELS
{
    template<typename Lambda>
    void RecognizeTextLanguages(wchar_t const* text, DWORD length, Lambda&& callback)
    {
        if (length == 0) return;

        Private::unique_bag bag;
        Private::MappingRecognizeTextFromService(ELS_GUID_LANGUAGE_DETECTION, text, length, bag);

        for (auto pos = reinterpret_cast<wchar_t const*>(bag.prgResultRanges[0].pData); *pos; pos += (wcslen(pos) + 1))
        {
            callback(pos);
        }
    }

    template<typename Lambda>
    void RecognizeTextScripts(wchar_t const* text, DWORD length, Lambda&& callback)
    {
        if (length == 0) return;

        Private::unique_bag bag;
        Private::MappingRecognizeTextFromService(ELS_GUID_SCRIPT_DETECTION, text, length, bag);

        for (DWORD rangeIndex = 0; rangeIndex < bag.dwRangesCount; ++rangeIndex)
        {
            callback(bag.prgResultRanges[rangeIndex].dwStartIndex, bag.prgResultRanges[rangeIndex].dwEndIndex,
                reinterpret_cast<wchar_t const*>(bag.prgResultRanges[rangeIndex].pData));
        }
    }

    template<typename Lambda>
    void TransliterateFromCyrillicToLatin(wchar_t const* text, DWORD length, Lambda&& callback)
    {
        Private::unique_bag bag;
        Private::MappingRecognizeTextFromService(ELS_GUID_TRANSLITERATION_CYRILLIC_TO_LATIN, text, length, bag);
        callback(reinterpret_cast<wchar_t const*>(bag.prgResultRanges[0].pData), bag.prgResultRanges[0].dwDataSize / sizeof(wchar_t));
    }
}

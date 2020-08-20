#pragma once

#include "LinguisticServices.g.h"

namespace winrt::Sample::implementation
{
    struct LinguisticServices
    {
        static Windows::Foundation::Collections::IVectorView<hstring> RecognizeTextLanguages(hstring const& text);
        static Windows::Foundation::Collections::IVectorView<RecognizedTextRun> RecognizeTextScripts(hstring const& text);
        static hstring TransliterateFromCyrillicToLatin(hstring const& text);

    };
}

namespace winrt::Sample::factory_implementation
{
    struct LinguisticServices : LinguisticServicesT<LinguisticServices, implementation::LinguisticServices>
    {
    };
}

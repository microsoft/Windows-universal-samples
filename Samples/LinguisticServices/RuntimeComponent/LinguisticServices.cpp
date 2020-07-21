#include "pch.h"
#include "LinguisticServices.h"
#include "LinguisticServices.g.cpp"

namespace winrt::Sample::implementation
{
    Windows::Foundation::Collections::IVectorView<hstring> LinguisticServices::RecognizeTextLanguages(hstring const& text)
    {
        std::vector<hstring> results;
        ELS::RecognizeTextLanguages(text.c_str(), text.size(), [&](auto lang)
            {
                results.emplace_back(lang);
            });
        return single_threaded_vector(std::move(results)).GetView();
    }

    Windows::Foundation::Collections::IVectorView<RecognizedTextRun> LinguisticServices::RecognizeTextScripts(hstring const& text)
    {
        std::vector<RecognizedTextRun> results;
        ELS::RecognizeTextScripts(text.c_str(), text.size(), [&](auto first, auto last, auto script)
            {
                results.emplace_back(RecognizedTextRun{ first, last, script });
            });
        return single_threaded_vector(std::move(results)).GetView();
    }

    hstring LinguisticServices::TransliterateFromCyrillicToLatin(hstring const& text)
    {
        hstring result;
        ELS::TransliterateFromCyrillicToLatin(text.c_str(), text.size(), [&](auto latin, auto length)
            {
                result = hstring(latin, length);
            });
        return result;
    }
}

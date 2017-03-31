#pragma once

#include <map>

namespace SDKTemplate
{
    ref class RecognizerHelper sealed
    {
    public:
        static Platform::String^ LanguageTagToRecognizerName(Platform::String^ bcp47tag);

    private:
        static std::map<Platform::String^, Platform::String^> Bcp47ToRecognizerNameDictionary;
    };
}



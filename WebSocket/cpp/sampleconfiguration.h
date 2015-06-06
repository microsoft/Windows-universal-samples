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

#include <collection.h>

namespace SDKTemplate
{
    public value struct Scenario
    {
        Platform::String^ Title;
        Platform::String^ ClassName;
    };

    partial ref class MainPage
    {
    public:
        static property Platform::String^ FEATURE_NAME
        {
            Platform::String^ get()
            {
                return ref new Platform::String(L"WebSocket");
            }
        }

        static property Platform::Array<Scenario>^ scenarios
        {
            Platform::Array<Scenario>^ get()
            {
                return scenariosInner;
            }
        }

        bool TryGetUri(Platform::String^ uriString, Windows::Foundation::Uri^* uri)
        {
            *uri = nullptr;

            Windows::Foundation::Uri^ webSocketUri;

            // Create a Uri instance and catch exceptions related to invalid input. This method returns 'true'
            // if the Uri instance was successfully created and 'false' otherwise.
            try
            {
                webSocketUri = ref new Windows::Foundation::Uri(StringTrimmer::Trim(uriString));
            }
            catch (Platform::NullReferenceException^ exception)
            {
                NotifyUser("Error: URI must not be null or empty.", NotifyType::ErrorMessage);
                return false;
            }
            catch (Platform::InvalidArgumentException^ exception)
            {
                NotifyUser("Error: Invalid URI", NotifyType::ErrorMessage);
                return false;
            }

            if (webSocketUri->Fragment != "")
            {
                NotifyUser("Error: URI fragments not supported in WebSocket URIs.", NotifyType::ErrorMessage);
                return false;
            }

            Platform::String^ wsScheme = "ws";
            Platform::String^ wssScheme = "wss";
            Platform::String^ scheme = webSocketUri->SchemeName;

            // Uri->SchemeName returns the canonicalized scheme name so we can use case-sensitive, ordinal string
            // comparison.
            if ((CompareStringOrdinal(scheme->Begin(), scheme->Length(), wsScheme->Begin(), wsScheme->Length(), false) != CSTR_EQUAL) && 
                (CompareStringOrdinal(scheme->Begin(), scheme->Length(), wssScheme->Begin(), wssScheme->Length(), false) != CSTR_EQUAL))
            {
                NotifyUser("Error: WebSockets only support ws:// and wss:// schemes.", NotifyType::ErrorMessage);
                return false;
            }

            *uri = webSocketUri;

            return true;
        }

    private:
        static Platform::Array<Scenario>^ scenariosInner;
    };

    class StringTrimmer
    {
    public:
        static Platform::String^ Trim(Platform::String^ s)
        {
            const WCHAR* first = s->Begin();
            const WCHAR* last = s->End();

            while (first != last && iswspace(*first))
            {
                ++first;
            }

            while (first != last && iswspace(last[-1]))
            {
                --last;
            }

            return ref new Platform::String(first, static_cast<unsigned int>(last - first));
        }
    };
}

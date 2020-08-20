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

#include "Scenario1_XmlReader.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_XmlReader : Scenario1_XmlReaderT<Scenario1_XmlReader>
    {
        Scenario1_XmlReader();

        fire_and_forget ReadXmlClick(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        static void ReadXml(std::wostream& result, com_ptr<IStream> const& readStream);
        static void ReadAttributes(std::wostream& result, com_ptr<IXmlReader> const& reader);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_XmlReader : Scenario1_XmlReaderT<Scenario1_XmlReader, implementation::Scenario1_XmlReader>
    {
    };
}

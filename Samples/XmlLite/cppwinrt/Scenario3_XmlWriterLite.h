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

#include "Scenario3_XmlWriterLite.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_XmlWriterLite : Scenario3_XmlWriterLiteT<Scenario3_XmlWriterLite>
    {
        Scenario3_XmlWriterLite();

        fire_and_forget DoSomething_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        void ReadAndWrite(com_ptr<IStream> const& readStream, com_ptr<IStream> const& writeStream);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_XmlWriterLite : Scenario3_XmlWriterLiteT<Scenario3_XmlWriterLite, implementation::Scenario3_XmlWriterLite>
    {
    };
}

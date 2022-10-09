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
#include <sstream>
#include "Scenario4_CompositeSettings.h"
#include "Scenario4_CompositeSettings.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario4_CompositeSettings::Scenario4_CompositeSettings()
    {
        InitializeComponent();
    }

    void Scenario4_CompositeSettings::WriteCompositeSetting_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Composite settings are used to group multiple settings.
        // Note that Composite settings may not be nested.
        ApplicationDataCompositeValue composite;
        composite.Insert(settingName1, box_value(1)); // example value
        composite.Insert(settingName2, box_value(L"world")); // example value

        localSettings.Values().Insert(settingName, composite);

        DisplayOutput();
    }

    void Scenario4_CompositeSettings::DeleteCompositeSetting_Click(IInspectable const&, RoutedEventArgs const&)
    {
        localSettings.Values().Remove(settingName);

        DisplayOutput();
    }

    void Scenario4_CompositeSettings::DisplayOutput()
    {
        auto composite = localSettings.Values().Lookup(settingName).as<ApplicationDataCompositeValue>();

        std::wstringstream output;
        if (!composite)
        {
            output << L"Composite Setting: <empty>";
        }
        else
        {
            output << L"Composite setting: {" << settingName1 << L" = " << composite.Lookup(settingName1).as<int>() << L", ";
            output << settingName2 << L" = \"" << composite.Lookup(settingName2).as<hstring>().c_str() << "\"}";
        }

        OutputTextBlock().Text(output.str());
    }

    void Scenario4_CompositeSettings::OnNavigatedTo(NavigationEventArgs const&)
    {
        DisplayOutput();
    }
}


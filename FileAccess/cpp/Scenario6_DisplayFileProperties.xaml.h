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

#include "pch.h"
#include "Scenario6_DisplayFileProperties.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// Displaying file properties.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario6 sealed
    {
    public:
        Scenario6();

    private:
        static Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ dateFormat;
        static Windows::Globalization::DateTimeFormatting::DateTimeFormatter^ timeFormat;
        static Platform::String^ dateAccessedProperty;
        static Platform::String^ fileOwnerProperty;

        MainPage^ rootPage;

        void ShowPropertiesButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

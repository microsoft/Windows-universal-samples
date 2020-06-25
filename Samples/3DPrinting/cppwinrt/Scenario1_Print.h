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

#include "Scenario1_Print.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Print : Scenario1_PrintT<Scenario1_Print>
    {
        Scenario1_Print();

        fire_and_forget CreateProgrammatically_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget CreateFromXml_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget LoadPackageFromFile_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget SavePackage_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget PrintPackage_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Graphics::Printing3D::Printing3D3MFPackage currentPackage{ nullptr };

        void EnablePackageOperationButtons();
        void OnTaskRequested(Windows::Graphics::Printing3D::Print3DManager const& sender, Windows::Graphics::Printing3D::Print3DTaskRequestedEventArgs const& args);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Print : Scenario1_PrintT<Scenario1_Print, implementation::Scenario1_Print>
    {
    };
}

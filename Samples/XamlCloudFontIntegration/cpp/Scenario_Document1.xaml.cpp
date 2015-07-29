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
#include "Scenario_Document1.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;


Scenario_Document1::Scenario_Document1()
{
    InitializeComponent();
}


void Scenario_Document1::Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Font formatting is being applied on page load rather than in XAML markup
    // so that the on-demand cloud font behavior can be experienced when the
    // app runs rather than when the XAML file is viewed in Visual Studio.

    // Formatting will be applied to all content elements in the page that don't 
    // already have FontFamily set.
    this->FontFamily = ref new Windows::UI::Xaml::Media::FontFamily("Neue Haas Grotesk Text Pro");
}

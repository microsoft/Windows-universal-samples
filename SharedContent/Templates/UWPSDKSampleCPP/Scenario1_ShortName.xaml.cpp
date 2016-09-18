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
#include "Scenario1_ShortName.xaml.h"

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

Scenario1_ShortName::Scenario1_ShortName() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void SDKTemplate::Scenario1_ShortName::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Hello", NotifyType::StatusMessage);
}

void SDKTemplate::Scenario1_ShortName::Button_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Hello", NotifyType::ErrorMessage);
}

void SDKTemplate::Scenario1_ShortName::Button_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Hello How are you today. This is a very long message with a lot of text.\nLet's throw in some newline characters\nand\nsee\nwhat\nhappens.\nadsf\nasdf\nasdf\nasfd\nasdf\nasdf\nasfd", NotifyType::StatusMessage);
}

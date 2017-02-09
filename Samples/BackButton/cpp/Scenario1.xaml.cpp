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
#include "Scenario1.xaml.h"
#include "SecondaryPage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

bool Scenario1::optedIn = false;

Scenario1::Scenario1() : rootPage(MainPage::Current)
{
    InitializeComponent();

    // I want this page to be always cached so that we don't have to add logic to save/restore state for the checkbox.
    NavigationCacheMode = Navigation::NavigationCacheMode::Required;
}

void Scenario1::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage = MainPage::Current;
}

void SDKTemplate::Scenario1::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto rootFrame = dynamic_cast<Windows::UI::Xaml::Controls::Frame^>(Window::Current->Content);

    // Navigate to the next page, with info in the parameters whether to enable the title bar UI or not.
    rootFrame->Navigate(SecondaryPage::typeid, optedIn);
}

void Scenario1::CheckBox_Toggle(Platform::Object^ sender, RoutedEventArgs e)
{
    optedIn = !optedIn;
}


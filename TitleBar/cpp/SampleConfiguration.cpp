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
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Custom colors", "SDKTemplate.Scenario1_Colors" },
    { "Custom drawing", "SDKTemplate.Scenario2_Extend" },
};

#pragma region Custom title bar
void MainPage::AddCustomTitleBar()
{
    if (customTitleBar == nullptr)
    {
        customTitleBar = ref new CustomTitleBar();
        customTitleBar->EnableControlsInTitleBar(areControlsInTitleBar);

        // Make the main page's content a child of the title bar
        // and make the title bar the new page content.
        UIElement^ mainContent = this->Content;
        this->Content = nullptr;
        customTitleBar->SetPageContent(mainContent);
        this->Content = customTitleBar;
    }
}

void MainPage::RemoveCustomTitleBar()
{
    if (customTitleBar != nullptr)
    {
        // Take the title bar's page content and make it
        // the window content.
        this->Content = customTitleBar->SetPageContent(nullptr);
        customTitleBar = nullptr;
    }
}


void MainPage::AreControlsInTitleBar::set(bool value)
{
    areControlsInTitleBar = value;
    if (customTitleBar != nullptr)
    {
        customTitleBar->EnableControlsInTitleBar(value);
    }
}
#pragma endregion
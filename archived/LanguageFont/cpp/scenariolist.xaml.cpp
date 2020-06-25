// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

//
// ScenarioList.xaml.cpp
// Implementation of the ScenarioList class
//

#include "pch.h"
#include "ScenarioList.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace CppSamplesUtils;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Interop;

ScenarioList::ScenarioList()
{
    InitializeComponent();
}

ScenarioList::~ScenarioList()
{
}

void ScenarioList::Scenarios_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    if(Scenarios->SelectedItem != nullptr)
    {
        rootPage->NotifyUser("", NotifyType::StatusMessage);

        ListBoxItem^ selectedListBoxItem = dynamic_cast<ListBoxItem^>(Scenarios->SelectedItem);
        SuspensionManager::SessionState()->Insert("SelectedScenario", selectedListBoxItem->Name);

        if (rootPage->InputFrame != nullptr && rootPage->OutputFrame != nullptr)
        {
             //Load the input and output pages for the current scenario into their respective frames
            TypeName inputPage = { "SDKTemplate" + "." + "ScenarioInput" + ((Scenarios->SelectedIndex + 1).ToString()), TypeKind::Custom };
            TypeName outputPage = { "SDKTemplate" + "." + "ScenarioOutput" + ((Scenarios->SelectedIndex + 1).ToString()), TypeKind::Custom };
            rootPage->DoNavigation(inputPage, rootPage->InputFrame, outputPage, rootPage->OutputFrame);
        }
    }
}

void ScenarioList::OnNavigatedTo(NavigationEventArgs^ e)
{
    rootPage = dynamic_cast<MainPage^>(e->Parameter);

    Scenarios->SelectionChanged += ref new SelectionChangedEventHandler(this, &ScenarioList::Scenarios_SelectionChanged);
    ListBoxItem^ startingScenario = nullptr;

    auto ps = SuspensionManager::SessionState();
    if (ps->HasKey("SelectedScenario"))
    {
        String^ item = dynamic_cast<String^>(ps->Lookup("SelectedScenario"));
        startingScenario = dynamic_cast<ListBoxItem^>(this->FindName(item));
    }

    Scenarios->SelectedItem = startingScenario != nullptr ? startingScenario : Scenario1;
}

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

//
// ScenarioList.xaml.h
// Declaration of the ScenarioList class
//

#pragma once

#include "pch.h"
#include "ScenarioList.g.h"
#include "MainPage.g.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ScenarioList sealed
    {
    public:
        ScenarioList();

    protected:
        void Scenarios_SelectionChanged(Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        ~ScenarioList();
        MainPage^ rootPage;
    };
}

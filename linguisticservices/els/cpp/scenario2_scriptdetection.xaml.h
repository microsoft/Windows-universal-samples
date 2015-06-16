// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "pch.h"
#include "Scenario2_ScriptDetection.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_ScriptDetection sealed
    {
    public:
        Scenario2_ScriptDetection();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;
        void Go_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}


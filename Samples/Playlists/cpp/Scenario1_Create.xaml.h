// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_Create.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Create sealed
    {
    public:
        Scenario1_Create();
    private:
        MainPage^ rootPage;
        void PickAudioButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario3_StreamVideo.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_StreamVideo sealed
    {
    public:
        Scenario3_StreamVideo();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        MainPage^ rootPage;
        void PickVideo_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}


// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "HomeGroupVideoStream.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class HomeGroupVideoStream sealed
    {
    public:
        HomeGroupVideoStream();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        MainPage^ rootPage;
        void Default_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}


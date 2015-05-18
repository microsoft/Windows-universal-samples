// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "SearchHomeGroup.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SearchHomeGroup sealed
    {
    public:
        SearchHomeGroup();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        MainPage^ rootPage;
        void Search_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario4_Snapshot.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Scenario4 sealed
    {
    public:
        Scenario4();

        property Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ ResultCollection;

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void FindButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        SDKTemplate::MainPage^ rootPage;
    };
}

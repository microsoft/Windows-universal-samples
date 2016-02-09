// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario8_PairDevice.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Scenario8 sealed
    {
    public:
        Scenario8();

        property Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ ResultCollection;

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UnpairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void ResultsListView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

        void StartWatcher();
        void StopWatcher();

        void UpdatePairingButtons();

        SDKTemplate::MainPage^ rootPage;
        Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher;
        Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Windows::Devices::Enumeration::DeviceInformation^>^ handlerAdded;
        Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Windows::Devices::Enumeration::DeviceInformationUpdate^>^ handlerUpdated;
        Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Windows::Devices::Enumeration::DeviceInformationUpdate^>^ handlerRemoved;
        Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Platform::Object^>^ handlerEnumCompleted;
        Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Platform::Object^>^ handlerStopped;
        Windows::Foundation::EventRegistrationToken handlerAddedToken;
        Windows::Foundation::EventRegistrationToken handlerUpdatedToken;
        Windows::Foundation::EventRegistrationToken handlerRemovedToken;
        Windows::Foundation::EventRegistrationToken handlerEnumCompletedToken;
        Windows::Foundation::EventRegistrationToken handlerStoppedAddedToken;
    };
}

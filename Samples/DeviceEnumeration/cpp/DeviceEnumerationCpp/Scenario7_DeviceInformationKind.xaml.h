// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario7_DeviceInformationKind.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Scenario7 sealed
    {
    public:
        Scenario7();

        property Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ ResultCollection;

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void StartWatcher();
        void StopWatcher();

        SDKTemplate::MainPage^ rootPage;
        Platform::Collections::Vector<Windows::Devices::Enumeration::DeviceWatcher^>^ deviceWatchers;
        Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Windows::Devices::Enumeration::DeviceInformation^>^ handlerAdded;
        Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Windows::Devices::Enumeration::DeviceInformationUpdate^>^ handlerUpdated;
        Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Windows::Devices::Enumeration::DeviceInformationUpdate^>^ handlerRemoved;
        Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Platform::Object^>^ handlerEnumCompleted;
        Windows::Foundation::TypedEventHandler<Windows::Devices::Enumeration::DeviceWatcher^, Platform::Object^>^ handlerStopped;
        Platform::Array<Windows::Foundation::EventRegistrationToken>^ handlerAddedTokens;
        Platform::Array<Windows::Foundation::EventRegistrationToken>^ handlerUpdatedTokens;
        Platform::Array<Windows::Foundation::EventRegistrationToken>^ handlerRemovedTokens;
        Platform::Array<Windows::Foundation::EventRegistrationToken>^ handlerEnumCompletedTokens;
        Platform::Array<Windows::Foundation::EventRegistrationToken>^ handlerStoppedTokens;
    };
}

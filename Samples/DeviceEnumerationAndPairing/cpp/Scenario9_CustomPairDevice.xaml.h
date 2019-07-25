// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario9_CustomPairDevice.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"
#include "DeviceWatcherHelper.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario9_CustomPairDevice sealed
    {
    public:
        Scenario9_CustomPairDevice();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void UnpairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void StartWatcher();
        void StopWatcher();

        void ResultsListView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);

        void PairingRequestedHandler(Windows::Devices::Enumeration::DeviceInformationCustomPairing^ sender, Windows::Devices::Enumeration::DevicePairingRequestedEventArgs^ args);

        void ShowPairingPanel(Platform::String^ text, Windows::Devices::Enumeration::DevicePairingKinds pairingKind);
        void HidePairingPanel();
        concurrency::task<Platform::String^> GetPinFromUserAsync();
        void CompleteProvidePinTask(Platform::String^ pin = nullptr);
        concurrency::task<bool> GetUserConfirmationAsync(Platform::String^ pin);
        void CompleteConfirmPinTask(bool accept);
        void okButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void verifyButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        concurrency::task<Windows::Security::Credentials::PasswordCredential^> GetPasswordCredentialFromUserAsync();
        void CompletePasswordCredential(Platform::String^ username = nullptr, Platform::String^ password = nullptr);
        void yesButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void noButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        
        void OnDeviceListChanged(Windows::Devices::Enumeration::DeviceWatcher^ sender, Platform::String^ id);
        void UpdatePairingButtons();
        Windows::Devices::Enumeration::DevicePairingKinds GetSelectedCeremonies();

        MainPage^ rootPage = MainPage::Current;
        Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ resultCollection = ref new Platform::Collections::Vector<DeviceInformationDisplay^>();
        DeviceWatcherHelper^ deviceWatcherHelper;

        concurrency::task_completion_event<Platform::String^> providePinTaskSrc;
        concurrency::task_completion_event<bool> confirmPinTaskSrc;
        concurrency::task_completion_event<Windows::Security::Credentials::PasswordCredential^> providePasswordCredentialSrc;
    };
}

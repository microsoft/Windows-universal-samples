//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "Scenario9_CustomPairDevice.g.h"
#include "MainPage.h"
#include "DeviceWatcherHelper.h"
#include "SampleConfiguration.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario9_CustomPairDevice : Scenario9_CustomPairDeviceT<Scenario9_CustomPairDevice>
    {
        Scenario9_CustomPairDevice();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void StartWatcherButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void StopWatcherButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget PairButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget UnpairButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void okButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void verifyButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void yesButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void noButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void ResultsListView_SelectionChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceInformationDisplay> resultCollection = winrt::single_threaded_observable_vector<SDKTemplate::DeviceInformationDisplay>();
        com_ptr<DeviceWatcherHelper> deviceWatcherHelper = make_self<DeviceWatcherHelper>(resultCollection, Dispatcher());
        SimpleTaskCompletionSource<hstring> providePinTaskSrc;
        SimpleTaskCompletionSource<bool> confirmPinTaskSrc;
        SimpleTaskCompletionSource<Windows::Security::Credentials::PasswordCredential> providePasswordCredentialTaskSrc;

        void StartWatcher();
        void StopWatcher();
        void OnDeviceListChanged(Windows::Devices::Enumeration::DeviceWatcher const& sender, hstring const& id);
        fire_and_forget PairingRequestedHandler(Windows::Devices::Enumeration::DeviceInformationCustomPairing const& sender, Windows::Devices::Enumeration::DevicePairingRequestedEventArgs e);
        void ShowPairingPanel(hstring const& text, Windows::Devices::Enumeration::DevicePairingKinds pairingKind);
        void HidePairingPanel();
        Windows::Foundation::IAsyncOperation<hstring> GetPinFromUserAsync();
        void CompleteProvidePinTask(hstring pin = {});
        Windows::Foundation::IAsyncOperation<Windows::Security::Credentials::PasswordCredential> GetPasswordCredentialFromUserAsync();
        void CompletePasswordCredential(hstring username = {}, hstring password = {});
        Windows::Foundation::IAsyncOperation<bool> GetUserConfirmationAsync(hstring const& pin);
        void CompleteConfirmPinTask(bool accept);
        Windows::Devices::Enumeration::DevicePairingKinds GetSelectedCeremonies();
        void UpdatePairingButtons();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario9_CustomPairDevice : Scenario9_CustomPairDeviceT<Scenario9_CustomPairDevice, implementation::Scenario9_CustomPairDevice>
    {
    };
}

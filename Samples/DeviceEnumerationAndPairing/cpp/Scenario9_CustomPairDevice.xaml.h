// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario9_CustomPairDevice.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Scenario9 sealed
    {
    public:
        Scenario9();

        property Windows::Foundation::Collections::IObservableVector<DeviceInformationDisplay^>^ ResultCollection;

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
        void yesButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void noButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        
        void UpdatePairingButtons();
        Windows::Devices::Enumeration::DevicePairingKinds GetSelectedCeremonies();

        SDKTemplate::MainPage^ rootPage;
        Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher;
        Windows::Foundation::EventRegistrationToken handlerAddedToken;
        Windows::Foundation::EventRegistrationToken handlerUpdatedToken;
        Windows::Foundation::EventRegistrationToken handlerRemovedToken;
        Windows::Foundation::EventRegistrationToken handlerEnumCompletedToken;
        Windows::Foundation::EventRegistrationToken handlerStoppedAddedToken;
        Windows::Foundation::EventRegistrationToken handlerPairingRequestedToken;

        std::unique_ptr<concurrency::task_completion_event<Platform::String^>> providePinTaskSrc;
        std::unique_ptr<concurrency::task_completion_event<bool>> confirmPinTaskSrc;
    };
}

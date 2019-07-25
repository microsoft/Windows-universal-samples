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

#include "pch.h"
#include "Scenario9_CustomPairDevice.h"
#include "DeviceHelpers.h"
#include "SampleConfiguration.h"
#include "Scenario9_CustomPairDevice.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Security::Credentials;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario9_CustomPairDevice::Scenario9_CustomPairDevice()
    {
        InitializeComponent();
        deviceWatcherHelper->DeviceChanged({ get_weak(), &Scenario9_CustomPairDevice::OnDeviceListChanged });
    }

    void Scenario9_CustomPairDevice::OnNavigatedTo(NavigationEventArgs const&)
    {
        resultsListView().ItemsSource(resultCollection);

        selectorComboBox().ItemsSource(DeviceSelectorChoices::PairingSelectors());
        selectorComboBox().SelectedIndex(0);

        protectionLevelComboBox().ItemsSource(ProtectionSelectorChoices::Selectors());
        protectionLevelComboBox().SelectedIndex(0);
    }

    void Scenario9_CustomPairDevice::OnNavigatedFrom(NavigationEventArgs const&)
    {
        deviceWatcherHelper->Reset();
        CompleteProvidePinTask(); // Abandon any previous pin request.
        CompletePasswordCredential(); // Abandon any previous pin request.
        CompleteConfirmPinTask(false); // Abandon any previous request.
    }

    void Scenario9_CustomPairDevice::StartWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StartWatcher();
    }

    void Scenario9_CustomPairDevice::StopWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StopWatcher();
    }

    void Scenario9_CustomPairDevice::StartWatcher()
    {
        startWatcherButton().IsEnabled(false);
        resultCollection.Clear();

        // Get the device selector chosen by the UI then add additional constraints for devices that
        // can be paired or are already paired.
        DeviceSelectorInfo* deviceSelectorInfo = FromInspectable<DeviceSelectorInfo>(selectorComboBox().SelectedItem());
        hstring selector = L"(" + deviceSelectorInfo->Selector() + L") AND (System.Devices.Aep.CanPair:=System.StructuredQueryType.Boolean#True OR System.Devices.Aep.IsPaired:=System.StructuredQueryType.Boolean#True)";

        DeviceWatcher deviceWatcher{ nullptr };
        if (deviceSelectorInfo->Kind() == DeviceInformationKind::Unknown)
        {
            // Kind will be determined by the selector
            deviceWatcher = DeviceInformation::CreateWatcher(
                selector,
                nullptr // don't request additional properties for this sample
            );
        }
        else
        {
            // Kind is specified in the selector info
            deviceWatcher = DeviceInformation::CreateWatcher(
                selector,
                nullptr, // don't request additional properties for this sample
                deviceSelectorInfo->Kind());
        }

        rootPage.NotifyUser(L"Starting Watcher...", NotifyType::StatusMessage);
        deviceWatcherHelper->StartWatcher(deviceWatcher);
        stopWatcherButton().IsEnabled(true);
    }

    void Scenario9_CustomPairDevice::StopWatcher()
    {
        stopWatcherButton().IsEnabled(false);

        deviceWatcherHelper->StopWatcher();

        startWatcherButton().IsEnabled(true);
    }

    void Scenario9_CustomPairDevice::OnDeviceListChanged(DeviceWatcher const&, hstring const& id)
    {
        // If the item being updated is currently "selected", then update the pairing buttons
        auto item = resultsListView().SelectedItem();
        DeviceInformationDisplay* deviceInformationDisplay = FromInspectable<DeviceInformationDisplay>(item);
        if ((deviceInformationDisplay != nullptr) && (deviceInformationDisplay->Id() == id))
        {
            UpdatePairingButtons();
        }
    }

    fire_and_forget Scenario9_CustomPairDevice::PairButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        PasswordCredential credential = co_await GetPasswordCredentialFromUserAsync();
        if (credential)
        {
            rootPage.NotifyUser(L"Entered", NotifyType::StatusMessage);
        }

        // Gray out the pair button and results view while pairing is in progress.
        resultsListView().IsEnabled(false);
        pairButton().IsEnabled(false);
        rootPage.NotifyUser(L"Pairing started. Please wait...", NotifyType::StatusMessage);

        // Get the device selected for pairing
        IInspectable item = resultsListView().SelectedItem();
        DeviceInformationDisplay* deviceInfoDisp = FromInspectable<DeviceInformationDisplay>(item);

        // Get ceremony type and protection level selections
        DevicePairingKinds ceremoniesSelected = GetSelectedCeremonies();
        IInspectable protectionItem = protectionLevelComboBox().SelectedItem();
        ProtectionLevelSelectorInfo* protectionLevelInfo = FromInspectable<ProtectionLevelSelectorInfo>(protectionItem);
        DevicePairingProtectionLevel protectionLevel = protectionLevelInfo->ProtectionLevel();

        DeviceInformationCustomPairing customPairing = deviceInfoDisp->DeviceInformation().Pairing().Custom();

        event_token token = customPairing.PairingRequested({ get_weak(), &Scenario9_CustomPairDevice::PairingRequestedHandler });
        DevicePairingResult result = co_await customPairing.PairAsync(ceremoniesSelected, protectionLevel);
        customPairing.PairingRequested(token);

        rootPage.NotifyUser(
            L"Pairing result = " + to_hstring(result.Status()),
            result.Status() == DevicePairingResultStatus::Paired ? NotifyType::StatusMessage : NotifyType::ErrorMessage);

        HidePairingPanel();
        UpdatePairingButtons();
        resultsListView().IsEnabled(true);
    }

    fire_and_forget Scenario9_CustomPairDevice::UnpairButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Gray out the pair button and results view while unpairing is in progress.
        resultsListView().IsEnabled(false);
        unpairButton().IsEnabled(false);
        rootPage.NotifyUser(L"Unpairing started. Please wait...", NotifyType::StatusMessage);

        IInspectable item = resultsListView().SelectedItem();
        DeviceInformationDisplay* deviceInfoDisp = FromInspectable<DeviceInformationDisplay>(item);

        DeviceUnpairingResult result = co_await deviceInfoDisp->DeviceInformation().Pairing().UnpairAsync();

        rootPage.NotifyUser(
            L"Unpairing result = " + to_hstring(result.Status()),
            result.Status() == DeviceUnpairingResultStatus::Unpaired ? NotifyType::StatusMessage : NotifyType::ErrorMessage);

        UpdatePairingButtons();
        resultsListView().IsEnabled(true);
    }

    void Scenario9_CustomPairDevice::ResultsListView_SelectionChanged(IInspectable const&, SelectionChangedEventArgs const&)
    {
        UpdatePairingButtons();
    }

    fire_and_forget Scenario9_CustomPairDevice::PairingRequestedHandler(DeviceInformationCustomPairing const&, DevicePairingRequestedEventArgs args)
    {
        auto lifetime = get_strong();

        switch (args.PairingKind())
        {
        case DevicePairingKinds::ConfirmOnly:
            // Windows itself will pop the confirmation dialog as part of "consent" if this is running on Desktop or Mobile
            // If this is an App for 'Windows IoT Core' where there is no Windows Consent UX, you may want to provide your own confirmation.
            args.Accept();
            break;

        case DevicePairingKinds::DisplayPin:
            // We just show the PIN on this side. The ceremony is actually completed when the user enters the PIN
            // on the target device. We automatically accept here since we can't really "cancel" the operation
            // from this side.
            args.Accept();

            // No need for a deferral since we don't need any decision from the user
            co_await resume_foreground(Dispatcher());

            ShowPairingPanel(
                L"Please enter this PIN on the device you are pairing with: " + args.Pin(),
                args.PairingKind());

            break;

        case DevicePairingKinds::ProvidePin:
        {
            // A PIN may be shown on the target device and the user needs to enter the matching PIN on
            // this Windows device. Get a deferral so we can perform the async request to the user.
            auto collectPinDeferral = args.GetDeferral();

            co_await resume_foreground(Dispatcher());

            hstring pin = co_await GetPinFromUserAsync();
            if (!pin.empty())
            {
                args.Accept(pin);
            }

            collectPinDeferral.Complete();
            break;
        }

        case DevicePairingKinds::ProvidePasswordCredential:
        {
            auto collectCredentialDeferral = args.GetDeferral();
            co_await resume_foreground(Dispatcher());

            PasswordCredential credential = co_await GetPasswordCredentialFromUserAsync();
            if (credential != nullptr)
            {
                args.AcceptWithPasswordCredential(credential);
            }
            collectCredentialDeferral.Complete();
            break;
        }

        case DevicePairingKinds::ConfirmPinMatch:
        {
            // We show the PIN here and the user responds with whether the PIN matches what they see
            // on the target device. Response comes back and we set it on the PinComparePairingRequestedData
            // then complete the deferral.
            auto displayMessageDeferral = args.GetDeferral();

            co_await resume_foreground(Dispatcher());
            bool accept = co_await GetUserConfirmationAsync(args.Pin());
            if (accept)
            {
                args.Accept();
            }

            displayMessageDeferral.Complete();
            break;
        }
        }
    }

    void Scenario9_CustomPairDevice::ShowPairingPanel(hstring const& text, DevicePairingKinds pairingKind)
    {
        pairingPanel().Visibility(Visibility::Collapsed);
        pinEntryTextBox().Visibility(Visibility::Collapsed);
        okButton().Visibility(Visibility::Collapsed);
        usernameEntryTextBox().Visibility(Visibility::Collapsed);
        passwordEntryTextBox().Visibility(Visibility::Collapsed);
        verifyButton().Visibility(Visibility::Collapsed);
        yesButton().Visibility(Visibility::Collapsed);
        noButton().Visibility(Visibility::Collapsed);
        pairingTextBlock().Text(text);

        switch (pairingKind)
        {
        case DevicePairingKinds::ConfirmOnly:
        case DevicePairingKinds::DisplayPin:
            // Don't need any buttons
            break;
        case DevicePairingKinds::ProvidePin:
            pinEntryTextBox().Text({});
            pinEntryTextBox().Visibility(Visibility::Visible);
            okButton().Visibility(Visibility::Visible);
            break;
        case DevicePairingKinds::ConfirmPinMatch:
            yesButton().Visibility(Visibility::Visible);
            noButton().Visibility(Visibility::Visible);
            break;
        case DevicePairingKinds::ProvidePasswordCredential:
            usernameEntryTextBox().Text({});
            passwordEntryTextBox().Text({});
            passwordEntryTextBox().Visibility(Visibility::Visible);
            usernameEntryTextBox().Visibility(Visibility::Visible);
            verifyButton().Visibility(Visibility::Visible);
            break;
        }

        pairingPanel().Visibility(Visibility::Visible);
    }

    void Scenario9_CustomPairDevice::HidePairingPanel()
    {
        pairingPanel().Visibility(Visibility::Collapsed);
        pairingTextBlock().Text({});
    }

    IAsyncOperation<hstring> Scenario9_CustomPairDevice::GetPinFromUserAsync()
    {
        HidePairingPanel();
        CompleteProvidePinTask(); // Abandon any previous pin request.

        ShowPairingPanel(
            L"Please enter the PIN shown on the device you're pairing with",
            DevicePairingKinds::ProvidePin);

        providePinTaskSrc = SimpleTaskCompletionSource<hstring>();

        return co_await providePinTaskSrc.Task();
    }

    // If pin is not provided, then any pending pairing request is abandoned.
    void Scenario9_CustomPairDevice::CompleteProvidePinTask(hstring pin)
    {
        providePinTaskSrc.SetResult(pin);
    }

    IAsyncOperation<PasswordCredential> Scenario9_CustomPairDevice::GetPasswordCredentialFromUserAsync()
    {
        HidePairingPanel();
        CompletePasswordCredential(); // Abandon any previous pin request.

        ShowPairingPanel(
            L"Please enter the username and password",
            DevicePairingKinds::ProvidePasswordCredential);

        providePasswordCredentialTaskSrc = SimpleTaskCompletionSource<PasswordCredential>();

        return co_await providePasswordCredentialTaskSrc.Task();
    }

    void Scenario9_CustomPairDevice::CompletePasswordCredential(hstring username, hstring password)
    {
        if (username.empty())// || password.empty())
        {
            providePasswordCredentialTaskSrc.SetResult(nullptr);
        }
        else
        {
            PasswordCredential passwordCredential;
            passwordCredential.UserName(username);
            passwordCredential.Password(password);
            providePasswordCredentialTaskSrc.SetResult(passwordCredential);
        }
    }

    IAsyncOperation<bool> Scenario9_CustomPairDevice::GetUserConfirmationAsync(hstring const& pin)
    {
        HidePairingPanel();
        CompleteConfirmPinTask(false); // Abandon any previous request.

        ShowPairingPanel(
            L"Does the following PIN match the one shown on the device you are pairing?: " + pin,
            DevicePairingKinds::ConfirmPinMatch);

        confirmPinTaskSrc = SimpleTaskCompletionSource<bool>();

        return co_await confirmPinTaskSrc.Task();
    }

    // If pin is not provided, then any pending pairing request is abandoned.
    void Scenario9_CustomPairDevice::CompleteConfirmPinTask(bool accept)
    {
        confirmPinTaskSrc.SetResult(accept);
    }


    void Scenario9_CustomPairDevice::okButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // OK button is only used for the ProvidePin scenario
        CompleteProvidePinTask(pinEntryTextBox().Text());
        HidePairingPanel();
    }

    void Scenario9_CustomPairDevice::verifyButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // verify button is only used for the ProvidePin scenario
        CompletePasswordCredential(usernameEntryTextBox().Text(), passwordEntryTextBox().Text());
        HidePairingPanel();
    }

    void Scenario9_CustomPairDevice::yesButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        CompleteConfirmPinTask(true);
        HidePairingPanel();
    }

    void Scenario9_CustomPairDevice::noButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        CompleteConfirmPinTask(false);
        HidePairingPanel();
    }

    DevicePairingKinds Scenario9_CustomPairDevice::GetSelectedCeremonies()
    {
        DevicePairingKinds ceremonySelection = DevicePairingKinds::None;
        if (confirmOnlyOption().IsChecked().Value()) ceremonySelection |= DevicePairingKinds::ConfirmOnly;
        if (displayPinOption().IsChecked().Value()) ceremonySelection |= DevicePairingKinds::DisplayPin;
        if (providePinOption().IsChecked().Value()) ceremonySelection |= DevicePairingKinds::ProvidePin;
        if (confirmPinMatchOption().IsChecked().Value()) ceremonySelection |= DevicePairingKinds::ConfirmPinMatch;
        if (passwordCredentialOption().IsChecked().Value()) ceremonySelection |= DevicePairingKinds::ProvidePasswordCredential;
        return ceremonySelection;
    }

    void Scenario9_CustomPairDevice::UpdatePairingButtons()
    {
        auto item = resultsListView().SelectedItem();
        DeviceInformationDisplay* deviceInfoDisp = FromInspectable<DeviceInformationDisplay>(item);
        if (deviceInfoDisp != nullptr &&
            deviceInfoDisp->DeviceInformation().Pairing().CanPair() &&
            !deviceInfoDisp->DeviceInformation().Pairing().IsPaired())
        {
            pairButton().IsEnabled(true);
        }
        else
        {
            pairButton().IsEnabled(false);
        }

        if (deviceInfoDisp != nullptr &&
            deviceInfoDisp->DeviceInformation().Pairing().IsPaired())
        {
            unpairButton().IsEnabled(true);
        }
        else
        {
            unpairButton().IsEnabled(false);
        }
    }
}


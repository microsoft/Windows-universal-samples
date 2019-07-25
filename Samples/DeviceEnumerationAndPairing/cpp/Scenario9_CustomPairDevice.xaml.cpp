// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario9_CustomPairDevice.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Security::Credentials;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario9_CustomPairDevice::Scenario9_CustomPairDevice()
{
    InitializeComponent();
    deviceWatcherHelper = ref new DeviceWatcherHelper(resultCollection, Dispatcher);
    deviceWatcherHelper->DeviceChanged += ref new TypedEventHandler<DeviceWatcher^, String^>(this, &Scenario9_CustomPairDevice::OnDeviceListChanged);
}

void Scenario9_CustomPairDevice::OnNavigatedTo(NavigationEventArgs^ e)
{
    resultsListView->ItemsSource = resultCollection;

    selectorComboBox->ItemsSource = DeviceSelectorChoices::PairingSelectors;
    selectorComboBox->SelectedIndex = 0;

    protectionLevelComboBox->ItemsSource = ProtectionSelectorChoices::Selectors;
    protectionLevelComboBox->SelectedIndex = 0;
}

void Scenario9_CustomPairDevice::OnNavigatedFrom(NavigationEventArgs^ e)
{
    deviceWatcherHelper->Reset();
}

void Scenario9_CustomPairDevice::StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StartWatcher();
}

void Scenario9_CustomPairDevice::StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopWatcher();
}

void Scenario9_CustomPairDevice::StartWatcher()
{
    startWatcherButton->IsEnabled = false;
    resultCollection->Clear();

    // Get the device selector chosen by the UI then add additional constraints for devices that 
    // can be paired or are already paired.
    DeviceSelectorInfo^ deviceSelectorInfo = static_cast<DeviceSelectorInfo^>(selectorComboBox->SelectedItem);
    String^ selector = "(" + deviceSelectorInfo->Selector + ")" + " AND (System.Devices.Aep.CanPair:=System.StructuredQueryType.Boolean#True OR System.Devices.Aep.IsPaired:=System.StructuredQueryType.Boolean#True)";

    DeviceWatcher^ deviceWatcher;
    if (deviceSelectorInfo->Kind == DeviceInformationKind::Unknown)
    {
        // Kind will be determined by the selector
        deviceWatcher = DeviceInformation::CreateWatcher(
            selector,
            nullptr); // don't request additional properties for this sample
    }
    else
    {
        // Kind is specified in the selector info
        deviceWatcher = DeviceInformation::CreateWatcher(
            selector,
            nullptr, // don't request additional properties for this sample
            deviceSelectorInfo->Kind);
    }

    rootPage->NotifyUser("Starting Watcher...", NotifyType::StatusMessage);
    deviceWatcherHelper->StartWatcher(deviceWatcher);
    stopWatcherButton->IsEnabled = true;
}

void Scenario9_CustomPairDevice::OnDeviceListChanged(DeviceWatcher^ sender, String^ id)
{
    // If the item being updated is currently "selected", then update the pairing buttons
    DeviceInformationDisplay^ selectedDeviceInfoDisp = safe_cast<DeviceInformationDisplay^>(resultsListView->SelectedItem);
    if ((selectedDeviceInfoDisp != nullptr) && (selectedDeviceInfoDisp->Id == id))
    {
        UpdatePairingButtons();
    }
}

void Scenario9_CustomPairDevice::StopWatcher()
{
    stopWatcherButton->IsEnabled = false;

    deviceWatcherHelper->StopWatcher();

    startWatcherButton->IsEnabled = true;
}

void Scenario9_CustomPairDevice::PairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Gray out the pair button and results view while pairing is in progress.
    resultsListView->IsEnabled = false;
    pairButton->IsEnabled = false;
    rootPage->NotifyUser("Pairing started. Please wait...", NotifyType::StatusMessage);

    DeviceInformationDisplay^ deviceInfoDisp = static_cast<DeviceInformationDisplay^>(resultsListView->SelectedItem);

    // Get ceremony type and protection level selections
    DevicePairingKinds ceremoniesSelected = GetSelectedCeremonies();
    ProtectionLevelSelectorInfo^ protectionLevelInfo = static_cast<ProtectionLevelSelectorInfo^>(protectionLevelComboBox->SelectedItem);
    DevicePairingProtectionLevel protectionLevel = protectionLevelInfo->ProtectionLevel;

    DeviceInformationCustomPairing^ customPairing = deviceInfoDisp->DeviceInfo->Pairing->Custom;

    // Hook up handlers for the pairing events before starting the pairing
    EventRegistrationToken handlerPairingRequestedToken = customPairing->PairingRequested += ref new TypedEventHandler<DeviceInformationCustomPairing^, DevicePairingRequestedEventArgs^>(this, &Scenario9_CustomPairDevice::PairingRequestedHandler);

    create_task(customPairing->PairAsync(ceremoniesSelected, protectionLevel)).then(
        [this, customPairing, handlerPairingRequestedToken](DevicePairingResult^ pairingResult)
    {
        rootPage->NotifyUser(
            "Pairing result = " + (pairingResult->Status == DevicePairingResultStatus::Paired ? "Paired" : "NotPaired"),
            pairingResult->Status == DevicePairingResultStatus::Paired ? NotifyType::StatusMessage : NotifyType::ErrorMessage);

        customPairing->PairingRequested -= handlerPairingRequestedToken;

        HidePairingPanel();
        UpdatePairingButtons();
        resultsListView->IsEnabled = true;
    }, task_continuation_context::use_current());
}

void Scenario9_CustomPairDevice::UnpairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Gray out the unpair button and results view while unpairing is in progress.
    resultsListView->IsEnabled = false;
    unpairButton->IsEnabled = false;
    rootPage->NotifyUser("Unpairing started. Please wait...", NotifyType::StatusMessage);

    DeviceInformationDisplay^ deviceInfoDisp = static_cast<DeviceInformationDisplay^>(resultsListView->SelectedItem);

    create_task(deviceInfoDisp->DeviceInfo->Pairing->UnpairAsync()).then(
        [this](DeviceUnpairingResult^ unpairingResult)
    {
        rootPage->NotifyUser(
            "Unpairing result = " + (unpairingResult->Status == DeviceUnpairingResultStatus::Unpaired ? "Unpaired" : "Failed"),
            unpairingResult->Status == DeviceUnpairingResultStatus::Unpaired ? NotifyType::StatusMessage : NotifyType::ErrorMessage);

        UpdatePairingButtons();
        resultsListView->IsEnabled = true;
    }, task_continuation_context::use_current());
}

void Scenario9_CustomPairDevice::PairingRequestedHandler(DeviceInformationCustomPairing^ sender, DevicePairingRequestedEventArgs^ args)
{
    switch (args->PairingKind)
    {
    case DevicePairingKinds::ConfirmOnly:
        // Windows itself will pop the confirmation dialog as part of "consent" if this is running on Desktop or Mobile
        // If this is an App for 'Windows IoT Core' where there is no Windows Consent UX, you may want to provide your own confirmation.
        args->Accept();
        break;

    case DevicePairingKinds::DisplayPin:
        // We just show the PIN on this side. The ceremony is actually completed when the user enters the PIN
        // on the target device. We automatically accept here since we can't really "cancel" the operation
        // from this side.
        args->Accept();

        Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
            [this, args]()
        {
            ShowPairingPanel(
                "Please enter this PIN on the device you are pairing with: " + args->Pin,
                args->PairingKind);
        }));
        break;

    case DevicePairingKinds::ProvidePin:
        {
            // A PIN may be shown on the target device and the user needs to enter the matching PIN on
            // this Windows device. Get a deferral so we can perform the async request to the user.
            auto collectPinDeferral = args->GetDeferral();

            Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
                [this, collectPinDeferral, args]()
            {
                GetPinFromUserAsync().then([this, collectPinDeferral, args](String^ pin)
                {
                    if (pin != nullptr)
                    {
                        args->Accept(pin);
                    }

                    collectPinDeferral->Complete();
                }, task_continuation_context::use_current());
            }));
        }
        break;

    case DevicePairingKinds::ProvidePasswordCredential:
        {
            auto collectCredentialDeferral = args->GetDeferral();
            rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
                [this, collectCredentialDeferral, args]()
            {
                GetPasswordCredentialFromUserAsync().then([this, collectCredentialDeferral, args](PasswordCredential^ credential)
                {
                    if (credential != nullptr)
                    {
                        args->AcceptWithPasswordCredential(credential);
                    }
                    collectCredentialDeferral->Complete();

                }, task_continuation_context::use_current());
            }));
        }
        break;

    case DevicePairingKinds::ConfirmPinMatch:
        {
            // We show the PIN here and the user responds with whether the PIN matches what they see
            // on the target device. Response comes back and we set it on the PinComparePairingRequestedData
            // then complete the deferral.
            auto displayMessageDeferral = args->GetDeferral();

            Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
                [this, displayMessageDeferral, args]()
            {
                GetUserConfirmationAsync(args->Pin).then([this, displayMessageDeferral, args](bool accept)
                {
                    if (accept)
                    {
                        args->Accept();
                    }

                    displayMessageDeferral->Complete();
                }, task_continuation_context::use_current());
            }));
        }
        break;
    }
}

void Scenario9_CustomPairDevice::ShowPairingPanel(Platform::String^ text, Windows::Devices::Enumeration::DevicePairingKinds pairingKind)
{
    pairingPanel->Visibility = ::Visibility::Collapsed;
    pinEntryTextBox->Visibility = ::Visibility::Collapsed;
    okButton->Visibility = ::Visibility::Collapsed;
    usernameEntryTextBox->Visibility = ::Visibility::Collapsed;
    passwordEntryTextBox->Visibility = ::Visibility::Collapsed;
    verifyButton->Visibility = ::Visibility::Collapsed;
    yesButton->Visibility = ::Visibility::Collapsed;
    noButton->Visibility = ::Visibility::Collapsed;
    pairingTextBlock->Text = text;

    switch (pairingKind)
    {
    case DevicePairingKinds::ConfirmOnly:
    case DevicePairingKinds::DisplayPin:
        // Don't need any buttons
        break;
    case DevicePairingKinds::ProvidePin:
        pinEntryTextBox->Text = "";
        pinEntryTextBox->Visibility = ::Visibility::Visible;
        okButton->Visibility = ::Visibility::Visible;
        break;
    case DevicePairingKinds::ConfirmPinMatch:
        yesButton->Visibility = ::Visibility::Visible;
        noButton->Visibility = ::Visibility::Visible;
        break;
    case DevicePairingKinds::ProvidePasswordCredential:
        usernameEntryTextBox->Text = "";
        passwordEntryTextBox->Text = "";
        passwordEntryTextBox->Visibility = ::Visibility::Visible;
        usernameEntryTextBox->Visibility = ::Visibility::Visible;
        verifyButton->Visibility = ::Visibility::Visible;
        break;
    }

    pairingPanel->Visibility = ::Visibility::Visible;
}

void Scenario9_CustomPairDevice::HidePairingPanel()
{
    pairingPanel->Visibility = ::Visibility::Collapsed;
    pairingTextBlock->Text = "";
}

concurrency::task<Platform::String^> Scenario9_CustomPairDevice::GetPinFromUserAsync()
{
    HidePairingPanel();
    CompleteProvidePinTask();
    providePinTaskSrc = task_completion_event<String^>();

    ShowPairingPanel(
        "Please enter the PIN shown on the device you're pairing with",
        DevicePairingKinds::ProvidePin);

    return concurrency::task<String^>(providePinTaskSrc);
}

void Scenario9_CustomPairDevice::CompleteProvidePinTask(Platform::String^ pin)
{
    providePinTaskSrc.set(pin);
}

concurrency::task<PasswordCredential^> Scenario9_CustomPairDevice::GetPasswordCredentialFromUserAsync()
{
    HidePairingPanel();
    CompletePasswordCredential(); // Abandon any previous pin request.

    ShowPairingPanel(
        "Please enter the username and password",
        DevicePairingKinds::ProvidePasswordCredential);

    providePasswordCredentialSrc = task_completion_event<PasswordCredential^>();

    return concurrency::task<PasswordCredential^>(providePasswordCredentialSrc);
}

void Scenario9_CustomPairDevice::CompletePasswordCredential(Platform::String^ username, Platform::String^ password)
{
    if (username)
    {
        auto passwordCredential = ref new PasswordCredential();
        passwordCredential->UserName = username;
        passwordCredential->Password = password;
        providePasswordCredentialSrc.set(passwordCredential);
    }
    else
    {
        providePasswordCredentialSrc.set(nullptr);
    }
}

concurrency::task<bool> Scenario9_CustomPairDevice::GetUserConfirmationAsync(Platform::String^ pin)
{
    HidePairingPanel();
    CompleteConfirmPinTask(false);
    confirmPinTaskSrc = task_completion_event<bool>();

    ShowPairingPanel(
        "Does the following PIN match the one shown on the device you are pairing?: " + pin,
        DevicePairingKinds::ConfirmPinMatch);

    return concurrency::task<bool>(confirmPinTaskSrc);
}

void Scenario9_CustomPairDevice::CompleteConfirmPinTask(bool accept)
{
    confirmPinTaskSrc.set(accept);
}

void Scenario9_CustomPairDevice::okButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // OK button is only used for the ProvidePin scenario
    CompleteProvidePinTask(pinEntryTextBox->Text);
    HidePairingPanel();
}

void SDKTemplate::Scenario9_CustomPairDevice::verifyButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // verify button is only used for the ProvidePin scenario
    CompletePasswordCredential(usernameEntryTextBox->Text, passwordEntryTextBox->Text);
    HidePairingPanel();
}

void Scenario9_CustomPairDevice::yesButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    CompleteConfirmPinTask(true);
    HidePairingPanel();
}

void Scenario9_CustomPairDevice::noButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    CompleteConfirmPinTask(false);
    HidePairingPanel();
}

DevicePairingKinds Scenario9_CustomPairDevice::GetSelectedCeremonies()
{
    DevicePairingKinds ceremonySelection = DevicePairingKinds::None;

    if (confirmOnlyOption->IsChecked->Value)
    {
        ceremonySelection = ceremonySelection | DevicePairingKinds::ConfirmOnly;
    }
    if (displayPinOption->IsChecked->Value)
    {
        ceremonySelection = ceremonySelection | DevicePairingKinds::DisplayPin;
    }
    if (providePinOption->IsChecked->Value)
    {
        ceremonySelection = ceremonySelection | DevicePairingKinds::ProvidePin;
    }
    if (confirmPinMatchOption->IsChecked->Value)
    {
        ceremonySelection = ceremonySelection | DevicePairingKinds::ConfirmOnly;
    }
    if (passwordCredentialOption->IsChecked->Value)
    {
        ceremonySelection = ceremonySelection | DevicePairingKinds::ProvidePasswordCredential;
    }

    return ceremonySelection;
}

void Scenario9_CustomPairDevice::ResultsListView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    UpdatePairingButtons();
}

void Scenario9_CustomPairDevice::UpdatePairingButtons()
{
    DeviceInformationDisplay^ deviceInfoDisp = static_cast<DeviceInformationDisplay^>(resultsListView->SelectedItem);

    if (nullptr != deviceInfoDisp &&
        deviceInfoDisp->DeviceInfo->Pairing->CanPair &&
        !deviceInfoDisp->DeviceInfo->Pairing->IsPaired)
    {
        pairButton->IsEnabled = true;
    }
    else
    {
        pairButton->IsEnabled = false;
    }

    if (nullptr != deviceInfoDisp &&
        deviceInfoDisp->DeviceInfo->Pairing->IsPaired)
    {
        unpairButton->IsEnabled = true;
    }
    else
    {
        unpairButton->IsEnabled = false;
    }
}


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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario9::Scenario9() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

void Scenario9::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResultCollection = ref new Vector<DeviceInformationDisplay^>();

    selectorComboBox->ItemsSource = DeviceSelectorChoices::PairingSelectors;
    selectorComboBox->SelectedIndex = 0;

    protectionLevelComboBox->ItemsSource = ProtectionSelectorChoices::Selectors;
    protectionLevelComboBox->SelectedIndex = 0;

    DataContext = this;
}

void Scenario9::OnNavigatedFrom(NavigationEventArgs^ e)
{
    StopWatcher();
}

void Scenario9::StartWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StartWatcher();
}

void Scenario9::StopWatcherButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StopWatcher();
}

void Scenario9::StartWatcher()
{
    startWatcherButton->IsEnabled = false;
    ResultCollection->Clear();

    // Get the device selector chosen by the UI then add additional constraints for devices that 
    // can be paired or are already paired.
    DeviceSelectorInfo^ deviceSelectorInfo = (DeviceSelectorInfo^)selectorComboBox->SelectedItem;
    String^ selector = "(" + deviceSelectorInfo->Selector + ")" + " AND (System.Devices.Aep.CanPair:=System.StructuredQueryType.Boolean#True OR System.Devices.Aep.IsPaired:=System.StructuredQueryType.Boolean#True)";

    if (deviceSelectorInfo->Kind == DeviceInformationKind::Unknown)
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
            deviceSelectorInfo->Kind);
    }

    // Hook up handlers for the watcher events before starting the watcher
    auto handlerAdded = ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(
        [this](DeviceWatcher^ sender, DeviceInformation^ deviceInfo)
    {
        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, deviceInfo]()
        {
            ResultCollection->Append(ref new DeviceInformationDisplay(deviceInfo));

            rootPage->NotifyUser(
                ResultCollection->Size.ToString() + " devices found.",
                NotifyType::StatusMessage);
        }));
    });
    handlerAddedToken = deviceWatcher->Added += handlerAdded;
    
    auto handlerUpdated = ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(
        [this](DeviceWatcher^ sender, DeviceInformationUpdate^ deviceInfoUpdate)
    {
        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, deviceInfoUpdate]()
        {
            // Find the corresponding updated DeviceInformation in the collection and pass the update object
            // to the Update method of the existing DeviceInformation. This automatically updates the object
            // for us.
            auto foundDeviceInfo = std::find_if(begin(ResultCollection), end(ResultCollection), [&](DeviceInformationDisplay^ di) { return (di->Id == deviceInfoUpdate->Id); });

            uint32 index = 0;
            if (foundDeviceInfo != end(ResultCollection))
            {
                (*foundDeviceInfo)->Update(deviceInfoUpdate);

                // If the item being updated is currently "selected", then update the pairing buttons
                DeviceInformationDisplay^ selectedDeviceInfoDisp = (DeviceInformationDisplay^)resultsListView->SelectedItem;
                if ((*foundDeviceInfo) == selectedDeviceInfoDisp)
                {
                    UpdatePairingButtons();
                }
            }
        }));
    });
    handlerUpdatedToken = deviceWatcher->Updated += handlerUpdated;

    auto handlerRemoved = ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(
        [this](DeviceWatcher^ sender, DeviceInformationUpdate^ deviceInfoUpdate)
    {
        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, deviceInfoUpdate]()
        {
            // Find the corresponding DeviceInformation in the collection and remove it
            auto foundDeviceInfo = std::find_if(begin(ResultCollection), end(ResultCollection), [&](DeviceInformationDisplay^ di){return (di->Id == deviceInfoUpdate->Id);});

            uint32 index = 0;
            if (foundDeviceInfo != end(ResultCollection) &&
                ResultCollection->IndexOf(*foundDeviceInfo, &index))
            {
                ResultCollection->RemoveAt(index);
            }

            rootPage->NotifyUser(
                ResultCollection->Size.ToString() + " devices found.",
                NotifyType::StatusMessage);
        }));
    });
    handlerRemovedToken = deviceWatcher->Removed += handlerRemoved;

    auto handlerEnumCompleted = ref new TypedEventHandler<DeviceWatcher^, Object^>(
        [this](DeviceWatcher^ sender, Object^ obj)
    {
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this]()
        {
            rootPage->NotifyUser(
                ResultCollection->Size.ToString() + " devices found. Enumeration completed. Watching for updates...",
                NotifyType::StatusMessage);
        }));
    });
    handlerEnumCompletedToken = deviceWatcher->EnumerationCompleted += handlerEnumCompleted;

    auto handlerStopped = ref new TypedEventHandler<DeviceWatcher^, Object^>(
        [this](DeviceWatcher^ sender, Object^ obj)
    {
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, sender]()
        {
            String^ output = ResultCollection->Size.ToString();
            output += " devices found. Watcher ";
            output += (DeviceWatcherStatus::Aborted == sender->Status) ? "aborted" : "stopped";

            rootPage->NotifyUser(output, NotifyType::StatusMessage);
        }));
    });
    handlerStoppedAddedToken = deviceWatcher->Stopped += handlerStopped;

    rootPage->NotifyUser("Starting Watcher...", NotifyType::StatusMessage);
    deviceWatcher->Start();
    stopWatcherButton->IsEnabled = true;
}

void Scenario9::StopWatcher()
{
    stopWatcherButton->IsEnabled = false;

    if (nullptr != deviceWatcher)
    {
        // First unhook all event handlers except the stopped handler. This ensures our
        // event handlers don't get called after stop, as stop won't block for any "in flight" 
        // event handler calls.  We leave the stopped handler as it's guaranteed to only be called
        // once and we'll use it to know when the query is completely stopped. 
        deviceWatcher->Added -= handlerAddedToken;
        deviceWatcher->Updated -= handlerUpdatedToken;
        deviceWatcher->Removed -= handlerRemovedToken;
        deviceWatcher->EnumerationCompleted -= handlerEnumCompletedToken;

        if (DeviceWatcherStatus::Started == deviceWatcher->Status ||
            DeviceWatcherStatus::EnumerationCompleted == deviceWatcher->Status)
        {
            deviceWatcher->Stop();
        }
    }

    startWatcherButton->IsEnabled = true;
}

void Scenario9::PairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Gray out the pair button and results view while pairing is in progress.
    resultsListView->IsEnabled = false;
    pairButton->IsEnabled = false;
    rootPage->NotifyUser("Pairing started. Please wait...", NotifyType::StatusMessage);

    DeviceInformationDisplay^ deviceInfoDisp = (DeviceInformationDisplay^)resultsListView->SelectedItem;

    // Get ceremony type and protection level selections
    DevicePairingKinds ceremoniesSelected = GetSelectedCeremonies();
    ProtectionLevelSelectorInfo^ protectionLevelInfo = (ProtectionLevelSelectorInfo^)protectionLevelComboBox->SelectedItem;
    DevicePairingProtectionLevel protectionLevel = protectionLevelInfo->ProtectionLevel;

    DeviceInformationCustomPairing^ customPairing = deviceInfoDisp->DeviceInfo->Pairing->Custom;

    // Hook up handlers for the pairing events before starting the pairing
    auto handlePairingRequested = ref new TypedEventHandler<DeviceInformationCustomPairing^, DevicePairingRequestedEventArgs^>(this, &Scenario9::PairingRequestedHandler);
    handlerPairingRequestedToken = customPairing->PairingRequested += handlePairingRequested;

    create_task(customPairing->PairAsync(ceremoniesSelected, protectionLevel)).then(
        [this, customPairing](DevicePairingResult^ pairingResult)
    {
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, pairingResult]()
        {
            rootPage->NotifyUser(
                "Pairing result = " + (pairingResult->Status == DevicePairingResultStatus::Paired ? "Paired" : "NotPaired"),
                pairingResult->Status == DevicePairingResultStatus::Paired ? NotifyType::StatusMessage : NotifyType::ErrorMessage);
        }));

        customPairing->PairingRequested -= handlerPairingRequestedToken;

        HidePairingPanel();
        UpdatePairingButtons();
        resultsListView->IsEnabled = true;
    }, task_continuation_context::use_current());
}

void Scenario9::UnpairButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Gray out the unpair button and results view while unpairing is in progress.
    resultsListView->IsEnabled = false;
    unpairButton->IsEnabled = false;
    rootPage->NotifyUser("Unpairing started. Please wait...", NotifyType::StatusMessage);

    DeviceInformationDisplay^ deviceInfoDisp = (DeviceInformationDisplay^)resultsListView->SelectedItem;

    create_task(deviceInfoDisp->DeviceInfo->Pairing->UnpairAsync()).then(
        [this](DeviceUnpairingResult^ unpairingResult)
    {
        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Low, ref new DispatchedHandler(
            [this, unpairingResult]()
        {
            rootPage->NotifyUser(
                "Unpairing result = " + (unpairingResult->Status == DeviceUnpairingResultStatus::Unpaired ? "Unpaired" : "Failed"),
                unpairingResult->Status == DeviceUnpairingResultStatus::Unpaired ? NotifyType::StatusMessage : NotifyType::ErrorMessage);
        }));

        UpdatePairingButtons();
        resultsListView->IsEnabled = true;
    }, task_continuation_context::use_current());
}

void Scenario9::PairingRequestedHandler(DeviceInformationCustomPairing^ sender, DevicePairingRequestedEventArgs^ args)
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

        rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
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

            rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
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

    case DevicePairingKinds::ConfirmPinMatch:
        {
            // We show the PIN here and the user responds with whether the PIN matches what they see
            // on the target device. Response comes back and we set it on the PinComparePairingRequestedData
            // then complete the deferral.
            auto displayMessageDeferral = args->GetDeferral();

            rootPage->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
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


void Scenario9::ShowPairingPanel(Platform::String^ text, Windows::Devices::Enumeration::DevicePairingKinds pairingKind)
{
    pairingPanel->Visibility = ::Visibility::Collapsed;
    pinEntryTextBox->Visibility = ::Visibility::Collapsed;
    okButton->Visibility = ::Visibility::Collapsed;
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
    }

    pairingPanel->Visibility = ::Visibility::Visible;
}

void Scenario9::HidePairingPanel()
{
    pairingPanel->Visibility = ::Visibility::Collapsed;
    pairingTextBlock->Text = "";
}

concurrency::task<Platform::String^> Scenario9::GetPinFromUserAsync()
{
    HidePairingPanel();
    CompleteProvidePinTask();
    providePinTaskSrc.reset(new task_completion_event<String^>());

    ShowPairingPanel(
        "Please enter the PIN shown on the device you're pairing with",
        DevicePairingKinds::ProvidePin);

    return concurrency::task<String^>(*providePinTaskSrc);
}

void Scenario9::CompleteProvidePinTask(Platform::String^ pin)
{
    if (providePinTaskSrc)
    {
        providePinTaskSrc->set(pin);
        providePinTaskSrc.reset();
    }
}

concurrency::task<bool> Scenario9::GetUserConfirmationAsync(Platform::String^ pin)
{
    HidePairingPanel();
    CompleteConfirmPinTask(false);
    confirmPinTaskSrc.reset(new task_completion_event<bool>());

    ShowPairingPanel(
        "Does the following PIN match the one shown on the device you are pairing?: " + pin,
        DevicePairingKinds::ConfirmPinMatch);

    return concurrency::task<bool>(*confirmPinTaskSrc);
}

void Scenario9::CompleteConfirmPinTask(bool accept)
{
    if (confirmPinTaskSrc)
    {
        confirmPinTaskSrc->set(accept);
        confirmPinTaskSrc.reset();
    }
}

void Scenario9::okButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // OK button is only used for the ProvidePin scenario
    CompleteProvidePinTask(pinEntryTextBox->Text);
    HidePairingPanel();
}

void Scenario9::yesButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    CompleteConfirmPinTask(true);
    HidePairingPanel();
}

void Scenario9::noButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    CompleteConfirmPinTask(false);
    HidePairingPanel();
}

DevicePairingKinds Scenario9::GetSelectedCeremonies()
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

    return ceremonySelection;
}

void Scenario9::ResultsListView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    UpdatePairingButtons();
}

void Scenario9::UpdatePairingButtons()
{
    DeviceInformationDisplay^ deviceInfoDisp = (DeviceInformationDisplay^)resultsListView->SelectedItem;

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


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
#include "Scenario1_BankCards.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238


Scenario1_BankCards::Scenario1_BankCards()
{
    InitializeComponent();
    rootPage = MainPage::Current;
}

/// <summary>
/// Creates the default magnetic stripe reader.
/// </summary>
task<void> Scenario1_BankCards::CreateDefaultReaderObject()
{
    return create_task(MagneticStripeReader::GetDefaultAsync()).then([this](MagneticStripeReader^ reader)
    {
        _reader = reader;
        if (_reader == nullptr)
        {
            rootPage->NotifyUser("Magnetic Stripe Reader not found. Please connect a Magnetic Stripe Reader.", NotifyType::ErrorMessage);
        }
    });
}

/// <summary>
/// This method claims the magnetic stripe reader.
/// </summary>
task<void> Scenario1_BankCards::ClaimReader()
{
    // claim the magnetic stripe reader
    return create_task(_reader->ClaimReaderAsync()).then([this](ClaimedMagneticStripeReader^ claimedReader)
    {
        _claimedReader = claimedReader;
        if (_claimedReader == nullptr)
        {
            rootPage->NotifyUser("Claim Magnetic Stripe Reader failed.", NotifyType::ErrorMessage);
        }
    });
}

void Scenario1_BankCards::ScenarioStartReadButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    create_task(CreateDefaultReaderObject()).then([this](void)
    {
        if (_reader != nullptr)
        {
            // after successful creation, claim the reader for exclusive use and enable it so that data reveived events are received.
            create_task(ClaimReader()).then([this](void)
            {
                if (_claimedReader)
                {
                    // It is always a good idea to have a release device requested event handler. If this event is not handled, there are chances of another app can 
                    // claim ownsership of the magnetic stripe reader.
                    _releaseDeviceRequestedToken = _claimedReader->ReleaseDeviceRequested::add(ref new EventHandler<ClaimedMagneticStripeReader^>(this, &Scenario1_BankCards::OnReleaseDeviceRequested));

                    // after successfully claiming and enabling, attach the BankCardDataReceived event handler.
                    // Note: If the scanner is not enabled (i.e. EnableAsync not called), attaching the event handler will not be any useful because the API will not fire the event 
                    // if the claimedScanner has not beed Enabled
                    _bankCardDataReceivedToken = _claimedReader->BankCardDataReceived::add(ref new TypedEventHandler<ClaimedMagneticStripeReader^, MagneticStripeReaderBankCardDataReceivedEventArgs^>(this, &Scenario1_BankCards::OnBankCardDataReceived));

                    // Ask the API to decode the data by default. By setting this, API will decode the raw data from the magnetic stripe reader
                    _claimedReader->IsDecodeDataEnabled = true;

                    create_task(_claimedReader->EnableAsync()).then([this](void)
                    {
                        rootPage->NotifyUser("Ready to swipe. Device ID: " + _claimedReader->DeviceId, NotifyType::StatusMessage);

                        // reset the button state
                        ScenarioEndReadButton->IsEnabled = true;
                        ScenarioStartReadButton->IsEnabled = false;
                    });
                }
            });
        }
    });
}

/// <summary>
/// Event handler for End button
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_BankCards::ScenarioEndReadButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ResetTheScenarioState();
}

/// <summary>
/// Event handler for the Release Device Requested event fired when magnetic stripe reader receives Claim request from another application
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the ClaimedMagneticStripeReader that is sending this request</param>
void Scenario1_BankCards::OnReleaseDeviceRequested(Platform::Object ^sender, Windows::Devices::PointOfService::ClaimedMagneticStripeReader ^args)
{
    // let us retain the device always. If it is not retained, this exclusive claim will be lost.
    args->RetainDevice();
}

/// <summary>
/// Event handler for the BankCardDataReceived event fired when a bank card is read by the magnetic stripe reader 
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the MagneticStripeReaderBankCardDataReceivedEventArgs which contains the data obtained in the swipe</param>
void Scenario1_BankCards::OnBankCardDataReceived(Windows::Devices::PointOfService::ClaimedMagneticStripeReader^sender, Windows::Devices::PointOfService::MagneticStripeReaderBankCardDataReceivedEventArgs^args)
{
    // read the data and display
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
        [this, args]()
    {
        rootPage->NotifyUser("Got data.", NotifyType::StatusMessage);

        ScenarioOutputAccountNumber->Text = args->AccountNumber;
        ScenarioOutputExpirationDate->Text = args->ExpirationDate;
        ScenarioOutputFirstName->Text = args->FirstName;
        ScenarioOutputMiddleInitial->Text = args->MiddleInitial;
        ScenarioOutputServiceCode->Text = args->ServiceCode;
        ScenarioOutputSuffix->Text = args->Suffix;
        ScenarioOutputSurname->Text = args->Surname;
        ScenarioOutputTitle->Text = args->Title;
    }));

}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1_BankCards::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
    Page::OnNavigatedTo(e);
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario1_BankCards::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
    Page::OnNavigatedFrom(e);
}

/// <summary>
/// Reset the Scenario state
/// </summary>
void Scenario1_BankCards::ResetTheScenarioState()
{
    if (_claimedReader != nullptr)
    {
        // Detach the datareceived event handler and releasedevicerequested event handler
        _claimedReader->BankCardDataReceived::remove(_bankCardDataReceivedToken);
        _claimedReader->ReleaseDeviceRequested::remove(_releaseDeviceRequestedToken);

        // release the Claimed Magnetic Stripe Reader and set to null
        delete _claimedReader;
        _claimedReader = nullptr;
    }

    if (_reader != nullptr)
    {
        // release the Magnetic Stripe Reader and set to null
        delete _reader;
        _reader = nullptr;
    }

    // Reset the strings in the UI
    rootPage->NotifyUser("Click the Start Receiving Data Button.", NotifyType::StatusMessage);

    ScenarioOutputAccountNumber->Text = "No data";
    ScenarioOutputExpirationDate->Text = "No data";
    ScenarioOutputFirstName->Text = "No data";
    ScenarioOutputMiddleInitial->Text = "No data";
    ScenarioOutputServiceCode->Text = "No data";
    ScenarioOutputSuffix->Text = "No data";
    ScenarioOutputSurname->Text = "No data";
    ScenarioOutputTitle->Text = "No data";

    // reset the button state
    ScenarioEndReadButton->IsEnabled = false;
    ScenarioStartReadButton->IsEnabled = true;
}

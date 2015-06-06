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
#include "Scenario2_AamvaCards.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
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


Scenario2_AamvaCards::Scenario2_AamvaCards()
{
    InitializeComponent();
    rootPage = MainPage::Current;
}

/// <summary>
/// Creates the default magnetic stripe reader.
/// </summary>
task<void> Scenario2_AamvaCards::CreateDefaultReaderObject()
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
task<void> Scenario2_AamvaCards::ClaimReader()
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

void Scenario2_AamvaCards::ScenarioStartReadButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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
                    _releaseDeviceRequestedToken = _claimedReader->ReleaseDeviceRequested::add(ref new EventHandler<ClaimedMagneticStripeReader^>(this, &Scenario2_AamvaCards::OnReleaseDeviceRequested));

                    // after successfully claiming and enabling, attach the AamvaCardDataReceived event handler.
                    // Note: If the scanner is not enabled (i.e. EnableAsync not called), attaching the event handler will not be any useful because the API will not fire the event 
                    // if the claimedScanner has not beed Enabled
                    _aamvaCardDataReceivedToken = _claimedReader->AamvaCardDataReceived::add(ref new TypedEventHandler<ClaimedMagneticStripeReader^, MagneticStripeReaderAamvaCardDataReceivedEventArgs^>(this, &Scenario2_AamvaCards::OnAamvaCardDataReceived));

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
void Scenario2_AamvaCards::ScenarioEndReadButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ResetTheScenarioState();
}

/// <summary>
/// Event handler for the Release Device Requested event fired when magnetic stripe reader receives Claim request from another application
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the ClaimedMagneticStripeReader that is sending this request</param>
void Scenario2_AamvaCards::OnReleaseDeviceRequested(Platform::Object ^sender, Windows::Devices::PointOfService::ClaimedMagneticStripeReader ^args)
{
    // let us retain the device always. If it is not retained, this exclusive claim will be lost.
    args->RetainDevice();
}

/// <summary>
/// Event handler for the DataReceived event fired when a AAMVA card is read by the magnetic stripe reader 
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the MagneticStripeReaderAamvaCardDataReceivedEventArgs which contains the data obtained in the scan</param>
void Scenario2_AamvaCards::OnAamvaCardDataReceived(Windows::Devices::PointOfService::ClaimedMagneticStripeReader^sender, Windows::Devices::PointOfService::MagneticStripeReaderAamvaCardDataReceivedEventArgs^args)
{
    // read the data and display
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
        [this, args]()
    {
        rootPage->NotifyUser("Got data.", NotifyType::StatusMessage);

        ScenarioOutputAddress->Text = args->Address;
        ScenarioOutputBirthDate->Text = args->BirthDate;
        ScenarioOutputCity->Text = args->City;
        ScenarioOutputClass->Text = args->Class;
        ScenarioOutputEndorsements->Text = args->Endorsements;
        ScenarioOutputExpirationDate->Text = args->ExpirationDate;
        ScenarioOutputEyeColor->Text = args->EyeColor;
        ScenarioOutputFirstName->Text = args->FirstName;
        ScenarioOutputGender->Text = args->Gender;
        ScenarioOutputHairColor->Text = args->HairColor;
        ScenarioOutputHeight->Text = args->Height;
        ScenarioOutputLicenseNumber->Text = args->LicenseNumber;
        ScenarioOutputPostalCode->Text = args->PostalCode;
        ScenarioOutputRestrictions->Text = args->Restrictions;
        ScenarioOutputState->Text = args->State;
        ScenarioOutputSuffix->Text = args->Suffix;
        ScenarioOutputSurname->Text = args->Surname;
        ScenarioOutputWeight->Text = args->Weight;
    }));

}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2_AamvaCards::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
    Page::OnNavigatedTo(e);
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario2_AamvaCards::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
    Page::OnNavigatedTo(e);
}


/// <summary>
/// Reset the Scenario state
/// </summary>
void Scenario2_AamvaCards::ResetTheScenarioState()
{
    if (_claimedReader != nullptr)
    {
        // Detach the datareceived event handler and releasedevicerequested event handler
        _claimedReader->AamvaCardDataReceived::remove(_aamvaCardDataReceivedToken);
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

    ScenarioOutputAddress->Text = "No data";
    ScenarioOutputBirthDate->Text = "No data";
    ScenarioOutputCity->Text = "No data";
    ScenarioOutputClass->Text = "No data";
    ScenarioOutputEndorsements->Text = "No data";
    ScenarioOutputExpirationDate->Text = "No data";
    ScenarioOutputEyeColor->Text = "No data";
    ScenarioOutputFirstName->Text = "No data";
    ScenarioOutputGender->Text = "No data";
    ScenarioOutputHairColor->Text = "No data";
    ScenarioOutputHeight->Text = "No data";
    ScenarioOutputLicenseNumber->Text = "No data";
    ScenarioOutputPostalCode->Text = "No data";
    ScenarioOutputRestrictions->Text = "No data";
    ScenarioOutputState->Text = "No data";
    ScenarioOutputSuffix->Text = "No data";
    ScenarioOutputSurname->Text = "No data";
    ScenarioOutputWeight->Text = "No data";

    // reset the button state
    ScenarioEndReadButton->IsEnabled = false;
    ScenarioStartReadButton->IsEnabled = true;
}

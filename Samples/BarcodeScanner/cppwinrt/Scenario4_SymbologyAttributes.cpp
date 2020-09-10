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
#include "Scenario4_SymbologyAttributes.h"
#include "Scenario4_SymbologyAttributes.g.cpp"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
    using namespace Windows::Devices::PointOfService;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Controls;
    using namespace Windows::UI::Xaml::Data;
    using namespace Windows::UI::Xaml::Navigation;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario4_SymbologyAttributes::Scenario4_SymbologyAttributes()
    {
        InitializeComponent();

        m_listOfSymbologies = single_threaded_observable_vector<SymbologyListEntry>();
        SymbologyListSource().Source(m_listOfSymbologies);
    }

    void Scenario4_SymbologyAttributes::OnNavigatedTo(NavigationEventArgs const&)
    {
        ResetTheScenarioState();
    }

    void Scenario4_SymbologyAttributes::OnNavigatedFrom(NavigationEventArgs const&)
    {
        ResetTheScenarioState();
    }

    /// <summary>
    /// Event Handler for Start Scan Button Click.
    /// Sets up the barcode scanner to be ready to receive the data events from the scan.
    /// </summary>
    fire_and_forget Scenario4_SymbologyAttributes::ScenarioStartScanButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        ScenarioStartScanButton().IsEnabled(false);

        m_rootPage.NotifyUser(L"Acquiring barcode scanner IInspectable const&.", NotifyType::StatusMessage);

        // create the barcode scanner.
        m_scanner = co_await BarcodeScanner::GetDefaultAsync();

        if (m_scanner)
        {
            // claim the scanner for exclusive use and enable it so that data received events are received.
            m_claimedScanner = co_await m_scanner.ClaimScannerAsync();
            if (m_claimedScanner)
            {
                // It is always a good idea to have a release device requested event handler. If this event is not handled, there are chances of another app can
                // claim ownership of the barcode scanner.
                m_scannerReleaseRequestedToken = m_claimedScanner.ReleaseDeviceRequested(auto_revoke, { get_weak(), &Scenario4_SymbologyAttributes::claimedScanner_ReleaseDeviceRequested });

                // after successfully claiming, attach the datareceived event handler.
                m_scannerDataReceivedToken = m_claimedScanner.DataReceived(auto_revoke, { get_weak(), &Scenario4_SymbologyAttributes::claimedScanner_DataReceived });

                // Ask the API to decode the data by default. By setting this, API will decode the raw data from the barcode scanner and
                // send the ScanDataLabel and ScanDataType in the DataReceived event
                m_claimedScanner.IsDecodeDataEnabled(true);

                // enable the scanner.
                // Note: If the scanner is not enabled (i.e. EnableAsync not called), attaching the event handler will not be any useful because the API will not fire the event
                // if the claimedScanner has not been Enabled
                co_await m_claimedScanner.EnableAsync();

                // after successful claim, list supported symbologies
                auto supportedSymbologies = co_await m_scanner.GetSupportedSymbologiesAsync();
                for (uint32_t symbology : supportedSymbologies)
                {
                    m_listOfSymbologies.Append(SymbologyListEntry{ symbology });
                }

                // reset the button state
                ScenarioEndScanButton().IsEnabled(true);

                m_rootPage.NotifyUser(L"Ready to scan. Device ID: " + m_claimedScanner.DeviceId(), NotifyType::StatusMessage);
            }
            else
            {
                m_scanner.Close();
                m_scanner = nullptr;
                ScenarioStartScanButton().IsEnabled(true);
                m_rootPage.NotifyUser(L"Claim barcode scanner failed.", NotifyType::ErrorMessage);
            }
        }
        else
        {
            ScenarioStartScanButton().IsEnabled(true);
            m_rootPage.NotifyUser(L"Barcode scanner not found. Please connect a barcode scanner.", NotifyType::ErrorMessage);
        }
    }

    /// <summary>
    /// Event handler for the Release Device Requested event fired when barcode scanner receives Claim request from another application
    /// </summary>
    /// <param name="args"> Contains the ClamiedBarcodeScanner that is sending this request</param>
    void Scenario4_SymbologyAttributes::claimedScanner_ReleaseDeviceRequested(IInspectable const&, ClaimedBarcodeScanner const& args)
    {
        // always retain the device
        args.RetainDevice();
        m_rootPage.NotifyUser(L"Event ReleaseDeviceRequested received. Retaining the barcode scanner.", NotifyType::StatusMessage);
    }

    /// <summary>
    /// Event handler for the DataReceived event fired when a barcode is scanned by the barcode scanner
    /// </summary>
    /// <param name="args"> Contains the BarcodeScannerReport which contains the data obtained in the scan</param>
    fire_and_forget Scenario4_SymbologyAttributes::claimedScanner_DataReceived(ClaimedBarcodeScanner const&, BarcodeScannerDataReceivedEventArgs args)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        // read the data from the buffer and convert to string.
        ScenarioOutputScanDataLabel().Text(GetDataLabelString(args.Report().ScanDataLabel(), args.Report().ScanDataType()));
        ScenarioOutputScanData().Text(GetDataString(args.Report().ScanData()));
        ScenarioOutputScanDataType().Text(BarcodeSymbologies::GetName(args.Report().ScanDataType()));
    }

    /// <summary>
    /// Reset the Scenario state
    /// </summary>
    void Scenario4_SymbologyAttributes::ResetTheScenarioState()
    {
        if (m_claimedScanner)
        {
            // Detach the event handlers
            m_scannerDataReceivedToken.revoke();
            m_scannerReleaseRequestedToken.revoke();
            // Release the Barcode Scanner and set to nullptr
            m_claimedScanner.Close();
            m_claimedScanner = nullptr;
        }

        if (m_scanner)
        {
            m_scanner.Close();
            m_scanner = nullptr;
        }

        m_symbologyAttributes = nullptr;

        // Reset the UI if we are still the current page.
        if (Frame().Content() == *this)
        {
            m_rootPage.NotifyUser(L"Click the start scanning button to begin.", NotifyType::StatusMessage);
            ScenarioOutputScanData().Text(L"No data");
            ScenarioOutputScanDataLabel().Text(L"No data");
            ScenarioOutputScanDataType().Text(L"No data");

            // reset the button state
            ScenarioEndScanButton().IsEnabled(false);
            ScenarioStartScanButton().IsEnabled(true);
            SetSymbologyAttributesButton().IsEnabled(false);
            EnableCheckDigit().IsEnabled(false);
            TransmitCheckDigit().IsEnabled(false);
            SetDecodeRangeLimits().IsEnabled(false);

            // reset symbology list
            m_listOfSymbologies.Clear();
        }
    }

    /// <summary>
    /// Event handler for End Scan Button Click.
    /// Releases the Barcode Scanner and resets the text in the UI
    /// </summary>
    void Scenario4_SymbologyAttributes::ScenarioEndScanButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // reset the scenario state
        ResetTheScenarioState();
    }

    /// <summary>
    /// Event handler for Symbology listbox selection changed.
    /// Get symbology attributes and populate attribute UI components
    /// </summary>
    fire_and_forget Scenario4_SymbologyAttributes::SymbologySelection_Changed(IInspectable const&, SelectionChangedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (m_claimedScanner)
        {
            auto symbologyListEntry = SymbologyListBox().SelectedItem().as<SymbologyListEntry>();
            if (symbologyListEntry)
            {
                SetSymbologyAttributesButton().IsEnabled(false);
                try
                {
                    m_symbologyAttributes = co_await m_claimedScanner.GetSymbologyAttributesAsync(symbologyListEntry.Id());
                }
                catch (hresult_error const&)
                {
                    m_symbologyAttributes = nullptr;
                }

                if (m_symbologyAttributes)
                {
                    SetSymbologyAttributesButton().IsEnabled(true);

                    // initialize attributes UIs
                    EnableCheckDigit().IsEnabled(m_symbologyAttributes.IsCheckDigitValidationSupported());
                    EnableCheckDigit().IsChecked(m_symbologyAttributes.IsCheckDigitValidationEnabled());
                    TransmitCheckDigit().IsEnabled(m_symbologyAttributes.IsCheckDigitTransmissionSupported());
                    TransmitCheckDigit().IsChecked(m_symbologyAttributes.IsCheckDigitTransmissionEnabled());
                    SetDecodeRangeLimits().IsEnabled(m_symbologyAttributes.IsDecodeLengthSupported());
                    bool decodeLengthEnabled = (m_symbologyAttributes.DecodeLengthKind() == BarcodeSymbologyDecodeLengthKind::Range);
                    SetDecodeRangeLimits().IsChecked(decodeLengthEnabled);
                    if (decodeLengthEnabled)
                    {
                        MinimumDecodeLength().Value((std::min)(m_symbologyAttributes.DecodeLength1(), m_symbologyAttributes.DecodeLength2()));
                        MaximumDecodeLength().Value((std::max)(m_symbologyAttributes.DecodeLength1(), m_symbologyAttributes.DecodeLength2()));
                    }
                }
                else
                {
                    m_rootPage.NotifyUser(L"Symbology attributes are not available.", NotifyType::ErrorMessage);
                    EnableCheckDigit().IsEnabled(false);
                    TransmitCheckDigit().IsEnabled(false);
                    SetDecodeRangeLimits().IsEnabled(false);
                }
            }
        }
    }

    fire_and_forget Scenario4_SymbologyAttributes::SetSymbologyAttributes_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (m_claimedScanner && m_symbologyAttributes)
        {
            // populate attributes
            if (m_symbologyAttributes.IsCheckDigitValidationSupported())
            {
                m_symbologyAttributes.IsCheckDigitValidationEnabled(EnableCheckDigit().IsChecked().Value());
            }
            if (m_symbologyAttributes.IsCheckDigitTransmissionSupported())
            {
                m_symbologyAttributes.IsCheckDigitTransmissionEnabled(TransmitCheckDigit().IsChecked().Value());
            }
            if (m_symbologyAttributes.IsDecodeLengthSupported())
            {
                if (SetDecodeRangeLimits().IsChecked().Value())
                {
                    m_symbologyAttributes.DecodeLengthKind(BarcodeSymbologyDecodeLengthKind::Range);
                    m_symbologyAttributes.DecodeLength1(static_cast<uint32_t>(MinimumDecodeLength().Value()));
                    m_symbologyAttributes.DecodeLength2(static_cast<uint32_t>(MaximumDecodeLength().Value()));
                }
                else
                {
                    m_symbologyAttributes.DecodeLengthKind(BarcodeSymbologyDecodeLengthKind::AnyLength);
                }
            }

            auto symbologyListEntry = SymbologyListBox().SelectedItem().as<SymbologyListEntry>();
            if (symbologyListEntry)
            {
                bool attributesSet = false;

                try
                {
                    attributesSet = co_await m_claimedScanner.SetSymbologyAttributesAsync(symbologyListEntry.Id(), m_symbologyAttributes);
                }
                catch (hresult_error const&)
                {
                    // Scanner could not set the attributes.
                }

                if (attributesSet)
                {
                    m_rootPage.NotifyUser(L"Attributes set for symbology '" + symbologyListEntry.Name() + L"'", NotifyType::StatusMessage);
                }
                else
                {
                    m_rootPage.NotifyUser(L"Attributes could not be set for symbology '" + symbologyListEntry.Name() + L"'.", NotifyType::ErrorMessage);
                }
            }
            else
            {
                m_rootPage.NotifyUser(L"Select a symbology from the list.", NotifyType::ErrorMessage);
            }
        }
    }
}
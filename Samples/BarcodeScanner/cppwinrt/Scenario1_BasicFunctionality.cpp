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
#include <thread>
#include <mutex>
#include "Scenario1_BasicFunctionality.h"
#include "Scenario1_BasicFunctionality.g.cpp"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Devices;
    using namespace Windows::Devices::Enumeration;
    using namespace Windows::Devices::PointOfService;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Navigation;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario1_BasicFunctionality::Scenario1_BasicFunctionality()
    {
        InitializeComponent();
    }

    void Scenario1_BasicFunctionality::OnNavigatedTo(NavigationEventArgs const&)
    {
        ResetTheScenarioState();
    }

    void Scenario1_BasicFunctionality::OnNavigatedFrom(NavigationEventArgs const&)
    {
        ResetTheScenarioState();
    }

    void Scenario1_BasicFunctionality::ResetTheScenarioState()
    {
        if (m_claimedScanner)
        {
            // Detach the event handlers
            m_claimedScanner.DataReceived(m_claimedScannerDataReceivedToken);
            m_claimedScanner.ReleaseDeviceRequested(m_claimedScannerReleaseDeviceRequestedToken);
            // Release the Barcode Scanner and set to null
            m_claimedScanner = nullptr;
        }

        m_scanner = nullptr;

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
            ScenarioSoftwareTriggerPanel().Visibility(Visibility::Collapsed);
        }
    }

    fire_and_forget Scenario1_BasicFunctionality::ScenarioStartScanButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        ScenarioStartScanButton().IsEnabled(false);

        m_rootPage.NotifyUser(L"Acquiring barcode scanner object.", NotifyType::StatusMessage);

        // Obtain the default barcode scanner.
        m_scanner = co_await winrt::BarcodeScanner::GetDefaultAsync();
        if (m_scanner)
        {
            // after successful creation, claim the scanner for exclusive use and enable it so that data reveived events are received.
            m_claimedScanner = co_await m_scanner.ClaimScannerAsync();

            if (m_claimedScanner)
            {
                // It is always a good idea to have a release device requested event handler. If this event is not handled, there are chances of another app can
                // claim ownership of the barcode scanner.
                m_claimedScannerReleaseDeviceRequestedToken = m_claimedScanner.ReleaseDeviceRequested({ get_weak(), &Scenario1_BasicFunctionality::claimedScanner_ReleaseDeviceRequested });

                // after successfully claiming, attach the datareceived event handler.
                m_claimedScannerDataReceivedToken = m_claimedScanner.DataReceived({ get_weak(), &Scenario1_BasicFunctionality::claimedScanner_DataReceived });

                // Ask the API to decode the data by default. By setting this, API will decode the raw data from the barcode scanner and
                // send the ScanDataLabel and ScanDataType in the DataReceived event
                m_claimedScanner.IsDecodeDataEnabled(true);

                // enable the scanner.
                // The scanner must be enabled in order to receive the DataReceived event.
                co_await m_claimedScanner.EnableAsync();

                m_rootPage.NotifyUser(L"Ready to scan. Device ID: " + m_scanner.DeviceId(), NotifyType::StatusMessage);
                ScenarioEndScanButton().IsEnabled(true);

                // If the scanner is a software scanner, show the software trigger buttons.
                if (!m_scanner.VideoDeviceId().empty())
                {
                    ScenarioSoftwareTriggerPanel().Visibility(Visibility::Visible);
                    ScenarioStartTriggerButton().IsEnabled(true);
                    ScenarioStopTriggerButton().IsEnabled(false);
                }
            }
            else
            {
                m_rootPage.NotifyUser(L"Claim barcode scanner failed.", NotifyType::ErrorMessage);
                ScenarioStartScanButton().IsEnabled(true);
            }
        }
        else
        {
            m_rootPage.NotifyUser(L"Barcode scanner not found. Please connect a barcode scanner.", NotifyType::ErrorMessage);
            ScenarioStartScanButton().IsEnabled(true);
        }
    }

    /// <summary>
    /// Event handler for the Release Device Requested event fired when barcode scanner receives Claim request from another application
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"> Contains the ClamiedBarcodeScanner that is sending this request</param>
    void Scenario1_BasicFunctionality::claimedScanner_ReleaseDeviceRequested(IInspectable const&, ClaimedBarcodeScanner const& e)
    {
        // always retain the device
        e.RetainDevice();

        m_rootPage.NotifyUser(L"Event ReleaseDeviceRequested received. Retaining the barcode scanner.", NotifyType::StatusMessage);
    }

    /// <summary>
    /// Event handler for the DataReceived event fired when a barcode is scanned by the barcode scanner
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"> Contains the BarcodeScannerReport which contains the data obtained in the scan</param>
    fire_and_forget Scenario1_BasicFunctionality::claimedScanner_DataReceived(ClaimedBarcodeScanner const&, BarcodeScannerDataReceivedEventArgs args)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        // read the data from the buffer and convert to string.
        ScenarioOutputScanDataLabel().Text(GetDataLabelString(args.Report().ScanDataLabel(), args.Report().ScanDataType()));
        ScenarioOutputScanData().Text(GetDataString(args.Report().ScanData()));
        ScenarioOutputScanDataType().Text(BarcodeSymbologies::GetName(args.Report().ScanDataType()));
    }

    void Scenario1_BasicFunctionality::ScenarioEndScanButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ResetTheScenarioState();
    }

    fire_and_forget Scenario1_BasicFunctionality::ScenarioStartTriggerButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ScenarioStartTriggerButton().IsEnabled(false);
        co_await m_claimedScanner.StartSoftwareTriggerAsync();
        ScenarioStopTriggerButton().IsEnabled(true);
    }

    fire_and_forget Scenario1_BasicFunctionality::ScenarioStopTriggerButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ScenarioStopTriggerButton().IsEnabled(false);
        co_await m_claimedScanner.StopSoftwareTriggerAsync();
        ScenarioStartTriggerButton().IsEnabled(true);
    }
}

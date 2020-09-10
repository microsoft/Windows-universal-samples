#include "pch.h"
#include "Scenario3_ActiveSymbologies.h"
#include "Scenario3_ActiveSymbologies.g.cpp"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
    using namespace Windows::Devices::PointOfService;
    using namespace Windows::UI::Core;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Data;
    using namespace Windows::UI::Xaml::Controls;
    using namespace Windows::UI::Xaml::Navigation;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario3_ActiveSymbologies::Scenario3_ActiveSymbologies()
    {
        InitializeComponent();

        m_listOfSymbologies = single_threaded_observable_vector<SDKTemplate::SymbologyListEntry>();
        SymbologyListSource().Source(m_listOfSymbologies);
    }

    /// <summary>
    /// Invoked when this page is about to be displayed in a Frame.
    /// </summary>
    void Scenario3_ActiveSymbologies::OnNavigatedTo(NavigationEventArgs const&)
    {
        ResetTheScenarioState();
    }

    /// <summary>
    /// Invoked when this page is no longer displayed.
    /// </summary>
    void Scenario3_ActiveSymbologies::OnNavigatedFrom(NavigationEventArgs const&)
    {
        ResetTheScenarioState();
    }

    /// <summary>
    /// Event Handler for Start Scan Button Click.
    /// Sets up the barcode scanner to be ready to receive the data events from the scan.
    /// </summary>
    fire_and_forget Scenario3_ActiveSymbologies::ScenarioStartScanButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        ScenarioStartScanButton().IsEnabled(false);

        m_rootPage.NotifyUser(L"Acquiring barcode scanner object.", NotifyType::StatusMessage);

        // create the barcode scanner.
        m_scanner = co_await BarcodeScanner::GetDefaultAsync();

        if (m_scanner)
        {
            // Claim the scanner for exclusive use and enable it so raises DataReceived events.
            m_claimedScanner = co_await m_scanner.ClaimScannerAsync();
            if (m_claimedScanner)
            {
                // It is always a good idea to have a release device requested event handler.
                // If this event is not handled, then another app can claim ownership of the barcode scanner.
                m_scannerReleaseRequestedToken = m_claimedScanner.ReleaseDeviceRequested({ get_weak(), &Scenario3_ActiveSymbologies::claimedScanner_ReleaseDeviceRequested });

                // after successfully claiming, attach the datareceived event handler.
                m_scannerDataReceivedToken = m_claimedScanner.DataReceived({ get_weak(), &Scenario3_ActiveSymbologies::claimedScanner_DataReceived });

                // Ask the platform to decode the data by default. When this is set, the platform
                // will decode the raw data from the barcode scanner and include in the
                // BarcodeScannerReport.ScanDataLabel and ScanDataType in the DataReceived event.
                m_claimedScanner.IsDecodeDataEnabled(true);

                // Enable the scanner so it raises DataReceived events.
                // Do this after adding the DataReceived event handler.
                co_await m_claimedScanner.EnableAsync();

                // after successful claim, list supported symbologies
                auto supportedSymbologies = co_await m_scanner.GetSupportedSymbologiesAsync();
                for (uint32_t symbology : supportedSymbologies)
                {
                    m_listOfSymbologies.Append(SymbologyListEntry{ symbology });
                }

                // reset the button state
                ScenarioEndScanButton().IsEnabled(true);
                SetActiveSymbologiesButton().IsEnabled(true);

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
    /// <param name="e"> Contains the ClamiedBarcodeScanner that is sending this request</param>
    void Scenario3_ActiveSymbologies::claimedScanner_ReleaseDeviceRequested(IInspectable const&, ClaimedBarcodeScanner const& e)
    {
        // always retain the device
        e.RetainDevice();
        m_rootPage.NotifyUser(L"Event ReleaseDeviceRequested received. Retaining the barcode scanner.", NotifyType::StatusMessage);
    }


    /// <summary>
    /// Event handler for the DataReceived event fired when a barcode is scanned by the barcode scanner
    /// </summary>
    /// <param name="args"> Contains the BarcodeScannerDataReceivedEventArgs which contains the data obtained in the scan</param>
    fire_and_forget Scenario3_ActiveSymbologies::claimedScanner_DataReceived(ClaimedBarcodeScanner const&, BarcodeScannerDataReceivedEventArgs args)
    {
        auto lifetime = get_strong();

        // need to update the UI data on the dispatcher thread.
        // update the UI with the data received from the scan.
        co_await resume_foreground(Dispatcher());

        // read the data from the buffer and convert to string.
        ScenarioOutputScanDataLabel().Text(GetDataLabelString(args.Report().ScanDataLabel(), args.Report().ScanDataType()));
        ScenarioOutputScanData().Text(GetDataString(args.Report().ScanData()));
        ScenarioOutputScanDataType().Text(BarcodeSymbologies::GetName(args.Report().ScanDataType()));
    }

    /// <summary>
    /// Reset the Scenario state
    /// </summary>
    void Scenario3_ActiveSymbologies::ResetTheScenarioState()
    {
        if (m_claimedScanner)
        {
            // Detach the event handlers
            m_claimedScanner.DataReceived(m_scannerDataReceivedToken);
            m_claimedScanner.ReleaseDeviceRequested(m_scannerReleaseRequestedToken);

            // Release the Barcode Scanner and set to nullptr
            m_claimedScanner.Close();
            m_claimedScanner = nullptr;
        }

        if (m_scanner)
        {
            m_scanner.Close();
            m_scanner = nullptr;
        }

        // Reset the UI if we are still the current page.
        if (Frame().Content() == *this)
        {
            m_rootPage.NotifyUser(L"Click the start scanning button to begin.", NotifyType::StatusMessage);
            ScenarioOutputScanData().Text(L"No data");
            ScenarioOutputScanDataLabel().Text(L"No data");
            ScenarioOutputScanDataType().Text(L"No data");

            // reset the button state
            SetActiveSymbologiesButton().IsEnabled(false);
            ScenarioEndScanButton().IsEnabled(false);
            ScenarioStartScanButton().IsEnabled(true);

            // reset symbology list
            m_listOfSymbologies.Clear();
        }
    }

    /// <summary>
    /// Event handler for End Scan Button Click.
    /// Releases the Barcode Scanner and resets the text in the UI
    /// </summary>
    void Scenario3_ActiveSymbologies::ScenarioEndScanButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ResetTheScenarioState();
    }

    fire_and_forget Scenario3_ActiveSymbologies::SetActiveSymbologies_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (m_claimedScanner)
        {
            std::vector<uint32_t> symbologyList;
            for (auto&& symbologyListEntry : m_listOfSymbologies)
            {
                if (symbologyListEntry.IsEnabled())
                {
                    symbologyList.push_back(symbologyListEntry.Id());
                }
            }

            co_await m_claimedScanner.SetActiveSymbologiesAsync(std::move(symbologyList));
        }
    }
}
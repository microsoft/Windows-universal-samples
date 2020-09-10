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

#include "Scenario2_MultipleScanners.h"
#include "Scenario2_MultipleScanners.g.cpp"

namespace winrt
{
    using namespace Windows::Devices;
    using namespace Windows::Devices::Enumeration;
    using namespace Windows::Devices::PointOfService;
    using namespace Windows::Foundation;
    using namespace Windows::UI;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Controls;
    using namespace Windows::UI::Xaml::Media;
    using namespace Windows::UI::Xaml::Navigation;
}

namespace winrt::SDKTemplate::implementation
{

    Scenario2_MultipleScanners::Scenario2_MultipleScanners()
    {
        InitializeComponent();
    }

    /// <summary>
    /// Invoked when this page is no longer displayed.
    /// </summary>
    /// <param name="e">Event data that describes how this page was exited.  The Parameter
    /// property is typically used to configure the page.</param>
    void Scenario2_MultipleScanners::OnNavigatedFrom(NavigationEventArgs const&)
    {
        ResetTheScenarioState();
    }

    /// <summary>
    /// Invoked when this page is about to be displayed in a Frame.
    /// </summary>
    /// <param name="e">Event data that describes how this page was reached.  The Parameter
    /// property is typically used to configure the page.</param>
    void Scenario2_MultipleScanners::OnNavigatedTo(NavigationEventArgs const&)
    {
        ResetTheScenarioState();
    }

    /// <summary>
    /// This is the click handler for the 'ScenarioStartScanningInstance1' button. It initiates creation of scanner instance 1.
    /// </summary>
    fire_and_forget Scenario2_MultipleScanners::ButtonStartScanningInstance1_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        //Get the handle to the default scanner
        if (co_await CreateDefaultScannerObjectAsync(BarcodeScannerInstance::Instance1))
        {
            //Claim the scanner
            if (co_await ClaimBarcodeScannerAsync(BarcodeScannerInstance::Instance1))
            {
                //add the event handlers
                m_dev1ReleaseRequestedToken = m_claimedBarcodeScannerInstance1.ReleaseDeviceRequested({ get_weak(), &Scenario2_MultipleScanners::claimedBarcodeScannerInstance1_ReleaseDeviceRequested });
                m_dev1DataReceivedToken = m_claimedBarcodeScannerInstance1.DataReceived({ get_weak(), &Scenario2_MultipleScanners::claimedBarcodeScannerInstance1_DataReceived });
                m_claimedBarcodeScannerInstance1.IsDecodeDataEnabled(true);

                //Enable the Scanner
                if (co_await EnableBarcodeScannerAsync(BarcodeScannerInstance::Instance1))
                {
                    //Set the UI state
                    ResetUI();
                    SetUI(BarcodeScannerInstance::Instance1);
                }
            }
            else
            {
                if (m_barcodeScannerInstance1)
                {
                    m_barcodeScannerInstance1.Close();
                    m_barcodeScannerInstance1 = nullptr;
                }
            }
        }
    }

    /// <summary>
    /// This method is called upon when a claim request is made on instance 1. If a retain request was placed on the device it rejects the new claim.
    /// </summary>
    fire_and_forget Scenario2_MultipleScanners::claimedBarcodeScannerInstance1_ReleaseDeviceRequested(IInspectable const&, ClaimedBarcodeScanner const&)
    {
        auto lifetime = get_strong();

        co_await resume_foreground(Dispatcher());

        //check if the instance wants to retain the device
        if (Retain1().IsChecked().Value())
        {
            try
            {
                //Retain the device
                m_claimedBarcodeScannerInstance1.RetainDevice();
            }
            catch (hresult_error const& exception)
            {
                m_rootPage.NotifyUser(L"Retain instance 1 failed: " + exception.message(), NotifyType::ErrorMessage);
            }
        }
        //Release the device
        else
        {
            m_claimedBarcodeScannerInstance1.Close();
            m_claimedBarcodeScannerInstance1 = nullptr;

            if (m_barcodeScannerInstance1)
            {
                m_barcodeScannerInstance1.Close();
                m_barcodeScannerInstance1 = nullptr;
            }
        }
    }

    /// <summary>
    /// This is the click handler for the 'ScenarioStartScanningInstance2' button. Initiates creation of scanner instance 2
    /// </summary>
    fire_and_forget Scenario2_MultipleScanners::ButtonStartScanningInstance2_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        //Get the handle to the default scanner
        if (co_await CreateDefaultScannerObjectAsync(BarcodeScannerInstance::Instance2))
        {
            //Claim the scanner
            if (co_await ClaimBarcodeScannerAsync(BarcodeScannerInstance::Instance2))
            {
                //set the handlers
                m_dev2ReleaseRequestedToken = m_claimedBarcodeScannerInstance2.ReleaseDeviceRequested({ get_weak(), &Scenario2_MultipleScanners::claimedBarcodeScannerInstance2_ReleaseDeviceRequested });
                m_dev2DataReceivedToken = m_claimedBarcodeScannerInstance2.DataReceived({ get_weak(), &Scenario2_MultipleScanners::claimedBarcodeScannerInstance2_DataReceived });

                //enable the scanner to decode the scanned data
                m_claimedBarcodeScannerInstance2.IsDecodeDataEnabled(true);

                //Enable the Scanner
                if (co_await EnableBarcodeScannerAsync(BarcodeScannerInstance::Instance2))
                {
                    //Set the UI state
                    ResetUI();
                    SetUI(BarcodeScannerInstance::Instance2);
                }
            }
            else
            {
                if (m_barcodeScannerInstance2)
                {
                    m_barcodeScannerInstance2.Close();
                    m_barcodeScannerInstance2 = nullptr;
                }
            }
        }
    }

    /// <summary>
    /// This method is called upon when a claim request is made on instance 2. If a retain request was placed on the device it rejects the new claim.
    /// </summary>
    fire_and_forget Scenario2_MultipleScanners::claimedBarcodeScannerInstance2_ReleaseDeviceRequested(IInspectable const&, ClaimedBarcodeScanner const&)
    {
        auto lifetime = get_strong();

        co_await resume_foreground(Dispatcher());

        //check if the instance wants to retain the device
        if (Retain2().IsChecked().Value())
        {
            try
            {
                //Retain the device
                m_claimedBarcodeScannerInstance2.RetainDevice();
            }
            catch (hresult_error const& exception)
            {
                m_rootPage.NotifyUser(L"Retain instance 2 failed: " + exception.message(), NotifyType::ErrorMessage);
            }
        }
        //Release the device
        else
        {
            m_claimedBarcodeScannerInstance2.Close();
            m_claimedBarcodeScannerInstance2 = nullptr;

            if (m_barcodeScannerInstance2)
            {
                m_barcodeScannerInstance2.Close();
                m_barcodeScannerInstance2 = nullptr;
            }
        }
    }

    /// <summary>
    /// This is the click handler for the 'ScenarioEndScanningInstance1' button.
    /// Initiates the disposal of scanner instance 1.
    /// </summary>
    void Scenario2_MultipleScanners::ButtonEndScanningInstance1_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (m_claimedBarcodeScannerInstance1)
        {
            //remove the event handlers
            m_claimedBarcodeScannerInstance1.ReleaseDeviceRequested(m_dev1ReleaseRequestedToken);
            m_claimedBarcodeScannerInstance1.DataReceived(m_dev1DataReceivedToken);

            //dispose the instance
            m_claimedBarcodeScannerInstance1.Close();
            m_claimedBarcodeScannerInstance1 = nullptr;
        }

        if (m_barcodeScannerInstance1)
        {
            m_barcodeScannerInstance1.Close();
            m_barcodeScannerInstance1 = nullptr;
        }

        //reset the UI

        ResetUI();

        m_rootPage.NotifyUser(L"Click a start scanning button to begin.", NotifyType::StatusMessage);
    }

    /// <summary>
    /// This is the click handler for the 'ScenarioEndScanningInstance2' button.
    /// Initiates the disposal fo scanner instance 2.
    /// </summary>
    void Scenario2_MultipleScanners::ButtonEndScanningInstance2_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (m_claimedBarcodeScannerInstance2)
        {
            //remove the event handlers
            m_claimedBarcodeScannerInstance2.ReleaseDeviceRequested(m_dev2ReleaseRequestedToken);
            m_claimedBarcodeScannerInstance2.DataReceived(m_dev2DataReceivedToken);

            //dispose the instance
            m_claimedBarcodeScannerInstance2.Close();
            m_claimedBarcodeScannerInstance2 = nullptr;
        }

        if (m_barcodeScannerInstance2)
        {
            m_barcodeScannerInstance2.Close();
            m_barcodeScannerInstance2 = nullptr;
        }

        //reset the UI
        ResetUI();

        m_rootPage.NotifyUser(L"Click a start scanning button to begin.", NotifyType::StatusMessage);
    }

    /// <summary>
    /// This method returns the first available Barcode Scanner. To enumerate and find a particular device use the device enumeration code.
    /// </summary>
    /// <returns>a boolean value based on whether it found a compatible scanner connected</returns>

    IAsyncOperation<bool> Scenario2_MultipleScanners::CreateDefaultScannerObjectAsync(BarcodeScannerInstance instance)
    {
        auto lifetime = get_strong();

        BarcodeScanner scanner = co_await BarcodeScanner::GetDefaultAsync();

        if (!scanner)
        {
            m_rootPage.NotifyUser(L"Barcode scanner not found. Please connect a barcode scanner.", NotifyType::ErrorMessage);
            co_return false;
        }

        switch (instance)
        {
        case BarcodeScannerInstance::Instance1:
            m_barcodeScannerInstance1 = scanner;
            break;
        case BarcodeScannerInstance::Instance2:
            m_barcodeScannerInstance2 = scanner;
            break;
        default:
            co_return false;
        }

        co_return true;
    }

    /// <summary>
    /// This method claims the connected scanner.
    /// </summary>
    /// <returns>a boolean based on whether it was able to claim the scanner.</returns>

    IAsyncOperation<bool> Scenario2_MultipleScanners::ClaimBarcodeScannerAsync(BarcodeScannerInstance instance)
    {
        auto lifetime = get_strong();
        bool claimAsyncStatus = false;
        //select the instance to claim
        switch (instance)
        {
        case BarcodeScannerInstance::Instance1:

            m_claimedBarcodeScannerInstance1 = co_await m_barcodeScannerInstance1.ClaimScannerAsync();
            if (!m_claimedBarcodeScannerInstance1)
                m_rootPage.NotifyUser(L"Instance 1 claim barcode scanner failed.", NotifyType::ErrorMessage);
            else
                claimAsyncStatus = true;
            break;

        case BarcodeScannerInstance::Instance2:

            m_claimedBarcodeScannerInstance2 = co_await m_barcodeScannerInstance2.ClaimScannerAsync();
            if (!m_claimedBarcodeScannerInstance2)
                m_rootPage.NotifyUser(L"Instance 2 claim barcode scanner failed.", NotifyType::ErrorMessage);
            else
                claimAsyncStatus = true;
            break;

        default:
            co_return claimAsyncStatus;
        }
        co_return claimAsyncStatus;
    }

    /// <summary>
    /// This method enables the connected scanner.
    /// </summary>
    /// <returns>a boolean based on whether it was able to enable the scanner.</returns>

    IAsyncOperation<bool> Scenario2_MultipleScanners::EnableBarcodeScannerAsync(BarcodeScannerInstance instance)
    {
        auto lifetime = get_strong();
        switch (instance)
        {
        case BarcodeScannerInstance::Instance1:
            co_await m_claimedBarcodeScannerInstance1.EnableAsync();
            m_rootPage.NotifyUser(L"Instance 1 ready to scan. Device ID: " + m_claimedBarcodeScannerInstance1.DeviceId(), NotifyType::StatusMessage);
            break;
        case BarcodeScannerInstance::Instance2:
            co_await m_claimedBarcodeScannerInstance2.EnableAsync();
            m_rootPage.NotifyUser(L"Instance 2 ready to scan. Device ID: " + m_claimedBarcodeScannerInstance2.DeviceId(), NotifyType::StatusMessage);
            break;
        default:
            co_return false;
        }
        co_return true;
    }

    /// <summary>
    /// Reset the Scenario state
    /// </summary>

    void Scenario2_MultipleScanners::ResetTheScenarioState()
    {
        if (m_claimedBarcodeScannerInstance1)
        {
            m_claimedBarcodeScannerInstance1.Close();
            m_claimedBarcodeScannerInstance1 = nullptr;
        }

        if (m_barcodeScannerInstance1)
        {
            m_barcodeScannerInstance1.Close();
            m_barcodeScannerInstance1 = nullptr;
        }

        if (m_claimedBarcodeScannerInstance2)
        {
            m_claimedBarcodeScannerInstance2.Close();
            m_claimedBarcodeScannerInstance2 = nullptr;
        }

        if (m_barcodeScannerInstance2)
        {
            m_barcodeScannerInstance2.Close();
            m_barcodeScannerInstance2 = nullptr;
        }

        ResetUI();
    }

    /// <summary>
    /// Resets the display Elements to original state
    /// </summary>

    void Scenario2_MultipleScanners::ResetUI()
    {
        Instance1Border().BorderBrush(SolidColorBrush{ Colors::Gray() });
        Instance2Border().BorderBrush(SolidColorBrush{ Colors::Gray() });

        ScanDataType1().Text(L"No data");
        ScanData1().Text(L"No data");
        DataLabel1().Text(L"No data");
        ScanDataType2().Text(L"No data");
        ScanData2().Text(L"No data");
        DataLabel2().Text(L"No data");

        ScenarioStartScanningInstance1().IsEnabled(true);
        ScenarioStartScanningInstance2().IsEnabled(true);
        ScenarioEndScanningInstance1().IsEnabled(false);
        ScenarioEndScanningInstance2().IsEnabled(false);
    }

    /// <summary>
    /// Sets the UI elements to a state corresponding to the current active Instance.
    /// </summary>
    /// <param name="instance">Corresponds to the current active instance</param>

    void Scenario2_MultipleScanners::SetUI(BarcodeScannerInstance instance)
    {
        Instance1Border().BorderBrush(SolidColorBrush{ Colors::Gray() });
        Instance2Border().BorderBrush(SolidColorBrush{ Colors::Gray() });

        switch (instance)
        {
        case BarcodeScannerInstance::Instance1:
            ScenarioStartScanningInstance1().IsEnabled(false);
            ScenarioStartScanningInstance2().IsEnabled(true);
            ScenarioEndScanningInstance1().IsEnabled(true);
            ScenarioEndScanningInstance2().IsEnabled(false);
            Instance1Border().BorderBrush(SolidColorBrush{ Colors::DarkBlue() });
            break;

        case BarcodeScannerInstance::Instance2:
            ScenarioStartScanningInstance1().IsEnabled(true);
            ScenarioStartScanningInstance2().IsEnabled(false);
            ScenarioEndScanningInstance1().IsEnabled(false);
            ScenarioEndScanningInstance2().IsEnabled(true);
            Instance2Border().BorderBrush(SolidColorBrush{ Colors::DarkBlue() });
            break;
        }
    }

    /// <summary>
    /// This is an event handler for the claimed scanner Instance 1 when it scans and recieves data
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>

    fire_and_forget Scenario2_MultipleScanners::claimedBarcodeScannerInstance1_DataReceived(ClaimedBarcodeScanner const&, BarcodeScannerDataReceivedEventArgs args)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        ScanDataType1().Text(BarcodeSymbologies::GetName(args.Report().ScanDataType()));
        DataLabel1().Text(GetDataLabelString(args.Report().ScanDataLabel(), args.Report().ScanDataType()));
        ScanData1().Text(GetDataString(args.Report().ScanData()));

        m_rootPage.NotifyUser(L"Instance 1 received data from the barcode scanner.", NotifyType::StatusMessage);
    }

    /// <summary>
    /// This is an event handler for the claimed scanner Instance 2 when it scans and recieves data
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>

    fire_and_forget Scenario2_MultipleScanners::claimedBarcodeScannerInstance2_DataReceived(ClaimedBarcodeScanner const&, BarcodeScannerDataReceivedEventArgs args)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());
        ScanDataType2().Text(BarcodeSymbologies::GetName(args.Report().ScanDataType()));
        DataLabel2().Text(GetDataLabelString(args.Report().ScanDataLabel(), args.Report().ScanDataType()));
        ScanData2().Text(GetDataString(args.Report().ScanData()));

        m_rootPage.NotifyUser(L"Instance 2 received data from the barcode scanner.", NotifyType::StatusMessage);
    }

}
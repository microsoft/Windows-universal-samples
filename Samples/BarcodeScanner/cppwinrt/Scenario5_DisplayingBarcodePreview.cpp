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
#include "Scenario5_DisplayingBarcodePreview.h"
#include "Scenario5_DisplayingBarcodePreview.g.cpp"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
    using namespace Windows::Devices;
    using namespace Windows::Devices::Enumeration;
    using namespace Windows::Devices::PointOfService;
    using namespace Windows::Graphics::Display;
    using namespace Windows::System::Display;
    using namespace Windows::UI::Xaml;
    using namespace Windows::UI::Xaml::Controls;
    using namespace Windows::UI::Xaml::Data;
    using namespace Windows::UI::Xaml::Navigation;
    using namespace Windows::Media::Capture;
}

namespace winrt::SDKTemplate::implementation
{
    Scenario5_DisplayingBarcodePreview::Scenario5_DisplayingBarcodePreview()
    {
        InitializeComponent();

        ScannerListSource().Source(m_barcodeScanners);
        m_watcher = DeviceInformation::CreateWatcher(BarcodeScanner::GetDeviceSelector());
        m_watcher.Added({ get_weak(), &Scenario5_DisplayingBarcodePreview::Watcher_Added });
        m_watcher.Removed({ get_weak(), &Scenario5_DisplayingBarcodePreview::Watcher_Removed });
        m_watcher.Updated({ get_weak(), &Scenario5_DisplayingBarcodePreview::Watcher_Updated });
        m_watcher.Start();

        DataContext(*this);
    }

    fire_and_forget Scenario5_DisplayingBarcodePreview::Watcher_Added(DeviceWatcher const& sender, DeviceInformation args)
    {
        auto lifetime = get_strong();
        co_await Dispatcher();

        m_barcodeScanners.Append({ args.Name(), args.Id() });

        if (m_barcodeScanners.Size() == 1)
        {
            ScannerListBox().SelectedIndex(0);
        }
    }

    fire_and_forget Scenario5_DisplayingBarcodePreview::Watcher_Removed(DeviceWatcher const&, DeviceInformationUpdate const&)
    {
        // We don't do anything here, but this event needs to be handled to enable realtime updates.
        // See https://aka.ms/devicewatcher_added.
        co_return;
    }

    fire_and_forget Scenario5_DisplayingBarcodePreview::Watcher_Updated(DeviceWatcher const&, DeviceInformationUpdate const&)
    {
        // We don't do anything here, but this event needs to be handled to enable realtime updates.
        //See https://aka.ms/devicewatcher_added.
        co_return;
    }

    fire_and_forget Scenario5_DisplayingBarcodePreview::OnNavigatedFrom(NavigationEventArgs const&)
    {
        auto lifetime = get_strong();

        m_watcher.Stop();

        if (m_isSelectionChanging)
        {
            isStopPending = true;
        }
        else
        {
            co_await CloseScannerResourcesAsync();
        }
    }

    /// <summary>
    /// Starts previewing the selected scanner's video feed and prevents the display from going to sleep.
    /// </summary>
    IAsyncAction Scenario5_DisplayingBarcodePreview::StartMediaCaptureAsync(hstring videoDeviceId)
    {
        auto lifetime = get_strong();
        m_mediaCapture = MediaCapture();

        // Register for a notification when something goes wrong
        m_mediaCapture.Failed({ get_weak(), &Scenario5_DisplayingBarcodePreview::MediaCapture_Failed });

        auto settings = MediaCaptureInitializationSettings();
        settings.VideoDeviceId(videoDeviceId);
        settings.StreamingCaptureMode(StreamingCaptureMode::Video);
        settings.SharingMode(MediaCaptureSharingMode::SharedReadOnly);

        // Initialize MediaCapture
        bool captureInitialized = false;
        try
        {
            co_await m_mediaCapture.InitializeAsync(settings);
            captureInitialized = true;
        }
        catch (hresult_access_denied const&)
        {
            m_rootPage.NotifyUser(L"The app was denied access to the camera", NotifyType::ErrorMessage);
        }
        catch (hresult_error const& e)
        {
            m_rootPage.NotifyUser(L"Failed to initialize the camera: " + e.message(), NotifyType::ErrorMessage);
        }

        if (captureInitialized)
        {
            // Prevent the device from sleeping while the preview is running.
            m_displayRequest.RequestActive();

            PreviewControl().Source(m_mediaCapture);
            co_await m_mediaCapture.StartPreviewAsync();
            co_await SetPreviewRotationAsync(DisplayInformation::GetForCurrentView().CurrentOrientation());
            IsPreviewing(true);
            RaisePropertyChanged(L"IsPreviewing");
        }
        else
        {
            m_mediaCapture.Close();
            m_mediaCapture = nullptr;
        }
    }

    /// <summary>
    /// Close the scanners and stop the preview.
    /// </summary>
    IAsyncAction Scenario5_DisplayingBarcodePreview::CloseScannerResourcesAsync()
    {
        auto lifetime = get_strong();
        if (m_claimedScanner)
        {
            m_claimedScanner.Close();
            m_claimedScanner = nullptr;
        }

        if (m_selectedScanner)
        {
            m_selectedScanner.Close();
            m_selectedScanner = nullptr;
        }

        SoftwareTriggerStarted(false);
        RaisePropertyChanged(L"SoftwareTriggerStarted");

        if (!IsPreviewing())
        {
            co_return;
        }

        if (m_mediaCapture)
        {
            co_await m_mediaCapture.StopPreviewAsync();
            m_mediaCapture.Close();
            m_mediaCapture = nullptr;
        }

        if (m_displayRequest)
        {
            m_displayRequest.RequestRelease();
            IsPreviewing(false);
            RaisePropertyChanged(L"IsPreviewing");
        }
    }

    /// <summary>
    /// Set preview rotation and mirroring state to adjust for the orientation of the camera, and for embedded cameras, the rotation of the device.
    /// </summary>
    /// <param name="message"></param>
    /// <param name="type"></param>
    IAsyncAction Scenario5_DisplayingBarcodePreview::SetPreviewRotationAsync(DisplayOrientations displayOrientation)
    {
        auto lifetime = get_strong();
        bool isExternalCamera;
        bool isPreviewMirrored;

        // Figure out where the camera is located to account for mirroring and later adjust rotation accordingly.
        DeviceInformation cameraInformation = co_await DeviceInformation::CreateFromIdAsync(m_selectedScanner.VideoDeviceId());

        if ((cameraInformation.EnclosureLocation() == nullptr) || (cameraInformation.EnclosureLocation().Panel() == Windows::Devices::Enumeration::Panel::Unknown))
        {
            isExternalCamera = true;
            isPreviewMirrored = false;
        }
        else
        {
            isExternalCamera = false;
            isPreviewMirrored = (cameraInformation.EnclosureLocation().Panel() == Windows::Devices::Enumeration::Panel::Front);
        }

        PreviewControl().FlowDirection(isPreviewMirrored ? FlowDirection::RightToLeft : FlowDirection::LeftToRight);

        if (!isExternalCamera)
        {
            // Calculate which way and how far to rotate the preview.
            int rotationDegrees = 0;
            switch (displayOrientation)
            {
            case DisplayOrientations::Portrait:
                rotationDegrees = 90;
                break;
            case DisplayOrientations::LandscapeFlipped:
                rotationDegrees = 180;
                break;
            case DisplayOrientations::PortraitFlipped:
                rotationDegrees = 270;
                break;
            case DisplayOrientations::Landscape:
            default:
                rotationDegrees = 0;
                break;
            }

            // The rotation direction needs to be inverted if the preview is being mirrored.
            if (isPreviewMirrored)
            {
                rotationDegrees = (360 - rotationDegrees) % 360;
            }

            // Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames.
            auto streamProperties = m_mediaCapture.VideoDeviceController().GetMediaStreamProperties(MediaStreamType::VideoPreview);
            static constexpr guid rotationGuid(0xC380465D, 0x2271, 0x428C, { 0x9B, 0x83, 0xEC, 0xEA, 0x3B, 0x4A, 0x85, 0xC1 });
            streamProperties.Properties().Insert(rotationGuid, box_value(rotationDegrees));
            co_await m_mediaCapture.SetEncodingPropertiesAsync(MediaStreamType::VideoPreview, streamProperties, nullptr);
        }
    }

    /// <summary>
    /// Media capture failed, potentially due to the camera being unplugged.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="errorEventArgs"></param>
    void Scenario5_DisplayingBarcodePreview::MediaCapture_Failed(MediaCapture const&, MediaCaptureFailedEventArgs const&)
    {
        m_rootPage.NotifyUser(L"Media capture failed. Make sure the camera is still connected.", NotifyType::ErrorMessage);
    }

    /// <summary>
    /// Event Handler for Show Preview Button Click.
    /// Displays the preview window for the selected barcode scanner.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    fire_and_forget Scenario5_DisplayingBarcodePreview::ShowPreviewButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (m_claimedScanner)
        {
            co_await m_claimedScanner.ShowVideoPreviewAsync();
        }
    }

    /// <summary>
    /// Event Handler for Hide Preview Button Click.
    /// Hides the preview window for the selected barcode scanner.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void Scenario5_DisplayingBarcodePreview::HidePreviewButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (m_claimedScanner)
        {
            m_claimedScanner.HideVideoPreview();
        }
    }

    /// <summary>
    /// Event Handler for Start Software Trigger Button Click.
    /// Starts scanning.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    fire_and_forget Scenario5_DisplayingBarcodePreview::StartSoftwareTriggerButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (m_claimedScanner)
        {
            co_await m_claimedScanner.StartSoftwareTriggerAsync();

            SoftwareTriggerStarted(true);
            RaisePropertyChanged(L"SoftwareTriggerStarted");
        }
    }

    /// <summary>
    /// Event Handler for Stop Software Trigger Button Click.
    /// Stops scanning.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    fire_and_forget Scenario5_DisplayingBarcodePreview::StopSoftwareTriggerButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (m_claimedScanner)
        {
            co_await m_claimedScanner.StopSoftwareTriggerAsync();

            SoftwareTriggerStarted(false);
            RaisePropertyChanged(L"SoftwareTriggerStarted");
        }
    }

    /// <summary>
    /// Event Handler for Flip Preview Button Click.
    /// Stops scanning.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void Scenario5_DisplayingBarcodePreview::FlipPreview_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (PreviewControl().FlowDirection() == FlowDirection::LeftToRight)
        {
            PreviewControl().FlowDirection(FlowDirection::RightToLeft);
        }
        else
        {
            PreviewControl().FlowDirection(FlowDirection::LeftToRight);
        }
    }

    /// <summary>
    /// Event handler for scanner listbox selection changed
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    fire_and_forget Scenario5_DisplayingBarcodePreview::ScannerSelection_Changed(IInspectable const& sender, SelectionChangedEventArgs const& args)
    {
        auto lifetime = get_strong();
        auto selectedScannerInfo = args.AddedItems().GetAt(0).as<SDKTemplate::BarcodeScannerInfo>();
        auto deviceId = selectedScannerInfo.DeviceId();

        if (m_isSelectionChanging)
        {
            m_pendingSelectionDeviceId = deviceId;
            return;
        }

        do
        {
            co_await SelectScannerAsync(deviceId);

            // Stop takes precedence over updating the selection.
            if (isStopPending)
            {
                co_await CloseScannerResourcesAsync();
                break;
            }

            deviceId = m_pendingSelectionDeviceId;
            m_pendingSelectionDeviceId.clear();
        } while (!deviceId.empty());
    }

    /// <summary>
    /// Select the scanner specified by its device ID.
    /// </summary>
    /// <param name="scannerDeviceId"></param>
    IAsyncAction Scenario5_DisplayingBarcodePreview::SelectScannerAsync(hstring scannerDeviceId)
    {
        auto lifetime = get_strong();
        m_isSelectionChanging = true;

        co_await CloseScannerResourcesAsync();

        m_selectedScanner = co_await BarcodeScanner::FromIdAsync(scannerDeviceId);

        if (m_selectedScanner)
        {
            m_claimedScanner = co_await m_selectedScanner.ClaimScannerAsync();
            if (m_claimedScanner)
            {
                co_await m_claimedScanner.EnableAsync();
                m_claimedScanner.Closed({ get_weak(), &Scenario5_DisplayingBarcodePreview::ClaimedScanner_Closed });
                ScannerSupportsPreview(!m_selectedScanner.VideoDeviceId().empty());
                RaisePropertyChanged(L"ScannerSupportsPreview");

                m_claimedScanner.DataReceived({ get_weak(), &Scenario5_DisplayingBarcodePreview::ClaimedScanner_DataReceived });

                if (ScannerSupportsPreview())
                {
                    co_await StartMediaCaptureAsync(m_selectedScanner.VideoDeviceId());
                }
            }
            else
            {
                m_rootPage.NotifyUser(L"Failed to claim the selected barcode scanner", NotifyType::ErrorMessage);
            }

        }
        else
        {
            m_rootPage.NotifyUser(L"Failed to create a barcode scanner object", NotifyType::ErrorMessage);
        }

        IsScannerClaimed(m_claimedScanner != nullptr);
        RaisePropertyChanged(L"IsScannerClaimed");

        m_isSelectionChanging = false;
    }

    /// <summary>
    /// Closed notification was received from the selected scanner.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void Scenario5_DisplayingBarcodePreview::ClaimedScanner_Closed(ClaimedBarcodeScanner const&, ClaimedBarcodeScannerClosedEventArgs const&)
    {
        // Resources associated to the claimed barcode scanner can be cleaned up here
    }

    /// <summary>
    /// Scan data was received from the selected scanner.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    fire_and_forget Scenario5_DisplayingBarcodePreview::ClaimedScanner_DataReceived(ClaimedBarcodeScanner const&, BarcodeScannerDataReceivedEventArgs args)
    {
        auto lifetime = get_strong();
        co_await Dispatcher();
        ScenarioOutputScanDataLabel().Text(GetDataLabelString(args.Report().ScanDataLabel(), args.Report().ScanDataType()));
        ScenarioOutputScanData().Text(GetDataString(args.Report().ScanData()));
        ScenarioOutputScanDataType().Text(BarcodeSymbologies::GetName(args.Report().ScanDataType()));
    }

    /// <summary>
    /// Update listeners that a property was changed so that data bindings can be updated.
    /// </summary>
    /// <param name="propertyName"></param>
    void Scenario5_DisplayingBarcodePreview::RaisePropertyChanged(hstring propertyName)
    {
        m_propertyChanged(*this, PropertyChangedEventArgs(propertyName));
    }

}

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

#pragma once

#include "Scenario5_DisplayingBarcodePreview.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario5_DisplayingBarcodePreview : Scenario5_DisplayingBarcodePreviewT<Scenario5_DisplayingBarcodePreview>
    {
        Scenario5_DisplayingBarcodePreview();

        bool IsScannerClaimed()
        {
            return m_isScannerClaimed;
        }
        void IsScannerClaimed(bool claimed)
        {
            m_isScannerClaimed = claimed;
        }

        bool IsPreviewing()
        {
            return m_isPreviewing;
        }
        void IsPreviewing(bool isPreviewing)
        {
            m_isPreviewing = isPreviewing;
        }

        bool ScannerSupportsPreview()
        {
            return m_scannerSupportsPreview;
        }
        void ScannerSupportsPreview(bool supportsPreview)
        {
            m_scannerSupportsPreview = supportsPreview;
        }

        bool SoftwareTriggerStarted()
        {
            return m_softwareTriggerStarted;
        }
        void SoftwareTriggerStarted(bool started)
        {
            m_softwareTriggerStarted = started;
        }

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
        {
            return m_propertyChanged.add(handler);
        }

        void PropertyChanged(event_token const& token) noexcept
        {
            return m_propertyChanged.remove(token);
        }

        fire_and_forget Watcher_Added(Windows::Devices::Enumeration::DeviceWatcher const& sender, Windows::Devices::Enumeration::DeviceInformation args);
        fire_and_forget Watcher_Updated(Windows::Devices::Enumeration::DeviceWatcher const& sender, Windows::Devices::Enumeration::DeviceInformationUpdate const& args);
        fire_and_forget Watcher_Removed(Windows::Devices::Enumeration::DeviceWatcher const& sender, Windows::Devices::Enumeration::DeviceInformationUpdate const& args);

        fire_and_forget OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& args);
        fire_and_forget ScannerSelection_Changed(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& args);
        void FlipPreview_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget ShowPreviewButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget StartSoftwareTriggerButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget StopSoftwareTriggerButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void HidePreviewButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        Windows::Foundation::IAsyncAction StartMediaCaptureAsync(hstring videoDeviceId);
        Windows::Foundation::IAsyncAction CloseScannerResourcesAsync();
        Windows::Foundation::IAsyncAction SetPreviewRotationAsync(Windows::Graphics::Display::DisplayOrientations displayOrientation);
        void MediaCapture_Failed(Windows::Media::Capture::MediaCapture const& sender, Windows::Media::Capture::MediaCaptureFailedEventArgs const& errorEventArgs);
        Windows::Foundation::IAsyncAction SelectScannerAsync(hstring scannerDeviceId);
        void ClaimedScanner_Closed(Windows::Devices::PointOfService::ClaimedBarcodeScanner const& sender, Windows::Devices::PointOfService::ClaimedBarcodeScannerClosedEventArgs const& args);

        fire_and_forget ClaimedScanner_DataReceived(Windows::Devices::PointOfService::ClaimedBarcodeScanner const&, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs args);

        void RaisePropertyChanged(hstring propertyName);

        bool m_isScannerClaimed = false;
        bool m_isPreviewing = false;
        bool m_scannerSupportsPreview = false;
        bool m_softwareTriggerStarted = false;

        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::BarcodeScannerInfo> m_barcodeScanners{ single_threaded_observable_vector<SDKTemplate::BarcodeScannerInfo>() };
        Windows::Devices::PointOfService::BarcodeScanner m_selectedScanner{ nullptr };
        Windows::Devices::PointOfService::ClaimedBarcodeScanner m_claimedScanner{ nullptr };
        Windows::Devices::Enumeration::DeviceWatcher m_watcher{ nullptr };

        Windows::System::Display::DisplayRequest m_displayRequest;
        Windows::Media::Capture::MediaCapture m_mediaCapture{ nullptr };

        bool m_isSelectionChanging = false;
        hstring m_pendingSelectionDeviceId;
        bool isStopPending = false;

        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario5_DisplayingBarcodePreview : Scenario5_DisplayingBarcodePreviewT<Scenario5_DisplayingBarcodePreview, implementation::Scenario5_DisplayingBarcodePreview>
    {
    };
}

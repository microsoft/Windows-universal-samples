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
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Scenario5_DisplayingBarcodePreview sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        Scenario5_DisplayingBarcodePreview();

        property bool IsScannerClaimed;
        property bool ScannerSupportsPreview;
        property bool IsPreviewing;
        property bool SoftwareTriggerStarted;

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage ^ rootPage = MainPage::Current;
        Windows::Foundation::Collections::IObservableVector<BarcodeScannerInfo^>^ barcodeScanners = ref new Platform::Collections::Vector<BarcodeScannerInfo^>();
        Windows::Devices::PointOfService::BarcodeScanner^ selectedScanner;
        Windows::Devices::PointOfService::ClaimedBarcodeScanner^ claimedScanner;
        Windows::Devices::Enumeration::DeviceWatcher^ watcher;

        Windows::System::Display::DisplayRequest^ displayRequest = ref new Windows::System::Display::DisplayRequest();
        Platform::Agile<Windows::Media::Capture::MediaCapture> mediaCapture;

        bool isSelectionChanging = false;
        Platform::String^ pendingSelectionDeviceId;
        bool isStopPending = false;

        static const GUID rotationGuid;

        Concurrency::task<void> StartMediaCaptureAsync(Platform::String^ videoDeviceId);
        Concurrency::task<void> CloseScannerResourcesAsync();
        Concurrency::task<void> SetPreviewRotationAsync(Windows::Graphics::Display::DisplayOrientations displayOrientation);
        Concurrency::task<void> ProcessScannerChangesAsync(Platform::String^ scannerDeviceId);
        Concurrency::task<void> SelectScannerAsync(Platform::String^ scannerDeviceId);

        void RaisePropertyChanged(Platform::String^ name);

        void Watcher_Added(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformation^ args);
        void Watcher_Removed(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformationUpdate^ args);
        void Watcher_Updated(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformationUpdate^ args);
        void MediaCapture_Failed(Windows::Media::Capture::MediaCapture^ sender, Windows::Media::Capture::MediaCaptureFailedEventArgs^ errorEventArgs);
        void StartSoftwareTriggerButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void StopSoftwareTriggerButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void ShowPreviewButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void HidePreviewButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void FlipPreview_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void ScannerSelection_Changed(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ args);
        void ClaimedScanner_Closed(Windows::Devices::PointOfService::ClaimedBarcodeScanner^ sender, Windows::Devices::PointOfService::ClaimedBarcodeScannerClosedEventArgs^ args);
        void OnDataReceived(Windows::Devices::PointOfService::ClaimedBarcodeScanner^ sender, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs^ args);
    };
}

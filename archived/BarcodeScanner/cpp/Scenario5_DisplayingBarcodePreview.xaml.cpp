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
#include "Scenario5_DisplayingBarcodePreview.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Media::Capture;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage::Streams;
using namespace Windows::System::Display;

const GUID Scenario5_DisplayingBarcodePreview::rotationGuid = { 0xC380465D, 0x2271, 0x428C, 0x9B, 0x83, 0xEC, 0xEA, 0x3B, 0x4A, 0x85, 0xC1 };

Scenario5_DisplayingBarcodePreview::Scenario5_DisplayingBarcodePreview()
{
    InitializeComponent();

    ScannerListSource->Source = barcodeScanners;

    watcher = DeviceInformation::CreateWatcher(BarcodeScanner::GetDeviceSelector());
    watcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(this, &Scenario5_DisplayingBarcodePreview::Watcher_Added);
    watcher->Removed += ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(this, &Scenario5_DisplayingBarcodePreview::Watcher_Removed);
    watcher->Updated += ref new TypedEventHandler<DeviceWatcher^, DeviceInformationUpdate^>(this, &Scenario5_DisplayingBarcodePreview::Watcher_Updated);

    watcher->Start();
    DataContext = this;
}

void Scenario5_DisplayingBarcodePreview::Watcher_Added(DeviceWatcher^ sender, DeviceInformation^ args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        BarcodeScannerInfo^ scannerInfo = ref new BarcodeScannerInfo(args->Name, args->Id);
        barcodeScanners->Append(scannerInfo);

        // Select the first scanner by default.
        if (barcodeScanners->Size == 1)
        {
            ScannerListBox->SelectedIndex = 0;
        }
    }));
}

void Scenario5_DisplayingBarcodePreview::Watcher_Removed(DeviceWatcher^ sender, DeviceInformationUpdate^ args)
{
    // We don't do anything here, but this event needs to be handled to enable realtime updates.
// See https://aka.ms/devicewatcher_added.
}

void Scenario5_DisplayingBarcodePreview::Watcher_Updated(DeviceWatcher^ sender, DeviceInformationUpdate^ args)
{
    // We don't do anything here, but this event needs to be handled to enable realtime updates.
    // See https://aka.ms/devicewatcher_added.
}

void Scenario5_DisplayingBarcodePreview::OnNavigatedFrom(NavigationEventArgs^ e)
{
    watcher->Stop();

    if (isSelectionChanging)
    {
        // If selection is changing, then let it know to stop media capture
        // when it's done.
        isStopPending = true;
    }
    else
    {
        // If selection is not changing, then it's safe to stop immediately.
        CloseScannerResourcesAsync();
    }
}

/// <summary>
/// Starts previewing the selected scanner's video feed and prevents the display from going to sleep.
/// </summary>
task<void> Scenario5_DisplayingBarcodePreview::StartMediaCaptureAsync(String^ videoDeviceId)
{
    mediaCapture = ref new MediaCapture;
    mediaCapture->Failed += ref new MediaCaptureFailedEventHandler(this, &Scenario5_DisplayingBarcodePreview::MediaCapture_Failed);

    auto settings = ref new MediaCaptureInitializationSettings();
    settings->VideoDeviceId = videoDeviceId;
    settings->StreamingCaptureMode = StreamingCaptureMode::Video;
    settings->SharingMode = MediaCaptureSharingMode::SharedReadOnly;

    return create_task(mediaCapture->InitializeAsync(settings)).then([this]
    {
        // Prevent the device from sleeping while the preview is running.
        displayRequest->RequestActive();
        PreviewControl->Source = mediaCapture.Get();
        return mediaCapture->StartPreviewAsync();
    }).then([this] {
        return SetPreviewRotationAsync(DisplayInformation::GetForCurrentView()->CurrentOrientation);
    }).then([this](task<void> task)
    {
        IsPreviewing = false;
        try
        {
            task.get();
            IsPreviewing = true;
        }
        catch (AccessDeniedException^)
        {
            rootPage->NotifyUser("The app was denied access to the camera", NotifyType::ErrorMessage);
        }
        catch (Exception^ e)
        {
            rootPage->NotifyUser("Failed to initialize the camera: " + e->Message, NotifyType::ErrorMessage);
        }

        RaisePropertyChanged(L"IsPreviewing");
    });
}

/// <summary>
/// Close the scanners and stop the preview.
/// </summary>
task<void> Scenario5_DisplayingBarcodePreview::CloseScannerResourcesAsync()
{
    delete claimedScanner;
    claimedScanner = nullptr;

    delete selectedScanner;
    selectedScanner = nullptr;

    SoftwareTriggerStarted = false;
    RaisePropertyChanged(L"SoftwareTriggerStarted");

    if (!IsPreviewing)
    {
        return completed_ui_task();
    }

    return completed_ui_task().then([this]
    {
        if (mediaCapture != nullptr)
        {
            return create_task(mediaCapture->StopPreviewAsync());
        }
        return completed_ui_task();
    }).then([this]
    {
        delete mediaCapture.Get();
        mediaCapture.Release();

        // Allow the display to go to sleep.
        displayRequest->RequestRelease();
        IsPreviewing = false;
        RaisePropertyChanged(L"IsPreviewing");
    });
}

task<void> Scenario5_DisplayingBarcodePreview::SetPreviewRotationAsync(DisplayOrientations displayOrientation)
{
    // Figure out where the camera is located to account for mirroring and later adjust rotation accordingly.
    return create_task(DeviceInformation::CreateFromIdAsync(selectedScanner->DeviceId)).then([this, displayOrientation](DeviceInformation^ cameraInformation)
    {
        bool isExternalCamera;
        bool isPreviewMirrored;

        if ((cameraInformation->EnclosureLocation == nullptr) || (cameraInformation->EnclosureLocation->Panel == Windows::Devices::Enumeration::Panel::Unknown))
        {
            isExternalCamera = true;
            isPreviewMirrored = false;
        }
        else
        {
            isExternalCamera = false;
            isPreviewMirrored = (cameraInformation->EnclosureLocation->Panel == Windows::Devices::Enumeration::Panel::Front);
        }

        PreviewControl->FlowDirection = isPreviewMirrored ? Windows::UI::Xaml::FlowDirection::RightToLeft : Windows::UI::Xaml::FlowDirection::LeftToRight;

        if (!isExternalCamera)
        {
            // Calculate which way and how far to rotate the preview
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

            // The rotation direction needs to be inverted if the preview is being mirrored
            if (isPreviewMirrored)
            {
                rotationDegrees = (360 - rotationDegrees) % 360;
            }

            // Add rotation metadata to the preview stream to make sure the aspect ratio / dimensions match when rendering and getting preview frames
            auto streamProperties = mediaCapture->VideoDeviceController->GetMediaStreamProperties(MediaStreamType::VideoPreview);
            streamProperties->Properties->Insert(rotationGuid, rotationDegrees);
            return create_task(mediaCapture->SetEncodingPropertiesAsync(MediaStreamType::VideoPreview, streamProperties, nullptr));
        }
        else
        {
            return task_from_result();
        }
    });
}

/// <summary>
/// Media capture failed, potentially due to the camera being unplugged.
/// </summary>
/// <param name="sender"></param>
/// <param name="errorEventArgs"></param>
void Scenario5_DisplayingBarcodePreview::MediaCapture_Failed(MediaCapture^ sender, MediaCaptureFailedEventArgs^ errorEventArgs)
{
    rootPage->NotifyUser("Media capture failed. Make sure the camera is still connected.", NotifyType::ErrorMessage);
}

/// <summary>
/// Shows a preview window for the selected scanner.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario5_DisplayingBarcodePreview::ShowPreviewButton_Click(Object^ sender, RoutedEventArgs^ args)
{
    if (claimedScanner != nullptr)
    {
        create_task(claimedScanner->ShowVideoPreviewAsync());
    }
}

/// <summary>
/// Hides the selected scanner's preview window.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario5_DisplayingBarcodePreview::HidePreviewButton_Click(Object^ sender, RoutedEventArgs^ args)
{
    if (claimedScanner != nullptr)
    {
        claimedScanner->HideVideoPreview();
    }
}

// <summary>
/// Setup the barcode scanner to be ready to receive the data events from the scan.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario5_DisplayingBarcodePreview::StartSoftwareTriggerButton_Click(Object^ sender, RoutedEventArgs^ args)
{
    if (claimedScanner != nullptr)
    {
        create_task(claimedScanner->StartSoftwareTriggerAsync()).then([this]
        {
            SoftwareTriggerStarted = true;
            RaisePropertyChanged(L"SoftwareTriggerStarted");
        });
    }
}

/// <summary>
/// Stops the selected scanner's software trigger.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario5_DisplayingBarcodePreview::StopSoftwareTriggerButton_Click(Object^ sender, RoutedEventArgs^ args)
{
    if (claimedScanner != nullptr)
    {
        create_task(claimedScanner->StopSoftwareTriggerAsync()).then([this]
        {
            SoftwareTriggerStarted = false;
            RaisePropertyChanged(L"SoftwareTriggerStarted");
        });
    }
}

/// <summary>
/// Flips the mirroring of the preview.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>

void Scenario5_DisplayingBarcodePreview::FlipPreview_Click(Object^ sender, RoutedEventArgs^ args)
{
    if (PreviewControl->FlowDirection == Windows::UI::Xaml::FlowDirection::LeftToRight)
    {
        PreviewControl->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;
    }
    else
    {
        PreviewControl->FlowDirection = Windows::UI::Xaml::FlowDirection::LeftToRight;
    }
}

/// <summary>
/// Event handler for scanner listbox selection changed.
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void Scenario5_DisplayingBarcodePreview::ScannerSelection_Changed(Object^ sender, SelectionChangedEventArgs^ args)
{
    auto selectedScannerInfo = safe_cast<BarcodeScannerInfo^>(args->AddedItems->GetAt(0));
    auto scannerDeviceId = selectedScannerInfo->DeviceId;

    if (isSelectionChanging)
    {
        // Keep the most recent selection.
        pendingSelectionDeviceId = scannerDeviceId;
        return;
    }

    ProcessScannerChangesAsync(scannerDeviceId);
}

/// <summary>
/// Process changes in the selected scanner until the selection stabilizes or we are asked to shut down.
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
task<void> Scenario5_DisplayingBarcodePreview::ProcessScannerChangesAsync(String^ scannerDeviceId)
{
    return SelectScannerAsync(scannerDeviceId).then([this]
    {
        // Stop takes precedence over updating the selection.
        if (isStopPending)
        {
            return CloseScannerResourcesAsync();
        }

        String^ deviceId = pendingSelectionDeviceId;
        pendingSelectionDeviceId = nullptr;

        if (deviceId->IsEmpty())
        {
            // No more processing to do.
            return task_from_result();
        }

        // Process the next one.
        return ProcessScannerChangesAsync(deviceId);
    });
}

/// <summary>
/// Select the scanner specified by its device ID.
/// </summary>
/// <param name="scannerDeviceId"></param>
task<void> Scenario5_DisplayingBarcodePreview::SelectScannerAsync(String^ scannerDeviceId)
{
    isSelectionChanging = true;

    return CloseScannerResourcesAsync().then([scannerDeviceId]
    {
        return BarcodeScanner::FromIdAsync(scannerDeviceId);
    }).then([this](BarcodeScanner^ scanner)
    {
        selectedScanner = scanner;
        if (selectedScanner != nullptr)
        {
            return create_task(selectedScanner->ClaimScannerAsync());
        }
        else
        {
            rootPage->NotifyUser(L"Failed to create a barcode scanner object", NotifyType::ErrorMessage);
            return task_from_result<ClaimedBarcodeScanner^>(nullptr);
        }
    }).then([this](ClaimedBarcodeScanner^ newClaimedScanner)
    {
        claimedScanner = newClaimedScanner;
        if (claimedScanner != nullptr)
        {
            return create_task(claimedScanner->EnableAsync());
        }
        else
        {
            if (selectedScanner != nullptr)
            {
                rootPage->NotifyUser(L"Failed to claim the selected barcode scanner", NotifyType::ErrorMessage);
            }
            return task_from_result();
        }
    }).then([this]
    {
        if (claimedScanner != nullptr)
        {
            claimedScanner->Closed += ref new TypedEventHandler<ClaimedBarcodeScanner^, ClaimedBarcodeScannerClosedEventArgs^>(this, &Scenario5_DisplayingBarcodePreview::ClaimedScanner_Closed);
            ScannerSupportsPreview = !selectedScanner->VideoDeviceId->IsEmpty();
            RaisePropertyChanged(L"ScannerSupportsPreview");

            claimedScanner->DataReceived += ref new TypedEventHandler<ClaimedBarcodeScanner^, BarcodeScannerDataReceivedEventArgs^>(this, &Scenario5_DisplayingBarcodePreview::OnDataReceived);

            if (ScannerSupportsPreview)
            {
                return StartMediaCaptureAsync(selectedScanner->VideoDeviceId);
            }
        }
        return task_from_result();
    }).then([this]
    {
        IsScannerClaimed = (claimedScanner != nullptr);
        RaisePropertyChanged("IsScannerClaimed");

        isSelectionChanging = false;
    });
}

/// <summary>
/// Closed notification was received from the selected scanner.
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>

void Scenario5_DisplayingBarcodePreview::ClaimedScanner_Closed(ClaimedBarcodeScanner^ sender, ClaimedBarcodeScannerClosedEventArgs^ args)
{
    // Resources associated to the claimed barcode scanner can be cleaned up here
}

/// <summary>
/// Event handler for the DataReceived event fired when a barcode is scanned by the barcode scanner
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the BarcodeScannerReport which contains the data obtained in the scan</param>
void Scenario5_DisplayingBarcodePreview::OnDataReceived(ClaimedBarcodeScanner^ sender, BarcodeScannerDataReceivedEventArgs^ args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        ScenarioOutputScanDataLabel->Text = GetDataLabelString(args->Report->ScanDataLabel, args->Report->ScanDataType);
        ScenarioOutputScanData->Text = GetDataString(args->Report->ScanData);
        ScenarioOutputScanDataType->Text = BarcodeSymbologies::GetName(args->Report->ScanDataType);
    }));
}

void Scenario5_DisplayingBarcodePreview::RaisePropertyChanged(String^ name)
{
    PropertyChanged(this, ref new PropertyChangedEventArgs(name));
}

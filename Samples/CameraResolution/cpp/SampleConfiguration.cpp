//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace Concurrency;
using namespace SDKTemplate;
using namespace Windows::Devices;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::UI::Xaml::Controls;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Change camera preview settings", "SDKTemplate.Scenario1_PreviewSettings" },
    { "Change preview and photo settings", "SDKTemplate.Scenario2_PhotoSettings" },
    { "Match aspect ratios", "SDKTemplate.Scenario3_AspectRatio" }
};

/// <summary>
/// Sets encoding properties on a camera stream. Ensures CaptureElement and preview stream are stopped before setting properties.
/// </summary>
task<void> MainPage::SetMediaStreamPropertiesAsync(MediaCapture^ mediaCapture, CaptureElement^ previewControl, MediaStreamType streamType, IMediaEncodingProperties^ encodingProperties)
{
    // Stop preview and unlink the CaptureElement from the MediaCapture object
    return create_task(mediaCapture->StopPreviewAsync())
        .then([this, mediaCapture, previewControl, streamType, encodingProperties]()
    {
        previewControl->Source = nullptr;
        // Apply desired stream properties
        return create_task(mediaCapture->VideoDeviceController->SetMediaStreamPropertiesAsync(streamType, encodingProperties));
    }).then([this, mediaCapture, previewControl]() {
        // Recreate the CaptureElement pipeline and restart the preview
        previewControl->Source = mediaCapture;
        return create_task(mediaCapture->StartPreviewAsync());
    });
}


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
#include "AudioCapturePermissions.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Media::Capture;

/// <summary>
/// On desktop/tablet systems, users are prompted to give permission to use capture devices on a 
/// per-app basis. Along with declaring the microphone DeviceCapability in the package manifest,
/// this method tests the privacy setting for microphone access for this application.
/// Note that this only checks the Settings->Privacy->Microphone setting, it does not handle
/// the Cortana/Dictation privacy check, however (Under Settings->Privacy->Speech, Inking and Typing).
/// 
/// Developers should ideally perform a check like this every time their app gains focus, in order to 
/// check if the user has changed the setting while the app was suspended or not in focus.
/// </summary>
/// <returns>true if the microphone can be accessed without any permissions problems.</returns>
IAsyncOperation<bool>^  AudioCapturePermissions::RequestMicrophonePermissionAsync()
{
    return create_async([]() 
    {   
		try
		{
			// Request access to the microphone only, to limit the number of capabilities we need
			// to request in the package manifest.
			MediaCaptureInitializationSettings^ settings = ref new MediaCaptureInitializationSettings();
			settings->StreamingCaptureMode = StreamingCaptureMode::Audio;
			settings->MediaCategory = MediaCategory::Speech;
			MediaCapture^ capture = ref new MediaCapture();

			return create_task(capture->InitializeAsync(settings))
				.then([](task<void> previousTask) -> bool
			{
				try
				{
					previousTask.get();
				}
				catch (AccessDeniedException^)
				{
					// The user has turned off access to the microphone. If this occurs, we should show an error, or disable
					// functionality within the app to ensure that further exceptions aren't generated when 
					// recognition is attempted.
					return false;
				}
				catch (Exception^ exception)
				{
					// This can be replicated by using remote desktop to a system, but not redirecting the microphone input.
					// Can also occur if using the virtual machine console tool to access a VM instead of using remote desktop.
					if (exception->HResult == AudioCapturePermissions::NoCaptureDevicesHResult)
					{
						auto messageDialog = ref new Windows::UI::Popups::MessageDialog("No Audio Capture devices are present on this system.");
						create_task(messageDialog->ShowAsync());
						return false;
					}

					throw;
				}
				return true;
			});
		}
		catch (Platform::ClassNotRegisteredException^ ex)
		{
			// If media player components are unavailable (eg, on an N SKU of windows), we may
			// get ClassNotRegisteredException when trying to check if we have permission to use
			// the microphone. 
			auto messageDialog = ref new Windows::UI::Popups::MessageDialog("Media Player Components unavailable.");
			create_task(messageDialog->ShowAsync());
			return create_task([] {return false; });
		}
    });
    
}
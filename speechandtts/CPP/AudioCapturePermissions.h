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
namespace SDKTemplate
{
    ref class AudioCapturePermissions sealed
    {
    public:
        static Windows::Foundation::IAsyncOperation<bool>^ RequestMicrophonePermissionAsync();

    private:
        // If no recording device is attached, attempting to get access to audio capture devices will throw 
        // a System.Exception object, with this HResult set.
        static const int NoCaptureDevicesHResult = -1072845856;
    };
}


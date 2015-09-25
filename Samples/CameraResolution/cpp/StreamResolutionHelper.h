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

#pragma once

namespace SDKTemplate
{
    ref class StreamResolutionHelper sealed
    {
    public:
        StreamResolutionHelper(Windows::Media::MediaProperties::IMediaEncodingProperties^ encodingProperties);

        Platform::String^ GetFriendlyName(bool showFrameRate = true);

        unsigned int Width();
        unsigned int Height();
        unsigned int FrameRate();
        double AspectRatio();

    private:
        Windows::Media::MediaProperties::IMediaEncodingProperties^ _encodingProperties;
        Platform::String^ _type;

        Platform::String^ IMAGE_ENCODING_STRING;
        Platform::String^ VIDEO_ENCODING_STRING;   
    };
}

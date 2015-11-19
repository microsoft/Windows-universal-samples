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
#include "StreamResolutionHelper.h"
#include <sstream>
#include <math.h>
#include <iomanip>

using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Media::MediaProperties;

StreamResolutionHelper::StreamResolutionHelper(IMediaEncodingProperties^ encodingProperties)
    : _encodingProperties(encodingProperties)
    , _type(encodingProperties->GetType()->FullName)
    , IMAGE_ENCODING_STRING(ref new String(L"Windows.Media.MediaProperties.ImageEncodingProperties"))
    , VIDEO_ENCODING_STRING(ref new String(L"Windows.Media.MediaProperties.VideoEncodingProperties"))
{
}

/// <summary>
/// Output properties to a readable format for UI purposes
/// eg. 1920x1080 [1.78] 30fps MPEG
/// </summary>
/// <param name="showFrameRate"></param>
/// <returns>Readable string</returns>
Platform::String^ StreamResolutionHelper::GetFriendlyName(bool showFrameRate)
{
    std::wstringstream ss;
  
    if (_type == IMAGE_ENCODING_STRING || !showFrameRate)
    {
        ss << Width() << "x" << Height() << " [" << std::setprecision(3) << AspectRatio() << "] " 
            << _encodingProperties->Subtype->ToString()->Data();
    }
    else if (_type == VIDEO_ENCODING_STRING)
    {
        ss << Width() << "x" << Height() << " [" << std::setprecision(3) << AspectRatio() << "] "
            << FrameRate() << "FPS " << _encodingProperties->Subtype->ToString()->Data();
    }
    
    return ref new String(ss.str().c_str());
}

unsigned int StreamResolutionHelper::Width()
{
    if (_type == IMAGE_ENCODING_STRING)
    {
        return static_cast<ImageEncodingProperties^>(_encodingProperties)->Width;
    }
    else if (_type == VIDEO_ENCODING_STRING)
    {
        return static_cast<VideoEncodingProperties^>(_encodingProperties)->Width;
    }

    return 0;
}

unsigned int StreamResolutionHelper::Height()
{
    if (_type == IMAGE_ENCODING_STRING)
    {
        return static_cast<ImageEncodingProperties^>(_encodingProperties)->Height;
    }
    else if (_type == VIDEO_ENCODING_STRING)
    {
        return static_cast<VideoEncodingProperties^>(_encodingProperties)->Height;
    }

    return 0;
}

unsigned int StreamResolutionHelper::FrameRate()
{
    if (_type == VIDEO_ENCODING_STRING)
    {
        VideoEncodingProperties^ videoEncodingProperties = static_cast<VideoEncodingProperties^>(_encodingProperties);
        if (videoEncodingProperties->FrameRate->Denominator != 0)
        {
            return videoEncodingProperties->FrameRate->Numerator / videoEncodingProperties->FrameRate->Denominator;
        }
    }

    return 0;
}

double StreamResolutionHelper::AspectRatio()
{
    double width = static_cast<double>(Width());
    double height = static_cast<double>(Height());
    return round(width != 0) ? width / height : NAN;
}

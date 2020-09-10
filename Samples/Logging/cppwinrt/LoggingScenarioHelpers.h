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

namespace winrt::SDKTemplate
{
    // Create a time stamp to append to log file names so 
    // each file has a unique name. The format is:
    //     YYMMDD-hhmmssMMM 
    // where
    //     YY == year
    //     MM == month
    //     DD == day
    //     hh == hours
    //     mm == minutes
    //     ss == seconds
    //     MMM == milliseconds
    inline std::wstring
    GetTimeStamp()
    {
        std::wstring result;
        SYSTEMTIME timeNow;
        GetLocalTime(&timeNow);
        result.resize(16);
        swprintf_s(
            &result[0],
            result.size() + 1,
            L"%02u%02u%02u-%02u%02u%02u%03u",
            timeNow.wYear - 2000,
            timeNow.wMonth,
            timeNow.wDay,
            timeNow.wHour,
            timeNow.wMinute,
            timeNow.wSecond,
            timeNow.wMilliseconds);
        return result;
    }

    inline Windows::UI::Xaml::Controls::ScrollViewer
    FindScrollViewer(Windows::UI::Xaml::DependencyObject const& control)
    {
        if (control == nullptr)
        {
            return nullptr;
        }

        int childCount = Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(control);
        if (childCount <= 0)
        {
            return nullptr;
        }

        for (int childIndex = 0; childIndex < childCount; childIndex++)
        {
            Windows::UI::Xaml::DependencyObject childControl =
                Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(control, childIndex);
            Windows::UI::Xaml::Controls::ScrollViewer viewer =
                childControl.try_as<Windows::UI::Xaml::Controls::ScrollViewer>();
            if (viewer != nullptr)
            {
                return viewer;
            }

            viewer = FindScrollViewer(childControl);
            if (viewer != nullptr)
            {
                return viewer;
            }
        }

        return nullptr;
    }
}

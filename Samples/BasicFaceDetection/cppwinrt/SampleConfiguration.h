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
#include "pch.h"

namespace winrt::SDKTemplate::SampleHelpers
{
    void HighlightFaces(
        Windows::UI::Xaml::Media::Imaging::WriteableBitmap const& displaySource,
        Windows::Foundation::Collections::IVector<Windows::Media::FaceAnalysis::DetectedFace> const& foundFaces,
        Windows::UI::Xaml::Controls::Canvas const& canvas,
        Windows::UI::Xaml::DataTemplate const& dataTemplate);
    void RepositionFaces(
        Windows::UI::Xaml::Media::Imaging::WriteableBitmap const& displaySource,
        Windows::UI::Xaml::Controls::Canvas const& canvas);
}

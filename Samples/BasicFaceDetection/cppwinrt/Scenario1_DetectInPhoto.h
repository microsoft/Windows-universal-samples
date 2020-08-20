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

#include "Scenario1_DetectInPhoto.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_DetectInPhoto : Scenario1_DetectInPhotoT<Scenario1_DetectInPhoto>
    {
        Scenario1_DetectInPhoto();

        fire_and_forget OpenFile_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void PhotoCanvas_SizeChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::SizeChangedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        // Limit on the height of the source image (in pixels) passed into FaceDetector for performance considerations.
        // Images larger that this size will be downscaled proportionally.
        //
        // This is an arbitrary value that was chosen for this scenario, in which FaceDetector performance isn't too important but face
        // detection accuracy is; a generous size is used.
        // Your application may have different performance and accuracy needs and you'll need to decide how best to control input.
        static constexpr uint32_t sourceImageHeightLimit = 1280;

        void SetupVisualization(Windows::UI::Xaml::Media::Imaging::WriteableBitmap const& displaySource, Windows::Foundation::Collections::IVector<Windows::Media::FaceAnalysis::DetectedFace> const& foundFaces);
        void ClearVisualization();
        Windows::Graphics::Imaging::BitmapTransform ComputeScalingTransformForSourceImage(Windows::Graphics::Imaging::BitmapDecoder const& sourceDecoder);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_DetectInPhoto : Scenario1_DetectInPhotoT<Scenario1_DetectInPhoto, implementation::Scenario1_DetectInPhoto>
    {
    };
}

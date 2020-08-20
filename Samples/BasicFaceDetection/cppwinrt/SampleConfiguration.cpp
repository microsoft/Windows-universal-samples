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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Media::FaceAnalysis;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;
using namespace winrt::SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Face detection C++/WinRT sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Detect Faces in Photos", xaml_typename<SDKTemplate::Scenario1_DetectInPhoto>() },
    Scenario{ L"Detect Faces in Webcam", xaml_typename<SDKTemplate::Scenario2_DetectInWebcam>() },
});

// Rescale the size and position of the face highlight
// to account for the difference between the size of the image and
// the canvas.
void ApplyScale(FrameworkElement const& box, double widthScale, double heightScale)
{
    // We saved the original size of the face box in the element's Tag field.
    BitmapBounds faceBox = unbox_value<BitmapBounds>(box.Tag());
    box.Width(faceBox.Width * widthScale);
    box.Height(faceBox.Height * heightScale);
    box.Margin({ faceBox.X * widthScale, faceBox.Y * heightScale, 0, 0 });
}

void SampleHelpers::HighlightFaces(
    WriteableBitmap const& displaySource,
    IVector<DetectedFace> const& foundFaces,
    Canvas const& canvas,
    DataTemplate const& dataTemplate)
{

    double widthScale = canvas.ActualWidth() / displaySource.PixelWidth();
    double heightScale = canvas.ActualHeight() / displaySource.PixelHeight();

    for (DetectedFace face : foundFaces)
    {
        // Create an element for displaying the face box but since we're using a Canvas
        // we must scale the elements according to the image's actual size.
        // The original FaceBox values are saved in the element's Tag field so we can update the
        // boxes when the Canvas is resized.
        FrameworkElement box = dataTemplate.LoadContent().as<FrameworkElement>();
        box.Tag(box_value(face.FaceBox()));
        ApplyScale(box, widthScale, heightScale);
        canvas.Children().Append(box);
    }

    hstring message;
    int32_t faceCount = foundFaces.Size();
    if (faceCount == 0)
    {
        message = L"Didn't find any human faces in the image";
    }
    else if (faceCount == 1)
    {
        message = L"Found a human face in the image";
    }
    else
    {
        message = L"Found " + to_hstring(faceCount) + L" human faces in the image";
    }

    MainPage::Current().NotifyUser(message, NotifyType::StatusMessage);

}

void SampleHelpers::RepositionFaces(WriteableBitmap const& displaySource, Canvas const& canvas)
{
    double widthScale = canvas.ActualWidth() / displaySource.PixelWidth();
    double heightScale = canvas.ActualHeight() / displaySource.PixelHeight();

    for (UIElement element : canvas.Children())
    {
        ApplyScale(element.as<FrameworkElement>(), widthScale, heightScale);
    }
}

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
#include "Scenario1_DetectInPhoto.h"
#include "Scenario1_DetectInPhoto.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Media::FaceAnalysis;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Media::Imaging;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_DetectInPhoto::Scenario1_DetectInPhoto()
    {
        InitializeComponent();
    }

    // Takes the photo image and FaceDetector results and assembles the visualization onto the Canvas.
    //
    // displaySource is a bitmap object holding the image we're going to display.
    // foundFaces is a list of detected faces; output from FaceDetector.
    void Scenario1_DetectInPhoto::SetupVisualization(WriteableBitmap const& displaySource, IVector<DetectedFace> const& foundFaces)
    {
        ImageBrush brush;
        brush.ImageSource(displaySource);
        brush.Stretch(Stretch::Fill);
        PhotoCanvas().Background(brush);

        SampleHelpers::HighlightFaces(displaySource, foundFaces, PhotoCanvas(), HighlightedFaceBox());
    }

    // Clears the display of image and face boxes.
    void Scenario1_DetectInPhoto::ClearVisualization()
    {
        PhotoCanvas().Background(nullptr);
        PhotoCanvas().Children().Clear();
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);
    }

    // Computes a BitmapTransform to downscale the source image if it's too large. 
    //
    // Performance of the FaceDetector degrades significantly with large images, and in most cases it's best to downscale
    // the source bitmaps if they're too large before passing them into FaceDetector. Remember through, your application's performance needs will vary.
    BitmapTransform Scenario1_DetectInPhoto::ComputeScalingTransformForSourceImage(BitmapDecoder const& sourceDecoder)
    {
        BitmapTransform transform;

        uint32_t sourceHeight = sourceDecoder.PixelHeight();
        if (sourceHeight > sourceImageHeightLimit)
        {
            double scalingFactor = (double)sourceImageHeightLimit / sourceHeight;

            transform.ScaledWidth((uint32_t)(sourceDecoder.PixelWidth() * scalingFactor));
            transform.ScaledHeight((uint32_t)(sourceHeight * scalingFactor));
        }

        return transform;
    }

    // Loads an image file (selected by the user) and runs the FaceDetector on the loaded bitmap. If successful calls SetupVisualization to display the results.
    fire_and_forget Scenario1_DetectInPhoto::OpenFile_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        FileOpenPicker photoPicker;
        photoPicker.ViewMode(PickerViewMode::Thumbnail);
        photoPicker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
        photoPicker.FileTypeFilter().ReplaceAll({ L".jpg", L".jpeg", L".png", L".bmp" });

        StorageFile photoFile = co_await photoPicker.PickSingleFileAsync();
        if (photoFile == nullptr)
        {
            co_return;
        }

        ClearVisualization();
        rootPage.NotifyUser(L"Opening...", NotifyType::StatusMessage);

        try
        {
            // Open the image file and decode the bitmap into memory.
            // We'll need to make 2 bitmap copies: one for the FaceDetector and another to display.
            IRandomAccessStream fileStream = co_await photoFile.OpenAsync(FileAccessMode::Read);
            {
                BitmapDecoder decoder = co_await BitmapDecoder::CreateAsync(fileStream);
                BitmapTransform transform = ComputeScalingTransformForSourceImage(decoder);

                SoftwareBitmap originalBitmap = co_await decoder.GetSoftwareBitmapAsync(decoder.BitmapPixelFormat(), BitmapAlphaMode::Ignore, transform, ExifOrientationMode::IgnoreExifOrientation, ColorManagementMode::DoNotColorManage);

                // We need to convert the image into a format that's compatible with FaceDetector.
                // Gray8 should be a good type but verify it against FaceDetector’s supported formats.
                static constexpr BitmapPixelFormat InputPixelFormat = BitmapPixelFormat::Gray8;
                if (FaceDetector::IsBitmapPixelFormatSupported(InputPixelFormat))
                {
                    SoftwareBitmap detectorInput = SoftwareBitmap::Convert(originalBitmap, InputPixelFormat);

                    // Create a WritableBitmap for our visualization display; copy the original bitmap pixels to wb's buffer.
                    WriteableBitmap displaySource(originalBitmap.PixelWidth(), originalBitmap.PixelHeight());
                    originalBitmap.CopyToBuffer(displaySource.PixelBuffer());

                    rootPage.NotifyUser(L"Detecting...", NotifyType::StatusMessage);

                    // Initialize our FaceDetector and execute it against our input image.
                    // NOTE: FaceDetector initialization can take a long time, and in most cases
                    // you should create a member variable and reuse the object.
                    // However, for simplicity in this scenario we instantiate a new instance each time.
                    FaceDetector detector = co_await FaceDetector::CreateAsync();
                    IVector<DetectedFace> faces = co_await detector.DetectFacesAsync(detectorInput);

                    // Create our display using the available image and face results.
                    SetupVisualization(displaySource, faces);
                }
                else
                {
                    rootPage.NotifyUser(L"PixelFormat 'Gray8' is not supported by FaceDetector", NotifyType::ErrorMessage);
                }
            }
        }
        catch (hresult_error const& ex)
        {
            ClearVisualization();
            rootPage.NotifyUser(ex.message(), NotifyType::ErrorMessage);
        }
    }

   // Updates any existing face bounding boxes in response to changes in the size of the Canvas.
    void Scenario1_DetectInPhoto::PhotoCanvas_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        // If the Canvas is resized we must recompute a new scaling factor and
        // apply it to each face box.
        ImageBrush brush = PhotoCanvas().Background().try_as<ImageBrush>();
        if (brush != nullptr)
        {
            WriteableBitmap displaySource = brush.ImageSource().as<WriteableBitmap>();
            SampleHelpers::RepositionFaces(displaySource, PhotoCanvas());
        }
    }
}

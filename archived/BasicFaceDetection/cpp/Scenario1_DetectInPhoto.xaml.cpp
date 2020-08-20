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
#include "Scenario1_DetectInPhoto.xaml.h"

using namespace SDKTemplate;

using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::FaceAnalysis;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Shapes;

SDKTemplate::DetectFacesInPhoto::DetectFacesInPhoto() :
    rootPage(MainPage::Current)
{
    InitializeComponent();
}

void DetectFacesInPhoto::OnNavigatedTo(NavigationEventArgs^ e)
{
    this->rootPage = MainPage::Current;
}

void DetectFacesInPhoto::SetupVisualization(
    WriteableBitmap^ displaySource,
    IVector<DetectedFace^>^ foundFaces)
{
    ImageBrush^ brush = ref new ImageBrush();
    brush->ImageSource = displaySource;
    brush->Stretch = Stretch::Fill;
    this->PhotoCanvas->Background = brush;

    double actualWidth = this->PhotoCanvas->ActualWidth;
    double actualHeight = this->PhotoCanvas->ActualHeight;

    if (foundFaces != nullptr && actualWidth != 0 && actualHeight != 0)
    {
        double widthScale = displaySource->PixelWidth / actualWidth;
        double heightScale = displaySource->PixelHeight / actualHeight;

        for (DetectedFace^ face : foundFaces)
        {
            // Create a rectangle element for displaying the face box but since we're using a Canvas
            // we must scale the rectangles according to the image’s actual size.
            // The original FaceBox values are saved in the Rectangle's Tag field so we can update the
            // boxes when the Canvas is resized.
            Rectangle^ box = ref new Rectangle();
            box->Tag = face->FaceBox;
            box->Width = face->FaceBox.Width / widthScale;
            box->Height = face->FaceBox.Height / heightScale;
            box->Fill = this->fillBrush;
            box->Stroke = this->lineBrush;
            box->StrokeThickness = this->lineThickness;
            box->Margin = Thickness(face->FaceBox.X / widthScale, face->FaceBox.Y / heightScale, 0, 0);

            this->PhotoCanvas->Children->Append(box);
        }
    }

    Platform::String^ message;
    if (foundFaces == nullptr || foundFaces->Size == 0)
    {
        message = "Didn't find any human faces in the image";
    }
    else if (foundFaces->Size == 1)
    {
        message = "Found a human face in the image";
    }
    else
    {
        message = "Found " + foundFaces->Size + " human faces in the image";
    }

    this->rootPage->NotifyUser(message, NotifyType::StatusMessage);
}

void DetectFacesInPhoto::ClearVisualization()
{
    this->PhotoCanvas->Background = nullptr;
    this->PhotoCanvas->Children->Clear();
    this->rootPage->NotifyUser(ref new Platform::String(), NotifyType::StatusMessage);
}

BitmapTransform^ DetectFacesInPhoto::ComputeScalingTransformForSourceImage(BitmapDecoder^ sourceDecoder)
{
    BitmapTransform^ transform = ref new BitmapTransform();

    if (sourceDecoder->PixelHeight > this->sourceImageHeightLimit)
    {
        float scalingFactor = this->sourceImageHeightLimit / static_cast<float>(sourceDecoder->PixelHeight);

        transform->ScaledWidth = static_cast<unsigned int>(std::floor(sourceDecoder->PixelWidth * scalingFactor));
        transform->ScaledHeight = static_cast<unsigned int>(std::floor(sourceDecoder->PixelHeight * scalingFactor));
    }

    return transform;
}

void DetectFacesInPhoto::ProcessFaceDetectionOnImage(StorageFile^ photoFile)
{
    this->ClearVisualization();
    this->rootPage->NotifyUser("Opening...", NotifyType::StatusMessage);

    // Open the image file and decode the bitmap into memory.
    // We'll need to make 2 bitmap copies: one for the FaceDetector and another to display.
    auto task = concurrency::create_task(photoFile->OpenAsync(FileAccessMode::Read))
        .then([this](IRandomAccessStream^ fileStream)
    {
        return concurrency::create_task(BitmapDecoder::CreateAsync(fileStream));

    }).then([this](BitmapDecoder^ decoder)
    {
        BitmapTransform^ transform = ComputeScalingTransformForSourceImage(decoder);

        return concurrency::create_task(decoder->GetSoftwareBitmapAsync(decoder->BitmapPixelFormat, BitmapAlphaMode::Ignore, transform, ExifOrientationMode::IgnoreExifOrientation, ColorManagementMode::DoNotColorManage));

    }).then([this](SoftwareBitmap^ originalBitmap)
    {
        // We need to convert the image into a format that's compatible with FaceDetector.
        // Gray8 should be a good type but verify it against FaceDetector’s supported formats.
        const BitmapPixelFormat InputPixelFormat = BitmapPixelFormat::Gray8;
        if (FaceDetector::IsBitmapPixelFormatSupported(InputPixelFormat))
        {
            SoftwareBitmap^ detectorInput = SoftwareBitmap::Convert(originalBitmap, InputPixelFormat);

            // Create a WritableBitmap for our visualization display; copy the original bitmap pixels to wb's buffer.
            WriteableBitmap^ displaySource = ref new WriteableBitmap(originalBitmap->PixelWidth, originalBitmap->PixelHeight);
            originalBitmap->CopyToBuffer(displaySource->PixelBuffer);

            this->rootPage->NotifyUser("Detecting...", NotifyType::StatusMessage);

            // Initialize our FaceDetector and execute it against our input image.
            // NOTE: FaceDetector initialization can take a long time, and in most cases
            // you should create a member variable and reuse the object.
            // However, for simplicity in this scenario we instantiate a new instance each time, but we
            // must be careful to hold a reference to the FaceDetector object (capture the detector variable)
            // during the DetectFacesAsync task.
            return concurrency::create_task(FaceDetector::CreateAsync())
                .then([this, detectorInput, displaySource](FaceDetector^ detector)
            {
                return concurrency::create_task(detector->DetectFacesAsync(detectorInput))
                    .then([this, detector, detectorInput, displaySource](IVector<DetectedFace^>^ faces)
                {
                    // Create our display using the available image and face results.
                    this->SetupVisualization(displaySource, faces);
                });
            });
        }
        else
        {
            // We need to return a task<void> object from this lambda to continue the task chain,
            // and also the call to BitmapPixelFormat::ToString() must occur on the UI thread.
            return concurrency::create_task(Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
                ref new Windows::UI::Core::DispatchedHandler([this, InputPixelFormat]()
            {
                this->rootPage->NotifyUser("PixelFormat '" + InputPixelFormat.ToString() + "' is not supported by FaceDetector", NotifyType::ErrorMessage);
            })));
        }

    }).then([this](concurrency::task<void> finalTask)
    {
        // Use a final continuation task to handle any exceptions that may have been thrown
        // during the task sequence.
        try
        {
            finalTask.get();
        }
        catch (Platform::Exception^ ex)
        {
            ClearVisualization();
            this->rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
        }
    });
}

void DetectFacesInPhoto::OpenFile_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    FileOpenPicker^ photoPicker = ref new FileOpenPicker();
    photoPicker->ViewMode = PickerViewMode::Thumbnail;
    photoPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    photoPicker->FileTypeFilter->Append(".jpg");
    photoPicker->FileTypeFilter->Append(".jpeg");
    photoPicker->FileTypeFilter->Append(".png");
    photoPicker->FileTypeFilter->Append(".bmp");

    concurrency::create_task(photoPicker->PickSingleFileAsync())
        .then([this](StorageFile^ photoFile)
    {
        if (photoFile != nullptr)
        {
            ProcessFaceDetectionOnImage(photoFile);
        }
    });
}

void DetectFacesInPhoto::PhotoCanvas_SizeChanged(Platform::Object^ sender, SizeChangedEventArgs^ e)
{
    try
    {
        double actualWidth = this->PhotoCanvas->ActualWidth;
        double actualHeight = this->PhotoCanvas->ActualHeight;

        // If the Canvas is resized we must recompute a new scaling factor and
        // apply it to each face box.
        if (this->PhotoCanvas->Background != nullptr && actualWidth != 0 && actualHeight != 0)
        {
            WriteableBitmap^ displaySource = dynamic_cast<WriteableBitmap^>(dynamic_cast<ImageBrush^>(this->PhotoCanvas->Background)->ImageSource);

            double widthScale = displaySource->PixelWidth / actualWidth;
            double heightScale = displaySource->PixelHeight / actualHeight;

            for (UIElement^ item : PhotoCanvas->Children)
            {
                Rectangle^ box = dynamic_cast<Rectangle^>(item);
                if (box == nullptr)
                {
                    continue;
                }

                // We saved the original size of the face box in the rectangles Tag field.
                BitmapBounds faceBounds = static_cast<BitmapBounds>(box->Tag);
                box->Width = static_cast<unsigned int>(faceBounds.Width / widthScale);
                box->Height = static_cast<unsigned int>(faceBounds.Height / heightScale);

                box->Margin = Thickness(faceBounds.X / widthScale, faceBounds.Y / heightScale, 0, 0);
            }
        }
    }
    catch (Platform::Exception^ ex)
    {
        this->rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
    }
}

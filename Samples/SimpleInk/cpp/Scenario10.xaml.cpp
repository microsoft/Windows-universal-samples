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
#include "Scenario10.xaml.h"
#include <robuffer.h> // IBufferByteAccess

using namespace Concurrency;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Platform::Collections;
using namespace SDKTemplate;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;

Scenario10::Scenario10()
{
    InitializeComponent();

    inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen | CoreInputDeviceTypes::Touch;
}

static inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw Exception::CreateException(hr);
    }
}

task<void> Scenario10::ColorizeCustomButtonAsync(Windows::UI::Color newColor)
{
    // Get the bitmap from the png
    auto sourceUri = ref new Uri("ms-appx:///Assets/ButtonIconFill.png");
    return create_task(StorageFile::GetFileFromApplicationUriAsync(sourceUri))
        .then([](StorageFile^ storageFile)
    {
        return create_task(storageFile->OpenAsync(FileAccessMode::Read));
    }).then([](IRandomAccessStream^ readableStream)
    {
        return create_task(BitmapDecoder::CreateAsync(readableStream));
    }).then([newColor](BitmapDecoder^ bitmapDecoder)
    {
        BitmapTransform^ dummyTransform = ref new BitmapTransform();
        return create_task(bitmapDecoder->GetPixelDataAsync(
            BitmapPixelFormat::Bgra8,
            BitmapAlphaMode::Straight,
            dummyTransform,
            ExifOrientationMode::RespectExifOrientation,
            ColorManagementMode::DoNotColorManage))
            .then([newColor, bitmapDecoder](PixelDataProvider^ pixelDataProvider)
        {
            // We now have direct access to the pixels.
            Platform::Array<byte>^ pixelData = pixelDataProvider->DetachPixelData();

            // Put everything into local variables to help the compiler optimize
            // this inner loop.
            byte* rawPixelData = pixelData->Data;
            unsigned pixelHeight = bitmapDecoder->PixelHeight;
            unsigned pixelWidth = bitmapDecoder->PixelWidth;

            for (unsigned row = 0; row < pixelHeight; row++)
            {
                for (unsigned col = 0; col < pixelWidth; col++)
                {
                    int offset = (row * (int)pixelWidth * 4) + (col * 4);

                    // Replace the pixel with newColor if it is our color key.
                    if ((rawPixelData[offset] <= 0x01) &&
                        (rawPixelData[offset + 1] <= 0x01) &&
                        (rawPixelData[offset + 2] <= 0x01) &&
                        (rawPixelData[offset + 3] != 0x00))
                    {
                        rawPixelData[offset] = newColor.B;
                        rawPixelData[offset + 1] = newColor.G;
                        rawPixelData[offset + 2] = newColor.R;
                        rawPixelData[offset + 3] = newColor.A;
                    }
                }
            }

            // Create a bitmap from our modified pixel data.
            auto bitmap = ref new WriteableBitmap((int)bitmapDecoder->PixelWidth, (int)bitmapDecoder->PixelHeight);

            ComPtr<IInspectable> bufferAsInspectable(reinterpret_cast<IInspectable*>(bitmap->PixelBuffer));
            ComPtr<IBufferByteAccess> bufferAsByteAccess;
            ThrowIfFailed(bufferAsInspectable.As(&bufferAsByteAccess));

            byte* pixels;
            ThrowIfFailed(bufferAsByteAccess->Buffer(&pixels));
            memcpy(pixels, pixelData->Data, pixelData->Length);
            bitmap->Invalidate();

            return bitmap;
        });
    }).then([this](WriteableBitmap^ bitmap)
    {
        CustomButtonFill->Source = bitmap;
    });
}

void Scenario10::inkToolbar_InkDrawingAttributesChanged(InkToolbar^ sender, Object^ args)
{
    // Change the color of the button icon for the calligraphic pen button
    if (CalligraphicPenButton == inkToolbar->ActiveTool)
    {
        ColorizeCustomButtonAsync(sender->InkDrawingAttributes->Color);
    }
}

void Scenario10::inkToolbar_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    auto selectedBrush = safe_cast<SolidColorBrush^>(CalligraphicPenButton->SelectedBrush);
    ColorizeCustomButtonAsync(selectedBrush->Color);
}

void Scenario10::OnSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
    HelperFunctions::UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
}


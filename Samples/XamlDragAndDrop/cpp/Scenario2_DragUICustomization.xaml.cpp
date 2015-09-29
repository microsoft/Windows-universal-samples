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
#include "Scenario2_DragUICustomization.xaml.h"

using namespace SDKTemplate;
using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage::Streams;
using namespace Windows::ApplicationModel::DataTransfer;


Scenario2_DragUICustomization::Scenario2_DragUICustomization()
    : rootPage(MainPage::Current),
    _insideName(ref new String(L"Inside")),
    _outsideName(ref new String(L"Outside")),
    _dropHere(ref new String(L"Drop here to insert text"))
{
    InitializeComponent();
}


bool Scenario2_DragUICustomization::IsChecked(Windows::UI::Xaml::Controls::Primitives::ToggleButton^ button)
{
    return (button->IsChecked != nullptr) && button->IsChecked->Value;
}

/// <summary>
/// Start of the Drag and Drop operation: we set some content and change the DragUI
/// depending on the selected options
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void Scenario2_DragUICustomization::SourceGrid_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args)
{
    args->Data->SetText(SourceTextBox->Text);
    if (IsChecked(DataPackageRB))
    {
        // Standard icon will be used as the DragUIContent
        args->DragUI->SetContentFromDataPackage();
    }
    else if (IsChecked(CustomContentRB))
    {
        // Generate a bitmap with only the TextBox
        // We need to take the deferral as the rendering won't be completed synchronously
        auto deferral = args->GetDeferral();
        auto rtb = ref new RenderTargetBitmap();
        create_task(rtb->RenderAsync(SourceTextBox)).then([rtb]()
        {
            return rtb->GetPixelsAsync();
        }).then([rtb, deferral, args](IBuffer^ buffer)
        {
            auto bitmap = SoftwareBitmap::CreateCopyFromBuffer(buffer,
                BitmapPixelFormat::Bgra8,
                rtb->PixelWidth,
                rtb->PixelHeight,
                BitmapAlphaMode::Premultiplied);
            args->DragUI->SetContentFromSoftwareBitmap(bitmap);
            deferral->Complete();
        });
    }
    // else just show the dragged UIElement
}

/// <summary>
/// Entering the Target, we'll change its background and optionally change the DragUI as well
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario2_DragUICustomization::TargetTextBox_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    /// Change the background of the target
    VisualStateManager::GoToState(this, _insideName, true);
    bool hasText = e->DataView->Contains(StandardDataFormats::Text);
    e->AcceptedOperation = hasText ? DataPackageOperation::Copy : DataPackageOperation::None;
    if (hasText)
    {
        e->DragUIOverride->Caption = _dropHere;
        // Now customize the content
        if (IsChecked(HideRB))
        {
            e->DragUIOverride->IsGlyphVisible = false;
            e->DragUIOverride->IsContentVisible = false;
        }
        else if (IsChecked(CustomRB))
        {
            auto bitmap = ref new BitmapImage(ref new Uri(ref new String(L"ms-appx:///Assets/dropcursor.png")));
            // Anchor will define how to position the image relative to the pointer
            Point anchor{ 0,52 }; // lower left corner of the image
            e->DragUIOverride->SetContentFromBitmapImage(bitmap, anchor);
            e->DragUIOverride->IsGlyphVisible = false;
            e->DragUIOverride->IsCaptionVisible = false;
        }
        // else keep the DragUI Content set by the source
    }
}

/// <summary>
/// DragLeave: Restore previous background
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario2_DragUICustomization::TargetTextBox_DragLeave(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    VisualStateManager::GoToState(this, _outsideName, true);
}

/// <summary>
/// Drop: restore the background and append the dragged text
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario2_DragUICustomization::TargetTextBox_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
    VisualStateManager::GoToState(this, _outsideName, true);
    bool hasText = e->DataView->Contains(StandardDataFormats::Text);
    e->AcceptedOperation = hasText ? DataPackageOperation::Copy : DataPackageOperation::None;
    if (hasText)
    {
        // if the result of the drop is not too important (and a text copy should have no impact on source)
        // we don't need to take the deferral and this will complete the operation faster
        create_task(e->DataView->GetTextAsync()).then([this](String^ text)
        {
            TargetTextBox->Text += text;
        });
    }
}

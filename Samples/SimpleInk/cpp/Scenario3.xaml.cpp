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
#include "Scenario3.xaml.h"
#include <WindowsNumerics.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <ppltasks.h>

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Input::Inking;
using namespace Windows::UI::Core;

Scenario3::Scenario3() : rootPage(MainPage::Current)
{
    InitializeComponent();

    penSize = minPenSize + penSizeIncrement * PenThickness->SelectedIndex;

    InkDrawingAttributes^ drawingAttributes = ref new InkDrawingAttributes();
    drawingAttributes->Color = Windows::UI::Colors::Red;
    drawingAttributes->Size = Size((float)penSize, (float)penSize);
    drawingAttributes->IgnorePressure = false;
    drawingAttributes->FitToCurve = true;

    inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
    inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen;
    inkCanvas->InkPresenter->StrokesCollected += ref new TypedEventHandler<InkPresenter^, InkStrokesCollectedEventArgs^>(this, &Scenario3::InkPresenter_StrokesCollected);
    inkCanvas->InkPresenter->StrokesErased += ref new TypedEventHandler<InkPresenter^, InkStrokesErasedEventArgs^>(this, &Scenario3::InkPresenter_StrokesErased);
}

void Scenario3::OnSizeChanged(Object^ sender, SizeChangedEventArgs e)
{
    HelperFunctions::UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
}

void Scenario3::InkPresenter_StrokesErased(InkPresenter^ sender, InkStrokesErasedEventArgs^ args)
{
    rootPage->NotifyUser(args->Strokes->Size + " stroke(s) erased!", NotifyType::StatusMessage);
}

void Scenario3::InkPresenter_StrokesCollected(InkPresenter^ sender, InkStrokesCollectedEventArgs^ args)
{
    rootPage->NotifyUser(args->Strokes->Size + " stroke(s) collected!", NotifyType::StatusMessage);
}

void Scenario3::OnSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
    HelperFunctions::UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
}

void Scenario3::OnPenColorChanged(Object^ sender, RoutedEventArgs^ e)
{
    if (inkCanvas != nullptr)
    {
        InkDrawingAttributes^ drawingAttributes = inkCanvas->InkPresenter->CopyDefaultDrawingAttributes();

        // Use button's background to set new pen's color
        auto btnSender = safe_cast<Button^>(sender);
        auto brush = safe_cast<Media::SolidColorBrush^>(btnSender->Background);

        drawingAttributes->Color = brush->Color;
        inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
    }
}

void Scenario3::OnPenThicknessChanged(Object^ sender, RoutedEventArgs^ e)
{
    if (inkCanvas != nullptr)
    {
        InkDrawingAttributes^ drawingAttributes = inkCanvas->InkPresenter->CopyDefaultDrawingAttributes();
        penSize = minPenSize + penSizeIncrement * PenThickness->SelectedIndex;
        String^ value = safe_cast<ComboBoxItem^>(PenType->SelectedItem)->Content->ToString();
        if (value == "Highlighter" || value == "Calligraphy")
        {
            // Make the pen tip rectangular for highlighter and calligraphy pen
            drawingAttributes->Size = Size((float)penSize, (float)(penSize * 2));
        }
        else
        {
            // Otherwise, use a square pen tip.
            drawingAttributes->Size = Size((float)penSize, (float)penSize);
        }
        inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
    }
}

void Scenario3::OnPenTypeChanged(Object^ sender, RoutedEventArgs^ e)
{
    if (inkCanvas != nullptr)
    {
        InkDrawingAttributes^ drawingAttributes = inkCanvas->InkPresenter->CopyDefaultDrawingAttributes();
        String^ value = safe_cast<ComboBoxItem^>(PenType->SelectedItem)->Content->ToString();

        if (value == "Ballpoint")
        {
            if (drawingAttributes->Kind != InkDrawingAttributesKind::Default)
            {
                auto newDrawingAttributes = ref new InkDrawingAttributes();
                newDrawingAttributes->Color = drawingAttributes->Color;
                drawingAttributes = newDrawingAttributes;
            }
            drawingAttributes->Size = Size((float)penSize, (float)penSize);
            drawingAttributes->PenTip = PenTipShape::Circle;
            drawingAttributes->DrawAsHighlighter = false;
            drawingAttributes->PenTipTransform = float3x2::identity();
        }
        else if (value == "Highlighter")
        {
            if (drawingAttributes->Kind != InkDrawingAttributesKind::Default)
            {
                auto newDrawingAttributes = ref new InkDrawingAttributes();
                newDrawingAttributes->Color = drawingAttributes->Color;
                drawingAttributes = newDrawingAttributes;
            }
            // Make the pen rectangular for highlighter
            drawingAttributes->Size = Size((float)penSize, (float)(penSize * 2));
            drawingAttributes->PenTip = PenTipShape::Rectangle;
            drawingAttributes->DrawAsHighlighter = true;
            drawingAttributes->PenTipTransform = float3x2::identity();
        }
        else if (value == "Calligraphy")
        {
            if (drawingAttributes->Kind != InkDrawingAttributesKind::Default)
            {
                auto newDrawingAttributes = ref new InkDrawingAttributes();
                newDrawingAttributes->Color = drawingAttributes->Color;
                drawingAttributes = newDrawingAttributes;
            }
            drawingAttributes->Size = Size((float)penSize, (float)(penSize * 2));
            drawingAttributes->PenTip = PenTipShape::Rectangle;
            drawingAttributes->DrawAsHighlighter = false;

            // Set a 45 degree rotation on the pen tip
            double radians = 45.0 * M_PI / 180.0;
            drawingAttributes->PenTipTransform = make_float3x2_rotation((float)radians, float2::zero());
        }
        else if (value == "Pencil")
        {
            if (drawingAttributes->Kind != InkDrawingAttributesKind::Pencil)
            {
                auto newDrawingAttributes = InkDrawingAttributes::CreateForPencil();
                newDrawingAttributes->Color = drawingAttributes->Color;
                newDrawingAttributes->Size = drawingAttributes->Size;
                drawingAttributes = newDrawingAttributes;
            }
        }
        inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
    }
}

void Scenario3::OnClear(Object^ sender, RoutedEventArgs^ e)
{
    inkCanvas->InkPresenter->StrokeContainer->Clear();
    rootPage->NotifyUser("Cleared Canvas", NotifyType::StatusMessage);
}

void Scenario3::OnSaveAsync(Object^ sender, RoutedEventArgs^ e)
{
    // We don't want to save an empty file
    if (inkCanvas->InkPresenter->StrokeContainer->GetStrokes()->Size == 0)
    {
        rootPage->NotifyUser("There is no ink to save.", NotifyType::ErrorMessage);
        return;
    }

    auto savePicker = ref new FileSavePicker();
    savePicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    savePicker->FileTypeChoices->Insert("Gif with embedded ISF", ref new Vector<String^>({ ".gif" }));
    savePicker->SuggestedFileName = "Scenario3";

    create_task(savePicker->PickSaveFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            // write to file
            return create_task(file->OpenAsync(FileAccessMode::ReadWrite))
                .then([this](IRandomAccessStream^ stream)
            {
                return create_task(inkCanvas->InkPresenter->StrokeContainer->SaveAsync(stream))
                    .then([this, stream](task<std::size_t> saveTask)
                {
                    // Always close the stream, even if an error occurs.
                    delete stream;

                    return saveTask;
                });
            }).then([this, file](task<std::size_t> openSaveTask)
            {
                // Report whether the file was successfully saved.
                try
                {
                    openSaveTask.get();
                    rootPage->NotifyUser(inkCanvas->InkPresenter->StrokeContainer->GetStrokes()->Size.ToString() + " stroke(s) saved!", NotifyType::StatusMessage);
                }
                catch (Platform::Exception^ ex)
                {
                    //Example output: The system cannot find the specified file.
                    rootPage->NotifyUser(ex->Message, NotifyType::ErrorMessage);
                }
            });
        }
        else
        {
            return task_from_result();
        }
    });
}

void Scenario3::OnLoadAsync(Object^ sender, RoutedEventArgs^ e)
{
    auto openPicker = ref new FileOpenPicker();
    openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
    openPicker->FileTypeFilter->Append(".gif");
    openPicker->FileTypeFilter->Append(".isf");

    create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            return create_task(file->OpenSequentialReadAsync())
                .then([this, file](IInputStream^ stream)
            {
                return create_task(inkCanvas->InkPresenter->StrokeContainer->LoadAsync(stream))
                    .then([stream](task<void> loadTask)
                {
                    // Always close the stream, even if an error occurs.
                    delete stream;

                    return loadTask;
                });
            }).then([this](task<void> openLoadTask)
            {
                // Report whether the file was successfully loaded.
                try
                {
                    openLoadTask.get();
                    rootPage->NotifyUser(inkCanvas->InkPresenter->StrokeContainer->GetStrokes()->Size.ToString() + " stroke(s) loaded!", NotifyType::StatusMessage);
                }
                catch (Platform::Exception^ ex)
                {
                    // Report any I/O errors.
                    rootPage->NotifyUser(ex->Message, NotifyType::ErrorMessage);
                }
            });
        }
        else
        {
            return task_from_result();
        }
    });
}

void Scenario3::TouchInkingCheckBox_Checked(Object^ sender, RoutedEventArgs^ e)
{
    auto types = inkCanvas->InkPresenter->InputDeviceTypes | CoreInputDeviceTypes::Touch;
    inkCanvas->InkPresenter->InputDeviceTypes = types;
    rootPage->NotifyUser("Enable Touch Inking", NotifyType::StatusMessage);
}

void Scenario3::TouchInkingCheckBox_Unchecked(Object^ sender, RoutedEventArgs^ e)
{
    auto types = inkCanvas->InkPresenter->InputDeviceTypes & ~CoreInputDeviceTypes::Touch;
    inkCanvas->InkPresenter->InputDeviceTypes = types;
    rootPage->NotifyUser("Disable Touch Inking", NotifyType::StatusMessage);
}

void Scenario3::ErasingModeCheckBox_Checked(Object^ sender, RoutedEventArgs^ e)
{
    inkCanvas->InkPresenter->InputProcessingConfiguration->Mode = InkInputProcessingMode::Erasing;
    rootPage->NotifyUser("Enable Erasing Mode", NotifyType::StatusMessage);
}

void Scenario3::ErasingModeCheckBox_Unchecked(Object^ sender, RoutedEventArgs^ e)
{
    inkCanvas->InkPresenter->InputProcessingConfiguration->Mode = InkInputProcessingMode::Inking;
    rootPage->NotifyUser("Disable Erasing Mode", NotifyType::StatusMessage);
}

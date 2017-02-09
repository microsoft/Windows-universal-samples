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
#include "Scenario2.xaml.h"
#include <WindowsNumerics.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input::Inking;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Shapes;

InkDrawingAttributes^ CalligraphicPen::CreateInkDrawingAttributesCore(Brush^ brush, double strokeWidth)
{
    InkDrawingAttributes^ inkDrawingAttributes = ref new InkDrawingAttributes();
    inkDrawingAttributes->PenTip = PenTipShape::Circle;
    inkDrawingAttributes->IgnorePressure = false;
    SolidColorBrush^ solidColorBrush = safe_cast<SolidColorBrush^>(brush);

    if (solidColorBrush != nullptr)
    {
        inkDrawingAttributes->Color = solidColorBrush->Color;
    }

    inkDrawingAttributes->Size = Size((float)strokeWidth, 2.0f * (float)strokeWidth);
    inkDrawingAttributes->PenTipTransform = make_float3x2_rotation((float)((M_PI * 45.0) / 180.0), float2::zero());

    return inkDrawingAttributes;
}

Scenario2::Scenario2()
{
    InitializeComponent();
    CalligraphyPen = (Symbol)0xEDFB;
    LassoSelect = (Symbol)0xEF20;
    TouchWriting = (Symbol)0xED5F;

    // Initialize the InkCanvas
    inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen;

    // Handlers to clear the selection when inking or erasing is detected
    inkCanvas->InkPresenter->StrokeInput->StrokeStarted += ref new TypedEventHandler<InkStrokeInput^, PointerEventArgs^>(this, &Scenario2::StrokeInput_StrokeStarted);
    inkCanvas->InkPresenter->StrokeInput->StrokeEnded += ref new TypedEventHandler<InkStrokeInput^, PointerEventArgs^>(this, &Scenario2::InkPresenter_StrokesErased);
}

void Scenario2::StrokeInput_StrokeStarted(InkStrokeInput^ sender, PointerEventArgs^ args)
{
    ClearSelection();
    inkCanvas->InkPresenter->UnprocessedInput->PointerPressed -= pointerPressedToken;
    inkCanvas->InkPresenter->UnprocessedInput->PointerMoved -= pointerMovedToken;
    inkCanvas->InkPresenter->UnprocessedInput->PointerReleased -= pointerReleasedToken;
}

void Scenario2::InkPresenter_StrokesErased(InkStrokeInput^ sender, PointerEventArgs^ args)
{
    ClearSelection();
    inkCanvas->InkPresenter->UnprocessedInput->PointerPressed -= pointerPressedToken;
    inkCanvas->InkPresenter->UnprocessedInput->PointerMoved -= pointerMovedToken;
    inkCanvas->InkPresenter->UnprocessedInput->PointerReleased -= pointerReleasedToken;
}

void Scenario2::OnSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
    HelperFunctions::UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
}

void Scenario2::Toggle_Custom(Object^ sender, RoutedEventArgs^ e)
{
    if (toggleButton->IsChecked)
    {
        auto types = inkCanvas->InkPresenter->InputDeviceTypes;
        inkCanvas->InkPresenter->InputDeviceTypes = types | CoreInputDeviceTypes::Touch;
    }
    else
    {
        auto types = inkCanvas->InkPresenter->InputDeviceTypes;
        inkCanvas->InkPresenter->InputDeviceTypes = types & ~CoreInputDeviceTypes::Touch;
    }
}

void Scenario2::UnprocessedInput_PointerPressed(InkUnprocessedInput^ sender, PointerEventArgs^  args)
{
    lasso = ref new Polyline();
    lasso->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Blue);
    lasso->StrokeThickness = 1;
    lasso->StrokeDashArray = ref new DoubleCollection();
    lasso->StrokeDashArray->Append(5.0);
    lasso->StrokeDashArray->Append(2.0);
    lasso->Points->Append(args->CurrentPoint->RawPosition);
    selectionCanvas->Children->Append(lasso);
    isBoundRect = true;
}

void Scenario2::UnprocessedInput_PointerMoved(InkUnprocessedInput^ sender, PointerEventArgs^  args)
{
    if (isBoundRect)
    {
        lasso->Points->Append(args->CurrentPoint->RawPosition);
    }
}

void Scenario2::UnprocessedInput_PointerReleased(InkUnprocessedInput^ sender, PointerEventArgs^ args)
{
    lasso->Points->Append(args->CurrentPoint->RawPosition);

    boundingRect = inkCanvas->InkPresenter->StrokeContainer->SelectWithPolyLine(lasso->Points);
    isBoundRect = false;
    DrawBoundingRect();
}

void Scenario2::DrawBoundingRect()
{
    selectionCanvas->Children->Clear();

    if (boundingRect.Width <= 0 || boundingRect.Height <= 0)
    {
        return;
    }

    auto rectangle = ref new Rectangle();
    rectangle->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Blue);
    rectangle->StrokeThickness = 1;
    rectangle->StrokeDashArray = ref new DoubleCollection();
    rectangle->StrokeDashArray->Append(5.0);
    rectangle->StrokeDashArray->Append(2.0);
    rectangle->Width = boundingRect.Width;
    rectangle->Height = boundingRect.Height;

    selectionCanvas->SetLeft(rectangle, boundingRect.X);
    selectionCanvas->SetTop(rectangle, boundingRect.Y);
    selectionCanvas->Children->Append(rectangle);
}


void Scenario2::ToolButton_Lasso(Object^ sender, RoutedEventArgs^ e)
{
    // By default, pen barrel button or right mouse button is processed for inking
    // Set the configuration to instead allow processing these input on the UI thread
    inkCanvas->InkPresenter->InputProcessingConfiguration->RightDragAction = InkInputRightDragAction::LeaveUnprocessed;

    pointerPressedToken = inkCanvas->InkPresenter->UnprocessedInput->PointerPressed += ref new TypedEventHandler<InkUnprocessedInput^, PointerEventArgs^>(this, &Scenario2::UnprocessedInput_PointerPressed);
    pointerMovedToken = inkCanvas->InkPresenter->UnprocessedInput->PointerMoved += ref new TypedEventHandler<InkUnprocessedInput^, PointerEventArgs^>(this, &Scenario2::UnprocessedInput_PointerMoved);
    pointerReleasedToken = inkCanvas->InkPresenter->UnprocessedInput->PointerReleased += ref new TypedEventHandler<InkUnprocessedInput^, PointerEventArgs^>(this, &Scenario2::UnprocessedInput_PointerReleased);
}

void Scenario2::ClearDrawnBoundingRect()
{
    if (selectionCanvas->Children->Size > 0)
    {
        selectionCanvas->Children->Clear();
        boundingRect = Rect::Empty;
    }
}

void Scenario2::OnCopy(Object^ sender, RoutedEventArgs^ e)
{
    inkCanvas->InkPresenter->StrokeContainer->CopySelectedToClipboard();
}

void Scenario2::OnCut(Object^ sender, RoutedEventArgs^ e)
{
    inkCanvas->InkPresenter->StrokeContainer->CopySelectedToClipboard();
    inkCanvas->InkPresenter->StrokeContainer->DeleteSelected();
    ClearDrawnBoundingRect();
}

void Scenario2::OnPaste(Object^ sender, RoutedEventArgs^ e)
{
    if (inkCanvas->InkPresenter->StrokeContainer->CanPasteFromClipboard())
    {
        inkCanvas->InkPresenter->StrokeContainer->PasteFromClipboard(Point((float)(scrollViewer->HorizontalOffset + 10) / scrollViewer->ZoomFactor, (float)(scrollViewer->VerticalOffset + 10) / scrollViewer->ZoomFactor));
    }
    else
    {
        rootPage->NotifyUser("Cannot paste from clipboard.", NotifyType::ErrorMessage);
    }
}

void Scenario2::ClearSelection()
{
    auto strokes = inkCanvas->InkPresenter->StrokeContainer->GetStrokes();
    for (auto stroke : strokes)
    {
        stroke->Selected = false;
    }
    ClearDrawnBoundingRect();
}

void Scenario2::CurrentToolChanged(InkToolbar^ sender, Object^ args)
{
    bool enabled = sender->ActiveTool->Equals(toolButtonLasso);

    ButtonCut->IsEnabled = enabled;
    ButtonCopy->IsEnabled = enabled;
    ButtonPaste->IsEnabled = enabled;
}

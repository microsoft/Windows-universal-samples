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
#include "Scenario13.xaml.h"
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

Scenario13::Scenario13()
{
    InitializeComponent();
    LassoSelect = (Symbol)0xEF20;

    // Initialize the InkCanvas
    inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen;

    // Handlers to clear the selection when inking or erasing is detected
    inkCanvas->InkPresenter->StrokeInput->StrokeStarted += ref new TypedEventHandler<InkStrokeInput^, PointerEventArgs^>(this, &Scenario13::StrokeInput_StrokeStarted);
    inkCanvas->InkPresenter->StrokeInput->StrokeEnded += ref new TypedEventHandler<InkStrokeInput^, PointerEventArgs^>(this, &Scenario13::InkPresenter_StrokesErased);
}

void Scenario13::StrokeInput_StrokeStarted(InkStrokeInput^ sender, PointerEventArgs^ args)
{
    ClearSelection();
    inkCanvas->InkPresenter->UnprocessedInput->PointerPressed -= pointerPressedToken;
    inkCanvas->InkPresenter->UnprocessedInput->PointerMoved -= pointerMovedToken;
    inkCanvas->InkPresenter->UnprocessedInput->PointerReleased -= pointerReleasedToken;
}

void Scenario13::InkPresenter_StrokesErased(InkStrokeInput^ sender, PointerEventArgs^ args)
{
    ClearSelection();
    inkCanvas->InkPresenter->UnprocessedInput->PointerPressed -= pointerPressedToken;
    inkCanvas->InkPresenter->UnprocessedInput->PointerMoved -= pointerMovedToken;
    inkCanvas->InkPresenter->UnprocessedInput->PointerReleased -= pointerReleasedToken;
}

void Scenario13::OnSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
    HelperFunctions::UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
}

void Scenario13::UnprocessedInput_PointerPressed(InkUnprocessedInput^ sender, PointerEventArgs^ args)
{
    if (SelectedBoudningBoxContainsPosition(args->CurrentPoint->Position))
    {
        return;
    }

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

void Scenario13::UnprocessedInput_PointerMoved(InkUnprocessedInput^ sender, PointerEventArgs^ args)
{
    if (isBoundRect)
    {
        lasso->Points->Append(args->CurrentPoint->RawPosition);
    }
}

void Scenario13::UnprocessedInput_PointerReleased(InkUnprocessedInput^ sender, PointerEventArgs^ args)
{
    lasso->Points->Append(args->CurrentPoint->RawPosition);

    boundingRect = inkCanvas->InkPresenter->StrokeContainer->SelectWithPolyLine(lasso->Points);
    isBoundRect = false;
    DrawBoundingRect();
}

void Scenario13::SelectionRectangle_ManipulationDelta(Object^ sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs^ e)
{
    CompositeTransform^ transform = (CompositeTransform^) selectionRectangle->RenderTransform;

    switch (currentManipulationType)
    {
        case ManipulationTypes::Move:
            MoveSelectedInkStrokes(e->Delta.Translation);
            transform->TranslateX += e->Delta.Translation.X;
            transform->TranslateY += e->Delta.Translation.Y;

            break;

        case ManipulationTypes::Size:
            //TODO: Play with Scaling factor to change speed at which the "box" will grow.
            auto scale = std::abs(1 + (float)e->Delta.Translation.X / 100);

            //TODO: need to stop scaling when the area gets too small, otherwise app will crash. Could be refined.
            if (selectionRectangle->Width <= 10 && scale < 1.0)
                return;

            transform->ScaleX *= scale;
            transform->ScaleY *= scale;

            auto offset = selectionRectangle->ActualOffset;
            auto center = new Windows::Foundation::Numerics::float2(offset.x + (float)transform->TranslateX, offset.y + (float)transform->TranslateY);

            ResizeSelectedInkStrokes(scale, *center);
            break;
    }
}

void Scenario13::SelectionRectangle_ManipulationCompleted(Object^ sender, Windows::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs^ args)
{
    currentManipulationType = ManipulationTypes::None;

    Windows::UI::Core::CoreWindow^ window = Windows::UI::Core::CoreWindow::GetForCurrentThread();
    window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);    
}

void Scenario13::SelectionRectangle_ManipulationStarted(Object^ sender, Windows::UI::Xaml::Input::ManipulationStartedRoutedEventArgs^ args)
{
    if (currentManipulationType != ManipulationTypes::None)
        return;

    Windows::UI::Core::CoreWindow^ window = Windows::UI::Core::CoreWindow::GetForCurrentThread();

    if (window->PointerCursor->Type == CoreCursorType::SizeAll)
    {
        currentManipulationType = ManipulationTypes::Move;
    }
    else
    {
        currentManipulationType = ManipulationTypes::Size;

        switch (resizePointerCornor)
        {
        case RecCornor::TopLeft:
        case RecCornor::BottomRight:
            window->PointerCursor = ref new CoreCursor(CoreCursorType::SizeNorthwestSoutheast, 0);
            break;
        case RecCornor::TopRight:
        case RecCornor::BottomLeft:
            window->PointerCursor = ref new CoreCursor(CoreCursorType::SizeNortheastSouthwest, 0);
            break;
        }
    }
}

void Scenario13::SelectionRectangle_PointerPressed(Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args)
{
    if (currentManipulationType != ManipulationTypes::None)
        return;

    Windows::UI::Core::CoreWindow^ window = Windows::UI::Core::CoreWindow::GetForCurrentThread();

    if (window->PointerCursor->Type == CoreCursorType::SizeAll)
    {
        currentManipulationType = ManipulationTypes::Move;
    }
    else
    {
        currentManipulationType = ManipulationTypes::Size;

        switch (resizePointerCornor)
        {
        case RecCornor::TopLeft:
        case RecCornor::BottomRight:
            window->PointerCursor = ref new CoreCursor(CoreCursorType::SizeNorthwestSoutheast, 0);
            break;
        case RecCornor::TopRight:
        case RecCornor::BottomLeft:
            window->PointerCursor = ref new CoreCursor(CoreCursorType::SizeNortheastSouthwest, 0);
            break;
        }
    }
}

void Scenario13::SelectionRectangle_PointerMoved(Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args)
{
    if (currentManipulationType != ManipulationTypes::None)
        return;

    Windows::UI::Core::CoreWindow^ window = Windows::UI::Core::CoreWindow::GetForCurrentThread();
    Point mousePos = args->GetCurrentPoint(selectionRectangle)->Position;

    window->PointerCursor = ref new CoreCursor(CoreCursorType::SizeAll, 0);
    float offset = 5;
    float recMinX, recMaxX, recMinY, recMaxY;
    recMinX = 0;
    recMinY = 0;

    recMaxX = (float)selectionRectangle->ActualWidth;
    recMaxY = (float)selectionRectangle->ActualHeight;

    if (mousePos.X >= recMinX - offset && mousePos.X <= recMinX + offset &&
        mousePos.Y >= recMinY - offset && mousePos.Y <= recMinY + offset)
    {
        window->PointerCursor = ref new CoreCursor(CoreCursorType::SizeNorthwestSoutheast, 0);
        resizePointerCornor = RecCornor::TopLeft;
        return;
    }
    else if (mousePos.X >= recMaxX - offset && mousePos.X <= recMaxX + offset &&
        mousePos.Y >= recMaxY - offset && mousePos.Y <= recMaxY + offset)
    {
        window->PointerCursor = ref new CoreCursor(CoreCursorType::SizeNorthwestSoutheast, 0);
        resizePointerCornor = RecCornor::BottomRight;
        return;
    }
    else if (mousePos.X >= recMinX - offset && mousePos.X <= recMinX + offset &&
        mousePos.Y >= recMaxY - offset && mousePos.Y <= recMaxY + offset)
    {
        window->PointerCursor = ref new CoreCursor(CoreCursorType::SizeNortheastSouthwest, 0);
        resizePointerCornor = RecCornor::BottomLeft;
        return;
    }
    else if (mousePos.X >= recMaxX - offset && mousePos.X <= recMaxX + offset &&
        mousePos.Y >= recMinY - offset && mousePos.Y <= recMinY + offset)
    {
        window->PointerCursor = ref new CoreCursor(CoreCursorType::SizeNortheastSouthwest, 0);
        resizePointerCornor = RecCornor::TopRight;
        return;
    }
}

void Scenario13::SelectionRectangle_PointerExited(Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ args)
{
    if (currentManipulationType != ManipulationTypes::None)
        return;

    Windows::UI::Core::CoreWindow^ window = Windows::UI::Core::CoreWindow::GetForCurrentThread();
    window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);
}

void Scenario13::ResizeSelectedInkStrokes(float scale, Windows::Foundation::Numerics::float2& center)
{

    auto allStrokes = inkCanvas->InkPresenter->StrokeContainer->GetStrokes();

    if (allStrokes == nullptr)
        return;

    float3x2 matrixSacale = make_float3x2_scale(scale, center);

    for (auto stroke : allStrokes)
    {
        if (stroke->Selected)
        {
            stroke->PointTransform *= matrixSacale;
        }
    }
}

void Scenario13::MoveSelectedInkStrokes(Point position)
{
    float3x2 matrix = make_float3x2_translation((float)position.X, (float)position.Y);

        auto allStrokes = inkCanvas->InkPresenter->StrokeContainer->GetStrokes();
        if (allStrokes == nullptr)
            return;

        for (auto stroke : allStrokes)
        {
            if (stroke->Selected)
            {
                stroke->PointTransform *= matrix;
            }
        }

}

void Scenario13::DrawBoundingRect()
{
    selectionCanvas->Children->Clear();

    if (boundingRect.Width <= 0 || boundingRect.Height <= 0)
    {
        return;
    }

    selectionRectangle = ref new Rectangle();
    selectionRectangle->Name = "SelectedBox";
    selectionRectangle->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Blue);
    selectionRectangle->Fill = ref new SolidColorBrush(Windows::UI::Colors::Transparent);
    selectionRectangle->StrokeThickness = 1;
    selectionRectangle->StrokeDashArray = ref new DoubleCollection();
    selectionRectangle->ManipulationMode = Windows::UI::Xaml::Input::ManipulationModes::All;
    selectionRectangle->RenderTransform = ref new CompositeTransform();
    selectionRectangle->StrokeDashArray->Append(5.0);
    selectionRectangle->StrokeDashArray->Append(2.0);
    selectionRectangle->Width = boundingRect.Width;
    selectionRectangle->Height = boundingRect.Height;

    selectionCanvas->SetLeft(selectionRectangle, boundingRect.X);
    selectionCanvas->SetTop(selectionRectangle, boundingRect.Y);


    manipulationStartedToken = selectionRectangle->ManipulationStarted += ref new Windows::UI::Xaml::Input::ManipulationStartedEventHandler(this, &Scenario13::SelectionRectangle_ManipulationStarted);
    manipulationCompletedToken = selectionRectangle->ManipulationCompleted += ref new Windows::UI::Xaml::Input::ManipulationCompletedEventHandler(this, &Scenario13::SelectionRectangle_ManipulationCompleted);
    manipulationDeltaToken = selectionRectangle->ManipulationDelta += ref new Windows::UI::Xaml::Input::ManipulationDeltaEventHandler(this, &Scenario13::SelectionRectangle_ManipulationDelta);
    pointerExitedRectangleToken = selectionRectangle->PointerExited += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &Scenario13::SelectionRectangle_PointerExited);
    pointerMovedRectangleToken = selectionRectangle->PointerMoved += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &Scenario13::SelectionRectangle_PointerMoved);
    pointerPressedRectangleToken = selectionRectangle->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &Scenario13::SelectionRectangle_PointerPressed);

    selectionCanvas->Children->Append(selectionRectangle);
}

void Scenario13::ToolButton_Lasso(Object^ sender, RoutedEventArgs^ e)
{
    // By default, pen barrel button or right mouse button is processed for inking
    // Set the configuration to instead allow processing these input on the UI thread
    inkCanvas->InkPresenter->InputProcessingConfiguration->RightDragAction = InkInputRightDragAction::LeaveUnprocessed;

    pointerPressedToken = inkCanvas->InkPresenter->UnprocessedInput->PointerPressed += ref new TypedEventHandler<InkUnprocessedInput^, PointerEventArgs^>(this, &Scenario13::UnprocessedInput_PointerPressed);
    pointerMovedToken = inkCanvas->InkPresenter->UnprocessedInput->PointerMoved += ref new TypedEventHandler<InkUnprocessedInput^, PointerEventArgs^>(this, &Scenario13::UnprocessedInput_PointerMoved);
    pointerReleasedToken = inkCanvas->InkPresenter->UnprocessedInput->PointerReleased += ref new TypedEventHandler<InkUnprocessedInput^, PointerEventArgs^>(this, &Scenario13::UnprocessedInput_PointerReleased);
}

void Scenario13::ClearDrawnBoundingRect()
{
    if (selectionCanvas->Children->Size > 0)
    {
        selectionCanvas->Children->Clear();

        selectionRectangle->ManipulationStarted -= manipulationStartedToken;
        selectionRectangle->ManipulationCompleted -= manipulationCompletedToken;
        selectionRectangle->ManipulationDelta -= manipulationDeltaToken;
        selectionRectangle->PointerExited -= pointerExitedRectangleToken;
        selectionRectangle->PointerMoved -= pointerMovedRectangleToken;
        selectionRectangle->PointerPressed -= pointerPressedRectangleToken;

        selectionRectangle = nullptr;

        boundingRect = Rect::Empty;
    }
}

void Scenario13::ClearSelection()
{
    auto strokes = inkCanvas->InkPresenter->StrokeContainer->GetStrokes();
    for (auto stroke : strokes)
    {
        stroke->Selected = false;
    }
    ClearDrawnBoundingRect();
}

bool Scenario13::SelectedBoudningBoxContainsPosition(Windows::Foundation::Point position)
{
    auto contains = !boundingRect.IsEmpty && boundingRect.Contains(position);
    return contains;
}


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

#include "Scenario2.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2 sealed
    {
    public:
        Scenario2();

        property Windows::UI::Xaml::Controls::Symbol CalligraphyPen;
        property Windows::UI::Xaml::Controls::Symbol LassoSelect;
        property Windows::UI::Xaml::Controls::Symbol TouchWriting;

    private:
        void StrokeInput_StrokeStarted(Windows::UI::Input::Inking::InkStrokeInput^ sender, Windows::UI::Core::PointerEventArgs^ args);
        void InkPresenter_StrokesErased(Windows::UI::Input::Inking::InkStrokeInput^ sender, Windows::UI::Core::PointerEventArgs^ args);

        void OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
        void Toggle_Custom(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ToolButton_Lasso(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void UnprocessedInput_PointerPressed(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^  args);
        void UnprocessedInput_PointerMoved(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^  args);
        void UnprocessedInput_PointerReleased(Windows::UI::Input::Inking::InkUnprocessedInput^ sender, Windows::UI::Core::PointerEventArgs^ args);

        void DrawBoundingRect();
        void ClearDrawnBoundingRect();
        void OnCopy(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnCut(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnPaste(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ClearSelection();
        void CurrentToolChanged(Windows::UI::Xaml::Controls::InkToolbar^ sender, Platform::Object^ args);

        MainPage^ rootPage = MainPage::Current;
        Windows::UI::Xaml::Shapes::Polyline^ lasso;
        Windows::Foundation::Rect boundingRect;
        bool isBoundRect;
        Windows::Foundation::EventRegistrationToken pointerPressedToken;
        Windows::Foundation::EventRegistrationToken pointerMovedToken;
        Windows::Foundation::EventRegistrationToken pointerReleasedToken;
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class CalligraphicPen sealed : Windows::UI::Xaml::Controls::InkToolbarCustomPen
    {
    public:

    protected:
        Windows::UI::Input::Inking::InkDrawingAttributes^ CreateInkDrawingAttributesCore(Windows::UI::Xaml::Media::Brush^ brush, double strokeWidth) override;
    };
}

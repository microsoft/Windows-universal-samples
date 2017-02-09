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
#include "Scenario6.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input::Inking;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario6::Scenario6() : rootPage(MainPage::Current)
{
    InitializeComponent();

    // Initialize the InkCanvas controls
    inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen;
    inkCanvas2->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen;

    inkCanvas->InkPresenter->UnprocessedInput->PointerEntered += ref new TypedEventHandler<InkUnprocessedInput^, PointerEventArgs^>(this, &Scenario6::UnprocessedInput_PointerEntered);
    inkCanvas2->InkPresenter->UnprocessedInput->PointerEntered += ref new TypedEventHandler<InkUnprocessedInput^, PointerEventArgs^>(this, &Scenario6::UnprocessedInput_PointerEntered1);
}

void Scenario6::UnprocessedInput_PointerEntered(InkUnprocessedInput^ sender, PointerEventArgs^ args)
{
    border1->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::Blue);
    border2->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::Gray);
    inkToolbar->TargetInkCanvas = inkCanvas;
}

void Scenario6::UnprocessedInput_PointerEntered1(InkUnprocessedInput^ sender, PointerEventArgs^ args)
{
    border1->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::Gray);
    border2->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::Blue);
    inkToolbar->TargetInkCanvas = inkCanvas2;
}

void Scenario6::OnSizeChanged(Platform::Object^ sender, SizeChangedEventArgs^ e)
{
    SetCanvasSize();
}


void Scenario6::SetCanvasSize()
{
    outputGrid->Width = RootGrid->ActualWidth / 2;
    outputGrid->Height = RootGrid->ActualHeight / 2;
    inkCanvas->Width = RootGrid->ActualWidth / 2;
    inkCanvas->Height = RootGrid->ActualHeight / 2;
    outputGrid2->Width = RootGrid->ActualWidth / 2;
    outputGrid2->Height = RootGrid->ActualHeight / 2;
    inkCanvas2->Width = RootGrid->ActualWidth / 2;
    inkCanvas2->Height = RootGrid->ActualHeight / 2;
}

void Scenario6::ToggleLogs(Platform::Object^ sender, RoutedEventArgs^ e)
{
    auto stencilButton = safe_cast<InkToolbarStencilButton^>(inkToolbar->GetMenuButton(InkToolbarMenuKind::Stencil));

    if (toggleLog->IsOn)
    {
        LogBorder->Visibility = Windows::UI::Xaml::Visibility::Visible;
        activeToolChangedToken = inkToolbar->ActiveToolChanged += ref new TypedEventHandler<InkToolbar^, Platform::Object^>(this, &Scenario6::InkToolbar_ActiveToolChanged);
        inkDrawingAttributesChangedToken = inkToolbar->InkDrawingAttributesChanged += ref new TypedEventHandler<InkToolbar^, Platform::Object^>(this, &Scenario6::InkToolbar_InkDrawingAttributesChanged);
        eraseAllClickedToken = inkToolbar->EraseAllClicked += ref new TypedEventHandler<InkToolbar^, Platform::Object^>(this, &Scenario6::InkToolbar_EraseAllClicked);
        stencilButtonCheckedChangedToken = inkToolbar->IsStencilButtonCheckedChanged += ref new TypedEventHandler<InkToolbar^, InkToolbarIsStencilButtonCheckedChangedEventArgs^>(this, &Scenario6::InkToolbar_IsStencilButtonCheckedChanged);
        selectedStencilChangedToken = stencilButton->RegisterPropertyChangedCallback(InkToolbarStencilButton::SelectedStencilProperty, ref new DependencyPropertyChangedCallback(this, &Scenario6::InkToolbar_SelectedStencilChanged));
    }
    else
    {
        LogBorder->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        textLogs->Text = "";
        inkToolbar->ActiveToolChanged -= activeToolChangedToken;
        inkToolbar->InkDrawingAttributesChanged -= inkDrawingAttributesChangedToken;
        inkToolbar->EraseAllClicked -= eraseAllClickedToken;
        inkToolbar->IsStencilButtonCheckedChanged -= stencilButtonCheckedChangedToken;
        stencilButton->UnregisterPropertyChangedCallback(InkToolbarStencilButton::SelectedStencilProperty, selectedStencilChangedToken);
    }
}

void Scenario6::InkToolbar_IsStencilButtonCheckedChanged(InkToolbar^ sender, InkToolbarIsStencilButtonCheckedChangedEventArgs^ args)
{
    textLogs->Text = "IsStencilButtonCheckedChanged(Kind = " + args->StencilKind.ToString() +
        ", IsChecked = " + args->StencilButton->IsChecked->Value.ToString() + ")\n" + textLogs->Text;
}

void Scenario6::InkToolbar_SelectedStencilChanged(DependencyObject^ sender, DependencyProperty^ dp)
{
    auto stencilButton = safe_cast<InkToolbarStencilButton^>(sender);
    textLogs->Text = "SelectedStencil changed to " + stencilButton->SelectedStencil.ToString() + "\n" + textLogs->Text;
}

void Scenario6::InkToolbar_EraseAllClicked(InkToolbar^ sender, Platform::Object^ args)
{
    textLogs->Text = "EraseAllClicked\n" + textLogs->Text;
}

void Scenario6::InkToolbar_InkDrawingAttributesChanged(InkToolbar^ sender, Platform::Object^ args)
{
    textLogs->Text = "InkDrawingAttributesChanged\n" + textLogs->Text;
}

void Scenario6::InkToolbar_ActiveToolChanged(InkToolbar^ sender, Platform::Object^ args)
{
    textLogs->Text = "ActiveToolChanged\n" + textLogs->Text;
}

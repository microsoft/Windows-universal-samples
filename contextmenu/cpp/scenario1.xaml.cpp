//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario1.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include "Scenario1.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario1::Scenario1()
{
    InitializeComponent();
    rootPage = MainPage::Current;
}

Rect Scenario1::GetElementRect(FrameworkElement^ element)
{
    GeneralTransform^ buttonTransform = element->TransformToVisual(nullptr);
    const Point pointOrig(0, 0);
    const Point pointTransformed = buttonTransform->TransformPoint(pointOrig);
    const Rect rect(pointTransformed.X, pointTransformed.Y, safe_cast<float>(element->ActualWidth), safe_cast<float>(element->ActualHeight));
    return rect;
}

void Scenario1::AttachmentImage_RightTapped(Object^ sender, RightTappedRoutedEventArgs^ e)
{
    // Create a menu and add commands specifying a callback delegate for each.
    // Since command delegates are unique, no need to specify command Ids.
    auto menu = ref new PopupMenu();
    menu->Commands->Append(ref new UICommand("Open with", ref new UICommandInvokedHandler([this](IUICommand^ command)
    {
        OutputTextBlock->Text = "'" + command->Label + "' selected";
    })));

    menu->Commands->Append(ref new UICommand("Save attachment", ref new UICommandInvokedHandler([this](IUICommand^ command)
    {
        OutputTextBlock->Text = "'" + command->Label + "' selected";
    })));

    // We don't want to obscure content, so pass in a rectangle representing the sender of the context menu event.
    auto rect = GetElementRect(safe_cast<FrameworkElement^>(sender));
    create_task(menu->ShowForSelectionAsync(rect)).then([this](IUICommand^ command)
    {
        if (command == nullptr)
        {
            // The command is null if no command was invoked.
            OutputTextBlock->Text = L"Context menu dismissed";
        }
    });
    OutputTextBlock->Text = "Context menu shown";
}

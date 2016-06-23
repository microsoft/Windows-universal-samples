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
// Scenario2.xaml.cpp
// Implementation of the Scenario2 class
//

#include "pch.h"
#include "Scenario2.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Foundation;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario2::Scenario2()
{
    InitializeComponent();
    rootPage = MainPage::Current;
}

// returns a rect for selected text in a single line.
// if no text is selected, returns caret location
// textbox should not be empty
Rect Scenario2::GetTextboxSelectionRect(TextBox^ textbox)
{
    Rect rectFirst, rectLast;
    if (textbox->SelectionStart == textbox->Text->Length())
    {
        rectFirst = textbox->GetRectFromCharacterIndex(textbox->SelectionStart - 1, true);
    }
    else
    {
        rectFirst = textbox->GetRectFromCharacterIndex(textbox->SelectionStart, false);
    }

    const int lastIndex = textbox->SelectionStart + textbox->SelectionLength;
    if (lastIndex == textbox->Text->Length())
    {
        rectLast = textbox->GetRectFromCharacterIndex(lastIndex - 1, true);
    }
    else
    {
        rectLast = textbox->GetRectFromCharacterIndex(lastIndex, false);
    }

    Rect selectionRect = RectHelper::Union(rectFirst, rectLast);

    GeneralTransform^ transform = textbox->TransformToVisual(nullptr);
    return transform->TransformBounds(selectionRect);
}

void Scenario2::ReadOnlyTextBox_ContextMenuOpening(Object^ sender, ContextMenuEventArgs^ e)
{
    e->Handled = true;
    TextBox^ textbox = safe_cast<TextBox^>(sender);

    if (textbox->SelectionLength > 0)
    {
        // Create a menu and add commands specifying an id value for each instead of a delegate.
        auto menu = ref new PopupMenu();

        menu->Commands->Append(ref new UICommand("Copy",      nullptr, PropertyValue::CreateInt32(1)));
        menu->Commands->Append(ref new UICommandSeparator());
        menu->Commands->Append(ref new UICommand("Highlight", nullptr, PropertyValue::CreateInt32(2)));
        menu->Commands->Append(ref new UICommand("Look up",   nullptr, PropertyValue::CreateInt32(3)));

        // We don't want to obscure content, so pass in a rectangle representing the selection area.
        // NOTE: this code only handles textboxes with a single line. If a textbox has multiple lines,
        //       then the context menu should be placed at cursor/pointer location by convention.
        Rect rectContent = GetTextboxSelectionRect(textbox);
        String^ selectedText = textbox->SelectedText;
        create_task(menu->ShowForSelectionAsync(rectContent)).then([this, selectedText](IUICommand^ command)
        {
            if (command != nullptr)
            {
                int const id = safe_cast<IBox<int>^>(command->Id)->Value;
                switch (id)
                {
                case 1:
                    {
                        auto dataPackage = ref new DataPackage();
                        dataPackage->SetText(selectedText);
                        Clipboard::SetContent(dataPackage);
                        OutputTextBlock->Text = "'" + command->Label + "'(" + id.ToString() + ")" + " selected; '" + selectedText + "' copied to clipboard";
                    }
                    break;

                case 2:
                    OutputTextBlock->Text = "'" + command->Label + "'(" + id.ToString() + ")" + " selected";
                    break;

                case 3:
                    OutputTextBlock->Text = "'" + command->Label + "'(" + id.ToString() + ")" + " selected";
                    break;
                }
            }
            else
            {
                OutputTextBlock->Text = "Context menu dismissed";
            }
        });
        OutputTextBlock->Text = "Context menu shown";
    }
    else
    {
        OutputTextBlock->Text = "Context menu not shown because there is no text selected";
    }
}

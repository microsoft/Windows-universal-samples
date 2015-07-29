// Copyright (c) Microsoft. All rights reserved.


#include "pch.h"
#include "ContextMenuFlyout.xaml.h"

using namespace SDKTemplate;
using namespace Windows::UI::Xaml::Controls;

ContextMenuFlyout::ContextMenuFlyout(std::shared_ptr<SceneComposer> sceneComposer)
{
    InitializeComponent();
    _sceneComposer = sceneComposer;
}

void ContextMenuFlyout::EnablePasteItemOnly(bool IsEnabled, Windows::Foundation::Point pos)
{
    // This method enables all items (cut/copy) when paste is disabled and then disables
    // them when paste is enabled. This is to provide a context menu that has certain functionality
    // enabled when a stylus is inside the selection rectangle and paste only when doing a gesture
    // outside of it.
    _pastePosition = pos;
    for (unsigned int index = 0; index < this->Items->Size; index++)
    {
        this->Items->GetAt(index)->IsEnabled = (PASTE_ITEM_INDEX == index) ? IsEnabled : !IsEnabled;
    }
}

void SDKTemplate::ContextMenuFlyout::CopyFlyoutItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_sceneComposer != nullptr)
    {
        _sceneComposer->CopySelected();
    }
}

void SDKTemplate::ContextMenuFlyout::CutFlyoutItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_sceneComposer != nullptr)
    {
        _sceneComposer->CutSelected();
    }
}

void SDKTemplate::ContextMenuFlyout::PasteFlyoutItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (_sceneComposer != nullptr)
    {
        _sceneComposer->PasteSelected(_pastePosition);
    }
}

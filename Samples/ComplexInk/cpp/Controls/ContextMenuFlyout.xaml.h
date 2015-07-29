// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Controls\ContextMenuFlyout.g.h"
#include "..\Renderers\SceneComposer.h"

#define PASTE_ITEM_INDEX    4 // Paste is the last item in the flyout counting the separators

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ContextMenuFlyout sealed
    {
    internal :
        ContextMenuFlyout(std::shared_ptr<SceneComposer> sceneComposer);
        void EnablePasteItemOnly(bool IsEnabled, Windows::Foundation::Point pos);

    private:
        void CopyFlyoutItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void CutFlyoutItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PasteFlyoutItem_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        std::shared_ptr<SceneComposer>  _sceneComposer;
        Windows::Foundation::Point      _pastePosition;
    };
}

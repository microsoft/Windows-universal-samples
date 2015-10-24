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

#include "Scenario3_StartDragAsync.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_StartDragAsync sealed
    {
    public:
        Scenario3_StartDragAsync();

    private:
        MainPage^ rootPage;

        std::mt19937 _rnd;
        // Collection of symbols to display and drag
        Platform::Collections::Vector<Platform::String^>^ _symbols;
        // Is a round running
        bool _isActive;
        // Keeping the Drag operation will allow to cancel it after it has started
        Windows::Foundation::IAsyncOperation<Windows::ApplicationModel::DataTransfer::DataPackageOperation>^ _dragOperation;
        // Symbol to drag
        Platform::String^ _symbol;
        // Timer limiting the time allowed to drag the symbol to the right target
        Windows::UI::Xaml::DispatcherTimer^ _timer;

        void DragCompleted(Windows::Foundation::IAsyncOperation<Windows::ApplicationModel::DataTransfer::DataPackageOperation>^ asyncInfo,
                           Windows::Foundation::AsyncStatus asyncStatus);
        void SourceTextBlock_PointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
        void SourceTextBlock_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args);

        void DropBorder_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
        void DropBorder_Drop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);

        bool IsTargetBorder(Platform::Object^ sender);
        void StartButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnTick(Platform::Object ^sender, Platform::Object ^args);

        void EndRound(bool hasWon);
    };
}

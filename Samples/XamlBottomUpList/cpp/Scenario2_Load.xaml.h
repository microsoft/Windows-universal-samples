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

#include "Scenario2_Load.g.h"

namespace SDKTemplate
{
    // Bindable class representing a single text message.
    public ref class TextMessage sealed
    {
    public:
        property Platform::String^ Body
        {
            Platform::String^ get() { return _message; }
            void set(Platform::String^ value) { _message = value; }
        }

        property Platform::String^ DisplayTime
        {
            Platform::String^ get() { return _displayTime; }
            void set(Platform::String^ value) { _displayTime = value; }
        }

        property bool IsSent
        {
            bool get() { return _isSent; }
            void set(bool value) { _isSent = value; }
        }

        property bool IsReceived
        {
            bool get() { return !_isSent; }
        }

    private:
        Platform::String^ _message;
        Platform::String^ _displayTime;
        bool _isSent;
    };

    /// <summary>
    /// This ListView is tailored to a Chat experience where the focus is on the last item in the list
    /// and as the user scrolls up the older messages are incrementally loaded.  We're performing our
    /// own logic to trigger loading more data.
    /// //
    /// Note: This is just delay loading the data, but isn't true data virtualization.  A user that
    /// scrolls all the way to the beginning of the list will cause all the data to be loaded.
    /// </summary>
    public ref class ChatListView sealed : public Windows::UI::Xaml::Controls::ListView
    {
    public:
        ChatListView();

    protected:
        void OnApplyTemplate() override;
        Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size finalSize) override;

    private:
        typedef Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::ListViewBase^, Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs^> CCCEventHandler;

        unsigned int itemsSeen;
        double averageContainerHeight = 0.0;
        bool processingScrollOffsets = false;
        bool processingScrollOffsetsDeferred = false;
        CCCEventHandler^ UpdateRunningAverageContainerHeightHandler = ref new CCCEventHandler(this, &ChatListView::UpdateRunningAverageContainerHeight);

        void StartProcessingDataVirtualizationScrollOffsets(double actualHeight);
        Concurrency::task<void> ProcessDataVirtualizationScrollOffsetsAsync(double actualHeight);
        void UpdateRunningAverageContainerHeight(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs^ args);
        void ScrollViewer_ViewChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs^ args);
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Load sealed
    {
    public:
        Scenario2_Load();
    protected:
        void SendTextMessage();
        void MessageTextBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
    private:
        Windows::UI::Xaml::Interop::IBindableObservableVector^ conversation;

        void OnChatViewContainerContentChanging(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs^ args);
    };
}

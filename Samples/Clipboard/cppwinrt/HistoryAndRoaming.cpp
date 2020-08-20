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
#include "HistoryAndRoaming.h"
#include "HistoryAndRoaming.g.cpp"

using namespace winrt;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    HistoryAndRoaming::HistoryAndRoaming()
    {
        InitializeComponent();
    }

    void HistoryAndRoaming::OnNavigatedTo(NavigationEventArgs const&)
    {
        historyEnabledToken = Clipboard::HistoryEnabledChanged({ get_weak(), &HistoryAndRoaming::OnHistoryEnabledChanged });
        roamingEnabledToken = Clipboard::RoamingEnabledChanged({ get_weak(), &HistoryAndRoaming::OnRoamingEnabledChanged });
        CheckHistoryAndRoaming();
    }

    void HistoryAndRoaming::OnNavigatedFrom(NavigationEventArgs const&)
    {
         Clipboard::HistoryEnabledChanged(historyEnabledToken);
         Clipboard::RoamingEnabledChanged(roamingEnabledToken);
    }

    fire_and_forget HistoryAndRoaming::CopyButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Generate a bitmap for our clipboard content.
        RenderTargetBitmap target;
        co_await target.RenderAsync(ClipboardContent());
        IBuffer buffer = co_await target.GetPixelsAsync();
        SoftwareBitmap bitmap = SoftwareBitmap::CreateCopyFromBuffer(buffer, BitmapPixelFormat::Bgra8, target.PixelWidth(), target.PixelHeight(), BitmapAlphaMode::Premultiplied);

        // Save the bitmap to a stream.
        InMemoryRandomAccessStream stream;
        BitmapEncoder encoder = co_await BitmapEncoder::CreateAsync(BitmapEncoder::PngEncoderId(), stream);
        encoder.SetSoftwareBitmap(bitmap);
        co_await encoder.FlushAsync();
        stream.Seek(0);

        // Create a DataPackage to hold the bitmap and text.
        DataPackage dataPackage;
        dataPackage.SetBitmap(RandomAccessStreamReference::CreateFromStream(stream));
        dataPackage.SetText(ClipboardContent().Text());

        ClipboardContentOptions options;

        // Customize clipboard history.
        if (IncludeAllInHistory().IsChecked().Value())
        {
            // This is the default, but set it explicitly for expository purposes.
            options.IsAllowedInHistory(true);
        }
        else if (IncludeTextInHistory().IsChecked().Value())
        {
            // This is the default, but set it explicitly for expository purposes.
            options.IsAllowedInHistory(true);
            // Allow only text to go into the history.
            options.HistoryFormats().Append(StandardDataFormats::Text());
        }
        else
        {
            // Exclude from history.
            options.IsAllowedInHistory(false);
        }

        // Customize clipboard roaming.
        if (AllowAllToRoam().IsChecked().Value())
        {
            // This is the default, but set it explicitly for expository purposes.
            options.IsRoamable(true);
        }
        else if (AllowTextToRoam().IsChecked().Value())
        {
            // This is the default, but set it explicitly for expository purposes.
            options.IsRoamable(true);
            // Allow only text to roam.
            options.RoamingFormats().Append(StandardDataFormats::Text());
        }
        else
        {
            // Exclude from roaming.
            options.IsRoamable(false);
        }

        // Set the data package to the clipboard with our custom options.
        if (Clipboard::SetContentWithOptions(dataPackage, options))
        {
            rootPage.NotifyUser(L"Text and bitmap have been copied to clipboard.", NotifyType::StatusMessage);
        }
        else
        {
            // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
            rootPage.NotifyUser(L"Error copying content to clipboard.", NotifyType::ErrorMessage);
        }
    }

    void HistoryAndRoaming::OnHistoryEnabledChanged(IInspectable const&, IInspectable const&)
    {
        CheckHistoryAndRoaming();
    }

    void HistoryAndRoaming::OnRoamingEnabledChanged(IInspectable const&, IInspectable const&)
    {
        CheckHistoryAndRoaming();
    }

    void HistoryAndRoaming::SelectVisibleElement(UIElement const& ifTrue, UIElement const& ifFalse, bool value)
    {
        ifTrue.Visibility(value ? Visibility::Visible : Visibility::Collapsed);
        ifFalse.Visibility(!value ? Visibility::Visible : Visibility::Collapsed);
    }

    fire_and_forget HistoryAndRoaming::CheckHistoryAndRoaming()
    {
        auto lifetime = get_strong();
        co_await Dispatcher();
        SelectVisibleElement(ClipboardHistoryBlock(), ClipboardHistoryUnavailableBlock(), Clipboard::IsHistoryEnabled());
        SelectVisibleElement(ClipboardRoamingBlock(), ClipboardRoamingUnavailableBlock(), Clipboard::IsRoamingEnabled());
    }
}


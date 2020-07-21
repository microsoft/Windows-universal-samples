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

#include "CopyImage.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct CopyImage : CopyImageT<CopyImage>
    {
        CopyImage();

        void CopyButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void CopyWithDelayedRenderingButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget PasteButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        fire_and_forget CopyBitmap(bool isDelayRendered);
        static fire_and_forget OnDeferredImageRequestedHandler(Windows::ApplicationModel::DataTransfer::DataProviderRequest request, Windows::Storage::StorageFile imageFile);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct CopyImage : CopyImageT<CopyImage, implementation::CopyImage>
    {
    };
}

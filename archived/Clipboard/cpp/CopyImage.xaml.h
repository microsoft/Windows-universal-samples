//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// CopyImage.xaml.h
// Declaration of the CopyImage class
//

#pragma once
#include "CopyImage.g.h"
#include "MainPage.g.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class CopyImage sealed
    {
    public:
        CopyImage();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;

        void CopyButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PasteButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void CopyWithDelayedRenderingButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void CopyBitmap(bool useDelayedRendering);
        void OnDeferredImageRequestedHandler(Windows::ApplicationModel::DataTransfer::DataProviderRequest^ request);

        Windows::Storage::Streams::RandomAccessStreamReference^ imgStreamRef;
    };
}

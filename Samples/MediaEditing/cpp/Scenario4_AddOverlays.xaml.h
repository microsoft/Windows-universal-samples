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

#include "Scenario4_AddOverlays.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4_AddOverlays sealed
    {
    public:
        Scenario4_AddOverlays();

    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        Windows::Media::Editing::MediaComposition^ composition;
        Windows::Storage::StorageFile^ baseVideoFile;
        Windows::Storage::StorageFile^ overlayVideoFile;
        Windows::Media::Core::MediaStreamSource^ mediaStreamSource;

        void ChooseVideo_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void AddAudio_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void ChooseBaseVideo_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ChooseOverlayVideo_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void CreateOverlays();
    };
}

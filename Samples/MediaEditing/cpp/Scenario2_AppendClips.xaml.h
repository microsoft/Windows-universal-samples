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

#include "Scenario2_AppendClips.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_AppendClips sealed
    {
    public:
        Scenario2_AppendClips();

    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        Windows::Media::Editing::MediaComposition^ composition;
        Windows::Storage::StorageFile^ firstVideoFile;
        Windows::Storage::StorageFile^ secondVideoFile;
        Windows::Media::Core::MediaStreamSource^ mediaStreamSource;

        void ChooseFirstVideo_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ChooseSecondVideo_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void AppendVideos_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

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

#include "Scenario1_TrimAndSaveClip.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_TrimAndSaveClip sealed
    {
    public:
        Scenario1_TrimAndSaveClip();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        Windows::Media::Core::MediaStreamSource^ mediaStreamSource;
        Windows::Storage::AccessCache::StorageItemAccessList^ storageItemAccessList;
        Windows::Media::Editing::MediaComposition^ composition;
        Windows::Storage::StorageFile^ pickedFile;
        
        void ChooseFile_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void TrimClip_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Save_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void EnableButtons(bool isEnabled);
    };
}

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

#include "Scenario5_DefiningCustomGlyphs.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class Scenario5_DefiningCustomGlyphs sealed
    {
    public:
        Scenario5_DefiningCustomGlyphs();

        // Public for binding purposes.
        static bool IsNonNull(Platform::Object^ item)
        {
            return item != nullptr;
        }

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::Devices::PointOfService::ClaimedLineDisplay^ lineDisplay;
        Windows::Storage::Streams::IBuffer^ glyphBuffer;

        Concurrency::task<void> InitializeAsync();

        Windows::Storage::Streams::IBuffer^ CreateSolidGlyphBuffer(int widthInPixels, int heightInPixels);

        void DefineGlyphButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ResetButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

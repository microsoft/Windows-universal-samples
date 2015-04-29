//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Others.xaml.h
// Declaration of the OtherScenarios class
//

#pragma once
#include "Others.g.h"
#include "MainPage.g.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class OtherScenarios sealed
    {
    public:
        OtherScenarios();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void ShowFormatButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void EmptyClipboardButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ClearOutputButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void RegisterClipboardContentChanged_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void DisplayFormats();
        void ClearOutput();

        static bool registerContentChanged;
        MainPage^ rootPage;
    };
}

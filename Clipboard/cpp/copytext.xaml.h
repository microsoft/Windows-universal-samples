//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// CopyText.xaml.h
// Declaration of the CopyText class
//

#pragma once
#include "CopyText.g.h"
#include "MainPage.g.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class CopyText sealed
    {
    public:
        CopyText();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;

        void CopyButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PasteButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        Platform::String^ htmlDescription;
        Platform::String^ textDescription;
        Platform::String^ imgSrc;
    };
}

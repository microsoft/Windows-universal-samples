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

//
// Scenario4.xaml.h
// Declaration of the Scenario4 class
//

#pragma once

#include "pch.h"
#include "Scenario4.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4 sealed
    {
    public:
        Scenario4();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;

        void GetThumbnailButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

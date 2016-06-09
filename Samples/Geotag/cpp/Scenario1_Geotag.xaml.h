// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_Geotag.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Geotag sealed
    {
    public:
        Scenario1_Geotag();
    protected:
        void ChooseFileButton_Click();
        void GetGeotagButton_Click();
        void SetGeotagFromGeolocatorButton_Click();
        void SetGeotagButton_Click();
    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::Storage::StorageFile^ file = nullptr;
    };
}

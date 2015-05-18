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
// SharePage.h
// Declaration of the SharePage class
//

#pragma once

#include "pch.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SharePage : Windows::UI::Xaml::Controls::Page
    {
    public:
        static Windows::Foundation::Uri^ GetApplicationLink(Platform::String^ sharePageFullName);

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void ShowUIButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        virtual bool GetShareContent(Windows::ApplicationModel::DataTransfer::DataRequest^ request);

    private:
        Windows::Foundation::EventRegistrationToken dataRequestedToken;

        void OnDataRequested(Windows::ApplicationModel::DataTransfer::DataTransferManager^ sender, Windows::ApplicationModel::DataTransfer::DataRequestedEventArgs^ e);
    };
}

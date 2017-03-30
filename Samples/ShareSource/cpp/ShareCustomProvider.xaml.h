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
// ShareCustomProvider.xaml.h
// Declaration of the ShareCustomProvider class
//

#pragma once

#include "pch.h"
#include "SharePage.h"
#include "ShareCustomProvider.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    public ref class ShareCustomProvider sealed
    {
    public:
        ShareCustomProvider();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual bool GetShareContent(Windows::ApplicationModel::DataTransfer::DataRequest^ request) override;

    private:
        Windows::Foundation::EventRegistrationToken shareProvidersRequestedToken{};

        void OnShareProvidersRequested(Windows::ApplicationModel::DataTransfer::DataTransferManager^ sender, Windows::ApplicationModel::DataTransfer::ShareProvidersRequestedEventArgs^ e);
        void OnShareToContosoChat(Windows::ApplicationModel::DataTransfer::ShareProviderOperation^ operation);
    };
}

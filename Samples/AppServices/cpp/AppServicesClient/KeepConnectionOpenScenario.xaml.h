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

#include "KeepConnectionOpenScenario.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class KeepConnectionOpenScenario sealed
    {
    public:
        KeepConnectionOpenScenario();

    private:
        MainPage^ rootPage = MainPage::Current;
        Windows::ApplicationModel::AppService::AppServiceConnection^ _connection;

        void OpenConnection_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Connection_ServiceClosed(Windows::ApplicationModel::AppService::AppServiceConnection^ sender, Windows::ApplicationModel::AppService::AppServiceClosedEventArgs^ args);
        void CloseConnection_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void GenerateRandomNumber_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

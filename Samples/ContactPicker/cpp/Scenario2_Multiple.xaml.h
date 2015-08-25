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

#include "Scenario2_Multiple.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Multiple sealed
    {
    public:
        Scenario2_Multiple();
        void PickContactsEmail();
        void PickContactsPhone();

    private:
        MainPage^ rootPage = MainPage::Current;

        void ClearOutputs();
        void ReportContactResults(Windows::Foundation::Collections::IVector<Windows::ApplicationModel::Contacts::Contact^>^ contacts);
    };
}

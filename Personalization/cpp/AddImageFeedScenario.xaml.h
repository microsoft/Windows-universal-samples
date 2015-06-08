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
// AddImageFeedScenario.xaml.h
// Declaration of the AddImageFeedScenario class
//

#pragma once
#include "AddImageFeedScenario.g.h"

namespace SDKTemplate
{
    public ref class AddImageFeedScenario sealed
    {
    public:
        AddImageFeedScenario();

    private:
        void SetDefaultButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void RemoveFeedButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

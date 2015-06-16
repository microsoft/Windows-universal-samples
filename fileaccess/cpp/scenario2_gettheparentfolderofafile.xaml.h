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

#include "pch.h"
#include "Scenario2_GetTheParentFolderOfAFile.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// Getting a file's parent folder.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2 sealed
    {
    public:
        Scenario2();

    private:
        MainPage^ rootPage;

        void GetParentButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

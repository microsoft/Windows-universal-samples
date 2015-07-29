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
// S5_AddWithAppContent.xaml.cpp
// Implementation of the S5_AddWithAppContent class
//

#include "pch.h"
#include "S5_AddWithAppContent.xaml.h"
#include "MainPage.xaml.h"
#include "Helpers.h"

using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

S5_AddWithAppContent::S5_AddWithAppContent()
{
    InitializeComponent();
    InitializeRevisionNumber();
}

void S5_AddWithAppContent::AddToIndex_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    AddAppContentFilesToIndexedFolder();
}

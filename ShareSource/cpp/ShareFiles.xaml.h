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
// ShareFiles.xaml.h
// Declaration of the ShareFiles class
//

#pragma once

#include "pch.h"
#include "SharePage.h"
#include "ShareFiles.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    public ref class ShareFiles sealed
    {
    public:
        ShareFiles();

    protected:
        virtual bool GetShareContent(Windows::ApplicationModel::DataTransfer::DataRequest^ request) override;

    private:
        Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile^>^ storageItems;

        void SelectFilesButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

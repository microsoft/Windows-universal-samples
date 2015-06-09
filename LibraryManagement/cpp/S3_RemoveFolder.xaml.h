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
// Scenario3.xaml.h
// Declaration of the Scenario3 class
//

#pragma once
#include "S3_RemoveFolder.g.h"

namespace SDKTemplate
{
    public ref class Scenario3 sealed
    {
    public:
        Scenario3();

    private:
        void RemoveFolderButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void GetPicturesLibrary();
        void FillComboBox();
        void UpdateControls();

        // The dispatcher for this page.  Because it is obtained from Window.Current, which is thread affine,
        // the dispatcher must be obtained from the UI thread this page will run.  It can then be referenced
        // from a background thread to run code that will update the UI.
        Windows::UI::Core::CoreDispatcher^ dispatcher;
        Windows::Storage::StorageLibrary^ picturesLibrary;
    };
}

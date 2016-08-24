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

#include "pch.h"
#include "Scenario6_AppListingURI.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Services::Store;
using namespace Windows::UI::Xaml;

Scenario6_AppListingURI::Scenario6_AppListingURI()
{
    InitializeComponent();

    storeContext = StoreContext::GetDefault();
}

void Scenario6_AppListingURI::DisplayLink(Object^ sender, RoutedEventArgs^ e)
{
    create_task(storeContext->GetStoreProductForCurrentAppAsync()).then([this](StoreProductResult^ result)
    {
        if (result->ExtendedError.Value != S_OK)
        {
            Utils::ReportExtendedError(result->ExtendedError);
            return;
        }

        Windows::System::Launcher::LaunchUriAsync(result->Product->LinkUri);
    });
}

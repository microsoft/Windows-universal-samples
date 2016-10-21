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
#include "Scenario5_UserCollection.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Services::Store;
using namespace Windows::UI::Xaml;

Scenario5_UserCollection::Scenario5_UserCollection()
{
    InitializeComponent();

    storeContext = StoreContext::GetDefault();
}

void Scenario5_UserCollection::GetUserCollectionButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    // Create a filtered list of the product AddOns I care about
    auto filterList = ref new Vector<String^>({ "Consumable", "Durable", "UnmanagedConsumable" });

    // Get user collection for this product, filtering for the types we know about
    create_task(storeContext->GetUserCollectionAsync(filterList)).then([this](StoreProductQueryResult^ collection)
    {
        ProductsListView->ItemsSource = Utils::CreateProductListFromQueryResult(collection, "collection items");
    }, task_continuation_context::get_current_winrt_context());
}

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
// ShareHtml.xaml.cpp
// Implementation of the ShareHtml class
//

#include "pch.h"
#include "ShareHtml.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

ShareHtml::ShareHtml()
{
    InitializeComponent();
    ShareWebView->Navigate(ref new Uri("http://msdn.microsoft.com"));
}

void ShareHtml::ShareWebView_NavigationCompleted(Object^ sender, WebViewNavigationCompletedEventArgs^ e)
{
    ShareWebView->Visibility = Windows::UI::Xaml::Visibility::Visible;
    BlockingRect->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    LoadingProgressRing->IsActive = false;
}

bool ShareHtml::GetShareContent(DataRequest^ request)
{
    // Get the user's selection from the WebView. Since this is an asynchronous operation we need to acquire the deferral first.
    auto deferral = request->GetDeferral();
    create_task(ShareWebView->CaptureSelectedContentToDataPackageAsync()).then([this, request, deferral](task<DataPackage^> requestDataTask)
    {
        try
        {
            auto requestData = requestDataTask.get();
            if ((requestData != nullptr) && (requestData->GetView()->AvailableFormats->Size > 0))
            {
                requestData->Properties->Title = "A web snippet for you";
                requestData->Properties->Description = "HTML selection from a WebView control"; // The description is optional.
                requestData->Properties->ContentSourceWebLink = ShareWebView->Source;
                request->Data = requestData;
                deferral->Complete();
            }
            else
            {
                // FailWithDisplayText calls Complete on the deferral.
                request->FailWithDisplayText("Make a selection in the WebView control and try again.");
            }
        }
        catch (Exception^)
        {
            deferral->Complete();
        }
    });

    // At this point, we haven't populated the data package yet. It's done asynchronously above.
    return false;
}

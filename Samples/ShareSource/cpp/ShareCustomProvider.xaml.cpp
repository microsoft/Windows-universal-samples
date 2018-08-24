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
// ShareCustomProvider.xaml.cpp
// Implementation of the ShareCustomProvider class
//

#include "pch.h"
#include "ShareCustomProvider.xaml.h"

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Navigation;

ShareCustomProvider::ShareCustomProvider()
{
    InitializeComponent();
}

void ShareCustomProvider::OnNavigatedTo(NavigationEventArgs^ e)
{
    SharePage::OnNavigatedTo(e);

    // Register the event to add a Share Provider
    DataTransferManager^ dataTransferManager = DataTransferManager::GetForCurrentView();
    shareProvidersRequestedToken = dataTransferManager->ShareProvidersRequested +=
        ref new TypedEventHandler<DataTransferManager^, ShareProvidersRequestedEventArgs^>(this, &ShareCustomProvider::OnShareProvidersRequested);
}

void ShareCustomProvider::OnNavigatedFrom(NavigationEventArgs^ e)
{
    DataTransferManager^ dataTransferManager = DataTransferManager::GetForCurrentView();
    dataTransferManager->ShareProvidersRequested -= shareProvidersRequestedToken;
    SharePage::OnNavigatedFrom(e);
}

bool ShareCustomProvider::GetShareContent(DataRequest^ request)
{
    DataPackage^ requestData = request->Data;
    requestData->Properties->Title = "I got a new high score!";
    requestData->SetText("I got a high score of 15063!");
    return true;
}

void ShareCustomProvider::OnShareProvidersRequested(DataTransferManager^ s, ShareProvidersRequestedEventArgs^ e)
{
    // Take a deferral so that we can perform async operations. (This sample
    // doesn't perform any async operations in the ShareProvidersRequested
    // event handler, but we take the deferral anyway to demonstrate the pattern.)
    auto deferral = e->GetDeferral();

    // Create the custom share provider and add it.
    RandomAccessStreamReference^ icon = RandomAccessStreamReference::CreateFromUri(ref new Uri("ms-appx:///Assets/windows-sdk.png"));
    ShareProvider^ provider = ref new ShareProvider("Contoso Chat", icon, Windows::UI::Colors::LightGray,
        ref new ShareProviderHandler(this, &ShareCustomProvider::OnShareToContosoChat));
    e->Providers->Append(provider);

    deferral->Complete();
}

void ShareCustomProvider::OnShareToContosoChat(ShareProviderOperation^ operation)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, operation]()
    {
        ShareCompletedText->Text = "";

        // Obtain the text from the data package. This should match
        // the information placed in it by the GetShareContent method.
        create_task(operation->Data->GetTextAsync()).then([this, operation](String^ text)
        {
            // The app can display custom UI to complete the Share operation.
            // Here, we simply display a progress control and delay for a while,
            // to simulate posting to the Contoso Chat service.
            ProgressControl->Visibility = Windows::UI::Xaml::Visibility::Visible;

            auto timer = ref new Windows::UI::Xaml::DispatcherTimer();
            timer->Interval = TimeSpan{ 2 * 1000 * 10000 }; // 2 seconds
            timer->Tick += ref new EventHandler<Object^>([this, timer, text, operation](Object^, Object^)
            {
                timer->Stop();

                // All done. Show the success message.
                ProgressControl->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
                ShareCompletedText->Text = "Your message '" + text + "' has been shared with Contoso Chat.";

                // Completing the operation causes ShareCompleted to be raised.
                operation->ReportCompleted();
            });
            timer->Start();
        });
    }));
}


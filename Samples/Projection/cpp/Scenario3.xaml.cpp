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
#include "Scenario3.xaml.h"
#include "MainPage.xaml.h"
#include "ProjectionViewPage.xaml.h"
#include "ViewLifetimeControl.h"

using namespace SDKSample;

using namespace Concurrency;
using namespace Platform;
using namespace SDKSample::SecondaryViewsHelpers;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::Devices::Enumeration;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238


Scenario3::Scenario3()
{
    InitializeComponent();
    thisViewId = ApplicationView::GetForCurrentView()->Id;
}

void Scenario3::StartProjecting(DeviceInformation^ selectedDevice)
{
    auto prerequisite = task<void>([]() {});
    // If projection is already in progress, then it could be shown on the monitor again
    // Otherwise, we need to create a new view to show the presentation
    if (MainPage::Current->ProjectionViewPageControl == nullptr)
    {
        // First, create a new, blank view
        auto mainDispatcher = Window::Current->Dispatcher;
        int mainViewId = thisViewId;
        prerequisite = create_task(CoreApplication::CreateNewView()->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([mainViewId, mainDispatcher]()
        {
            // ViewLifetimeControl is a wrapper to make sure the view is closed only
            // when the app is done with it
            MainPage::Current->ProjectionViewPageControl = ViewLifetimeControl::CreateForCurrentView();

            // Assemble some data necessary for the new page
            auto initData = ref new ProjectionViewPageInitializationData();
            initData->MainDispatcher = mainDispatcher;
            initData->ProjectionViewPageControl = MainPage::Current->ProjectionViewPageControl;
            initData->MainViewId = mainViewId;

            // Display the page in the view. Note that the view will not become visible
            // until "StartProjectingAsync" is called
            auto rootFrame = ref new Windows::UI::Xaml::Controls::Frame();
            rootFrame->Navigate(TypeName(ProjectionViewPage::typeid), initData);
            Window::Current->Content = rootFrame;

            // The call to Window.Current.Activate is required starting in Windos 10.
            // Without it, the view will never appear.
            Window::Current->Activate();
        })));
    }

    prerequisite.then([this, selectedDevice]()
    {
        try
        {
            // Start/StopViewInUse are used to signal that the app is interacting with the
            // view, so it shouldn't be closed yet, even if the user loses access to it
            MainPage::Current->ProjectionViewPageControl->StartViewInUse();
            // Show the view on a second display (if available) or on the primary display
            create_task(ProjectionManager::StartProjectingAsync(
                MainPage::Current->ProjectionViewPageControl->Id,
                thisViewId,
                selectedDevice
                )).then([]() {
                MainPage::Current->ProjectionViewPageControl->StopViewInUse();
            });
        }
        catch (ObjectDisposedException^)
        {
            // The projection view is being disposed
            MainPage::Current->NotifyUser("The projection view is being disposed", NotifyType::ErrorMessage);
        }
    });
}



void Scenario3::LoadAndDisplayScreens_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        // Use the device selector query of the ProjectionManager to list wired/wireless displays
        String^ projectorSelectorQuery = ProjectionManager::GetDeviceSelector();

        findAndProject_button->IsEnabled = false;

        // Clear the list box
        displayList_listview->Items->Clear();
        displayList_listview->Visibility = Windows::UI::Xaml::Visibility::Visible;

        MainPage::Current->NotifyUser("Searching for devices...", NotifyType::StatusMessage);

        // Calling the device API to find devices based on the device query
        create_task(DeviceInformation::FindAllAsync(
            projectorSelectorQuery
            )).then([=](DeviceInformationCollection^ outputDevices) {

            for (unsigned int i = 0; i < outputDevices->Size; i++)
            {
                DeviceInformation^ device = outputDevices->GetAt(i);
                DeviceInformationViewTemplate^ deviceInfoTemplate = ref new DeviceInformationViewTemplate();
                deviceInfoTemplate->Name = device->Name;
                deviceInfoTemplate->Id = device->Id;
                displayList_listview->Items->InsertAt(i, deviceInfoTemplate);
            }
            
            findAndProject_button->IsEnabled = true;
            MainPage::Current->NotifyUser("Found devices are now listed.", SDKSample::NotifyType::StatusMessage);
        });
    }
    catch (Exception^)
    {
        MainPage::Current->NotifyUser("An error occured when querying and listing devices.", SDKSample::NotifyType::ErrorMessage);
    }
}


void Scenario3::displayList_listview_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    try
    {
        // Get the DeviceInformation object of selected ListView item
        ListView^ devicesList = safe_cast<ListView^>(sender);
        DeviceInformationViewTemplate^ selectedDeviceInfoViewTemplate = safe_cast<DeviceInformationViewTemplate^>(devicesList->SelectedItem);

        if (selectedDeviceInfoViewTemplate != nullptr)
        {
            create_task(DeviceInformation::CreateFromIdAsync(selectedDeviceInfoViewTemplate->Id)).then([=](DeviceInformation^ selectedDevice) {
                // Start projecting to the selected display
                StartProjecting(selectedDevice);

            displayList_listview->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
            });
        }

    }
    catch (Exception^)
    {
        MainPage::Current->NotifyUser("An error occured when starting projecting.", SDKSample::NotifyType::ErrorMessage);
    }
}

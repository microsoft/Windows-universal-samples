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
#include "Scenario1.h"
#include "Scenario1.g.cpp"
#include "SampleConfiguration.h"
#include "SizePreferenceString.h"
#include "ViewLifetimeControl.h"

using namespace winrt;
using namespace Windows::ApplicationModel::Core; 
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

static hstring DEFAULT_TITLE = L"New window";

namespace winrt::SDKTemplate::implementation
{
    Scenario1::Scenario1()
    {
        InitializeComponent();
    }

    void Scenario1::OnNavigatedTo(NavigationEventArgs const&)
    {
        SizePreferenceChooser().ItemsSource(GenerateSizePreferenceBinding());
        SizePreferenceChooser().SelectedIndex(0);

        // "UseNone" is not a valid choice for the incoming view, so only include
        // it in the anchor size preference chooser
        auto anchorSizeChoices = GenerateSizePreferenceBinding();
        anchorSizeChoices.Append(make<SizePreferenceString>(ViewSizePreference::UseNone, L"UseNone"));
        AnchorSizePreferenceChooser().ItemsSource(anchorSizeChoices);
        AnchorSizePreferenceChooser().SelectedIndex(0);

        // This collection is being bound to the current thread. 
        // So, make sure you only update the collection and items
        // contained in it from this thread.
        ViewChooser().ItemsSource(SampleState::SecondaryViews);
    }

    IObservableVector<IInspectable> Scenario1::GenerateSizePreferenceBinding()
    {        
        auto sizeChoices = single_threaded_observable_vector<IInspectable>();

        sizeChoices.Append(make<SizePreferenceString>(ViewSizePreference::Default, L"Default"));
        sizeChoices.Append(make<SizePreferenceString>(ViewSizePreference::UseHalf, L"UseHalf"));
        sizeChoices.Append(make<SizePreferenceString>(ViewSizePreference::UseLess, L"UseLess"));
        sizeChoices.Append(make<SizePreferenceString>(ViewSizePreference::UseMinimum, L"UseMinimum"));
        sizeChoices.Append(make<SizePreferenceString>(ViewSizePreference::UseMore, L"UseMore"));

        return sizeChoices;
    }

    fire_and_forget Scenario1::CreateView_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Set up the secondary view, but don't show it yet
        co_await resume_foreground(CoreApplication::CreateNewView().Dispatcher());

        // We are now in the newly-created window.

        // This object is used to keep track of the views and important
        // details about the contents of those views across threads
        // In your app, you would probably want to track information
        // like the open document or page inside that window

        auto newViewControl = ViewLifetimeControl::CreateForCurrentView();
        newViewControl.Title(DEFAULT_TITLE);
        // Increment the ref count because we just created the view and we have a reference to it                
        newViewControl.StartViewInUse();

        Controls::Frame frame;
        frame.Navigate(xaml_typename<SecondaryViewPage>(), newViewControl);

        Window::Current().Content(frame);
        // This is a change from 8.1: In order for the view to be displayed later it needs to be activated.
        Window::Current().Activate();

        ApplicationView::GetForCurrentView().Title(newViewControl.Title());

        // Be careful! This collection is bound to the main thread,
        // so make sure to update it only from that thread.
        co_await resume_foreground(SampleState::MainDispatcher);

        // We are now back in the main window.
        SampleState::SecondaryViews.Append(newViewControl);
    }

    fire_and_forget Scenario1::ShowAsStandalone_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        auto selectedView = ViewChooser().SelectedItem().as<SDKTemplate::ViewLifetimeControl>();
        auto sizePreference = SizePreferenceChooser().SelectedItem().as<SDKTemplate::SizePreferenceString>();
        auto anchorSizePreference = AnchorSizePreferenceChooser().SelectedItem().as<SDKTemplate::SizePreferenceString>();
        if (selectedView != nullptr && sizePreference != nullptr && anchorSizePreference != nullptr)
        {
            try
            {
                // Prevent the view from closing while
                // switching to it
                selectedView.StartViewInUse();

                // Show the previously created secondary view, using the size
                // preferences the user specified. In your app, you should
                // choose a size that's best for your scenario and code it
                // instead of requiring the user to decide.
                bool viewShown = co_await ApplicationViewSwitcher::TryShowAsStandaloneAsync(
                    selectedView.Id(),
                    sizePreference.Preference(),
                    ApplicationView::GetForCurrentView().Id(),
                    anchorSizePreference.Preference());
                if (!viewShown)
                {
                    // The window wasn't actually shown, so StopViewInUse the reference to it
                    // This may trigger the window to be destroyed
                    rootPage.NotifyUser(L"The view was not shown. Make sure it has focus", NotifyType::ErrorMessage);
                }
                // Signal that switching has completed and let the view close
                selectedView.StopViewInUse();
            }
            catch (hresult_error& ex)
            {
                if (ex.to_abi() == RO_E_CLOSED)
                {
                    // The view could be in the process of closing, and
                    // this thread just hasn't updated. As part of being closed,
                    // this thread will be informed to clean up its list of
                    // views (see SecondaryViewPage.cpp)
                }
                else
                {
                    throw;
                }
            }
        }
        else
        {
            rootPage.NotifyUser(L"Please choose a view to show, a size preference for each view", NotifyType::ErrorMessage);
        }
    }
}

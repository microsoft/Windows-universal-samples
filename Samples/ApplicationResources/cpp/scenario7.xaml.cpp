//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario7.xaml.cpp
// Implementation of the Scenario7 class
//

#include "pch.h"
#include "Scenario7.xaml.h"

using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::Foundation::Collections;
using namespace Windows::System::Threading;
using namespace Windows::Foundation;
using namespace Platform;

Scenario7::Scenario7()
{
    InitializeComponent();
    eventToken.Value = 0;
    defaultContextForCurrentView = ResourceContext::GetForCurrentView();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario7::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void Scenario7::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (eventToken.Value != 0)
    {
        defaultContextForCurrentView->QualifierValues->MapChanged -= eventToken;
        eventToken.Value = 0;
    }
} 

void Scenario7::ContextChanged(IObservableMap<String^, String^>^ sender, IMapChangedEventArgs<String^>^ eventArg)
{
    this->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, 
        ref new Windows::UI::Core::DispatchedHandler([this]() 
        {
            this->Scenario7TextBlock->Text = ResourceManager::Current->MainResourceMap->GetValue("Resources/string1", defaultContextForCurrentView)->ValueAsString;
        }));
}
void Scenario7::Scenario7Button_Show_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (eventToken.Value == 0)
    {
        Scenario7TextBlock->Text = ResourceManager::Current->MainResourceMap->GetValue("Resources/string1", defaultContextForCurrentView)->ValueAsString;
        eventToken = defaultContextForCurrentView->QualifierValues->MapChanged += ref new MapChangedEventHandler<String^, String^>(this, &Scenario7::ContextChanged);
    }
}


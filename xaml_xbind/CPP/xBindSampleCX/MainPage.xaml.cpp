//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "MainPage.xaml.h"

using namespace xBindSampleCX;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Interop;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

//MainPage^ MainPage::Current = nullptr;

MainPage::MainPage()
{
    InitializeComponent();

    // This is a static public property that allows downstream pages to get a handle to the MainPage instance
    // in order to call methods that are in this class.
    MainPage::Current = this;
}

void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
    //// Populate the ListBox with the scenarios as defined in SampleConfiguration.cpp.
    //auto itemCollection = ref new Platform::Collections::Vector<Object^>();
    //int i = 1;
    //for (auto const& s : MainPage::Current->scenarios)
    //{
    //    // Create a textBlock to hold the content and apply the ListItemTextStyle from Styles.xaml
    //    TextBlock^ textBlock = ref new TextBlock();
    //    ListBoxItem^ item = ref new ListBoxItem();
    //    auto style = App::Current->Resources->Lookup("ListItemTextStyle");

    //    textBlock->Text = (i++)->ToString() + ") " + s->Title;
    //    textBlock->Style = safe_cast<Windows::UI::Xaml::Style ^>(style);

    //    item->Name = s->ClassName;
    //    item->Content = textBlock;
    //    itemCollection->Append(item);
    //}

    // Set the newly created itemCollection as the ListBox ItemSource.
    ScenarioControl->ItemsSource = MainPage::Current->scenarios;
    int startingScenarioIndex;

    if (Window::Current->Bounds.Width < 640)
    {
        startingScenarioIndex = -1;
    }
    else
    {
        startingScenarioIndex = 0;
    }

    ScenarioControl->SelectedIndex = startingScenarioIndex;
    ScenarioControl->ScrollIntoView(ScenarioControl->SelectedItem);
}


void MainPage::ScenarioControl_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
    ListBox^ scenarioListBox = safe_cast<ListBox^>(sender); //as ListBox;
    Scenario^ item = dynamic_cast<Scenario^>(scenarioListBox->SelectedItem);
    if (item != nullptr)
    {
        // Clear the status block when changing scenarios
        NotifyUser("", NotifyType::StatusMessage);

        // Navigate to the selected scenario.
        TypeName scenarioType = { item->ClassName, TypeKind::Custom };
        ScenarioFrame->Navigate(scenarioType, this);

        if (Window::Current->Bounds.Width < 640)
        {
            Splitter->IsPaneOpen = false;
            StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        }
    }
}

void MainPage::NotifyUser(String^ strMessage, NotifyType type)
{
    switch (type)
    {
    case NotifyType::StatusMessage:
        StatusBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Green);
        break;
    case NotifyType::ErrorMessage:
        StatusBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Red);
        break;
    default:
        break;
    }
    StatusBlock->Text = strMessage;

    // Collapse the StatusBlock if it has no text to conserve real estate.
    if (StatusBlock->Text != "")
    {
        StatusPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
    else
    {
        StatusPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
}

void MainPage::Footer_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto uri = ref new Uri((String^)((HyperlinkButton^)sender)->Tag);
    Windows::System::Launcher::LaunchUriAsync(uri);
}

void MainPage::Button_Click(Object^ sender, RoutedEventArgs^ e)
{
    Splitter->IsPaneOpen = !Splitter->IsPaneOpen;
    StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

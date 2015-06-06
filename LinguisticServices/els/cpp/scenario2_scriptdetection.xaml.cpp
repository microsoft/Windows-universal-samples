// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2_ScriptDetection.xaml.h"

#include "elsutil.h"

using namespace SDKTemplate;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_ScriptDetection::Scenario2_ScriptDetection()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2_ScriptDetection::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void Scenario2_ScriptDetection::Go_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Button^ b = safe_cast<Button^>(sender);
    if (b != nullptr)
    {
        auto input = TextInput->Text;
        auto output = ELS::RecognizeTextScripts(input->Data());
        TextOutput->Text = ref new Platform::String(output.c_str());
    }
}

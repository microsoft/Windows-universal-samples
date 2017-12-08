// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1Basic.xaml.h"
#include "PageToPrint.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Printing;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario1Basic::Scenario1Basic()
{
    InitializeComponent();
}

void Scenario1Basic::OnPrintButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    printHelper->ShowPrintUIAsync();
}

void Scenario1Basic::OnNavigatedTo(NavigationEventArgs^ e)
{
    if (PrintManager::IsSupported())
    {
        // Tell the user how to print
        MainPage::Current->NotifyUser("Print contract registered with customization, use the Print button to print.", NotifyType::StatusMessage);
    }
    else
    {
        // Remove the print button
        InvokePrintingButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

        // Inform user that Printing is not supported
        MainPage::Current->NotifyUser("Printing is not supported.", NotifyType::ErrorMessage);

        // Printing-related event handlers will never be called if printing
        // is not supported, but it's okay to register for them anyway.
    }

    // Initalize common helper class and register for printing
    printHelper = ref new PrintHelper(this);
    printHelper->RegisterForPrinting();

    // Initialize print content for this scenario
    printHelper->PreparePrintContent(ref new PageToPrint());
}

void Scenario1Basic::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (printHelper)
    {
        printHelper->UnregisterForPrinting();
    }
}

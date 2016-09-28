// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario2StandardOptions.xaml.h"
#include "PageToPrint.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Printing;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

void StandardOptionsPrintHelper::PrintTaskRequested(PrintManager^ sender, PrintTaskRequestedEventArgs^ e)
{
    PrintTask^ printTask = e->Request->CreatePrintTask("C++ Printing SDK Sample", ref new PrintTaskSourceRequestedHandler([=](PrintTaskSourceRequestedArgs^ args)
    {
        args->SetSource(PrintDocumentSource);
    }));

    // Clear the print options that are displayed beacuse adding the same one multiple times will throw an exception
    IVector<String^>^ displayedOptions = printTask->Options->DisplayedOptions;
    displayedOptions->Clear();

    // Choose the printer options to be shown.
    // The order in which the options are appended determines the order in which they appear in the UI
    displayedOptions->Append(StandardPrintTaskOptions::Copies);
    displayedOptions->Append(StandardPrintTaskOptions::Orientation);
    displayedOptions->Append(StandardPrintTaskOptions::MediaSize);
    displayedOptions->Append(StandardPrintTaskOptions::Collation);
    displayedOptions->Append(StandardPrintTaskOptions::Duplex);

    // Preset the default value of the printer option
    printTask->Options->MediaSize = PrintMediaSize::NorthAmericaLegal;

    // Print Task event handler is invoked when the print job is completed.
    printTask->Completed += ref new TypedEventHandler<PrintTask^, PrintTaskCompletedEventArgs^>([=](PrintTask^ sender, PrintTaskCompletedEventArgs^ e)
    {
        // Notify the user when the print operation fails.
        if (e->Completion == Windows::Graphics::Printing::PrintTaskCompletion::Failed)
        {
            auto callback = ref new Windows::UI::Core::DispatchedHandler([=]()
            {
                MainPage::Current->NotifyUser(ref new String(L"Failed to print."), NotifyType::ErrorMessage);
            });

            ScenarioPage->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, callback);
        }
    });
}


Scenario2StandardOptions::Scenario2StandardOptions()
{
    InitializeComponent();
}

void Scenario2StandardOptions::OnPrintButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    printHelper->ShowPrintUIAsync();
}

void Scenario2StandardOptions::OnNavigatedTo(NavigationEventArgs^ e)
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
    printHelper = ref new StandardOptionsPrintHelper(this);
    printHelper->RegisterForPrinting();

    // Initialize print content for this scenario
    printHelper->PreparePrintContent(ref new PageToPrint());
}

void Scenario2StandardOptions::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (printHelper)
    {
        printHelper->UnregisterForPrinting();
    }
}

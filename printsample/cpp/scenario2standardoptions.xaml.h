// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario2StandardOptions.g.h"
#include "PrintHelper.h"

namespace SDKTemplate
{
    ref class StandardOptionsPrintHelper sealed : public PrintHelper
    {
    public:
        StandardOptionsPrintHelper(Page^ scenarioPage) : PrintHelper(scenarioPage) { }

    protected:
        /// <summary>
        /// This is the event handler for PrintManager.PrintTaskRequested.
        /// In order to ensure a good user experience, the system requires that the app handle the PrintTaskRequested event within the time specified by PrintTaskRequestedEventArgs.Request.Deadline.
        /// Therefore, we use this handler to only create the print task.
        /// The print settings customization can be done when the print document source is requested.
        /// </summary>
        /// <param name="sender">PrintManager</param>
        /// <param name="e">PrintTaskRequestedEventArgs</param>
        virtual void PrintTaskRequested(Windows::Graphics::Printing::PrintManager^ sender, Windows::Graphics::Printing::PrintTaskRequestedEventArgs^ e) override;
    };

    /// <summary>
    /// Scenario that demos how to add customizations in the displayed options list.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2StandardOptions sealed
    {
    public:
        Scenario2StandardOptions();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        PrintHelper^ printHelper;
        void OnPrintButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

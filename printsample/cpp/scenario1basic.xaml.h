// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1Basic.g.h"
#include "PrintHelper.h"

namespace SDKTemplate
{
    /// <summary>
    /// Scenario that demonstrates how to invoke the Print UI
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1Basic sealed
    {
    public:
        Scenario1Basic();
    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        PrintHelper^ printHelper;
        void OnPrintButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

//
// SecondaryPage.xaml.h
// Declaration of the SecondaryPage class
//

#pragma once

#include "SecondaryPage.g.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class SecondaryPage sealed
    {
    public:
        SecondaryPage();

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    };
}

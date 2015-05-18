//
// PageToPrint.xaml.h
// Declaration of the PageToPrint class
//

#pragma once

#include "PageToPrint.g.h"

namespace SDKTemplate
{
    /// <summary>
    /// Page content to send to the printer
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class PageToPrint sealed
    {
    public:
        PageToPrint();

        property Windows::UI::Xaml::Controls::RichTextBlock^ TextContentBlock;
    };
}

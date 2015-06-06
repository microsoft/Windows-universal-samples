// Copyright (c) Microsoft. All rights reserved.

#pragma once
#include "ContinuationPage.g.h"

namespace SDKTemplate
{
    /// <summary>
    /// A paged used to flow text from a given text container
    /// Usage: Output scenarios 1-4 might not fit entirely on a given "printer page"
    /// In that case simply add new continuation pages of the given size until all the content can be displayed
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class ContinuationPage sealed
    {
    public:
        /// <summary>
        /// Creates a continuation page and links text-flow to a text flow container
        /// </summary>
        /// <param name="textLinkContainer">Text link container which will flow text into this page</param>
        ContinuationPage(Windows::UI::Xaml::Controls::RichTextBlockOverflow^ textLinkContainer);
    };
}

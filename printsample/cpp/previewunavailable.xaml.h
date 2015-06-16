// Copyright (c) Microsoft. All rights reserved.

#pragma once
#include "PreviewUnavailable.g.h"

namespace SDKTemplate
{
    /// <summary>
    /// Used in scenario 5 to display a custom print preview unavailable page
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class PreviewUnavailable sealed
    {
    public:
        PreviewUnavailable();

        /// <summary>
        /// Preview unavailable page constructor
        /// </summary>
        /// <param name="paperSize">The printing page paper size</param>
        /// <param name="printableSize">The usable/"printable" area on the page</param>
        PreviewUnavailable(Windows::Foundation::Size paperSize, Windows::Foundation::Size printableSize);
    };
}

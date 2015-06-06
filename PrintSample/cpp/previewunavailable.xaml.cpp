// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "PreviewUnavailable.xaml.h"

using namespace SDKTemplate;

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

PreviewUnavailable::PreviewUnavailable()
{
    InitializeComponent();
}

PreviewUnavailable::PreviewUnavailable(Size paperSize, Size printableSize)
{
    InitializeComponent();

    Page->Width = paperSize.Width;
    Page->Height = paperSize.Height;

    PrintablePage->Width = printableSize.Width;
    PrintablePage->Height = printableSize.Height;
}

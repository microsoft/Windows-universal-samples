// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "ContinuationPage.xaml.h"

using namespace SDKTemplate;
using namespace Windows::UI::Xaml::Controls;

ContinuationPage::ContinuationPage(RichTextBlockOverflow^ textLinkContainer)
{
    InitializeComponent();
    textLinkContainer->OverflowContentTarget = ContinuationPageLinkedContainer;
}

//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario1.xaml.cpp
// Implementation of the Scenario1 class
//

#include "pch.h"
#include <ppltasks.h>
#include "Find.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::UI::Text;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;

Scenario1::Scenario1()
{
    InitializeComponent();
    FindQueryText->Text = "continent OR can OR rain";
    ContentText->Text = "Mount Rainier is on the North American Continent.";
}

void Scenario1::Find_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ContentTextOutput->Text = "";
    auto mySemanticTextQuery = ref new Windows::Data::Text::SemanticTextQuery(FindQueryText->Text);
    IVectorView<Windows::Data::Text::TextSegment>^ ranges = mySemanticTextQuery->Find(ContentText->Text);
    HighlightRanges(ContentTextOutput, ContentText->Text, ranges);
}


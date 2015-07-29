//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

//
// Scenario2.xaml.cpp
// Implementation of the Scenario2 class
//

#include "pch.h"
#include <ppltasks.h>
#include "FindInProperty.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;

Scenario2::Scenario2()
{
    InitializeComponent();
    FindQueryText->Text = "Title:Good OR Content:Tree";
    TitleText->Text = "These are Good Times";
    ContentText->Text = "There are many good times to be had. Go for a walk among the trees.";
}

void Scenario2::Find_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ContentTextOutput->Text = "";
    TitleTextOutput->Text = "";
    auto mySemanticTextQuery = ref new Windows::Data::Text::SemanticTextQuery(FindQueryText->Text);
    IVectorView<Windows::Data::Text::TextSegment>^ ranges = mySemanticTextQuery->FindInProperty(ContentText->Text, "System.Search.Contents");
    HighlightRanges(ContentTextOutput, ContentText->Text, ranges);
    IVectorView<Windows::Data::Text::TextSegment>^ rangesTitle = mySemanticTextQuery->FindInProperty(TitleText->Text, "System.Title");
    HighlightRanges(TitleTextOutput, TitleText->Text, rangesTitle);
}

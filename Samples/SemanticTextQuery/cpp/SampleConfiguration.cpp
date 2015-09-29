// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;
using namespace concurrency;
using namespace Platform;
using namespace Windows::Data::Text;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Text;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Find Scenario",                            "SDKTemplate.Scenario1" }, 
    { "FindInProperty Scenario",                  "SDKTemplate.Scenario2" },
    { "GetMatchingPropertiesWithRanges Scenario", "SDKTemplate.Scenario3" }
};

void ::SDKTemplate::HighlightRanges(TextBlock^ tb,  String^ TextContent, IVectorView<TextSegment>^ ranges)
{
    int currentPosition = 0;
    const wchar_t* StringData = TextContent->Data();
    for (const auto& range : ranges)
    {
        // Add the next chunk of non-range text
        if ((int)range.StartPosition > currentPosition)
        {
            int length = (int)range.StartPosition - currentPosition;
            String^ normalString = "";
            for (int i = currentPosition; i < (currentPosition + length); i++)
            {
                normalString += StringData[i];
            }
            Run^ normaltext = ref new Run();
            normaltext->Text = normalString;
            tb->Inlines->Append(normaltext);
            currentPosition += length;
        }
        // Add the next range
        String^ boldString = "";
        for (int i = (int)range.StartPosition; i < ((int)range.StartPosition + (int)range.Length); i++)
        {
            boldString += StringData[i];
        }
        Run^ boldtext = ref new Run();
        boldtext->Text = boldString;
        boldtext->FontWeight = FontWeights::Bold;
        tb->Inlines->Append(boldtext);
        currentPosition += (int)range.Length;
    }
    // Add the text after the last matching segment
    if (currentPosition < (int)TextContent->Length())
    {
        String^ finalString = "";
        for (int i = currentPosition; i < (int)TextContent->Length(); i++)
        {
            finalString += StringData[i];
        }
        StringData[currentPosition];
        Run^ normaltext = ref new Run();
        normaltext->Text = finalString;
        tb->Inlines->Append(normaltext);
    }
    Run^ eol = ref new Run();
    eol->Text = "\r\n";
    tb->Inlines->Append(eol);
}

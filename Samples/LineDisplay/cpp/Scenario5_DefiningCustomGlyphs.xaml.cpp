//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Scenario5_DefiningCustomGlyphs.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Security::Cryptography;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Navigation;

Scenario5_DefiningCustomGlyphs::Scenario5_DefiningCustomGlyphs()
{
    InitializeComponent();
    DataContext = this;
}

void Scenario5_DefiningCustomGlyphs::OnNavigatedTo(NavigationEventArgs^ e)
{
    InitializeAsync();
}

void Scenario5_DefiningCustomGlyphs::OnNavigatedFrom(NavigationEventArgs^ e)
{
    delete lineDisplay;
    lineDisplay = nullptr;
}

static String^ StringFromUtf32(unsigned int utf32)
{
    wchar_t glyphString[3] = {};
    if (utf32 < 0x10000)
    {
        glyphString[0] = static_cast<wchar_t>(utf32);
    }
    else
    {
        Windows::Data::Text::UnicodeCharacters::GetSurrogatePairFromCodepoint(utf32, &glyphString[0], &glyphString[1]);
    }
    return ref new String(glyphString);
}

task<void> Scenario5_DefiningCustomGlyphs::InitializeAsync()
{
    ResetButton->IsEnabled = false;
    SupportedGlyphsComboBox->Items->Clear();

    lineDisplay = co_await rootPage->ClaimScenarioLineDisplayAsync();

    if ((lineDisplay != nullptr) && lineDisplay->Capabilities->CanDisplayCustomGlyphs)
    {
        std::wstring sampleText = L"ABC";
        for (unsigned int glyphCode : lineDisplay->CustomGlyphs->SupportedGlyphCodes)
        {
            String^ glyphString = StringFromUtf32(glyphCode);
            Helpers::AddItem(SupportedGlyphsComboBox, glyphCode, "Code " + glyphCode.ToString() + " (UTF-32 '" + glyphString + "')");
            if (sampleText.length() < 10)
            {
                sampleText.append(glyphString->Data());
            }
        }

        if (SupportedGlyphsComboBox->Items->Size > 0)
        {
            SupportedGlyphsComboBox->SelectedIndex = 0;
        }

        Size glyphSize = lineDisplay->CustomGlyphs->SizeInPixels;
        glyphBuffer = CreateSolidGlyphBuffer(static_cast<int>(glyphSize.Width), static_cast<int>(glyphSize.Height));

        co_await lineDisplay->DefaultWindow->TryDisplayTextAsync(ref new String(sampleText.c_str()));
    }

    ResetButton->IsEnabled = true;
}

void Scenario5_DefiningCustomGlyphs::DefineGlyphButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    invoke_async_lambda([=]() -> task<void>
    {
        auto selectedGlyphCode = Helpers::GetSelectedItemTag<unsigned int>(SupportedGlyphsComboBox);
        if (co_await lineDisplay->CustomGlyphs->TryRedefineAsync(selectedGlyphCode, glyphBuffer) &&
            co_await lineDisplay->DefaultWindow->TryRefreshAsync())
        {
            rootPage->NotifyUser("Redefined glyph " + selectedGlyphCode.ToString() + ".", NotifyType::StatusMessage);
        }
        else
        {
            rootPage->NotifyUser("Failed to redefine glyph and refresh.", NotifyType::ErrorMessage);
        }
    });
}

void Scenario5_DefiningCustomGlyphs::ResetButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    invoke_async_lambda([=]() -> task<void>
    {
        delete lineDisplay;
        lineDisplay = nullptr;
        co_await InitializeAsync();
    });
}

IBuffer^ Scenario5_DefiningCustomGlyphs::CreateSolidGlyphBuffer(int widthInPixels, int heightInPixels)
{
    // Data is sent in one byte chunks, so find the minimum number of whole bytes 
    // needed to define each row of the glyph.
    int bytesPerRow = (widthInPixels + 7) / 8;

    auto newGlyphBuffer = ref new Array<byte>(bytesPerRow * heightInPixels);
    for (unsigned int i = 0; i < newGlyphBuffer->Length; i++)
    {
        // Set every bit so that we draw a solid rectangle.
        newGlyphBuffer[i] = 0xFF;
    }

    return CryptographicBuffer::CreateFromByteArray(newGlyphBuffer);
}

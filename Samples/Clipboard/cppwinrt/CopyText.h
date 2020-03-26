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

#pragma once

#include "CopyText.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct CopyText : CopyTextT<CopyText>
    {
        CopyText();

        void CopyButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget PasteButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        hstring imgSrc = L"ms-appx-web:///assets/windows-sdk.png";
        hstring htmlFragment = L"Use clipboard to copy and paste text in different formats, including plain text, and formatted text (HTML). <br />"
            L" To <b>copy</b>, add text formats to a <i>DataPackage</i>, and then place <i>DataPackage</i> to Clipboard.<br /> "
            L"To <b>paste</b>, get <i>DataPackageView</i> from clipboard, and retrieve the content of the desired text format from it.<br />"
            L"To handle local image files in the formatted text (such as the one below), use ResourceMap collection."
            L"<br /><img id=\"scenario1LocalImage\" src=\"" +
            imgSrc +
            L"\" /><br />"
            L"<i>DataPackage</i> class is also used to share or send content between applications. <br />"
            L"See the Share SDK sample for more information.";

        fire_and_forget DisplayResourceMapAsync(Windows::ApplicationModel::DataTransfer::DataPackageView dataPackageView);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct CopyText : CopyTextT<CopyText, implementation::CopyText>
    {
    };
}

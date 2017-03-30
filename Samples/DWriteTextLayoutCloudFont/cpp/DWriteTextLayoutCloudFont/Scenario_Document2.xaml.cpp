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
#include "Scenario_Document2.xaml.h"
#include "DocumentPresenter.h"

using namespace SDKTemplate;
using namespace DWriteTextLayoutCloudFontImplementation;

using Platform::Object;
using Platform::String;
using Windows::UI::Color;
using Windows::UI::ColorHelper;
using Windows::UI::Xaml::SizeChangedEventArgs;


// The DocumentPresenter class is used to handle, at a high level, the text layout that
// uses downloadable fonts, the display of that layout, and interaction with DirectWrite's
// font download mechanisms. The lower-level implementation is handled in classes that the
// DocumentPresenter class uses. The TextLayout and FontDownloadListener classes are the
// primary focus for this sample.


Scenario_Document2::Scenario_Document2()
{
    InitializeComponent();

    String^ text =
        L"住山掲性蘇際連際不派人集国重五。開分由生理捕普路武踏芭星通捕員会配。視写勢象東部終味真働京請童木産目価準騰地。勝年格予国御稿質格堂表東込紅統驚繰都。障続択写反香雨形混化会安気予又営呼供内役。易世鳴王実少快妻代生重禁寄試早注会夫要。読覆直豊月関事止発番激皇護守反兵目。東日抽徳在転法円制聞店大県特不社衣秋。" L"\xd\xa\xd\xa"
        L"福子会況皇経病展確典盤無果層告逃。与事講責批連値現覧否死昔現兆同禁合徒入。分界走陽輝知明筋明尽申演総際男未天会占。測要家門宅投告毎丘上拡変県後校望憲気酸足。真近前国支界踊情提継底説級写世提秀米要歩。時止治際将表熊詳動学演名能。俳安体亮月見街十載備承前雲楽載媒戦婚米質。皇何果泰機同相暮田善世転。";

    String^ downloadableFontName = ref new String(L"FangSong");

    // Set the colors to be used for the text layout. Also apply to the frame background.
    Color textColor = ColorHelper::FromArgb(0xFF, 0xFF, 0xFF, 0xFF);
    Color textBackgroundColor = ColorHelper::FromArgb(0xFF, 0x30, 0x30, 0x30);

    // Create the DocumentPresenter for our document.
    m_documentPresenter = ref new DocumentPresenter(
        text,
        downloadableFontName,
        L"zh-Hans", // Language tag for the content (Simplified Chinese).
        textColor,
        textBackgroundColor,
        TextLayoutImage // The target image control for the SurfaceImageSource presenting the text layout.
    );
}



// Event handlers:

void Scenario_Document2::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    m_documentPresenter = nullptr;
}


void Scenario_Document2::TextLayoutFrame_SizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
    // Get the current available width and update the TextLayout if different.
    // The available width will be the actual width less the left and right padding.
    //
    // A MinWidth set in the XAML markup ensures we have valid width after deducting
    // the padding.

    double width = TextLayoutFrame->ActualWidth;
    auto padding = TextLayoutFrame->Padding;
    width -= (padding.Left + padding.Right);

    // XAML uses DIPs expressed as double. DirectWrite uses DIPs expressed as float.
    // casting here.
    m_documentPresenter->SetLayoutWidth(static_cast<float>(width));
}

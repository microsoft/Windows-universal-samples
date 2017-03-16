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

using namespace SDKTemplate;
using namespace DWriteTextLayoutCloudFontImplementation;

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

using Windows::Graphics::Display::DisplayInformation;

// See Scenario_Document1.xaml.cpp for information on code structure in this file.


Scenario_Document2::Scenario_Document2()
    : m_rootPage(MainPage::Current)
{
    InitializeComponent();

    String^ text =
        L"住山掲性蘇際連際不派人集国重五。開分由生理捕普路武踏芭星通捕員会配。視写勢象東部終味真働京請童木産目価準騰地。勝年格予国御稿質格堂表東込紅統驚繰都。障続択写反香雨形混化会安気予又営呼供内役。易世鳴王実少快妻代生重禁寄試早注会夫要。読覆直豊月関事止発番激皇護守反兵目。東日抽徳在転法円制聞店大県特不社衣秋。" L"\xd\xa\xd\xa"
        L"福子会況皇経病展確典盤無果層告逃。与事講責批連値現覧否死昔現兆同禁合徒入。分界走陽輝知明筋明尽申演総際男未天会占。測要家門宅投告毎丘上拡変県後校望憲気酸足。真近前国支界踊情提継底説級写世提秀米要歩。時止治際将表熊詳動学演名能。俳安体亮月見街十載備承前雲楽載媒戦婚米質。皇何果泰機同相暮田善世転。"; 

    m_downloadableFontName = ref new Platform::String(L"FangSong");

    // Set the colors to be used for the text layout. Also apply to the frame background.
    m_textColor = Windows::UI::ColorHelper::FromArgb(0xFF, 0xFF, 0xFF, 0xFF);
    m_textBackgroundColor = Windows::UI::ColorHelper::FromArgb(0xFF, 0x30, 0x30, 0x30);
    TextLayoutFrame->Background = ref new SolidColorBrush(m_textBackgroundColor);

    // Create the TextLayout for our text content.
    m_textLayout = ref new TextLayout(text, "zh-Hans", m_downloadableFontName, 20, 300);

    // Update the status bar to show what fonts have actually been used.
    UpdateStatus();

    // Initialize FontDownloadListener, register the event handler, and initiate 
    // the download.
    FontDownloadListener::Initialize();
    m_downloadCompletedEventToken = FontDownloadListener::DownloadCompleted += ref new FontDownloadCompletedHandler(this, &Scenario_Document2::FontDownloadListener_DownloadCompleted);
    FontDownloadListener::BeginDownload();

    // Handle DPI.
    m_displayInformation = DisplayInformation::GetForCurrentView();
    m_dpi = m_displayInformation->LogicalDpi;
    m_dpiChangedEventToken = m_displayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Scenario_Document2::DisplayInformation_DpiChanged);

    // Handle window visibility change.
    m_visibilityChangedEventToken = Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &SDKTemplate::Scenario_Document2::Window_VisibilityChanged);
}

void Scenario_Document2::OnNavigatedFrom(NavigationEventArgs^ e)
{
    FontDownloadListener::DownloadCompleted -= m_downloadCompletedEventToken;
    m_displayInformation->DpiChanged -= m_dpiChangedEventToken;
    Window::Current->VisibilityChanged -= m_visibilityChangedEventToken;
}

// Event handlers:

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
    if (static_cast<float>(width) != m_textLayout->Width)
    {
        // Update the width of the TextLayout and present the updated layout.
        m_textLayout->Width = static_cast<float>(width);
        PresentTextLayout();
    }
}



void Scenario_Document2::FontDownloadListener_DownloadCompleted()
{
    // DownloadCompleted is called from a worker thread, so schedule work
    // in the UI thread.
    this->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this]() { RequestTextLayoutUpdate(); })
        );
}


void Scenario_Document2::DisplayInformation_DpiChanged(DisplayInformation^ displayInformation, Object^ e)
{
    m_dpi = displayInformation->LogicalDpi;
    PresentTextLayout();
}


void Scenario_Document2::Window_VisibilityChanged(Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e)
{
    // Re-create the SurfaceImageSource if the window has just become visible.
    if (e->Visible)
    {
        PresentTextLayout();
    }
}



// Private helper methods:

void Scenario_Document2::RequestTextLayoutUpdate()
{
    // Don't initiate new work while there's work already in progress.
    if (!m_layoutUpdateInProgress)
    {
        m_layoutUpdateInProgress = true;

        this->Dispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Low,
            ref new Windows::UI::Core::DispatchedHandler([this]() { UpdateTextLayout(); })
            );
    }
}


void Scenario_Document2::UpdateTextLayout()
{
    // Refresh the TextLayout and present the updated layout. The refreshed
    // layout will be able to use fonts that have been downloaded.
    m_textLayout->InvalidateLayout();
    PresentTextLayout();

    // Update the status bar to show what fonts have actually been used.
    UpdateStatus();

    m_layoutUpdateInProgress = false;
}


void Scenario_Document2::PresentTextLayout()
{
    // Create a new TextLayoutImageSource based on the text layout and
    // use it as the source for the XAML image element.
    m_textLayoutImageSource = ref new TextLayoutImageSource(m_textLayout, m_dpi, m_textColor, m_textBackgroundColor);
    TextLayoutImage->Source = m_textLayoutImageSource;
}


void Scenario_Document2::UpdateStatus()
{
    // Get the fonts that have actually been used in the text layout.
    Platform::String^ fontsUsed = m_textLayout->FontsUsed;

    // The status will reflect whether the font used was the requested font
    // or a fallback font.
    NotifyType statusType = (fontsUsed == m_downloadableFontName) ? NotifyType::StatusMessage : NotifyType::ErrorMessage;

    // Now update the status.
    m_rootPage->NotifyUser("Fonts actually used: " + fontsUsed, statusType);
}

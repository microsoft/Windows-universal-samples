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
#include "Scenario_Document3.xaml.h"

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


Scenario_Document3::Scenario_Document3()
    : m_rootPage(MainPage::Current)
{
    InitializeComponent();

    String^ text =
        L"चिदंश रखते जिम्मे एसेएवं निर्माता जैसी दर्शाता विस्तरणक्षमता विकेन्द्रियकरण मुक्त प्रमान सभीकुछ नवंबर नीचे भारत संसाध मुश्किले प्रतिबध्दता बाजार और्४५० सोफ़्टवेर नाकर विश्व जनित बातसमय एकत्रित शुरुआत मर्यादित वर्ष सुचना कुशलता समस्याओ भाषा लेकिन स्थिति संस्थान सार्वजनिक लक्षण मानव प्रमान विकसित बनाकर रखति संस्था लचकनहि वातावरण मुखय बनाकर एवम् प्रदान ब्रौशर बनाना" L"\xd\xa\xd\xa"
        L"दोषसके निर्माण शारिरिक देने बढाता एवम् संस्थान वार्तालाप मुख्य आवश्यक अविरोधता विषय सदस्य बलवान सहायता केन्द्रिय एवम् कराना वर्तमान निर्देश कीसे पढने सारांश गटकउसि प्राधिकरन लेकिन विभाजन प्रेरना समस्याए करता पहेला देते ध्येय उद्योग सामूहिक भेदनक्षमता है।अभी कार्यकर्ता समाजो दारी संसाध";

    m_downloadableFontName = ref new Platform::String(L"Kokila");

    // Set the colors to be used for the text layout. Also apply to the frame background.
    m_textColor = Windows::UI::ColorHelper::FromArgb(0xFF, 0xFF, 0xFF, 0xFF);
    m_textBackgroundColor = Windows::UI::ColorHelper::FromArgb(0xFF, 0x30, 0x30, 0x30);
    TextLayoutFrame->Background = ref new SolidColorBrush(m_textBackgroundColor);

    // Create the TextLayout for our text content.
    m_textLayout = ref new TextLayout(text, "hi", m_downloadableFontName, 20, 300);

    // Update the status bar to show what fonts have actually been used.
    UpdateStatus();

    // Initialize FontDownloadListener, register the event handler, and initiate 
    // the download.
    FontDownloadListener::Initialize();
    m_downloadCompletedEventToken = FontDownloadListener::DownloadCompleted += ref new FontDownloadCompletedHandler(this, &Scenario_Document3::FontDownloadListener_DownloadCompleted);
    FontDownloadListener::BeginDownload();

    // Handle DPI.
    m_displayInformation = DisplayInformation::GetForCurrentView();
    m_dpi = m_displayInformation->LogicalDpi;
    m_dpiChangedEventToken = m_displayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Scenario_Document3::DisplayInformation_DpiChanged);

    // Handle window visibility change.
    m_visibilityChangedEventToken = Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &Scenario_Document3::Window_VisibilityChanged);
}

void Scenario_Document3::OnNavigatedFrom(NavigationEventArgs^ e)
{
    FontDownloadListener::DownloadCompleted -= m_downloadCompletedEventToken;
    m_displayInformation->DpiChanged -= m_dpiChangedEventToken;
    Window::Current->VisibilityChanged -= m_visibilityChangedEventToken;
}


// Event handlers:

void Scenario_Document3::TextLayoutFrame_SizeChanged(Object^ sender, SizeChangedEventArgs^ e)
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


void Scenario_Document3::FontDownloadListener_DownloadCompleted()
{
    // DownloadCompleted is called from a worker thread, so schedule work
    // in the UI thread.
    this->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this]() { RequestTextLayoutUpdate(); })
        );
}


void Scenario_Document3::DisplayInformation_DpiChanged(DisplayInformation^ displayInformation, Object^ e)
{
    m_dpi = displayInformation->LogicalDpi;
    PresentTextLayout();
}


void Scenario_Document3::Window_VisibilityChanged(Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e)
{
    // Re-create the SurfaceImageSource if the window has just become visible.
    if (e->Visible)
    {
        PresentTextLayout();
    }
}



// Private helper methods:

void Scenario_Document3::RequestTextLayoutUpdate()
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


void Scenario_Document3::UpdateTextLayout()
{
    // Refresh the TextLayout and present the updated layout. The refreshed
    // layout will be able to use fonts that have been downloaded.
    m_textLayout->InvalidateLayout();
    PresentTextLayout();

    // Update the status bar to show what fonts have actually been used.
    UpdateStatus();

    m_layoutUpdateInProgress = false;
}


void Scenario_Document3::PresentTextLayout()
{
    // Create a new TextLayoutImageSource based on the text layout and
    // use it as the source for the XAML image element.
    m_textLayoutImageSource = ref new TextLayoutImageSource(m_textLayout, m_dpi, m_textColor, m_textBackgroundColor);
    TextLayoutImage->Source = m_textLayoutImageSource;
}


void Scenario_Document3::UpdateStatus()
{
    // Get the fonts that have actually been used in the text layout.
    Platform::String^ fontsUsed = m_textLayout->FontsUsed;

    // The status will reflect whether the font used was the requested font
    // or a fallback font.
    NotifyType statusType = (fontsUsed == m_downloadableFontName) ? NotifyType::StatusMessage : NotifyType::ErrorMessage;

    // Now update the status.
    m_rootPage->NotifyUser("Fonts actually used: " + fontsUsed, statusType);
}

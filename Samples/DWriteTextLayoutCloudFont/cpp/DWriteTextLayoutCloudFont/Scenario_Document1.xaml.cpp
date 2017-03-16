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
#include "Scenario_Document1.xaml.h"

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

// Text will be laid out using IDWriteTextLayout (implemented in the  
// TextLayout wrapper class) and then presented within XAML using a  
// SurfaceImageSource (implemented as the TextLayoutImageSource class). 
//
// The width and height of the SurfaceImageSource need to  be determined before 
// it is created. This will be done by getting the current width of the XAML 
// frame in which the content will be presented, creating the text layout using 
// that width, and then getting the height determined for the layout.
//
// The content will be presented in the TextLayoutFrame element (a 
// ScrollViewer), and the actual width of that element won't be set until after 
// the page is loaded. We can initially create the text layout in the page 
// constructor with a default width (300) and then update it as the size of 
// TextLayoutFrame changes.
//
// The layout will be drawn using a font that may not be installed on the 
// system. (For this page, the font used is contained in an optional Windows 
// feature that is not installed by default.) The text layout will initially 
// use fallback fonts. After it is created, the FontDownloadListener class will 
// be used to interact with the DirectWrite font download queue to request that 
// the specified font data be downloaded and then to get a notification when 
// the data is downloaded. When that event fires, the text layout will be 
// invalidated, forcing the text to be layed out again. This time, it will be
// able to use the selected font data that has been downloaded.


Scenario_Document1::Scenario_Document1()
    : m_rootPage(MainPage::Current)
{
    InitializeComponent();

    String^ text =
        L"Lorem ipsum dolor sit amet, pri ea voluptua consetetur. Vel eu altera omittam. Voluptaria reprehendunt vel eu, eum te graeco omnium blandit, mel nullam persius verterem eu. An euismod salutandi vis, vero exerci oblique ex mea. Cu quo audiam verear, esse facilis omittantur usu ei. Eam id dolor corpora oportere, vix dolor tincidunt id, viris dolorem vix ne. Minim gubergren id cum, vim ei quas denique prodesset." L"\xd\xa\xd\xa"
        L"Duo quem justo assentior ei. Cu simul quidam mei, ut stet incorrupte definitionem duo. Ne mei utinam graece intellegam, no causae mediocritatem vix, vim mundi aperiri detracto ei. Dictas definiebas ei in appellantur."; 

    m_downloadableFontName = ref new Platform::String(L"Neue Haas Grotesk Text Pro");

    // Set the colors to be used for the text layout. Also apply to the frame background.
    m_textColor = Windows::UI::ColorHelper::FromArgb(0xFF, 0xFF, 0xFF, 0xFF);
    m_textBackgroundColor = Windows::UI::ColorHelper::FromArgb(0xFF, 0x30, 0x30, 0x30);
    TextLayoutFrame->Background = ref new SolidColorBrush(m_textBackgroundColor);

    // Create the TextLayout for our text content.
    m_textLayout = ref new TextLayout(text, "en-US", m_downloadableFontName, 20, 300);

    // Update the status bar to show what fonts have actually been used.
    UpdateStatus();

    // Initialize FontDownloadListener, register the event handler, and initiate 
    // the download.
    FontDownloadListener::Initialize();
    m_downloadCompletedEventToken = FontDownloadListener::DownloadCompleted += ref new FontDownloadCompletedHandler(this, &Scenario_Document1::FontDownloadListener_DownloadCompleted);
    FontDownloadListener::BeginDownload();

    // When the SurfaceImageSource is created, we'll need to know the current DPI.
    // We'll also need an event handler to re-create the image source if the DPI changes.
    m_displayInformation = DisplayInformation::GetForCurrentView();
    m_dpi = m_displayInformation->LogicalDpi;
    m_dpiChangedEventToken = m_displayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &Scenario_Document1::DisplayInformation_DpiChanged);

    // When the app window is hidden, XAML may release resource used by the 
    // SurfaceImageSource. We'll need to make sure it gets re-created if the
    // window was hidden and then becomes visible again.
    m_visibilityChangedEventToken = Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &Scenario_Document1::Window_VisibilityChanged);

    // As the page is loaded, after this constructor has exited, XAML will be
    // re-sizing the TextLayoutFrame scroll viewer element and the SizeChanged
    // handler will get invoked. That's when the image source will first get created.
}

void Scenario_Document1::OnNavigatedFrom(NavigationEventArgs^ e)
{
    FontDownloadListener::DownloadCompleted -= m_downloadCompletedEventToken;
    m_displayInformation->DpiChanged -= m_dpiChangedEventToken;
    Window::Current->VisibilityChanged -= m_visibilityChangedEventToken;
}

// Event handlers:

void Scenario_Document1::TextLayoutFrame_SizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
    // Get the current available width and update the TextLayout if different.
    // The available width will be the actual width less the left and right padding.
    //
    // Note: when this page is presented in a narrow window, or if this page and
    // the navigation pane are both open simultanously on a narrow display, the
    // width of our containing frame could become very small, and subtracting the
    // left and right padding values could even result in a negative value. That
    // would result in an error when we try to revise the text layout. This issue
    // is avoided, however, by having a MinWidth set on the grid for this page in
    // the XAML markup.
    //
    // There's a related issue: setting the text layout to a narrow width causes
    // it to have a large height. When we create the SurfaceImageSource used for
    // displaying the text layout, that large height could exceed the graphics
    // device resources on older devices. A check for this is included in the 
    // TextLayoutImageSource class, but it can result in some of the text layout 
    // being clipped. This should not happen on devices that support D3D feature
    // level 9_3 or later. Managing graphics device resources is beyond the scope 
    // of this sample, however, so that known issue is left as is.

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


void Scenario_Document1::FontDownloadListener_DownloadCompleted()
{
    // DownloadCompleted is called from a worker thread, so schedule work
    // in the UI thread.

    // Even though we call BeginDownload only once after created the text layout,
    // the download will have some latency, and changes to the layout that happen
    // before the download completes, such as changing the width, can result in
    // the same download request being added to the queue again. Because the XAML
    // framework monitors the font download queue, it can detect these new requests
    // and make its own BeginDownload calls. As a result, this DownloadCompleted
    // handler may get called multiple times as a result of our one use of a
    // downloadable font.
    //
    // We don't want to redundantly update the text layout and generate a new 
    // SurfaceImageSource on every one of these calls. To avoid doing that, we 
    // won't directly initiate that work. Instead, we'll use RequestTextLayoutUpdate
    // to indicate that that work is needed. It will effectively aggregate multiple
    // requests into a single operation.
    
    this->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this]() { RequestTextLayoutUpdate(); })
        );
}


void Scenario_Document1::DisplayInformation_DpiChanged(DisplayInformation^ displayInformation, Object^ e)
{
    m_dpi = displayInformation->LogicalDpi;
    PresentTextLayout();
}


void Scenario_Document1::Window_VisibilityChanged(Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e)
{
    // Re-create the SurfaceImageSource if the window has just become visible.
    if (e->Visible)
    {
        PresentTextLayout();
    }
}



// Private helper methods:

void Scenario_Document1::RequestTextLayoutUpdate()
{
    // We'll track that requests to update the layout have been made. If the actual 
    // work isn't already started, we'll initiate it by calling UpdateLayout. But we 
    // won't initiate new work while there's work already in progress. We'll schedule 
    // the work with lower priority, and that will allow multiple requests coming in 
    // rapid succession to get aggregated into a single call to UpdateLayout.
    //
    // Because this will be done on the UI thread, we're ensured to act on every
    // download of new data. There won't be a risk that the font data we care about
    // will finish downloading while we're busy responding to another download but
    // that we'll end up ignoring it. If the download we care about completes before 
    // UpdateLayout actually runs, the font will already be cached and will get used. 
    // But if it completes after UpdateLayout has been started, then the 
    // DownloadCompleted event will get handled in a new request after UpdateLayout
    // completes.

    if (!m_layoutUpdateInProgress)
    {
        m_layoutUpdateInProgress = true;

        this->Dispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Low,
            ref new Windows::UI::Core::DispatchedHandler([this]() { UpdateTextLayout(); })
            );
    }
}


void Scenario_Document1::UpdateTextLayout()
{
    // Refresh the TextLayout and present the updated layout. The refreshed
    // layout will be able to use fonts that have been downloaded.
    m_textLayout->InvalidateLayout();
    PresentTextLayout();

    // Update the status bar to show what fonts have actually been used.
    UpdateStatus();

    m_layoutUpdateInProgress = false;
}


void Scenario_Document1::PresentTextLayout()
{
    // Create a new TextLayoutImageSource based on the text layout and
    // use it as the source for the XAML image element.
    m_textLayoutImageSource = ref new TextLayoutImageSource(m_textLayout, m_dpi, m_textColor, m_textBackgroundColor);
    TextLayoutImage->Source = m_textLayoutImageSource;
}


void Scenario_Document1::UpdateStatus()
{
    // Get the fonts that have actually been used in the text layout.
    Platform::String^ fontsUsed = m_textLayout->FontsUsed;

    // The status will reflect whether the font used was the requested font
    // or a fallback font.
    NotifyType statusType = (fontsUsed == m_downloadableFontName) ? NotifyType::StatusMessage : NotifyType::ErrorMessage;

    // Now update the status.
    m_rootPage->NotifyUser("Fonts actually used: " + fontsUsed, statusType);
}

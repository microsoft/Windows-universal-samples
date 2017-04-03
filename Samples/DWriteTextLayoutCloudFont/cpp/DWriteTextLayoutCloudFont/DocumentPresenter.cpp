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
#include "DocumentPresenter.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;
using namespace DWriteTextLayoutCloudFontImplementation;

using Platform::String;
using Windows::Foundation::TypedEventHandler;
using Windows::Graphics::Display::DisplayInformation;
using Windows::UI::Color;
using Windows::UI::Xaml::Controls::Image;
using Windows::UI::Xaml::Controls::ScrollViewer;
using Windows::UI::Xaml::Window;
using Windows::UI::Xaml::WindowVisibilityChangedEventHandler;


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
// To present Direct2D content within the XAML frame element, a SurfaceImageSource
// implemented in the TextLayoutImageSource class) will be created and used as the
// source for an image control contained within the TextLayoutFrame element. The
// SurfaceImageSource implementation is not directly related to use of downloadable
// fonts in DirectWrite, so not the primary focus of this sample.
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


DocumentPresenter::DocumentPresenter(
    String^ text,
    String^ downloadableFontName,
    String^ languageTag,
    Color textColor,
    Color textBackgroundColor,
    Image^ imageTarget
) :
    m_downloadableFontName(downloadableFontName),
    m_textColor(textColor),
    m_textBackgroundColor(textBackgroundColor),
    m_imageTarget(imageTarget)
{
    // Text content, fonts and colours are defined in each page. This class handles the
    // implementation:
    //
    // - Creating the text layout object.
    // - Creating the surface image source object.
    // - Interacting with the font download listener object.
    // - Handling events that require updating the text layout or surface image source.

    // Create the TextLayout for our text content.
    m_textLayout = ref new TextLayout(text, languageTag, m_downloadableFontName, /* fontSize */ 20, /* default width */ 300);

    // Update the app status bar to show what fonts have actually been used. This will
    // ask the text layout for info that will require it to draw, which in turn will
    // cause the layout to add remote fonts to the font download queue. The status bar
    // is an informational tool for the sample. In an actual app, the layout would get
    // used in other ways involving drawing or measuring that would cause it to add
    // remote fonts to the font download queue.
    UpdateStatus();

    // Initialize FontDownloadListener, register a DownloadCompleted event handler, and
    // initiate a download.
    //
    // The FontDownloadListener is a singleton with static members. Calling Initialize
    // from any context will create the global singleton and set up its resources.
    //
    // The event handler must be unregistered before the page is destroyed. We'll do
    // that in the destructor. If we were to have XAML cache a page that has the
    // DocumentPresenter when the app navigates away from the page, then the handler
    // will still be hooked up, so the event won't be missed and the layout will be
    // updated.
    FontDownloadListener::Initialize();
    m_downloadCompletedEventToken = FontDownloadListener::DownloadCompleted += ref new FontDownloadCompletedHandler(this, &DocumentPresenter::FontDownloadListener_DownloadCompleted);
    FontDownloadListener::BeginDownload();

    // Handle DpiChanged -- the SurfaceImageSource will need to be re-created.
    m_displayInformation = DisplayInformation::GetForCurrentView();
    m_dpi = m_displayInformation->LogicalDpi;
    m_dpiChangedEventToken = m_displayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DocumentPresenter::DisplayInformation_DpiChanged);

    // Handle window VisibilityChanged -- the SurfaceImageSource will need to be re-created.
    m_visibilityChangedEventToken = Window::Current->VisibilityChanged += ref new WindowVisibilityChangedEventHandler(this, &DocumentPresenter::Window_VisibilityChanged);

} // DocumentPresenter()

DocumentPresenter::~DocumentPresenter()
{
    FontDownloadListener::DownloadCompleted -= m_downloadCompletedEventToken;
    m_displayInformation->DpiChanged -= m_dpiChangedEventToken;
    Window::Current->VisibilityChanged -= m_visibilityChangedEventToken;
}

void DocumentPresenter::SetLayoutWidth(float width)
{
    if (width != m_textLayout->Width)
    {
        m_textLayout->Width = width;
        PresentTextLayout();
    }
}

// Event handlers

void DocumentPresenter::DisplayInformation_DpiChanged(DisplayInformation^ displayInformation, Platform::Object^ e)
{
    // Re-create the SurfaceImageSource if the DPI has changed.
    m_dpi = displayInformation->LogicalDpi;
    PresentTextLayout();
}

void DocumentPresenter::FontDownloadListener_DownloadCompleted()
{
    // DownloadCompleted is called from a worker thread, so schedule work
    // in the UI thread.
    MainPage::Current->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this]() { RequestTextLayoutUpdate(); })
    );
}

void DocumentPresenter::Window_VisibilityChanged(Platform::Object ^sender, Windows::UI::Core::VisibilityChangedEventArgs ^e)
{
    // Re-create the SurfaceImageSource if the window has just become visible.
    if (e->Visible)
    {
        PresentTextLayout();
    }
}

// Private helper methods

void DocumentPresenter::PresentTextLayout()
{
    // Create a new TextLayoutImageSource based on the text layout and
    // use it as the source for the XAML image element.
    m_textLayoutImageSource = ref new TextLayoutImageSource(m_textLayout, m_dpi, m_textColor, m_textBackgroundColor);
    m_imageTarget->Source = m_textLayoutImageSource;
}

void DocumentPresenter::RequestTextLayoutUpdate()
{
    // Don't initiate new work while there's work already in progress.
    if (!m_layoutUpdateInProgress)
    {
        m_layoutUpdateInProgress = true;

        // Scheduling the update as a low-priority task so that work for multiple,
        // consecutive events that occur in rapid succession will get consolidated.
        MainPage::Current->Dispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Low,
            ref new Windows::UI::Core::DispatchedHandler([this]() { UpdateTextLayout(); })
        );
    }
}

void DocumentPresenter::UpdateTextLayout()
{
    // We'll be completing the layout update, so we can clear the flag used
    // for consolidating multiple events occuring in rapid succession into
    // a single task.
    m_layoutUpdateInProgress = false;

    // Refresh the TextLayout and present the updated layout. The refreshed
    // layout will be able to use fonts that have been downloaded.
    m_textLayout->InvalidateLayout();
    PresentTextLayout();

    // Update the status bar to show what fonts have actually been used.
    UpdateStatus();
}

void DocumentPresenter::UpdateStatus()
{
    // Get the fonts that have actually been used in the text layout.
    String^ fontsUsed = m_textLayout->FontsUsed;

    // The status will reflect whether the font used was the requested font
    // or a fallback font.
    NotifyType statusType = (fontsUsed == m_downloadableFontName) ? NotifyType::StatusMessage : NotifyType::ErrorMessage;

    // Now update the status.
    MainPage::Current->NotifyUser("Fonts actually used: " + fontsUsed, statusType);
}

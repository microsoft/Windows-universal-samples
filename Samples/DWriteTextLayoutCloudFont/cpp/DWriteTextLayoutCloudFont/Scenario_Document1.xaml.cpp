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



Scenario_Document1::Scenario_Document1()
{
    InitializeComponent();

    String^ text =
        L"Lorem ipsum dolor sit amet, pri ea voluptua consetetur. Vel eu altera omittam. Voluptaria reprehendunt vel eu, eum te graeco omnium blandit, mel nullam persius verterem eu. An euismod salutandi vis, vero exerci oblique ex mea. Cu quo audiam verear, esse facilis omittantur usu ei. Eam id dolor corpora oportere, vix dolor tincidunt id, viris dolorem vix ne. Minim gubergren id cum, vim ei quas denique prodesset." L"\xd\xa\xd\xa"
        L"Duo quem justo assentior ei. Cu simul quidam mei, ut stet incorrupte definitionem duo. Ne mei utinam graece intellegam, no causae mediocritatem vix, vim mundi aperiri detracto ei. Dictas definiebas ei in appellantur.";

    String^ downloadableFontName = ref new String(L"Neue Haas Grotesk Text Pro");

    // Set the colors to be used for the text layout. Also apply to the frame background.
    Color textColor = ColorHelper::FromArgb(0xFF, 0xFF, 0xFF, 0xFF);
    Color textBackgroundColor = ColorHelper::FromArgb(0xFF, 0x30, 0x30, 0x30);

    // Create the DocumentPresenter for our document.
    m_documentPresenter = ref new DocumentPresenter(
        text,
        downloadableFontName,
        L"hi", // Language tag for the content (Hindi).
        textColor,
        textBackgroundColor,
        TextLayoutImage // The target image control for the SurfaceImageSource presenting the text layout.
    );

    // As the page is loaded, after this constructor has exited, XAML will be re-sizing
    // the TextLayoutFrame scroll viewer element and the TextLayoutFrame_SizeChanged
    // handler will get invoked. That's when the image source will first get created.
}



// Event handlers:

void Scenario_Document1::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
    m_documentPresenter = nullptr;
}


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
    // casting here.
    m_documentPresenter->SetLayoutWidth(static_cast<float>(width));
}

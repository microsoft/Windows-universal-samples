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

#include "Payload.g.h"

namespace SDKTemplate
{
    /// <summary>
    /// This control is used to render an xml markup (via the Markup dependency property), which is then used as the 
    /// payload to send via a toast notification when the "Send notification" button is clicked.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Payload sealed
    {
    public:
        Payload();

        static void RegisterDependencyProperties();
        static property Windows::UI::Xaml::DependencyProperty^ MarkupProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get() { return s_markupProperty; }
        }

        property Platform::String^ Markup
        {
            Platform::String^ get() { return (Platform::String^)GetValue(s_markupProperty); }
            void set(Platform::String^ value) { SetValue(s_markupProperty, value); SetFormattedMarkup(value); }
        }

        static property Windows::UI::Xaml::DependencyProperty^ FormattedMarkupProperty
        {
            Windows::UI::Xaml::DependencyProperty^ get() { return s_formattedMarkupProperty; }
        }

        property Platform::String^ FormattedMarkup
        {
            Platform::String^ get() { return (Platform::String^)GetValue(s_formattedMarkupProperty); }
            void set(Platform::String^ value) { SetValue(s_formattedMarkupProperty, value); }
        }

    private:
        void OnSendClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SetFormattedMarkup(Platform::String^ markup);

        static Windows::UI::Xaml::DependencyProperty^ s_markupProperty;
        static Windows::UI::Xaml::DependencyProperty^ s_formattedMarkupProperty;
    };
}

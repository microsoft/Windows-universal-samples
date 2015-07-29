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

#include "Scenario1_Basic.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Basic sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        Scenario1_Basic();

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

        property Windows::UI::ViewManagement::UserInteractionMode InteractionMode
        {
            Windows::UI::ViewManagement::UserInteractionMode get()
            {
                return interactionMode;
            }
            void set(Windows::UI::ViewManagement::UserInteractionMode value)
            {
                if (interactionMode != value)
                {
                    interactionMode = value;
                    PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("InteractionMode"));
                }
            }
        }

        property Windows::UI::Xaml::Style^ CheckBoxStyle
        {
            Windows::UI::Xaml::Style^ get()
            {
                return checkBoxStyle;
            }
            void set(Windows::UI::Xaml::Style^ value)
            {
                if (checkBoxStyle != value)
                {
                    checkBoxStyle = value;
                    PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("CheckBoxStyle"));
                }
            }
        }

    protected:
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;
        Windows::UI::ViewManagement::UserInteractionMode interactionMode;
        Windows::UI::Xaml::Style^ checkBoxStyle;

        void OnWindowResize(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e);
        void UpdateContent();

        Windows::Foundation::EventRegistrationToken windowResizeToken;
    };
}

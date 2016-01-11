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

#include "App.g.h"
#include "ViewModel\ViewModelLocator.h"

namespace AdventureWorks
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    ref class App sealed
    {
    protected:
        virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;
        virtual void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs^ e) override;

    internal:
        App();

    private:
        void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
        void OnNavigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
        void OnBackRequested(Platform::Object^ sender, Windows::UI::Core::BackRequestedEventArgs^ e);

        Platform::String^ SemanticInterpretation(Platform::String^ tag, Windows::Media::SpeechRecognition::SpeechRecognitionResult^ result);
    };
}

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
#include "Scenario4.xaml.h"
#include "RecognizerHelper.h"

#include <ppltasks.h>

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input::Inking;
using namespace Windows::UI::Text::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario4::Scenario4()
{
    InitializeComponent();

    // Initialize drawing attributes. These are used in inking mode.
    InkDrawingAttributes^ drawingAttributes = ref new InkDrawingAttributes();
    drawingAttributes->Color = Windows::UI::Colors::Red;
    float penSize = 4.0f;
    drawingAttributes->Size = Size(penSize, penSize);
    drawingAttributes->IgnorePressure = false;
    drawingAttributes->FitToCurve = true;

    // Show the available recognizers
    inkRecognizerContainer = ref new InkRecognizerContainer();
    recoView = inkRecognizerContainer->GetRecognizers();
    if (recoView->Size > 0)
    {
        for (InkRecognizer^ recognizer : recoView)
        {
            RecoName->Items->Append(recognizer->Name);
        }
    }
    else
    {
        RecoName->IsEnabled = false;
        RecoName->Items->Append("No Recognizer Available");
    }
    RecoName->SelectedIndex = 0;

    // Set the text services so we can query when language changes
    textServiceManager = CoreTextServicesManager::GetForCurrentView();
    languageChangedToken = textServiceManager->InputLanguageChanged += ref new TypedEventHandler<CoreTextServicesManager^, Object^>(this, &Scenario4::TextServiceManager_InputLanguageChanged);

    SetDefaultRecognizerByCurrentInputMethodLanguageTag();

    // Initialize the InkCanvas
    inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
    inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen | CoreInputDeviceTypes::Touch;
}

void Scenario4::OnNavigatedFrom(NavigationEventArgs^ e)
{
    InstallReco->IsOpen = false;
    textServiceManager->InputLanguageChanged -= languageChangedToken;
}

void Scenario4::OnSizeChanged(Object^ sender, SizeChangedEventArgs e)
{
    HelperFunctions::UpdateCanvasSize(RootGrid, Output, inkCanvas);
}

void Scenario4::OnRecognizeAsync(Object^ sender, RoutedEventArgs^ e)
{
    auto currentStrokes = inkCanvas->InkPresenter->StrokeContainer->GetStrokes();
    if (currentStrokes->Size > 0)
    {
        RecognizeBtn->IsEnabled = false;
        ClearBtn->IsEnabled = false;
        RecoName->IsEnabled = false;

        create_task(inkRecognizerContainer->RecognizeAsync(inkCanvas->InkPresenter->StrokeContainer, InkRecognitionTarget::All))
            .then([this](IVectorView<InkRecognitionResult^>^ recognitionResults)
        {
            if (recognitionResults->Size > 0)
            {
                // Display recognition result
                String^ str = "Recognition result:";
                for (InkRecognitionResult^ r : recognitionResults)
                {
                    str += " " + r->GetTextCandidates()->GetAt(0);
                }
                rootPage->NotifyUser(str, NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("No text recognized.", NotifyType::StatusMessage);
            }

            RecognizeBtn->IsEnabled = true;
            ClearBtn->IsEnabled = true;
            RecoName->IsEnabled = true;
        });
    }
    else
    {
        rootPage->NotifyUser("Must first write something.", NotifyType::ErrorMessage);
    }
}

void Scenario4::OnRecognizerChanged(Object^ sender, RoutedEventArgs^ e)
{
    String^ selectedValue = (String^)RecoName->SelectedValue;
    SetRecognizerByName(selectedValue);
}

void Scenario4::OnClear(Object^ sender, RoutedEventArgs^ e)
{
    inkCanvas->InkPresenter->StrokeContainer->Clear();
    rootPage->NotifyUser("Cleared Canvas.", NotifyType::StatusMessage);
}

bool Scenario4::SetRecognizerByName(String^ recognizerName)
{
    bool recognizerFound = false;

    for (InkRecognizer^ reco : recoView)
    {
        if (recognizerName == reco->Name)
        {
            inkRecognizerContainer->SetDefaultRecognizer(reco);
            recognizerFound = true;
            break;
        }
    }

    if (!recognizerFound && rootPage != nullptr)
    {
        rootPage->NotifyUser("Could not find target recognizer.", NotifyType::ErrorMessage);
    }

    return recognizerFound;
}

void Scenario4::TextServiceManager_InputLanguageChanged(CoreTextServicesManager^ sender, Object^ args)
{
    SetDefaultRecognizerByCurrentInputMethodLanguageTag();
}


void Scenario4::SetDefaultRecognizerByCurrentInputMethodLanguageTag()
{
    // Query recognizer name based on current input method language tag (bcp47 tag)
    auto currentInputLanguage = textServiceManager->InputLanguage;

    if (currentInputLanguage != previousInputLanguage)
    {
        // try query with the full BCP47 name
        String^ recognizerName = RecognizerHelper::LanguageTagToRecognizerName(currentInputLanguage->LanguageTag);

        if (!recognizerName->IsEmpty())
        {
            for (unsigned int index = 0; index < recoView->Size; index++)
            {
                InkRecognizer^ recognizer = recoView->GetAt(index);
                if (recognizer->Name == recognizerName)
                {
                    inkRecognizerContainer->SetDefaultRecognizer(recognizer);
                    RecoName->SelectedIndex = index;
                    previousInputLanguage = currentInputLanguage;
                    break;
                }
            }
        }
    }
}

void Scenario4::RecoButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    InstallReco->IsOpen = !InstallReco->IsOpen;
}

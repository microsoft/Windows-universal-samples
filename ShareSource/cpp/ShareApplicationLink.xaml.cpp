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

//
// ShareApplicationLink.xaml.cpp
// Implementation of the ShareApplicationLink class
//

#include "pch.h"
#include "ShareApplicationLink.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::UI;
using namespace Windows::Storage::Streams;

ShareApplicationLink::ShareApplicationLink()
{
    InitializeComponent();
    
    // Populate the ApplicationLinkComboBox with the application links for all of the scenarios
    auto scenarioList = ref new Platform::Collections::Vector<Object^>();
    Uri^ itemToSelect = nullptr;
    for (unsigned int i = 0; i < SDKTemplate::MainPage::scenarios->Length; ++i)
    {
        Scenario s = MainPage::scenarios[i];
        Uri^ applicationLink = SharePage::GetApplicationLink(s.ClassName);
        scenarioList->Append(applicationLink);
        if (s.ClassName->Equals(GetType()->FullName))
        {
            itemToSelect = applicationLink;
        }
    }
    ApplicationLinkComboBox->ItemsSource = scenarioList;
    ApplicationLinkComboBox->SelectedItem = itemToSelect; // Default selection to the application link for this scenario
}

bool ShareApplicationLink::GetShareContent(DataRequest^ request)
{
    bool succeeded = false;

    Uri^ selectedApplicationLink = dynamic_cast<Uri^>(ApplicationLinkComboBox->SelectedItem);
    if (selectedApplicationLink != nullptr)
    {
        request->Data->Properties->Title = TitleInputBox->Text;
        request->Data->Properties->Description = DescriptionInputBox->Text;
        request->Data->SetApplicationLink(selectedApplicationLink);

        // Place the selected logo and the background color in the data package properties
        if (MicrosoftLogo->IsChecked->Value)
        {
            request->Data->Properties->Square30x30Logo = RandomAccessStreamReference::CreateFromUri(ref new Uri("ms-appx:///assets/microsoftLogo.png"));
            request->Data->Properties->LogoBackgroundColor = GetLogoBackgroundColor();
        }
        else if (VisualStudioLogo->IsChecked->Value)
        {
            request->Data->Properties->Square30x30Logo = RandomAccessStreamReference::CreateFromUri(ref new Uri("ms-appx:///assets/visualStudioLogo.png"));
            request->Data->Properties->LogoBackgroundColor = GetLogoBackgroundColor();
        }

        succeeded = true;
    }
    else
    {
        request->FailWithDisplayText(L"Select the application link you would like to share and try again.");
    }

    return succeeded;
}

Color ShareApplicationLink::GetLogoBackgroundColor()
{
    Color backgroundColor;
    backgroundColor.R = (unsigned char)RedSlider->Value;
    backgroundColor.G = (unsigned char)GreenSlider->Value;
    backgroundColor.B = (unsigned char)BlueSlider->Value;
    backgroundColor.A = (unsigned char)AlphaSlider->Value;

    return backgroundColor;
}

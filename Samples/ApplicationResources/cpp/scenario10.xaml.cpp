//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario10.xaml.cpp
// Implementation of the Scenario10 class
//

#include "pch.h"
#include "Scenario10.xaml.h"

using namespace SDKTemplate;
using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel::Resources::Core;


Scenario10::Scenario10()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario10::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;
}

void SDKTemplate::Scenario10::Scenario10Button_Show_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Button^ b = safe_cast<Button^>(sender);
    if (b != nullptr)
    {
        // use a cloned context for this scenario so that qualifier values set
        // in this scenario don't impact behaviour in other scenarios that
        // use a default context for the view (crossover effects between
        // the scenarios will not be expected)
        auto context = ResourceContext::GetForCurrentView()->Clone();

        auto selectedLanguage = Scenario10ComboBox_Language->SelectedValue;
        auto selectedScale = Scenario10ComboBox_Scale->SelectedValue;
        auto selectedContrast = Scenario10ComboBox_Contrast->SelectedValue;
        auto selectedHomeRegion = Scenario10ComboBox_HomeRegion->SelectedValue;

        if (selectedLanguage != nullptr)
        {
            context->QualifierValues->Insert("language", selectedLanguage->ToString());
        }
        if (selectedScale != nullptr)
        {
            context->QualifierValues->Insert("scale", selectedScale->ToString());
        }
        if (selectedContrast != nullptr)
        {
            context->QualifierValues->Insert("contrast", selectedContrast->ToString());
        }
        if (selectedHomeRegion != nullptr)
        {
            context->QualifierValues->Insert("homeregion", selectedHomeRegion->ToString());
        }

        Platform::Collections::Vector<String^>^ resourceIds = ref new Platform::Collections::Vector<String^>();
        //auto resourceIds = new list<String^>();
        resourceIds->Append("LanguageOnly");
        resourceIds->Append("ScaleOnly");
        resourceIds->Append("ContrastOnly");
        resourceIds->Append("HomeRegionOnly");
        resourceIds->Append("MultiDimensional");
        Scenario10_SearchMultipleResourceIds(context, resourceIds);
    }
}


void SDKTemplate::Scenario10::Scenario10_SearchMultipleResourceIds(ResourceContext^ context, Platform::Collections::Vector<String^>^ resourceIds)
{
    Scenario10TextBlock->Text = "";
    auto dimensionMap = ResourceManager::Current->MainResourceMap->GetSubtree("dimensions");

    for (unsigned int it = 0; it < resourceIds->Size; it++)
    {
        String^ id = resourceIds->GetAt(it);
        NamedResource^ namedResource = dimensionMap->Lookup(id);
        if (namedResource)
        {
            auto resourceCandidates = namedResource->ResolveAll(context);
            Scenario10_ShowCandidates(resourceIds->GetAt(it), resourceCandidates);
        }
    }
}

void SDKTemplate::Scenario10::Scenario10_ShowCandidates(String^ resourceId, Windows::Foundation::Collections::IVectorView<ResourceCandidate^>^ resourceCandidates)
{
    // print 'resourceId', 'found value', 'qualifer info', 'matching condition'
    String^ outText = "resourceId: dimensions\\" + resourceId + "\r\n";

    for(unsigned int i =0; i < resourceCandidates->Size; i++)
    {
        ResourceCandidate^ candidate = resourceCandidates->GetAt(i);
        auto value = candidate->ValueAsString;

        outText += "    Candidate " + i.ToString() + ":" + value + "\r\n";
        for (unsigned int j = 0; j < candidate->Qualifiers->Size; j++)
        {
            auto qualifier = candidate->Qualifiers->GetAt(j);
            auto qualifierName = qualifier->QualifierName;
            auto qualifierValue = qualifier->QualifierValue;
            outText += "        Qualifer: " + qualifierName + ": " + qualifierValue + "\r\n";
            outText += "        Matching: IsMatch (" + qualifier->IsMatch.ToString() + ")  " + "IsDefault (" + qualifier->IsDefault.ToString() + ")" + "\r\n";
        }
    }

    this->Scenario10TextBlock->Text += outText + "\r\n";

}

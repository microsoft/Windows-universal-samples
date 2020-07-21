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
#include "Scenario4_CharacterGrouping.h"
#include "Scenario4_CharacterGrouping.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::Collation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::SDKTemplate::implementation
{
    Scenario4_CharacterGrouping::Scenario4_CharacterGrouping()
    {
        InitializeComponent();
    }


    void Scenario4_CharacterGrouping::AddGroup(hstring const& label, UIElement const& element)
    {
        Groups.try_emplace(label, element);
        GroupingsGrid().Items().Append(element);
    }

    void Scenario4_CharacterGrouping::GetGroups_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        hstring lang = LanguageText().Text();
        if (lang == L"Default")
        {
            ActiveGroupings = CharacterGroupings();
        }
        else if (Language::IsWellFormed(lang))
        {
            ActiveGroupings = CharacterGroupings(lang);
        }
        else
        {
            rootPage.NotifyUser(L"Invalid language tag.", NotifyType::ErrorMessage);
            return;
        }

        GroupingsGrid().Items().Clear();
        Groups.clear();
        for (CharacterGrouping grouping : ActiveGroupings)
        {
            hstring label = grouping.Label();
            // Save the "other" group for the end.
            if (!label.empty())
            {
                TextBlock textBlock;
                textBlock.Text(label);
                AddGroup(label, textBlock);
            }
        }
        SymbolIcon symbolIcon;
        symbolIcon.Symbol(Symbol::Globe);
        AddGroup(GlobeString, symbolIcon);
    }

    void Scenario4_CharacterGrouping::AssignGroup_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        if (ActiveGroupings == nullptr)
        {
            rootPage.NotifyUser(L"Select a language first.", NotifyType::ErrorMessage);
            return;
        }
        hstring text = CandidateText().Text();
        if (text.empty())
        {
            rootPage.NotifyUser(L"Enter text to assign to a group.", NotifyType::ErrorMessage);
            return;
        }
        hstring label = ActiveGroupings.Lookup(text);
        auto found = Groups.find(label);
        if (found == Groups.end())
        {
            label = GlobeString;
            found = Groups.find(label);
        }
        GroupingsGrid().SelectedItem(found->second);
        GroupingResult().Text(L"Group \"" + label + L"\"");
        found->second.StartBringIntoView();
    }
}

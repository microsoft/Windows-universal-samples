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
#include "Scenario8_UpdateAsync.h"
#include "Scenario8_UpdateAsync.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::StartScreen;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario8_UpdateAsync::Scenario8_UpdateAsync()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario8_UpdateAsync::UpdateDefaultLogo_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();          

        if (SecondaryTile::Exists(SampleHelpers::LogoSecondaryTileId))
        {
            // Add the properties we want to update (logo in this example)
            SecondaryTile secondaryTile(SampleHelpers::LogoSecondaryTileId);
            secondaryTile.VisualElements().Square150x150Logo(Uri(L"ms-appx:///Assets/squareTileLogoUpdate-sdk.png"));
            bool isUpdated = co_await secondaryTile.UpdateAsync();

            if (isUpdated)
            {
                rootPage.NotifyUser(L"Secondary tile logo updated.", NotifyType::StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(L"Secondary tile logo not updated.", NotifyType::ErrorMessage);
            }
        }
        else
        {
            rootPage.NotifyUser(L"Please pin a secondary tile using scenario 1 before updating the logo.", NotifyType::ErrorMessage);
        }
    }
}
